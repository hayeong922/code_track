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
#include <dirent.h>

#define MAXBUFSIZE  100 //���� ������
// #define BUFSIZE  2048 //���� ������

#define FILENAME 100

struct packet_header{
    char command[FILENAME];
    char filename[FILENAME];
    int filesize;
};

int main(int argc, char *argv[]) {
    int sock; 
    int nbytes;
    unsigned int remote_length;         //length of the sockaddr_in structure
    char buffer[MAXBUFSIZE];
    int left_size;
    char check_command[MAXBUFSIZE];
    int delete_status;                  // variable for deletion
    char delete_file[MAXBUFSIZE];       // file name that will be deleted

    FILE *stream; //���� �����
    // for ls command
    // DIR *dir;
    // struct dirent *ent;
    // if((dir = opendir("udp_server")!= NULL){
    //     while(())
    // })
    DIR *d;
    struct dirent *dir;

    struct sockaddr_in sin, remote;
    struct packet_header header;
    // int clnt_addr_size;
    // int addrlen = sizeof(sin);
    int addrlen = sizeof(remote);


    if (argc != 2) {
        printf("usage: %s port\n", argv[0]);
        exit(1);
    }

     // ���� ����
    memset(&sin, 0, addrlen); //bzero((char *)&servaddr,addrlen);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(atoi(argv[1]));
    
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
        // printf("in the loop\n");
        // nbytes = recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
        nbytes = recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, sizeof(remote));


        // printf("filname: %s\n", header.filename);
        // printf("File size: %d\n", header.filesize);
        // printf("command and filename: %s %s\n\n", header.command,header.filename);

        // exit gracefully condition
        // strcat()
        // strcpy(check_command,)
        strcpy(check_command, header.command);
        if(strcmp(check_command,"exit")==0){
            printf("Command Exit\n");
            exit(0);
        }

        if(strcmp(check_command,"delete")==0){
            strcpy(delete_file,header.filename);
            printf("file name that will be deleted %s\n",delete_file);
            delete_status = remove(delete_file);

            if(delete_status == 0){
                printf("%s file deleted successfully.\n",delete_file);
                break;
            }else{
                printf("Cannot find the file with that name.\n");
                // break at this point but after plugging more commands make switch statement
                break;
            }
        }

        if(strcmp(check_command,"ls")==0){
            d = opendir(".");
            if(d){
                while((dir = readdir(d)) != NULL)
                if(dir->d_type == DT_REG){
                    printf("%s\n",dir->d_name);
                }
                closedir(d);
            }
            // tsystem("ls");
            break;
            // or return 0
        }
        // when put
        if(strcmp(check_command,"put")== 0){
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
        // printf("file transmission finished\n");
        printf("file transmission finished and saved\n");
        fclose(stream);
     }

     if(strcmp(check_command,"get")== 0){
        printf("get function called\n");
        printf("get filename %s\n",header.filename);
        strcpy(buffer,header.filename);

        if((stream = fopen(buffer,"rb"))== 0){
            printf("Error");
            exit(1);
        }

        if(stream){
            strcpy(header.filename,buffer);

            fseek(stream, 0, SEEK_END); // end fo file
            header.filesize = ftell(stream);    // current value of the position indicator is returned

            // this part is added
            // strcpy(header.command,command);
            //send file name
            // sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                        
            fseek(stream,0,SEEK_SET);

            printf("sending\n");

            //printf("nbytes %d\n",fread(buffer,1,MAXBUFSIZE,stream));


            while((nbytes = fread(buffer,1,MAXBUFSIZE,stream))!=0)
            {
                int result = sendto(sock,buffer,nbytes,0,(struct sockaddr*)&remote,sizeof(remote));
                printf("nbytes being send: %d\n",nbytes);
                printf("%d\n",result);
            }
            fclose(stream);

        }
        printf("sent file from server\n");
     }

        
    } while (1);
    close(sock);
    return 0;
}
