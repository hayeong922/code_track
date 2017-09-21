#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 30
#define FILENAME 100

void error_handling(char *message);
char* itoa(int value);

struct packet_header {

	char filename[FILENAME];
	int filesize;	
};

int main(int argc, char **argv)
{
	int sock;
	FILE *file;
	char message[BUFSIZE];
	int str_len;
	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;
	struct packet_header header;

	if ( argc != 3 )
	{
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

//	sock = socket(PF_INET, SOCK_DGRAM, 0);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sock == -1 )
	{
		error_handling("socket() error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	while(1)
	{
		fputs("Write filename to send (q to quit): ", stdout);
		fgets(message, BUFSIZE, stdin);
		message[strlen(message)-1] = 0;

		if ( !strcmp(message, "q") )
			break;
		
		file = fopen(message, "r");
		if ( file )
		{
			strcpy(header.filename, message);
			
			fseek(file, 0, SEEK_END); //������ ��
			header.filesize = ftell(file);
	
			printf("Send header\n");	
			printf("Header size: %d\n", sizeof(header));

			sendto(sock, &header, sizeof(header), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
						
			fseek(file, 0, SEEK_SET); //������ ����
			while ( (str_len = fread(message, 1, BUFSIZE, file)) != 0 )
			{
				sendto(sock, message, str_len, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));	
				printf("Send packet %d bytes\n", str_len);
			} 	
				
			fclose(file);
			close(sock);
			break;
		}
		else
		{
			fputs("Invalid file name.\n", stdout);
		}	
	}
	printf("done");
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
