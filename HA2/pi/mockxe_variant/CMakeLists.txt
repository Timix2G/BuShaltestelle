cmake_minimum_required(VERSION 3.0)

project(pi_thread)

add_executable(pi_thread pi_thread.c)
target_compile_options(${PROJECT_NAME} PRIVATE
        -std=c17 -Wall -Wextra -Wpedantic -lpthreads -lm
)