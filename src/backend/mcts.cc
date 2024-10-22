#ifndef MCTS_CC
#define MCTS_CC

#include "mcts.hpp"

void atomic_add(std::atomic<double>& atomic_var, double value) {
    double old_value = atomic_var.load();  // Step 1: Load the current value
    while (!atomic_var.compare_exchange_weak(old_value, old_value + value)) {
        // Step 2: Retry if another thread has changed the value
        // The loop retries loading the current value and attempts the exchange
    }
}

StateNode::~StateNode() {
    if (this->expanded) {
        for (int mi = 0; mi < this->num_moves; mi++)
            delete this->moves[mi];
        delete[] this->moves;
    }
}

MoveNode::~MoveNode() {
    for (int nsi = 0; nsi < this->num_next_states; nsi++)
        delete this->next_states[nsi];
    delete[] this->next_states;
}

void EQElem::set_result(torch::Tensor result) {
    this->results.index_put_({Slice(eval_start, eval_end)}, result.index({Slice(batch_start, batch_end)}));
    this->last_updated = this->eval_end;
    this->partial_completion_event.set();
}


StateNode* MCTS::selection(StateNode* node) {
    while (node->expanded && !node->state->game_over) {
        // PUCT
        double* u = new double[node->num_moves]();
        
        int sum = 0;
        for (int i = 0; i < node->num_moves; i++) { sum += node->moves[i]->n.load(); }
        double sqrt_sum = std::sqrt(sum);
        for (int i = 0; i < node->num_moves; i++) {
            u[i] = c_puct * node->moves[i]->p.load() * sqrt_sum / (1 + node->moves[i]->n.load());
        }

        int max_i = 0;
        double max = node->moves[0]->q.load() + u[0];
        for (int i = 1; i < node->num_moves; i++) {
            if (max < node->moves[i]->q.load() + u[i]) {
                max = node->moves[i]->q.load() + u[i];
                max_i = i;
            }
        }
        delete[] u; 

        // Adding virtual losses to node
        node->moves[max_i]->n.fetch_add(this->n_vl);
        atomic_add(node->moves[max_i]->w, (double)-this->n_vl);

        // Uniformly randomly sample the next state 
        int next_state_choice = this->model->throw_gen->get_randint(0, node->moves[max_i]->num_next_states);
        node = node->moves[max_i]->next_states[next_state_choice];
    }
    return node;
}

StateNode* MCTS::expansion(StateNode* node) {
    node->expansion_mtx.lock();

    // If the node has not been expanded by another thread, expand it
    if (!node->expanded) {
        if (!node->state->game_over) {
            // Create move nodes for all possible moves
            std::vector<Move> moves = this->model->all_possible_moves(node->state);
            if (moves.size() > 0) {
                // If there are moves available, create nodes for them
                node->moves = new MoveNode*[moves.size()];
                for (int mi = 0; mi < moves.size(); mi++) 
                    node->moves[mi] = new MoveNode(moves[mi], node);
                node->num_moves = moves.size();
            } else {
                // If there are no available moves for the player, add a pass move
                node->moves = new MoveNode*[1];
                Move pass_move;  // Pass move
                node->moves[0] = new MoveNode(pass_move, node);
                node->num_moves = 1;
            }

            // For each move, create next state nodes.
            for (int mi = 0; mi < node->num_moves; mi++) {
                std::vector<StatePtr> next_states = this->model->generate_next_states(node->state, node->moves[mi]->move);
                node->moves[mi]->next_states = new StateNode*[next_states.size()];
                for (int nsi = 0; nsi < next_states.size(); nsi++)
                    node->moves[mi]->next_states[nsi] = new StateNode(next_states[nsi], node->moves[mi]);
                node->moves[mi]->num_next_states = next_states.size();
            }
        }
        node->expanded = true;
        // LOCK IS NOT RELEASED UNTIL p FOR node IS CALCULATED. THIS IS TO MAKE SURE OTHER THREADS WAIT FOR EXPANSION COMPLETION
    } else {
        // Else, release lock immediately and resume from selection phase
        node->expansion_mtx.unlock();
        if (!node->state->game_over) {
            node = this->selection(node);
            node = this->expansion(node);
        }
    }
    return node;
}

