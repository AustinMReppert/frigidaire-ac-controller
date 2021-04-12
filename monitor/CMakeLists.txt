cmake_minimum_required(VERSION 3.17)
project(ac_proto)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(ac_proto main.cpp)

target_link_libraries(ac_proto ${CONAN_LIBS})