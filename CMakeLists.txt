cmake_minimum_required(VERSION 3.10)
project(ECSE427 C)

set(CMAKE_C_STANDARD 99)

# there is most likely a better way of doing this in order to generate the executables
set(a2_lib PRIVATE
        a2_sut
        pthread
        )

set(a2_queue PRIVATE Assignment-2/queue.h)

add_library(
        a2_sut
        Assignment-2/sut.c
        Assignment-2/sut.h
)

add_executable(test1 Assignment-2/test1.c)
target_link_libraries(test1 ${a2_lib})
target_include_directories(test1 ${a2_queue})

add_executable(test2 Assignment-2/test2.c)
target_link_libraries(test2 ${a2_lib})
target_include_directories(test2 ${a2_queue})

add_executable(test3 Assignment-2/test3.c)
target_link_libraries(test3 ${a2_lib})
target_include_directories(test3 ${a2_queue})

add_executable(test4 Assignment-2/test4.c)
target_link_libraries(test4 ${a2_lib})
target_include_directories(test4 ${a2_queue})

add_executable(test5 Assignment-2/test5.c)
target_link_libraries(test5 ${a2_lib})
target_include_directories(test5 ${a2_queue})