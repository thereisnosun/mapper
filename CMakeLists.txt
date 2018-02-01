cmake_minimum_required(VERSION 2.8.9)
set (CMAKE_CXX_STANDARD 11)
project(mapper)
include_directories(3rd-party/libdwarf/libdwarf/ 3rd-party/libdwarf/_Release/libdwarf/)
add_executable(mapper src/Mapper.cpp src/main.cpp)
find_library(BOOST_PROGRAM_OPT libboost_program_options.a HINTS /usr/lib/x86_64-linux-gnu/)
target_link_libraries(mapper libdwarf.a libelf.so.1 z ${BOOST_PROGRAM_OPT})