double MCTS::evaluation(StateNode* node) {
    double ret = 0.0;
    if (!node->state->game_over) {
        
        // Convert next states to tensor representations and send them to value net for evaluations
        int num_next_states = node->num_moves * 6; // Shortcut as we know there will only be 6 next_states
        torch::Tensor states = torch::zeros({num_next_states, 59, 26}, torch::kFloat32);
        torch::Tensor invert_mask = torch::ones({node->num_moves,}, torch::kInt32); 

        for (int mi = 0; mi < node->num_moves; mi++) 
            for (int nsi = 0; nsi < node->moves[mi]->num_next_states; nsi++) {
                states[mi * 6 + nsi] = get_state_tensor_repr(node->moves[mi]->next_states[nsi]->state, this->model->get_config());
                invert_mask[mi] = node->state->current_player == node->moves[mi]->next_states[nsi]->state->current_player ? 1 : -1;
            }

        EQElem* eq_elem = new EQElem(states);
        this->evaluation_queue->push(eq_elem);

        bool evaluation_complete = false;
        while (!evaluation_complete) {
            // Wait for a trigger to check all evaluations are completed
            eq_elem->partial_completion_event.wait();
            // After trigger, check if evaluations are complete
            evaluation_complete = eq_elem->is_evaluated();
            eq_elem->partial_completion_event.clear();
        }

        // Now that evaluations are complete, use the results
        torch::Tensor results = eq_elem->results;
        
        delete eq_elem;

        results = results.reshape({-1, 6}).mean(-1) * invert_mask;
        torch::Tensor probs = torch::softmax(results / this->prior_temp, /*dim=*/-1);


        for (int mi = 0; mi < node->num_moves; mi++)
            node->moves[mi]->p.store(probs[mi].item<float>());

        ret = (probs * results).sum().item<float>();
        ret *= node->state->current_player == this->owner? 1.0 : -1.0;
    }
    else {
        // If game_over state, find the winner player
        ret = this->model->check_player_completed(node->state, this->owner)? 1 : -1; 
    }
    return ret;
}

int MCTS::backup(StateNode* node, double ret) {
    // ret value must always be in the context of the owner player.
    int depth = 0;
    while (node->parent) {

        MoveNode* parent_action_node = node->parent; 
        int player_multiplier = parent_action_node->parent->state->current_player == this->owner ? 1 : -1;
        parent_action_node->n.fetch_add(1 - n_vl);
        atomic_add(parent_action_node->w, (player_multiplier * ret) + n_vl);
        parent_action_node->q.store(parent_action_node->w.load() / parent_action_node->n.load());
        
        node = parent_action_node->parent;
        depth++;
    }
    return depth;
}

int MCTS::simulation() {
    // auto start = std::chrono::high_resolution_clock::now();
    StateNode* node = this->root;
    double ret = 0.0;
    int depth = 0;
    if (!this->stop_flag.load())
        node = this->selection(node);
    if (!this->stop_flag.load())
        node = this->expansion(node);
    if (!this->stop_flag.load()) {
        ret = this->evaluation(node);
        node->expansion_mtx.unlock();
    }
    if (!this->stop_flag.load())
        depth = this->backup(node, ret);
    // auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "SimTime : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    
    return depth;
}

void MCTS::search(int num_simulations) {

    // Start evaluation thread
    this->stop_flag.store(false);
    std::thread eval_thread(nnet_evaluations, std::ref(this->stop_flag), this->evaluation_queue, this->v_net, this->evaluation_batch_size);
    
    // Start a thread pool to perform simulations
    thread_pool simulation_pool(this->sim_pool_size);
    std::vector<std::future<int>> futures;
    futures.reserve(num_simulations);

    // Push the simulation jobs in pool
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_simulations; i++) {
        futures.push_back(simulation_pool.push([this](std::atomic<bool>&) { return this->simulation(); }));
    }
    
    // Wait for all jobs to complete
    int max_depth = 0;
    for (auto& f: futures) {
        int depth = f.get();
        if (depth > max_depth)
            max_depth = depth;
    }
    auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "Time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    // std::cout << "Max depth : " << max_depth << "\n";
    
    // Wait for evaluation thread to stop
    this->stop_flag.store(true);
    eval_thread.join();
    this->evaluation_queue->clear();
}

int MCTS::select_next_move(double selection_temp) {
    // This method must be called on an expanded node

    // pi(a|s) = N(s,a)^(1/t) / sum(N(s,a)^(1/t))
    double sum_n = 0;
    for (int mi = 0; mi < this->root->num_moves; mi++)
        sum_n += std::pow(this->root->moves[mi]->n.load(), 1.0/selection_temp);
    
    double* pi = new double[this->root->num_moves]();
    
    for (int mi = 0; mi < this->root->num_moves; mi++)
        pi[mi] = std::pow(this->root->moves[mi]->n.load(), 1.0/selection_temp) / sum_n;

    
    // std::cout << "pi : [ ";
    // for(int mi = 0; mi < this->root->num_moves; mi++)
    //     std::cout << pi[mi] << ", ";
    // std::cout << "]\n";

    // Sampling from pi(a|s)
    double random_u = this->model->throw_gen->get_randreal(0.0, 1.0);
    double sum = 0.0;
    int selected_mi = 0;
    for (int mi = 0; mi < this->root->num_moves; mi++) {
        sum += pi[mi];
        if (random_u < sum) {
            selected_mi = mi;
            break;
        }
    }

    // std::cout << "Sampled mi: " << selected_mi << std::endl;

    delete[] pi;
    return selected_mi;
} 

void MCTS::take_move(int move_idx, StatePtr next_state) {
    if (!this->root->expanded) {
        this->expansion(this->root);
        this->root->expansion_mtx.unlock();
    }

    // Finding and isolating the next root
    StateNode* next_root = nullptr; 
    for (int nsi = 0; nsi < this->root->moves[move_idx]->num_next_states; nsi++) {
        if (this->root->moves[move_idx]->next_states[nsi]->state->dice_roll == next_state->dice_roll) {
            next_root = this->root->moves[move_idx]->next_states[nsi];
            this->root->moves[move_idx]->next_states[nsi] = nullptr;
            break;
        }
    }
    // Deleting the tree except the sub-tree from next_root
    delete this->root;
    this->root = next_root;
    this->root->parent = nullptr;

}

