cmake_minimum_required(VERSION 3.17)
project(custom_sink)

add_executable(custom_sink main.cpp)
#target_link_libraries(shared_const_buffer PRIVATE fmt::fmt)
#target_link_libraries(custom_sink PRIVATE spdlog::spdlog)   # bigger, faster

