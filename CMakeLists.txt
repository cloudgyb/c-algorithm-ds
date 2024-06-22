cmake_minimum_required(VERSION 3.28)
project(c-algorithm-ds C)

set(CMAKE_C_STANDARD 11)

add_executable(c-algorithm-ds src/main.c
        src/queue/blocking_queue.c
        src/queue/blocking_queue.h
        src/queue/blocking_queue_test.c
        src/queue/blocking_queue_test.h)

target_link_libraries(c-algorithm-ds pthread)