void nnet_evaluations(
    std::atomic<bool>& stop_flag, 
    std::shared_ptr<TSQueue<EQElem*>> evaluation_queue, 
    ValueNet v_net,
    int eval_bs
) {
    while(!stop_flag.load()) {
        // Fetch from queue until batch fill 
        
        int current_elem_idx = 0;
        torch::Tensor states = torch::zeros({eval_bs, 59, 26}, torch::kFloat32);
        std::vector<EQElem*> queue_elems_in_evaluation;

        while (current_elem_idx < eval_bs) {
            auto popped = evaluation_queue->pop(std::chrono::milliseconds(10)); // Timeout of 10ms
            if(popped.has_value()) {
                EQElem* elem = *popped;

                queue_elems_in_evaluation.push_back(elem);

                // Slice a set of states from the elem 
                int batch_start = current_elem_idx;
                int batch_end = eval_bs;
                int eval_start = elem->eval_end;
                if ((batch_start + elem->total - elem->eval_end) > eval_bs)
                    evaluation_queue->push(elem);
                else
                    batch_end = batch_start + elem->total - elem->eval_end;
                int eval_end = elem->eval_end + batch_end - batch_start;

                elem->batch_start = batch_start;
                elem->batch_end = batch_end;
                elem->eval_start = eval_start;
                elem->eval_end = eval_end;
                states.index_put_({Slice(batch_start, batch_end)}, elem->states.index({Slice(eval_start, eval_end)}));

                current_elem_idx = batch_end;

            } else {
                // Timeout occured
                // std::cout << "Timeout cei: " << current_elem_idx << "\n";  
                break;
            }
        }

        // Now that a batch of states is created, perform v_net evaluation
        {
            torch::NoGradGuard no_grad; // with torch.no_grad()
            torch::Tensor results = v_net(states);
            // Set results back to the elems
            for (auto elem : queue_elems_in_evaluation)
                elem->set_result(results.reshape({-1}));
        }
    }
}


// MCTS Test
// int main(int argc, char* argv[]) {
    
//     std::vector<std::vector<std::string>> colours_config = {
//         {"red", "yellow"}, 
//         {"blue", "green"}
//     };
//     std::shared_ptr<GameConfig> config = std::make_shared<GameConfig>(colours_config);
//     Ludo game(config);
//     game.reset();

//     ValueNet v_net = ValueNet(26, 128, 1024, "cuda", 0.1);
//     std::cout << "Num parameters: " << count_parameters(v_net) << "\n";
//     fs::path players_dir("../run1/players/network_2024_Sep_02_09_33_50_799.pth");
//     torch::load(v_net, players_dir.string());
//     v_net->eval();
//     std::cout << "Loaded v_net.\n";

//     std::vector<MCTS*> mc_trees;

//     for (int player = 0; player < game.state->n_players; player++)
//         mc_trees.push_back(new MCTS(game.state, player, game.model, 1.0, 3, v_net));

//     std::cout << game.state->repr() << "\n";
//     while(!game.state->game_over) {
        
//         mc_trees[game.state->current_player]->search(500);

//         std::cout << "p : [ ";
//         for(int mi = 0; mi < mc_trees[game.state->current_player]->root->num_moves; mi++)
//             std::cout << mc_trees[game.state->current_player]->root->moves[mi]->p.load() << ", ";
//         std::cout << "]\n";

//         std::cout << "n : [ ";
//         for(int mi = 0; mi < mc_trees[game.state->current_player]->root->num_moves; mi++)
//             std::cout << mc_trees[game.state->current_player]->root->moves[mi]->n.load() << ", ";
//         std::cout << "]\n";
        
//         std::cout << "w : [ ";
//         for(int mi = 0; mi < mc_trees[game.state->current_player]->root->num_moves; mi++)
//             std::cout << mc_trees[game.state->current_player]->root->moves[mi]->w.load() << ", ";
//         std::cout << "]\n";

//         std::cout << "q : [ ";
//         for(int mi = 0; mi < mc_trees[game.state->current_player]->root->num_moves; mi++)
//             std::cout << mc_trees[game.state->current_player]->root->moves[mi]->q.load() << ", ";
//         std::cout << "]\n";

//         int selected_move_idx = mc_trees[game.state->current_player]->select_next_move();
//         MoveNode* selected_move_node = mc_trees[game.state->current_player]->root->moves[selected_move_idx]; 
//         std::cout << selected_move_node->move.repr() << "\n";
//         game.turn(selected_move_node->move, game.state->last_move_id+1);

//         for (int player = 0; player < game.state->n_players; player++) 
//             mc_trees[player]->take_move(selected_move_idx, game.state);

//         std::cout << game.state->repr() << "\n";
//     }


//     for (int player = 0; player < game.state->n_players; player++)
//         delete mc_trees[player];


//     return 0;
// }

#endif