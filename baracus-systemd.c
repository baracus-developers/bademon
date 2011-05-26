#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include "baracus-systemd.h"
#include "baracus-pgsql.h"

extern int linuxrc_main(int argc, char **argv, char **env);

pthread_t baracusd_nfsio, baracusd_socket, baracusd_mount;
char *appname = NULL;

void syslog_info(const char *fmt, ...) {
	int pid;
	char *msg;
	va_list ap;

	if ((msg = malloc(SYSLOG_MAX_LEN)) == NULL)
		return;

	pid = getpid();

	va_start(ap, fmt);
	//vsnprintf(msg, SYSLOG_MAX_LEN, fmt, ap);
	vsprintf(msg, fmt, ap);
	va_end(ap);

	syslog(LOG_INFO, "%s[%d] %s", appname, pid, msg);
	printf("syslog: [%d] %s\n",pid, msg);
}


/* broken pipes */
void sig_pipe(int signum)
{
  syslog_info("broken pipe");
}


int main(int argc, char ** argv, char **env) 
{
//	rc = linuxrc_main(argc, argv, env);
	int rc;
	int             rec_count;
	int             row;
	int             col;

	static char *dbname = "baracus";
	static char *username = "dancer";
	static char *password = "baractopus";
	static char *hostaddr = "172.17.1.251";
	static int port = 5162;
	static int timeout = 10;

	PGconn	 *conn;
	PGresult *res;

	appname = argv[0];
	syslog_info("Appd %s running as %s\n", baracus_appname, appname);
	
	baracus_db_connect(hostaddr, dbname, username, password, port, timeout);
	baracus_query();
	
	return 0;
}


