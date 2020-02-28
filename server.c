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
	fprintf( stderr, "%s\n\t%s\n", msg, strerror( errno ) );
	exit( exitCode );
}

// The user entered something stupid. Tell him.
void usage( char *argv0 )
{
	printf( "usage : %s portnumber\n", argv0 );
	exit( 0 );
}

int main(int argc, char **argv)
{

	int commsock, len;                                                                                              //initialisze listensocket, communication socket
	int listensock = 0;
	struct sockaddr_in serv_addr, client_addr;                                                                      //new sockaddr_in serv_addr (server address), client_addr
	socklen_t addrLen = sizeof(struct sockaddr_in);                                                                 //length of struct sockaddr_in
	char revBuff[BUF_LEN];                                                                                          //buffer to recieve and send data

	// Check for right number of arguments
	if ( argc < 2 ) usage( argv[0] );

	if ((listensock = socket(AF_INET, SOCK_STREAM, 0))==-1){                                                        //create listensocket --> AF_INET(address family (int this case ipv4)
                                                                                                                                        //SOCK_STREAM --> connection based protocol, two parties --> tcp
        sysErr("Server Fault: SOCKET", -1);
	}
    //printf("[SERVER]: SOCKET successfull: %i\n",listensock);                                                      //Debug: print socketnumber


	// Set params so that we receive IPv4 packets from anyone on the specified port
	memset( &serv_addr, 0, addrLen );                                                                               //fill memory block with sockaddr_in serv_addr with length of it
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);                                                                  //accept all connections --> ip: 0.0.0.0
	serv_addr.sin_family      = AF_INET;                                                                            //fill address family, in this case ipv4
	serv_addr.sin_port        = htons( (u_short)atoi( argv[ 1 ] ) );                                                //fill port as the network-byte-order prefers --> this case 34343

	if (bind(listensock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1){                                    //bind to address 0.0.0.0
        sysErr("Server Fault: BIND",-2);                                                                            //if failed --> print it to user
	}
    //printf("[SERVER]: BIND successful to address: %s\n", inet_ntoa(serv_addr.sin_addr));                           //Debug: print that bind was successful


	if (listen(listensock, 10)==-1){                                                                                //listen on listensocket to new connetion wishes, queue size 10
        sysErr("Server Fault: LISTEN", -3);                                                                         //if failed --> print to user
	}
    //printf("[SERVER]: LISTEN successful \n");                                                                     //Debug: print that listen was successful


	while(true){
        //printf("[SERVER]: Begin while\n");

        if ((commsock = accept(listensock, (struct sockaddr*)&client_addr, &addrLen)) == -1){                       //create communication socket, used to connection with client
            sysErr("Server Fault: ACCEPT", -2);                                                                     // if failed --> print it
        }
        //printf("[SERVER]: ACCEPT successful (established connection to ip address: %s)\n", inet_ntoa(client_addr.sin_addr));      //Debug: print that client was connected + client ip address



        memset(revBuff, 0, BUF_LEN);                                                                                //fill memory block with buffer and length of it
        if((len = recvfrom( commsock, revBuff, BUF_LEN-1, 0, (struct sockaddr *) &client_addr, &addrLen)) == (size_t) -1){      //len = length recieved data from client
          //len = recvfrom(communication socket, save recieved data to revBuff, length of data, flags, struct of client, length of struct
            sysErr( "Server Fault : RECVFROM", -5 );                                                                //failed? --> print it
        }
        write(commsock, revBuff, len);                                                                              //send recieved data to user back (echo!)
        //printf("[SERVER]: Got it and sent!: %s\n", revBuff);                                                      //Debug: print, that recieved and sent back to client
        close(commsock);                                                                                            //close communication socket
	}

    close(listensock);                                                                                              //close listen socket
	return 0;
}
