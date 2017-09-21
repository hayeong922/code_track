//
// Created by Hayeong Song on 9/20/17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define MAXBUFSIZE 100
#define FILENAME 100
#define PORT 5001

struct packet_header{
    char filename[FILENAME];
    int filesize;
};

int main (int argc, char * argv[] )
{

    int sock;                           //This will be our socket
    struct sockaddr_in sin, remote;     //"Internet socket address structure"
    unsigned int remote_length;         //length of the sockaddr_in structure
    int nbytes;                        //number of bytes we receive in our message
    char buffer[MAXBUFSIZE];             //a buffer to store our received message
    int left_size;
    // FILE *stream;
    FILE* file;

    struct packet_header header;
//    int clnt_addr_size;
    int addrlen = sizeof(sin);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    if (argc != 2)
    {
        printf ("USAGE:  <port>\n",argv[0]);
        exit(0);
    }

    /******************
      This code populates the sockaddr_in struct with
      the information about our socket
     ******************/
    bzero(&sin,sizeof(sin));                    //zero the struct
    sin.sin_family = AF_INET;                   //address family
    sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
    sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine
    //Causes the system to create a generic socket of type UDP (datagram)
    // maybe later PF_INET
    if ((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        printf("unable to create socket");
    }

    /******************
      Once we've created a socket, we must bind that socket to the
      local address and port we've supplied in the sockaddr_in struct
     ******************/
    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        printf("unable to bind socket\n");
    }

    do{
        printf("waiting on port %d\n",PORT);
        remote_length = sizeof(remote);

        nbytes = recvfrom(sock,&header,sizeof(header),0,(struct sockaddr*)&remote,&remote_length);
// 
        file = fopen(header.filename, "w");
        left_size = header.filesize;    

        // printf("Receive file: %s\n", header.filename);
        printf("File size: %d\n", header.filesize);

        if(nbytes > 0){
            // stream = fopen(header.filename,"W+b");
            // left_size = header.filesize;
            
            printf("Receive file: %s \n",header.filename);

            do{
            //     setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(struct timeval*)&tv,sizeof(struct timeval));

               // nbytes = recvfrom(sock,buffer,MAXBUFSIZE,0,(struct sockaddr*)&remote,&remote_length);
                nbytes = recvfrom(sock,buffer,sizeof(buffer),0,(struct sockaddr*)&remote,&remote_length);


                // if(nbytes == -1){
                //     printf("Time-out\n\n");
                //     exit(1);
                // }
                // if(nbytes>0)
                printf("%s\n",buffer);
                // printf("Receive %d bytes\n", nbytes);
                fwrite(buffer,1,nbytes,file);
                // left_size -= nbytes;

                // if(left_size <=0)
                //     break;

            }while(1);
            printf("file transmitted\n");
            fclose(file);
            close(sock);
        }

    }while(1);


//    remote_length = sizeof(remote);
//
//    //waits for an incoming message
//    bzero(buffer,sizeof(buffer));
//    nbytes = nbytes = **** CALL RECVFROM() HERE ****;
//
//    printf("The client says %s\n", buffer);
//
//    char msg[] = "orange";
//    nbytes = **** CALL SENDTO() HERE ****;
//
//    close(sock);
}

