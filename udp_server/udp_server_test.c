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

#define MAXBUFSIZE  100 //���� ������
// #define BUFSIZE  2048 //���� ������

#define FILENAME 100

struct packet_header{
    char filename[FILENAME];
    int filesize;
};

int main(int argc, char *argv[]) {
    int sock; 
    int nbytes;
    unsigned int remote_length;         //length of the sockaddr_in structure
    char buffer[MAXBUFSIZE];
    int left_size;

    FILE *stream; //���� �����

    struct sockaddr_in sin, remote;
    struct packet_header header;
    // int clnt_addr_size;
    int addrlen = sizeof(sin);

    if (argc != 2) {
        printf("usage: %s port\n", argv[0]);
        exit(1);
    }

     // ���� ����
    memset(&sin, 0, addrlen); //bzero((char *)&servaddr,addrlen);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(atoi(argv[1])); //argv[1]���� port ��ȣ ������ ��

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket fail");
        exit(0);
    }

    // ���� ���� �ּҷ� bind()
    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind fail");
        exit(0);
    }

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
        printf("file transmission finished\n");
        fclose(stream);
    } while (1);
    close(sock);
    return 0;
}