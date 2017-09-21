// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>

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

#define MAXBUFSIZE  100 
// #define BUFSIZE  1024 

#define FILENAME 100

struct packet_header{
    char filename[FILENAME];
    int filesize;
};

int main(int argc, char *argv[]) {
    int sock;            //socket
    int nbytes;         // number og bytes send by sendto
    char buffer[MAXBUFSIZE];
    // int n_size;

    FILE *stream; //file name

    struct sockaddr_in remote;  // Internet socket address structure
    struct packet_header header;
    int addrlen = sizeof(remote); 

    
    if (argc < 3) {
        printf("usage: %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    /******************
      Here we populate a sockaddr_in struct with
      information regarding where we'd like to send our packet 
      i.e the Server.
     ******************/    
    bzero(&remote,sizeof(remote));                 //zero the struct
    remote.sin_family = AF_INET;                    //address family
    remote.sin_addr.s_addr = inet_addr(argv[1]);    //sets remote IP address
    remote.sin_port = htons(atoi(argv[2]));         //sets port to network byter order

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("unable to crate socket");
    }

    char command[] = "put";

    while (1){
        printf("put filename: ");
        fgets(buffer, MAXBUFSIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;
        printf("client buffer: %s\n",buffer);

        if (!strcmp(buffer, "q"))
            break;

        if ((stream = fopen(buffer, "rb")) == 0){
            printf("Error");
            exit(1);
        }

        /******************
          sendto() sends immediately.  
          it will report an error if the message fails to leave the computer
          however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
         ******************/
        
        if (stream)
        {
            strcpy(header.filename, buffer);

            fseek(stream, 0, SEEK_END); //������ ��
            header.filesize = ftell(stream);

            printf("send header(filename, filesize)\n");

            //�����̸��� ���� ������ ������(header)
            sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

            fseek(stream, 0, SEEK_SET); //������ ����

            while ((nbytes = fread(buffer,1,MAXBUFSIZE,stream)) != 0)
            {
                sendto(sock, buffer, nbytes, 0, (struct sockaddr*)&remote, addrlen);
            }
            fclose(stream);
            close(sock); //socket close
            break;
        }
        
        // printf("send file name\n");

        // // clear buffer and get the file if necessry?
        // struct sockaddr_in from_addr;
        // int addr_length = sizeof(struct sockaddr);
        // bezero(buffers,sizeof(buffer));

        // nbytes =recvfrom


        printf("send file data\n");
    }


    return 0;
}
