
set(SRC 
    engine.cc 
    random_game.cc
    proto_utils.cc
)
set(SERVER_SRC example_server.cpp)

add_executable(eng ${SRC})
add_executable(eserver ${SERVER_SRC})
target_link_libraries(eng PRIVATE myproto)
target_link_libraries(eserver PRIVATE myproto)
