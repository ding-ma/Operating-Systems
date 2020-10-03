#Assignment 1 OS

## How to run
1. run `cmake .` at the root of this directory
2. run `make` to create the `backend` executable and `frontend` executable


## Notes
The code has been tested and it works on `teaching.cs.mcgill.ca`

* Assume that there are multiple clients connected to the server. If one of them write `shutdown`, it will terminate the server. 
Then, if the other connected client  tries to input a command to the server, their session will terminate. Also, no new clients will be able to connect once the server is shutdown. 

* Apparently we had to do struct passing between the client and server, I knew this a bit too late and I simply wrapped the message within a struct and sent it. Otherwise, I had to modify a lot of my code. 