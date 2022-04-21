cmake_minimum_required(VERSION 3.14)
project(HW4 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(HW4
        compiler.h
        driver.c
        lex.c
        parser2.c
        vm.c)
