cmake_minimum_required(VERSION 3.10)
project(mayocc C)

set(CMAKE_C_STANDARD 11)

add_executable(mayocc mayocc.c)

enable_testing()

add_test(
        NAME test
        COMMAND ../test.sh
)