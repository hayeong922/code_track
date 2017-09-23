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

void run_exit(char *filename){
    printf("Command Exit\n");
    exit(0);
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

// void run_ls(DIR *d, struct dirent *dir){
//     d = opendir(".");
//     if(d){
//         while((dir = readdir(d)) != NULL)
//         if(dir->d_type == DT_REG){
//             printf("%s\n",dir->d_name);
//         }
//         closedir(d);
//     }
    
// }

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

    struct sockaddr_in sin,remote;
    struct packet_header header;
    int addrlen = sizeof(remote); 
    int command_num;
    int left_size;
    unsigned int remote_length;

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
    // bind for both to interact(recvfrom and sendto) 

    bzero(&remote,sizeof(remote)); //bzero((char *)&servaddr, sizeof(servaddr));
    remote.sin_family = AF_INET; //���ͳ� Addr Family
    remote.sin_addr.s_addr = inet_addr(argv[1]); //argv[1]���� �ּҸ� ������
    remote.sin_port = htons(atoi(argv[2])); //argv[2]���� port�� ������


    bzero(&sin,sizeof(sin)); //bzero((char *)&servaddr, sizeof(servaddr));
    sin.sin_family = AF_INET; //���ͳ� Addr Family
    sin.sin_addr.s_addr = htonl(INADDR_ANY); //argv[1]���� �ּҸ� ������
    sin.sin_port = htons(atoi("5002"));  //argv[2]���� port�� ������
    

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

        command_num = assign_command(command);

        switch(command_num){
            case GET:
                printf("Get command!\n");
                strcpy(header.filename,buffer);
                strcpy(header.command,command);
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                
                remote_length = sizeof(remote);
                // what size and receive
                nbytes = recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
                // this is being recieved
                if ((stream = fopen(header.filename, "w+b")) == NULL){
                    printf("File open Error");
                    exit(1);
                    }
                left_size = header.filesize;
                do{
                    nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);
                    printf("received %d\n",nbytes);
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
            case PUT:
                    if ((stream = fopen(buffer, "rb")) == 0){
                        printf("Error");
                        exit(1);
                    }
                    if (stream)
                    {
                        strcpy(header.filename, buffer);

                        fseek(stream, 0, SEEK_END); // end fo file
                        header.filesize = ftell(stream);    // current value of the position indicator is returned
                        // this part is added
                        strcpy(header.command,command);

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
                break;
            case DELETE:
                strcpy(header.filename,buffer);
                strcpy(header.command,command);
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                break;
            case LS:
                strcpy(header.command,command);
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                break;
            case EXIT:
                strcpy(header.command,command);
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                break;
        }    
    }
    return 0;
}