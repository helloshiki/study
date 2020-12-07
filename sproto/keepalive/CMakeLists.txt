project(keepalive)

add_executable(keepalive main.cpp)
#target_link_libraries(shared_const_buffer PRIVATE fmt::fmt)
target_link_libraries(spb PRIVATE spdlog::spdlog sproto_static)