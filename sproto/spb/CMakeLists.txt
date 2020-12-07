project(spb)

add_executable(spb main.cpp packet.cpp packet.h)
#target_link_libraries(shared_const_buffer PRIVATE fmt::fmt)
target_link_libraries(spb PRIVATE spdlog::spdlog sproto_static)   # bigger, faster
add_dependencies(spb sproto_static)