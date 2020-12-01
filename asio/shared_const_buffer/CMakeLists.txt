cmake_minimum_required(VERSION 3.17)
project(shared_const_buffer)

add_executable(shared_const_buffer main.cpp)

#target_link_libraries(shared_const_buffer PRIVATE fmt::fmt-header-only)
target_link_libraries(shared_const_buffer PRIVATE fmt::fmt)
target_link_libraries(shared_const_buffer PRIVATE spdlog::spdlog)   # bigger, faster

