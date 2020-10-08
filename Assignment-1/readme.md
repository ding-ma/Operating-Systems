#Assignment 1 OS

## How to run
1. run `cmake .` at the root of this directory
2. run `make` to create the `backend` executable and `frontend` executable


## Notes
The code has been tested and it works on `teaching.cs.mcgill.ca`

* message struct is declared in `a1_lib.h` in order to both the server and client to know about the format. 
* After a client write shutdown, the server will wait for others to finish. If another client tires to connect, it will accept it then close it.