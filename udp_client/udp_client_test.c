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
    char filename[FILENAME];
    int filesize;
};

int main(int argc, char *argv[]) {
    int sock; //socket
    int nbytes;
    char buffer[MAXBUFSIZE];
    // int n_size;

    FILE *stream; //���� �����

    struct sockaddr_in remote;
    struct packet_header header;
    int addrlen = sizeof(remote); //���� �ּ��� size�� ����

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

   //socket ���� 0���� ������ Error
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("unable to crate socket");
    }

    while (1){
        printf("file name to send(q to quit): ");
        fgets(buffer, MAXBUFSIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;

        if (!strcmp(buffer, "q"))
            break;

        if ((stream = fopen(buffer, "rb")) == 0){
            printf("Error");
            exit(1);
        }

        //stream ���� ���� �б�
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
        printf("send file data\n");
    }
    return 0;
}
