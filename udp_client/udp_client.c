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
#include <errno.h>

// this part added
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
//

#define MAXBUFSIZE 100

//filename part added
#define FILENAME 100
struct packet_header{
    char filename[FILENAME];
    int filesize;
};

//

/* You will have to modify the program below */

int main (int argc, char * argv[])
{

    int nbytes;                             // number of bytes send by sendto()
    int sock;                               //this will be our socket
    char buffer[MAXBUFSIZE];
    int n_size;

    FILE *stream;
    FILE *fp;

    struct sockaddr_in remote;              //"Internet socket address structure"
    struct packet_header header;
    int addrlen = sizeof(remote);

    if (argc < 3)
    {
        printf("USAGE:  <server_ip> <server_port>\n");
        exit(1);
    }

    /******************
      Here we populate a sockaddr_in struct with
      information regarding where we'd like to send our packet
      i.e the Server.
     ******************/
    bzero(&remote,sizeof(remote));               //zero the struct
    remote.sin_family = AF_INET;                 //address family
    remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
    remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

    //Causes the system to create a generic socket of type UDP (datagram)
    if((sock = socket(remote.sin_family,SOCK_DGRAM,0))<0)
    {
        printf("unable to create socket");
    }

    /******************
      sendto() sends immediately.
      it will report an error if the message fails to leave the computer
      however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
     ******************/
//    char command[] = "apple";
    while(1){
        printf("file name(q to quit:)");
        fgets(buffer,MAXBUFSIZE,stdin);
        buffer[strlen(buffer)-1]=0;

        if(!strcmp(buffer,"q"))
            break;

        printf("%s\n",buffer);
        if((fp= fopen(buffer,"r"))== 0){
            printf("Error");
            exit(1);
        }

            strcpy(header.filename, buffer);

//            fseek(fp,0,SEEK_END);
//            header.filesize =ftell(fp);

            printf("send file data\n");

            sendto(sock,&header,sizeof(header),0,(struct sockaddr*)&remote,addrlen);

            //fseek(stream,0,SEEK_SET);

            //nbytes = sendto();
//            while((n_size = fread(buffer,1,MAXBUFSIZE,stream))!= 0)//fgets
            // printf("%s\n", buffer);
            // printf("%s\n", fgets(buffer,MAXBUFSIZE,fp));
            while(fgets(buffer,MAXBUFSIZE,fp))//fgets
            {
                sendto(sock,buffer,sizeof(buffer),0,(struct sockaddr*)&remote,addrlen);
                // sendto(sock,buffer,MAXBUFSIZE,0,(struct sockaddr*)&remote,addrlen);
                printf("Send packet %d bytes\n", sizeof(buffer));
                // printf("%s\n",buffer);

            }
            fclose(fp);
            close(sock);
            break;
    }
    printf("done");
    return 0;

    // this part needs implementation

//    char command[] = "apple";
//    nbytes = **** CALL SENDTO() HERE ****;
//
//    // Blocks till bytes are received
//    struct sockaddr_in from_addr;
//    int addr_length = sizeof(struct sockaddr);
//    bzero(buffer,sizeof(buffer));
//    nbytes = **** CALL RECVFROM() HERE ****;
//
//    printf("Server says %s\n", buffer);
//
//    close(sock);



}

