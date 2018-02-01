cmake_minimum_required(VERSION 2.8.9)
set (CMAKE_CXX_STANDARD 11)
project(mapper)
include_directories("3rd-party/libdwarf/libdwarf/")
add_executable(mapper mapper.cpp main.cpp)
target_link_libraries(mapper libdwarf.a)
target_link_libraries(mapper libelf.so.1)
target_link_libraries(mapper z)
find_library(BOOST_PROGRAM_OPT libboost_program_options.a HINTS /usr/lib/x86_64-linux-gnu/)
target_link_libraries(mapper PUBLIC ${BOOST_PROGRAM_OPT})



