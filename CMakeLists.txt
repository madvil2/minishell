cmake_minimum_required(VERSION 3.28)
project(minishell C)

set(CMAKE_C_STANDARD 17)

add_executable(minishell srcs/parsing/main.c
        srcs/minishell.c)
