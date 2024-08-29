
set(SRC 
    engine.cc 
    random_game.cc
    proto_utils.cc
)
# set(SERVER_SRC example_server.cpp)
set(INIT_SRC initializer.cc)
set(MANAGER_SRC managers.cc)

add_executable(eng ${SRC})
# add_executable(eserver ${SERVER_SRC})
add_executable(initializer ${INIT_SRC})
add_executable(managers ${MANAGER_SRC})


target_link_libraries(eng PRIVATE myproto)
# target_link_libraries(eserver PRIVATE myproto)
target_link_libraries(initializer PRIVATE myproto)
target_link_libraries(managers PRIVATE myproto)