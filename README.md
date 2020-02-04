This is a simple implementation of TCP client and server using socket in C.

Clone to local machine, then build by following commands.
```
gcc client.c -o client
gcc server.c -o server
```
Open one terminal for server, provide port number and run:
```
./server port
```
Open one or more terminal for clients, run:
```
./client hostname port
```
hostname is the name of local machine, it can be found by run command
```
$ hostname
```
Follow the message and have fun.
