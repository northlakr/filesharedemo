#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define PORT 7061

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("arg error\n");
		return 0;
	}

	int s_fd;
	struct sockaddr_in server_addr;
	int err = 0;
	char buff[1024] = {"client hello"};

	s_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (s_fd < 0)
	{
		printf("socket error\n");
		return 1;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

//	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	err = connect(s_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if (err != 0)
	{
		printf("connect error:%d\n", err);
		return 1;
	}
	send(s_fd, buff, sizeof(buff), 0);
	printf("%s sended\n", buff);
	close(s_fd);

	return 0;
}
