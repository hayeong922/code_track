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

void split_func(char *s){
    char *token = strtok(s," ");

    int count = 0;
    while(token != NULL){
        if(count == 0){
            strcpy(glob_cmd,token);
        }else{
                strcpy(glob_filename, token);
        }
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

    bzero(&remote,sizeof(remote)); 
    remote.sin_family = AF_INET; 
    remote.sin_addr.s_addr = inet_addr(argv[1]); 
    remote.sin_port = htons(atoi(argv[2])); 

    bzero(&sin,sizeof(sin)); 
    sin.sin_family = AF_INET; 
    sin.sin_addr.s_addr = htonl(INADDR_ANY); 
    sin.sin_port = htons(atoi("5002"));  
    

    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("unable to crate socket");
    }

    char command[] = "delete";
    while (1){
        printf("Put command(and filename):");
        fgets(buffer, MAXBUFSIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;

        // take the input and split by spacce and assign command file name to global var
        split_func(buffer);
        
        strcpy(buffer,glob_filename);
        strcpy(command,glob_cmd);

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
                        printf("send file data\n");
                        fclose(stream);
                        // close(sock); //socket close
                        break;
                    }
                    printf("send file data\n");

                    // recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
                    // printf("Server says %s\n",header.command);
                    // recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
                    // printf("Server says %s\n",header.command);
                break;
            case DELETE:
                strcpy(header.filename,buffer);
                strcpy(header.command,command);
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

                // recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
                // printf("Server says %s\n",header.command);
                break;
            case LS:
                strcpy(header.command,command);
                strcpy(header.filename,"");
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);
                
                do{
                    nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);
                    printf("%s\n",buffer);
                    if (nbytes < 0) {
                        break;
                        perror("recvfrom fail");
                        exit(1);
                    }
                } while (1);
                // printf("file successfully received\n");
                recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
                // printf("Server says %s\n",header.command);
                break;
            case EXIT:
                strcpy(header.command,command);
                strcpy(header.filename,"");
                sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, addrlen);

                // recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
                // printf("Server says %s\n",header.command);
                break;
        }  


        // this part is message receiveing from server, recvfrom()
        recvfrom(sock, &header, sizeof(header), 0, (struct sockaddr*)&remote, &remote_length);
        printf("Server says %s\n",header.command);
        // printf("Server says %s\n",buffer);  
    }
    close(sock);
    return 0;
}