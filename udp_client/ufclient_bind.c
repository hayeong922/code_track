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

#define MAXBUFSIZE  100 //���� ������
// #define BUFSIZE  1024 //���� ������

#define FILENAME 100

struct packet_header{
    char command[FILENAME];
    char filename[FILENAME];
    int filesize;
};

int main(int argc, char *argv[]) {
    int sock; //socket
    int nbytes;
    char buffer[MAXBUFSIZE];
    // int n_size;

    FILE *stream; //���� �����

    struct sockaddr_in sin,remote;
    struct packet_header header;
    int addrlen = sizeof(remote); //���� �ּ��� size�� ����
    unsigned int remote_length;
    int left_size;

    //./fclient.c ip�ּ�, ��Ʈ��ȣ
    // if (argc != 3) {
    if (argc < 3) {
        printf("usage: %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    /******************
      Here we populate a sockaddr_in struct with
      information regarding where we'd like to send our packet 
      i.e the Server.
     ******************/    
    bzero(&remote,sizeof(remote)); //bzero((char *)&servaddr, sizeof(servaddr));
    remote.sin_family = AF_INET; //���ͳ� Addr Family
    remote.sin_addr.s_addr = inet_addr(argv[1]); //argv[1]���� �ּҸ� ������
    remote.sin_port = htons(atoi(argv[2])); //argv[2]���� port�� ������

     bzero(&sin,sizeof(sin)); //bzero((char *)&servaddr, sizeof(servaddr));
    sin.sin_family = AF_INET; //���ͳ� Addr Family
    sin.sin_addr.s_addr = htonl(INADDR_ANY); //argv[1]���� �ּҸ� ������
    sin.sin_port = htons(atoi("5002"));  //argv[2]���� port�� ������
    // sin.sin_port = htons(5002); //argv[2]���� port�� ������


   //socket ���� 0���� ������ Error
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("unable to crate socket");
    }

    if(bind(sock,(struct sockaddr *)&sin,sizeof(sin))<0){
        perror("bind fail");
        exit(0);
    }

    // char command[] = "put";
    char command[] = "get";

    while (1){
        printf("file name to send(q to quit): ");
        fgets(buffer, MAXBUFSIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;

        // un comment this later

        // if (!strcmp(buffer, "q"))
        //     break;

        // if ((stream = fopen(buffer, "rb")) == 0){
        //     printf("Error");
        //     exit(1);
        // }

        // stream ���� ���� �б�
        // if (stream)
        // {
        //     strcpy(header.filename, buffer);

        //     fseek(stream, 0, SEEK_END); // end fo file
        //     header.filesize = ftell(stream);    // current value of the position indicator is returned

        //     // this part is added
        //     strcpy(header.command,command);

        //     printf("send header(command,filename, filesize)\n");
        //     // printf("send header(filename, filesize)\n");

        //     //send(header)
        //     sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

        //     fseek(stream, 0, SEEK_SET); //seek beginning of the file

        //     while ((nbytes = fread(buffer,1,MAXBUFSIZE,stream)) != 0)
        //     {
        //         sendto(sock, buffer, nbytes, 0, (struct sockaddr*)&remote, addrlen);
        //     }
        //     fclose(stream);
        //     close(sock); //socket close
        //     break;
        // }
        // printf("send file data\n");

        // strcpy(header.filename, buffer);

        // fseek(stream, 0, SEEK_END); // end fo file
        // header.filesize = ftell(stream);    // current value of the position indicator is returned

        // // this part is added
        // strcpy(header.command,command);

        printf("send header(command,filename, filesize)\n");
        // printf("send header(filename, filesize)\n");

        //send(header)
        strcpy(header.command,"get");
        strcpy(header.filename,"foo1");
        header.filesize = 100;
        sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

        //recv file name
        printf("filname: %s\n", header.filename);
        printf("File size: %d\n", header.filesize);
        printf("command and filename: %s %s\n", header.command,header.filename);

        //recv file and write
        if ((stream = fopen(header.filename, "w+b")) == NULL){
            printf("File open Error");
            exit(1);
        }
        
        left_size = header.filesize;

        //file �̸� ���� �޾�����, fileũ�� ���۹޾Ҵٸ�
        //file ���� ���� �ޱ�
        do{
            printf("received loop\n");
            nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);
            fwrite(buffer, 1, nbytes, stream);
            printf("recevied %d\n",MAXBUFSIZE);
            left_size -= nbytes;

            if (left_size <= 0)
                break;
            if (nbytes < 0) {
                perror("recvfrom fail");
                exit(1);
            }
        } while (1);
        // printf("file transmission finished\n");
        printf("file transmission finished and saved\n");
        fclose(stream);
     }



        return 0;
    }
    // return 0;
// }
