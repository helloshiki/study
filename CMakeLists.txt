cmake_minimum_required(VERSION 3.17)
project(study)

set(CMAKE_CXX_STANDARD 14)
include_directories(
        pkgs/spdlog/include
        pkgs/fmt/include
        pkgs/cxxopts/include
        pkgs/asio/include
        gits/sproto/
#        pkgs/websocketpp
        #        vmroot/include
)

#link_directories(
#        vmroot/lib
#)

#add_definitions(-DSPDLOG_FMT_EXTERNAL)

add_subdirectory(pkgs/spdlog)
add_subdirectory(pkgs/fmt)
add_subdirectory(asio)
add_subdirectory(gits)
add_subdirectory(sproto)