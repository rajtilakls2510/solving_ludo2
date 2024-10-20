#ifndef MCTS_CC
#define MCTS_CC

#include "mcts.hpp"

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
    // std::cout << "2 States size : ";
    // print_size(this->states);
    // std::cout << "2 Results size : ";
    // print_size(this->results);
    // std::cout << "Slicing 2 bs: "<< batch_start<<" be: "<< batch_end<<" es: "<< eval_start<<" ee: "<< eval_end<<"\n";
    this->results.index_put_({Slice(eval_start, eval_end)}, result.index({Slice(batch_start, batch_end)}));
    this->last_updated = this->eval_end;
    this->partial_completion_event.set();
}


StateNode* MCTS::selection(StateNode* node) {
    while (node->expanded && !node->state->game_over) {
        // std::cout << "Num moves : " << node->num_moves << ", Num next states : " << node->moves[0]->num_next_states << std::endl;
        // std::cout << node->state->repr();
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
    node->stats_update_mtx.lock();

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
        node->stats_update_mtx.unlock();
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
        // std::cout << "State dim : " << states.size(0) << "\n";
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
    // std::cout << "BACKUP " << node->state->repr();
    while (node->parent) {

        MoveNode* parent_action_node = node->parent; 
        int player_multiplier = parent_action_node->parent->state->current_player == this->owner ? 1 : -1;
        parent_action_node->n.fetch_add(1 - n_vl);
        atomic_add(parent_action_node->w, (player_multiplier * ret) + n_vl);
        parent_action_node->q.store(parent_action_node->w.load() / parent_action_node->n.load());
        
        // std::cout << "BACKUP " << parent_action_node->parent->state->repr() <<
        // " Move : " << parent_action_node->move.repr() << " n : " << parent_action_node->n.load() <<
        // " w : " << parent_action_node->w.load() << " q : " << parent_action_node->q.load() << "\n";  
        
        node = parent_action_node->parent;
        depth++;
    }
    // std::cout << "BFFFF\n";
    return depth;
}

int MCTS::simulation() {
    auto start = std::chrono::high_resolution_clock::now();
    StateNode* node = this->root;
    double ret = 0.0;
    int depth = 0;
    if (!this->stop_flag.load())
        node = this->selection(node);
    if (!this->stop_flag.load())
        node = this->expansion(node);
    if (!this->stop_flag.load()) {
        ret = this->evaluation(node);
        node->stats_update_mtx.unlock();
    }
    if (!this->stop_flag.load())
        depth = this->backup(node, ret);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "SimTime : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    
    return depth;
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
                // std::cout << "Slicing 1 bs: "<< batch_start<<" be: "<< batch_end<<" es: "<< eval_start<<" ee: "<< eval_end<<"\n";
                states.index_put_({Slice(batch_start, batch_end)}, elem->states.index({Slice(eval_start, eval_end)}));

                current_elem_idx = batch_end;

            } else {
                // Timeout occured
                std::cout << "Timeout cei: " << current_elem_idx << "\n";  
                break;
            }
        }

        // Now that a batch of states is created, perform v_net evaluation
        {
            torch::NoGradGuard no_grad; // with torch.no_grad()
            // std::cout << "1 States size : ";
            // print_size(states);
            torch::Tensor results = v_net(states);
            // std::cout << "1 Results size : ";
            // print_size(results);
            // Set results back to the elems
            for (auto elem : queue_elems_in_evaluation)
                elem->set_result(results.reshape({-1}));
        }
        // std::cout << "Evaluating batch...\n"; 
        
    }
}


// MCTS Test
int main(int argc, char* argv[]) {
    
    std::vector<std::vector<std::string>> colours_config = {
        {"red", "yellow"}, 
        {"blue", "green"}
    };
    std::shared_ptr<GameConfig> config = std::make_shared<GameConfig>(colours_config);
    Ludo game(config);
    game.reset();

    ValueNet v_net = ValueNet(26, 128, 1024, "cuda", 0.1);
    fs::path players_dir("../run1/players/network_2024_Sep_02_09_33_50_799.pth");
    torch::load(v_net, players_dir.string());
    v_net->eval();
    std::cout << "Loaded v_net.\n";

    MCTS mcts(game.state, game.state->current_player, game.model, 3.0, 3, v_net);
    std::cout << "Created mcts obj...\n";

    std::thread eval_thread(nnet_evaluations, std::ref(mcts.stop_flag), mcts.evaluation_queue, mcts.v_net, mcts.evaluation_batch_size);
    
    thread_pool simulation_pool(10);
    std::vector<std::future<int>> futures;
    mcts.simulation();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 20000; i++) {
        futures.push_back(simulation_pool.push([&mcts](std::atomic<bool>&) { return mcts.simulation(); }));
    }
    
    int max_depth = 0;
    for (auto& f: futures) {
        int depth = f.get();
        if (depth > max_depth)
            max_depth = depth;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    std::cout << "Max depth : " << max_depth << "\n";
    // mcts.expansion(mcts.root);
    // StateNode* leaf_node = mcts.selection(mcts.root);
    // // StateNode* leaf_node2 = mcts.selection(mcts.root);
    // double ret = mcts.evaluation(mcts.root);
    // mcts.root->stats_update_mtx.unlock();

    // std::cout << mcts.root->state->repr();
    // std::cout << leaf_node->state->repr();
    // std::cout << "Ret : " << ret << "\n";
    // // std::cout << leaf_node2->state->repr();



    // mcts.backup(leaf_node, ret);
    // mcts.backup(leaf_node2, 3.0);

    // std::this_thread::sleep_for(std::chrono::seconds(5));
    
    mcts.stop_flag.store(true);
    std::cout << "Stop called\n";

    eval_thread.join();
    return 0;
}

#endif