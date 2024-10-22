find_package(Torch CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS})

set(SRC 
    engine.cc 
    random_game.cc
    proto_utils.cc
    network_utils.cc
)
set(INIT_SRC 
    initializer.cc
    network.cc
    network_utils.cc    
)
set(MANAGER_SRC 
    managers.cc
    engine.cc
    proto_utils.cc
)
set(RND_ACTOR_SRC 
    engine.cc 
    proto_utils.cc 
    random_actor.cc
)
# set(NET_SRC network.cc)
# set(MCTS_SRC 
#     engine.cc
#     mcts.cc
#     network.cc
#     network_utils.cc
# )
set(MCTS_ACTOR_SRC 
    engine.cc
    mcts.cc
    network.cc
    network_utils.cc
    proto_utils.cc
    mcts_actor.cc
)

add_executable(eng ${SRC})
add_executable(initializer ${INIT_SRC})
add_executable(managers ${MANAGER_SRC})
add_executable(random_actor ${RND_ACTOR_SRC})
# add_executable(net ${NET_SRC})
# add_executable(mcts ${MCTS_SRC})
add_executable(mcts_actor ${MCTS_ACTOR_SRC})

target_link_libraries(eng PRIVATE myproto ${TORCH_LIBRARIES})
target_link_libraries(initializer PRIVATE myproto ${TORCH_LIBRARIES})
target_link_libraries(managers PRIVATE myproto)
target_link_libraries(random_actor PRIVATE myproto)
# target_link_libraries(net ${TORCH_LIBRARIES})
# target_link_libraries(mcts ${TORCH_LIBRARIES})
target_link_libraries(mcts_actor PRIVATE myproto ${TORCH_LIBRARIES})