/* TCP server */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 1024
#define BACKLOG 5

void error(const char *msg){
	perror(msg); // For error that affects errno(error number)
	exit(1);
}

void communicate(int sockfd, char *buffer, int cli_num){
	int n;
	
	memset(buffer, 0, BUFSIZE);
	n = read(sockfd, buffer, BUFSIZE-1);// n is number of char if it is valid reading, including last '\0'
	if (n < 0) error("ERROR reading from socket");
//	printf("n: %d\n", n);
	printf("Client %d message: %s\n", cli_num, buffer);
	char reply[] = "Message received.";
	n = write(sockfd, reply, strlen(reply));
	if (n < 0) error("ERROR writing to socket");
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portnum;
	socklen_t clilen;
	char buffer[BUFSIZE];
	struct sockaddr_in server_addr, client_addr;
	
	if (argc < 2) {
		fprintf(stderr,"Usage: %s port\n", argv[0]);// error not affect errno
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);/* create socket, AF_INET:internet domain, 
	SOCK_STREAM use TCP, zero for third arg allow OS to choose proper protocols.*/
	if (sockfd < 0) 
		error("ERROR opening socket");
	memset((char *)&server_addr, 0, sizeof(server_addr));
	portnum = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; //the address of host machine
	server_addr.sin_port = htons(portnum);
	/* bind ask the kernel to associate the server’s socket address with a socket descriptor 
	server_addr should be casted to struct sockaddr. There are other type of socket 
	other than internet socket. It is a C-style polymorphism.
	*/
	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
		error("ERROR binding socket");
	/* 
	Socket descriptor from socket() is an active socket that should be on the client end.
	listen() function tell the kernel that it will be used by a server.
	Backlog, the number of outstanding connection requests that the kernel 
	should queue up before starting to refuse requests*/
	listen(sockfd, BACKLOG);
	clilen = sizeof(client_addr);
	int ctr = 0; // number of forked process, also indicate number of client requests
	int pid = 1; // initialize, allow parent process to fork
	while(1){
		newsockfd = accept(sockfd, 
						 (struct sockaddr *) &client_addr, 
						 &clilen); /* wait for new client connection request. 
						Note that third arg should be a pointer*/
		if (newsockfd < 0) 
			error("ERROR accepting socket");
			
	    printf("Established.\n");
		if(pid > 0) {
			pid = fork();
			ctr++;
		}
		/* parent process only fork child process. Must place after new socket connection accepted(triggered by client request). 
		If the other way around child process not copy newsockfd, it can complete with 
		its parent process to accept the socket. It child gets it, request gets done. 
		But if parent gets it, it won't do the communication, leading to fail request.
		*/
		if(pid == 0){ // Child processes do communication, then exit successfully
//			printf("Child, Loop num: %d, pid: %d\n", ctr++, pid); // debug
			communicate(newsockfd, buffer, ctr);
			close(newsockfd);
			close(sockfd);
			exit(0); // terminate after use, or processes are too many
		}else{ 
		/* pid>0 parent process doesn't deal with actual communication. 
		Since it creates a child process and copy its status, it bahaves as if it does.
		Why parent process cannot deal with communication? When both parent and child process 
		are in communication, another client request will have to wait for either of them to finish, 
		since no process can accept new socket.
		*/
//			printf("Parent, Loop num: %d, pid: %d\n", ctr++, pid); // debug
			close(newsockfd);
		}

	}
	return 0; 
	
}