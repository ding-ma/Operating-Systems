## Assignment 2

To compile the program run the following commands
1. `cmake .` at the root of the library
1. `make`
This will create the executables for the tests 1 to 6. It will also generate `sut.a` and `socket.a`. 
These are the two library files if other tests wants to be ran.

### To compile custom tests
####First option
1. `gcc -o EXE_NAME YOUR_TEST  sut.a socket.a -pthread`
1. `./EXE_NAME`

####Second option
1. `gcc YOUR_TEST  sut.a socket.a -pthread`. 
1. `./a.out`

The `pthread` flag will tell the compiler to add multithreading with the pthread library.

### Some notes
* In order to run `test4`, `test5`, or `test6`, the `server` needs to be running. 
In order to simulate a better `read()` from the sut library, the sends message a message randomly between 1 and 10s interval.
The `recv_message` method will need to be commented out if we are only testing `read()`. 
The `send_message` method will need to be commented out if we are only testing `write()`.
See `small_server.c` code for more details on what to comment out.

* The host is `0.0.0.0` and the port is `6969` for the server.

