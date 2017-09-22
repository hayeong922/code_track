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

char glob_cmd[MAXBUFSIZE];
char glob_filename[MAXBUFSIZE];

struct packet_header{
    char command[FILENAME];
    char filename[FILENAME];
    int filesize;
};

void split_func(char *s){
    printf("token start\n");
    char *token = strtok(s," ");

    int count = 0;
    while(token != NULL){
        if(count == 0){
            strcpy(glob_cmd,token);
        }else{
                strcpy(glob_filename, token);
        }
        printf("%s\n",token);
        token = strtok(NULL," ");
        count++;
    }
}

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

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("unable to crate socket");
    }

    // char command[] = "put";
    char command[] = "delete";

    while (1){
        printf("Put command(and filename):");
        fgets(buffer, MAXBUFSIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;

        // take the input and split by spacce and assign command file name to global var
        split_func(buffer);
        
        strcpy(buffer,glob_filename);
        strcpy(command,glob_cmd);
        printf("command and file name %s, %s\n",glob_cmd, glob_filename);
        printf("buffer %s after assignment\n",buffer);

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

            fseek(stream, 0, SEEK_END); // end fo file
            header.filesize = ftell(stream);    // current value of the position indicator is returned

            // this part is added
            strcpy(header.command,command);

            printf("send header(command,filename, filesize)\n");
            // printf("send header(filename, filesize)\n");

            //send(header)
            sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

            fseek(stream, 0, SEEK_SET); //seek beginning of the file

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