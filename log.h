#ifndef _LOG_H_
#define _LOG_H_
#define LOG_ERROR "ERROR"
#define LOG_NOMAL "NOMAL"
struct _log_para
{
	char *type;
	char *description;
}*log_para;
int mark_log(char *type, char *description);
#endif
