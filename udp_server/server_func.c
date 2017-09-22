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
    if(strcat(cmd,"get")== 0){
        return GET;
    }else if(strcat(cmd,"put")== 0){
        return PUT;
    }else if(strcat(cmd,"delete")== 0){
        return DELETE;
    }else if(strcat(cmd,"ls")== 0){
        return LS;
    }else if(strcat(cmd,"exit")== 0){
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

void run_exit(char *filename){
    printf("Command Exit\n");
    exit(0);
}

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

    struct sockaddr_in sin, remote;
    struct packet_header header;
    // int clnt_addr_size;
    int addrlen = sizeof(sin);
    int command_num;
    DIR *d;
    struct dirent *dir;

    if (argc != 2) {
        printf("usage: %s port\n", argv[0]);
        exit(1);
    }

    // 
    memset(&sin, 0, addrlen); //bzero((char *)&servaddr,addrlen);
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
        nbytes = recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
        // this is being recieved

        printf("filname: %s\n", header.filename);
        printf("File size: %d\n", header.filesize);
        printf("command: %d\n", header.command);

        // printf("command and filename: %s %s\n", header.command,header.filename);

        // strcpy(check_command, header.command);
        // call function that assign number to command input
        command_num = assign_command(header.command);
        strcpy(glob_filename,header.filename);

        switch(command_num){
            case GET:
                run_get(glob_filename);
                break;
            case PUT:
                prtinf("*************put************\n");
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
                printf("file transmission finished and saved\n");
                fclose(stream);
                break;
            case DELETE:
                //imp
                run_delete(glob_filename);
                break;
            case LS:
                //imp
                run_ls(&d,&dir);
                break;
            case EXIT:
                //imp
                run_exit(glob_filename);
                break;

        }
        
        // if ((stream = fopen(header.filename, "w+b")) == NULL){
        //     printf("File open Error");
        //     exit(1);
        // }
        // left_size = header.filesize;

        // //file �̸� ���� �޾�����, fileũ�� ���۹޾Ҵٸ�
        // //file ���� ���� �ޱ�
        // do{
        //     //���� ���� �޽��� nbyte ����,
        //     nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);
        //     fwrite(buffer, 1, nbytes, stream);
        //     left_size -= nbytes;

        //     if (left_size <= 0)
        //         break;
        //     if (nbytes < 0) {
        //         perror("recvfrom fail");
        //         exit(1);
        //     }
        // } while (1);
        // // printf("file transmission finished\n");
        // printf("file transmission finished and saved\n");
        // fclose(stream);
    } while (1);
    close(sock);
    return 0;
}