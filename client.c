#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

const size_t BUF_LEN = 128;

// Something unexpected happened. Report error and terminate.
void sysErr( char *msg, int exitCode )
{
	fprintf( stderr,"%s\n\t%s\n", msg, strerror(errno));
	exit(exitCode);
}

// The user entered something stupid. Tell him.
void usage( char *argv0 )
{
	printf( "usage : %s server-ip portnumber\n", argv0 );
	exit( 0 );
}

int main(int argc, char **argv)
{
	int sockfd;                                                                                 //initialize socket,
	socklen_t addrLen = sizeof(struct sockaddr_in);                                             //length of sockaddr_in as addrLen
	struct sockaddr_in serv_addr;                                                               //new sockaddr_in as serv_addr -->server address, port...
	char sendBuff[BUF_LEN], recvBuff[BUF_LEN];                                                  //send and recieve buffer --> text which sent or recieved
	//sendBuff[BUF_LEN-1] = 0;

	// Check for right number of arguments
	if ( argc < 3 ) usage( argv[0] );                                                           //if not enough arguments print to user

	// Fill struct addr with IP addr, port and protocol family
	memset( &serv_addr, 0, addrLen );                                                           //fill memory block with sockaddr_in serv_addr with length of it
	if ( ( serv_addr.sin_addr.s_addr = inet_addr( argv[ 1 ] ) ) == INADDR_NONE ) {              //Fill server-address with refered argument
		sysErr( "Client Fault: NOT_AN_IP", -1 );                                                // if no address or other error than print error
	}
    //printf("[CLIENT]: IP successful\n");                                                      //Debug: print, that ip was successful

	serv_addr.sin_family = AF_INET;                                                             //fill address family, in this case ipv4
	serv_addr.sin_port = htons( (u_short)atoi( argv[ 2 ] ) );                                   //fill port as the network-byte-order prefers

	// Create new socket based on the values from
	if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {                               //create socket, SOCK_STREAM --> connection based protocol, two parties --> tcp
		sysErr( "Client Fault: SOCKET", -2 );                                                   //if error --> print that
	}
    //printf("[CLIENT]: SOCKET successful: %i\n", sockfd);                                      //Debug: print that socket created


	if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){                 //connect to server
        sysErr("Client Fault: CONNECT",-2);                                                     //if this failed --> print to user
	}
    //printf("[CLIENT]: Connection established successful\n");                                  //Debug: print that connection established


	printf("Enter String to send on Server: ");
	fgets(sendBuff,BUF_LEN-1,stdin);                                                            //fill sendBuff with user input
	printf("Sending...     : %s\n", sendBuff);
	if ( sendto( sockfd, sendBuff, strlen(sendBuff), 0, (struct sockaddr *) &serv_addr, addrLen ) ==  -1 ) {    //send message to server, sendBuff: user input,
       //sendto(socket, data_to_send, length of data, flags, sockaddr --> to, sockaddr length)
			sysErr( "Client Fault: SENDTO", -4 );                                               // if this failed --> print it to user
	}
    //printf("[CLIENT]: SEND successful %s\n", sendBuff);                                       //Debug: print that message was sent


    if(write(STDOUT_FILENO, recvBuff, read(sockfd, recvBuff, sizeof(recvBuff)-1)) == -1){       //answer from server read() will be printed at STDOUT
        printf("Client Fault: MESSAGE\n");
    }

	close( sockfd );                                                                            //close socket
	return 0;
}
