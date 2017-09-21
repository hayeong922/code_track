// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>

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

#define MAXBUFSIZE  100 
// #define BUFSIZE  2048 

#define FILENAME 100

struct packet_header{
    char filename[FILENAME];
    int filesize;
};

int main(int argc, char *argv[]) {
    int sock;                           //this is the socket
    int nbytes;                         // number of bytes we receive in out message
    unsigned int remote_length;         //length of the sockaddr_in structure
    char buffer[MAXBUFSIZE];            // a buffer to stroe our received message
    int left_size;

    FILE *stream; 

    struct sockaddr_in sin, remote;     //"Internet socket address structure"
    struct packet_header header;
    // int clnt_addr_size;
    int addrlen = sizeof(sin);

    if (argc != 2) {
        printf("usage: <port>\n");
        exit(1);
    }

    /******************
      This code populates the sockaddr_in struct with
      the information about our socket
     ******************/
    bzero(&sin,sizeof(sin));                    // zero the struct
    sin.sin_family = AF_INET;                   // address family
    sin.sin_addr.s_addr = htonl(INADDR_ANY);    // supplies the IP address of the local machine    
    sin.sin_port = htons(atoi(argv[1]));        // htons() sets the port # to network byte order

    // causes the sustem to create a generic socket of type UDP(datagram)
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket fail");
        exit(0);
    }

    /******************
      Once we've created a socket, we must bind that socket to the 
      local address and port we've supplied in the sockaddr_in struct
     ******************/
    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind fail");
        exit(0);
    }

    remote_length = sizeof(remote);

    // waits for an incoming message
    bzero(buffer,sizeof(buffer));
    nbytes = recvfrom(sock,buffer,MAXBUFSIZE,0,(struct sockaddre*)&remote,&remote_length);

    printf("The client says %s\n",buffer);

    // char msg[] = 'orange';
    // nbytes = sendto()

    // close(sock);

    // file transmission part
    bzero(buffer,sizeof(buffer));
    do{
        //���� �̸�
        remote_length = sizeof(remote);
        nbytes = recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);

        printf("filname: %s\n", header.filename);
        printf("File size: %d\n", header.filesize);

        if ((stream = fopen(header.filename, "w+b")) == NULL){
            printf("File open Error");
            exit(1);
        }
        left_size = header.filesize;

        //file �̸� ���� �޾�����, fileũ�� ���۹޾Ҵٸ�
        //file ���� ���� �ޱ�
        do{
            //���� ���� �޽��� nbyte ����,
            nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);
            fwrite(buffer, 1, nbytes, stream);
            left_size -= nbytes;

            if (left_size <= 0)
                break;
            if (nbytes < 0) {
                perror("recvfrom fail");
                exit(1);
            }
        } while (1);
        printf("file transmission finished and saved in server\n");
        fclose(stream);
    } while (1);
    close(sock);
    return 0;
}
