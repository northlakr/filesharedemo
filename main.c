#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "log.h"

#define BUFFLEN 1024
#define SERVER_PORT 7061
#define BACKLOG 5
#define CLIENT_NUM 1024

pthread_mutex_t mutex;
int run = 1;
int ret_value = 0;
int connect_run = 1;
int request_run = 1;
int main_run = 1;
int connect_host[CLIENT_NUM];
int connect_number = 0;
int socket_server = 0;

struct test
{
	int a;
	int b;
};

void sig_int(int sign)
{
	printf("catch a SIGINT signal\n");
	connect_run = 0;
	request_run = 0;
	main_run = 0;
	run = 0;
	printf("connect_run:%d\nrequest_run:%d\n", connect_run, request_run);
	if (socket_server)
	{
		close(socket_server);
		printf("server is closed\nid is :%d\n", socket_server);
	}
	exit(0);
}

void call_log(char *str1, char *str2)
{
	pthread_mutex_lock(&mutex);
	mark_log(str1, str2);
	pthread_mutex_unlock(&mutex);
}

void *log_thread(void *arg)
{
	int *running = arg;
	while (*running)
	{
		printf("child thread is running\n");
		//mark_log(LOG_NOMAL, "child thread");
		call_log(LOG_NOMAL, "child thread");
		sleep(1);
	}

	printf("child thread quit\n");
	ret_value = 1;
	pthread_exit((void *)&ret_value);
}

static void *handle_connect(void *argv)
{
	int s_server = *((int *)argv);
	int s_client = -1;
	struct sockaddr_in from;
	int len = sizeof(from);
	while (connect_run)
	{
		int i = 0;
		printf("waiting for clients\n");
		int s_client = accept(s_server, (struct sockaddr *)&from, &len);
		printf("accept a client from:%s\n", inet_ntoa(from.sin_addr));
		for(i=0; i<CLIENT_NUM; i++)
		{
			if (connect_host[i] == -1)
			{
				connect_host[i] = s_client;
				connect_number++;
				break;
			}
		}
	}
	return;
}

static void *handle_request(void *argv)
{
	time_t now;
	char buff[BUFFLEN];
	int n = 0;
	int maxfd = -1;
	fd_set scanfd;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	int i = 0;
	int err = -1;

	printf("selecting for clients\n");
	while (request_run)
	{
		maxfd = -1;
		FD_ZERO(&scanfd);

		for(i=0; i<CLIENT_NUM; i++)
		{
			if (connect_host[i] != -1)
			{
				FD_SET(connect_host[i], &scanfd);
				if (maxfd < connect_host[i])
				{
					maxfd = connect_host[i];
				}
			}
		}

		timeout.tv_sec = 1;
		/* printf("selecting for clients\n"); */
		err = select(maxfd + 1, &scanfd, NULL, NULL, &timeout);
		/* printf("error code is :%d\n", err); */
		if ((err > 0) && (connect_number > 0))
		{
			for(i=0; i<CLIENT_NUM; i++)
			{
				if (connect_host[i] != -1
						&& FD_ISSET(connect_host[i], &scanfd))
				{
					memset(buff, 0, BUFFLEN);
					n = recv(connect_host[i], buff, BUFFLEN, 0);
					printf("recv strings:%s\n", buff);
					connect_host[i] = -1;
					connect_number--;
					close(connect_host[i]);
				}
			}
		}
	}
	return;
}

int main(int argc, char *argv[])
{
	pthread_t log_pt;
	pthread_t thread_do[2];
	int *join_ret = NULL;
	struct sockaddr_in local;
	socket_server = socket(AF_INET, SOCK_STREAM, 0); //(1)socket
	printf("init socket_server is :%d\n", socket_server);
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);
	signal(SIGINT, sig_int);

	int err = bind(socket_server, (struct sockaddr *)&local, sizeof(local)); //(2)bind
	if (err == -1)
	{
		printf("bind error\n");
		return 1;
	}
	err = listen(socket_server, BACKLOG); //(3)listen
	if (err == -1)
	{
		printf("listen error\n");
		return 1;
	}
	memset(connect_host, -1, CLIENT_NUM);
	pthread_create(&thread_do[0], NULL, handle_connect, (void *)&socket_server);
	pthread_create(&thread_do[1], NULL, handle_request, NULL);
	pthread_join(thread_do[0], NULL);
	pthread_join(thread_do[1], NULL);
	close(socket_server);
	//pthread_mutex_init(&mutex, NULL);
	// int ret = pthread_create(&log_pt, NULL, (void *)log_thread, &run);
	// if (ret != 0)
	// {
		// printf("create thread error\n");
		// return 1;
	// }

	// int i = 0;
	// for(i=0; i<4; i++)
	// {
		// printf("main thread running\n");
		// //mark_log(LOG_NOMAL, "main thread");
		// call_log(LOG_NOMAL, "main thread");
		// sleep(1);
	// }

	// run = 0;
	// pthread_join(log_pt, (void *)&join_ret);
	// printf("child thread return %d\n", *join_ret);
	//pthread_mutex_destroy(&mutex);
	return 0;
}
