How to run:
- First run the server with:  
gcc server.c   
./a.out  
  
- Next, run the client with:  
gcc client.c  
./a.out  


This assignment is supposed to make a mini version of networking where
a server and a client communicate with each other in order for the 
client to be able to download some files from the native directory of
the server to its own.

I have handled the case where file doesnot exist in server it prints error message.

Multiple files can be transfered at once.

Percentage is also printed of how much file is received in client.
