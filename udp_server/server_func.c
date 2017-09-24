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

#define MAXBUFSIZE  100 

#define FILENAME 100

#define GET 1
#define PUT 2
#define DELETE 3
#define LS 4
#define EXIT 5

char glob_cmd[MAXBUFSIZE];
char glob_filename[MAXBUFSIZE];

struct packet_header{
    char command[FILENAME];
    char filename[FILENAME];
    int filesize;
};

int assign_command(char *cmd){
    if(strcmp(cmd,"get")== 0){
        return GET;
    }else if(strcmp(cmd,"put")== 0){
        return PUT;
    }else if(strcmp(cmd,"delete")== 0){
        return DELETE;
    }else if(strcmp(cmd,"ls")== 0){
        return LS;
    }else if(strcmp(cmd,"exit")== 0){
        return EXIT;
    }

}

void run_get(char *filename){

}

void run_put(char *filename, int filesize){
       
}

void run_delete(char *filename){
    printf("file name that will be deleted %s\n",filename);
    int delete_status;
    delete_status = remove(filename);

    if(delete_status == 0){
        printf("%s file deleted successfully.\n",filename);
    }else{
        printf("Cannot find the file with that name.\n");
        // break at this point but after plugging more commands make switch statement
    }    
}

void run_ls(DIR *d, struct dirent *dir){
    d = opendir(".");
    if(d){
        while((dir = readdir(d)) != NULL)
        if(dir->d_type == DT_REG){
            printf("%s\n",dir->d_name);
        }
        closedir(d);
    }
    
}

// void run_exit(char *filename){
//     printf("Command Exit\n");
//     exit(0);
// }

int main(int argc, char *argv[]) {
    int sock; 
    int nbytes;
    unsigned int remote_length;         //length of the sockaddr_in structure
    char buffer[MAXBUFSIZE];
    int left_size;
    char check_command[MAXBUFSIZE];
    int delete_status;                  // variable for deletion
    char delete_file[MAXBUFSIZE];       // file name that will be deleted
    char msg[MAXBUFSIZE];

    FILE *stream; //���� �����

    struct sockaddr_in sin, remote;
    struct packet_header header;
    // int clnt_addr_size;

    // int addrlen = sizeof(sin);
    // this part have been changed, feel free to uncomment it
    int addrlen = sizeof(remote);
    int command_num;
    DIR *d;
    struct dirent *dir;

    if (argc != 2) {
        printf("usage: %s port\n", argv[0]);
        exit(1);
    }

    // 
    memset(&sin, 0, sizeof(sin)); //bzero((char *)&servaddr,addrlen);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(atoi(argv[1])); //argv[1]���� port ��ȣ ������ ��

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket fail");
        exit(0);
    }

    //bind
    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind fail");
        exit(0);
    }

    do{
        remote_length = sizeof(remote);
        // this is the part where I receive command and sometimes file name as well
        nbytes = recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
        printf("\nThe client say: %s %s\n",header.command, header.filename);
        // have to write parameter when there is no file name and just command

        command_num = assign_command(header.command);
        strcpy(glob_filename,header.filename);

        switch(command_num){
            case GET:
                printf("get function called\n");
                strcpy(buffer,glob_filename);
                if ((stream = fopen(buffer, "rb")) == 0){
                        printf("Error");
                        exit(1);
                    }
                if (stream){
                    strcpy(header.filename, buffer);

                    fseek(stream, 0, SEEK_END); // end fo file
                    header.filesize = ftell(stream);    // current value of the position indicator is returned

                    // this part is added
                    strcpy(header.command,"get");

                    // send what command it is?
                    printf("send in stream if\n");
                    sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

                    fseek(stream, 0, SEEK_SET); //seek beginning of the file

                    while ((nbytes = fread(buffer,1,MAXBUFSIZE,stream)) != 0)
                    {
                        sendto(sock, buffer, nbytes, 0, (struct sockaddr*)&remote, addrlen);
                    }
                    // printf("********File sent!!!!!!*****\n");
                    fclose(stream);
                    printf("stream was just closed and is done transmitting\n");
                    // added to check
                    strcpy(header.command,"send file from server.\n");
                    break;
                }
                // printf("send file data\n");
                // strcpy(header.command,"successful file transmission.\n");
                break;
            case PUT:
                // run_put(sock,buffer,glob_filename,header.filesize);
                if ((stream = fopen(header.filename, "w+b")) == NULL){
                    printf("File open Error");
                    exit(1);
                    }
                left_size = header.filesize;
                do{
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
                // char msg[] ="File transimission finished and saved\n";
                // strcpy(header.command,msg);
                // sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                printf("file transmission finished and saved.\n");
                // char msg[] ="File transimission finished and saved\n";

                strcpy(header.command,"file transmission finished and saved.\n");
                // sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                fclose(stream);
                break;
            case DELETE:
                //imp
                run_delete(glob_filename);
                // char msg[] ="deletion successful\n";
                strcpy(header.command,"deletion successful.\n");
                // sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                break;
            case LS:
                //imp
                // run_ls(&d,&dir);
                d = opendir(".");
                if(d){
                    while((dir = readdir(d)) != NULL){
                        if(dir->d_type == DT_REG){
                            strcpy(buffer,dir->d_name);
                            sendto(sock, buffer, nbytes, 0, (struct sockaddr*)&remote, addrlen);
                            // printf("%s\n",dir->d_name);
                        }
                }
                    closedir(d);
                    sendto(sock, "-1", nbytes, 0, (struct sockaddr*)&remote, addrlen);
                }
                // char msg[] ="sent a list of files in current directory\n";
                strcpy(header.command,"sent a list of files in current directory.\n");
                // sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                break;
            case EXIT:
                //imp
                // run_exit(glob_filename);
                // char msg[] ="graceful exit\n";
                strcpy(header.command,"graceful exit.\n");
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

                exit(0);
                break;

        }

        sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
    
    } while (1);
    close(sock);
    return 0;
}