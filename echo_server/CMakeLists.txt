cmake_minimum_required(VERSION 3.9)
project(echo_server)

find_package(cpprestsdk REQUIRED)

add_executable(echo_server main.cpp)
target_link_libraries(echo_server PRIVATE cpprestsdk::cpprest)
