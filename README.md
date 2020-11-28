# Operating System Class (ECSE 427)

## Assignments
* Assignment 1: RCP Server and client 
* Assignment 2: Simple threading scheduler library
* Assignment 3: Simple memory allocator library (malloc, free, and realloc functions)

Each of the assignments contains their own `CMakeLists.txt`. The one located at the root of this directory is used by CLion.

## Compile with Docker
Docker can be used to set up as localhost host to compile. The image has been set to mimic Trottier Computers. 
* Run `docker-compse up` in the root of this directory
* To connect via ssh: `ssh user@localhost -p3333` with password `password`
* You can setup a remote toolchain on Clion and compile automatically to the container
* The container will automatically restart on system boot-up, that option can be disabled in the `docker-compose.yml` by replacing `restart: always` to `restart: no`

Compile `sanityCheck.c` using different toolchains, you should see different name and hostnames.


## About copyrights 
If this helped you in your assignment/work, give it a star and make sure to cite it. 