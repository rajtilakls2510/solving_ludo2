#ifndef MCTS_HPP
#define MCTS_HPP

#include <iostream>
#include "engine.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include "threading_utils.hpp"
#include "network_utils.hpp"
#include <memory>
#include <torch/torch.h>
#include "network.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;  
using namespace torch::indexing;

class StateNode;

class MoveNode;


class StateNode {
public:
    StatePtr state;
    MoveNode* parent{nullptr};
    std::mutex stats_update_mtx;
    bool expanded{false};
    int num_moves{0};
    MoveNode** moves{nullptr};

    StateNode(StatePtr state, MoveNode* parent) : state(state), parent(parent) {}
    ~StateNode();
};

class MoveNode {
public:
    Move move;
    StateNode* parent{nullptr};
    int num_next_states{0};
    StateNode** next_states{nullptr};

    std::atomic<double> p{0.0};
    std::atomic<int> n{0};
    std::atomic<double> w{0.0};
    std::atomic<double> q{0.0};

    MoveNode(Move move, StateNode* parent) : move(move), parent(parent) {};
    ~MoveNode();
};

void atomic_add(std::atomic<double>& atomic_var, double value) {
    double old_value = atomic_var.load();  // Step 1: Load the current value
    while (!atomic_var.compare_exchange_weak(old_value, old_value + value)) {
        // Step 2: Retry if another thread has changed the value
        // The loop retries loading the current value and attempts the exchange
    }
}

class EQElem {
public:
    Event partial_completion_event;
    torch::Tensor states;
    torch::Tensor results;
    int total{0}, eval_start{0}, eval_end{0}, batch_start{0}, batch_end{0}, last_updated{0};


    EQElem(torch::Tensor states): states(states)  { 
        this->total = states.size(0); 
        results = torch::zeros({states.size(0),}, torch::kFloat32);
    }
    bool is_evaluated() { return this->last_updated == this->total;}
    void set_result(torch::Tensor result);
};

class MCTS {
public: // TODO: Change to private after testing
    StateNode* root;        // Holds the root StateNode of the tree
    short owner;            // Holds the owner player of the tree 
    LudoModelPtr model;     // Holds the model
    double c_puct;          // The amount of exploration 
    short n_vl;             // The amount of virtual losses to add
    std::atomic<bool> stop_flag{false};    // Indicator to stop all MCTS activities
    std::shared_ptr<TSQueue<EQElem*>> evaluation_queue;
    ValueNet v_net;
    int evaluation_batch_size{64};
    double prior_temp{1.0};

    // Performs the selection process and returns the left node reached by the process
    StateNode* selection(StateNode* node);

    // Performs an expansion step on the node.
    // Note: If the given node is already being expanded by another thread, this thread will
    //       wait for expansion completion and restart from the selection process to get a new 
    //       leaf node and perform expansion on this. 
    StateNode* expansion(StateNode* node);
    
    // Given an expanded node, it performs neural network evaluation of the next states and returns a backup value
    double evaluation(StateNode* node);

    // Backs up the 'Return' on the path traced by selection step. Returns depth
    int backup(StateNode* node, double ret);
    
    // Performs a single simulation process consisting of selection, expansion, evaluation and backup phases. Returns depth.
    int simulation();


public:
    MCTS(StatePtr state, short owner, LudoModelPtr model, double c_puct, short n_vl, ValueNet v_net) : owner(owner), model(model), c_puct(c_puct), n_vl(n_vl), v_net(v_net) {
        this->evaluation_queue = std::make_shared<TSQueue<EQElem*>>();
        this->root = new StateNode(state, nullptr);
    }

    // Performs search for a particular number of simulations in multiple-threads
    void search(int num_simulations);

    // Samples a move from the tree posterior distribution
    MoveNode* select_next_move(double selection_temp=1.0);

    // Takes the move in the tree and updates to the next state
    void take_move(MoveNode* move, StatePtr next_state);

    ~MCTS() { 
        stop_flag.store(true);
        evaluation_queue->clear();
        delete root; 
    }
};

// Performs neural network evaluations for states put in the evaluation queue in a separate thread
void nnet_evaluations(
    std::atomic<bool>& stop_flag, 
    std::shared_ptr<TSQueue<EQElem*>> evaluation_queue, 
    ValueNet v_net,
    int eval_bs    
);

#endif