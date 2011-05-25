#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <pgsql/libpq-fe.h>

#include "baracus-systemd.h"

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

	PGconn	 *conn;
	PGresult *res;

	appname = argv[0];
	syslog_info("Appd %s running as %s\n", baracus_appname, appname);
	
	conn = PQconnectdb("hostaddr = '127.0.0.1' port = '5162' dbname = 'baracus' user = 'dancer' password = 'baractopus' connect_timeout = '10'");
 
        if (PQstatus(conn) == CONNECTION_BAD) {
                puts("We were unable to connect to the database");
                exit(0);
        }
	else
	{
		puts("Conncection successful.");
	}

        res = PQexec(conn,
                "SELECT * FROM pg_roles");

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                puts("We did not get any data!");
                exit(0);
        }

        rec_count = PQntuples(res);

        printf("We received %d records.\n", rec_count);
        puts("==========================");

        for (row=0; row<rec_count; row++) {
                for (col=0; col<3; col++) {
                        printf("%s\t", PQgetvalue(res, row, col));
                }
                puts("");
        }

        puts("==========================");

        PQclear(res);

        PQfinish(conn);
	
	return 0;
}


