#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

#define BUFSIZE 1024

void error(const char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, portnum;
	struct sockaddr_in server_addr;
	struct hostent *server;
	struct in_addr IP;
	

	char buffer[BUFSIZE];
	if (argc < 3) {
	   fprintf(stderr,"Usage: %s hostname port\n", argv[0]);
	   exit(1);
	}
	portnum = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET:Ipv4, SOCK_STREAM:TCP
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR host not found\n");
		exit(1);
	}
	printf("Host name: %s\n", server->h_name);
	printf("Host address type: %d\n", server->h_addrtype);
	// print all IP address of host
	for(int i=0 ; server->h_addr_list[i]!=NULL ;i++){
		IP.s_addr = *((uint32_t*) server->h_addr_list[i]);
		printf("Host IP address[%d]: ", i);
		// Reverse byte order to show IP address
		for(int j=0;j<4;j++){
			printf("%d.", (IP.s_addr<<(3-j)*8)>>24);
		}
		printf("\n");
//		printf("Host addres[%d]: %s\n", i, inet_ntoa(IP)); // alternative way to show IP
	}
	memset((char *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	memcpy((char *)&server_addr.sin_addr.s_addr,
			(char *)server->h_addr,
			server->h_length);
	server_addr.sin_port = htons(portnum);// htons reverse byte's order
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
		error("ERROR connecting to socket");
	printf("Enter your message: ");
	memset(buffer, 0, BUFSIZE);
	fgets(buffer, BUFSIZE-1, stdin); // communication can wait here
	if (write(sockfd, buffer, strlen(buffer)) < 0) 
		 error("ERROR writing to socket");
	memset(buffer, 0, BUFSIZE);
	if (read(sockfd, buffer, BUFSIZE-1) < 0) 
		 error("ERROR reading from socket");
	printf("%s\n",buffer);
	close(sockfd);
	
	return 0;
	
}