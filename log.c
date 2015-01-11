#include <stdio.h>
#include <time.h>
#include "log.h"
#define LOG_NAME "log.log"
int mark_log(char *type, char *description)
{
	FILE *fp;
	fp = fopen(LOG_NAME, "a+");
	if (fp == NULL)
	{
		return 0;
	}
	time_t seconds = time(NULL);
	char *local = ctime(&seconds);
	fprintf(fp, "%s", local);
	fprintf(fp, "%s ", type);
	fprintf(fp, "%s\n\n", description);
	fclose(fp);
	return 1;
}

int main(int argc, char *argv[])
{
	int i = 0;
	for(i=0; i<4; i++)
	{
		printf("%d\n", i);
		mark_log("hello", "hello");
		sleep(1);
	}
	return 0;
}
