
set(SRC 
    engine.cc 
    random_game.cc
    proto_utils.cc
)

add_executable(eng ${SRC})
target_link_libraries(eng PRIVATE myproto)
