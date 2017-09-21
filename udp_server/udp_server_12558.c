#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 30 
#define FILENAME 100

void error_handling(char *message);

struct packet_header {

	char filename[FILENAME];
	int filesize;
};

int main(int argc, char **argv)
{
	int serv_sock;
	char message[BUFSIZE];
	int str_len;
	int num;
	int left_size; 

	FILE* file;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	struct packet_header header;
	int clnt_addr_size;

	if ( argc != 2 )
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if ( serv_sock == -1 )
	{
		error_handling("socket() error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if ( bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1 )
	{
		error_handling("bind() error");
	}

	

	do
	{
		clnt_addr_size = sizeof(clnt_addr);

		str_len = recvfrom(serv_sock, &header, sizeof(header), 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

		file = fopen(header.filename, "w");
		left_size = header.filesize;	

		printf("Receive file: %s\n", header.filename);
		printf("File size: %d\n", header.filesize);

		do	
		{
		 	str_len = recvfrom(serv_sock, message, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size); 
			fwrite(message, 1, str_len, file);
			printf("Received %d bytes\n", str_len);				
			left_size -= str_len;
			printf("%d bytes left\n", left_size);
	
			if ( left_size <= 0 )
				break;	
		}
		while (1);
		printf("file transmission finished\n");

		fclose(file);
	}
	while(1);

	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
