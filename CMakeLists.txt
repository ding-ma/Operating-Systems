cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

add_library(
        a2_sut
        Assignment-2/sut.c
        Assignment-2/sut.h
)
# there is most likely a better way of doing this in order to generate the executables
add_executable(test1 Assignment-2/test1.c)
target_link_libraries(test1 PRIVATE
        a2_sut
        pthread
        )
target_include_directories(test1 PRIVATE Assignment-2/queue.h)

add_executable(test2 Assignment-2/test2.c)
target_link_libraries(test2 PRIVATE
        a2_sut
        pthread
        )
target_include_directories(test2 PRIVATE Assignment-2/queue.h)

add_executable(test3 Assignment-2/test3.c)
target_link_libraries(test3 PRIVATE
        a2_sut
        pthread
        )
target_include_directories(test3 PRIVATE Assignment-2/queue.h)