#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include "baracus-systemd.h"
#include "baracus-pgsql.h"
#include "baracus-ssl.h"

const char * default_cfg_file = "/etc/sysconfig/baracus-systemd";
char * config_file;

int debug;         /* global debug flag - no forking to b/g if set*/
static int config_ignore;
static int nofork;   

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

/* exit */
void sig_baracus_exit(int signum)
{
  syslog_info("Baracus exit signal");
}

/* reconfig */
void sig_baracus_config(int signum)
{
  syslog_info("Baracus config signal");
}

int sig_setup(void)
{
	//int sigs[] = { SIGPIPE, SIGTERM, SIGABRT, SIGHUP, };

	if( signal( SIGPIPE, sig_pipe ) == SIG_ERR ) {
		syslog_info("Failed installing SIGPIPE handler\n");
         	return -1;
     	}

    	if( signal( SIGTERM, sig_baracus_exit ) == SIG_ERR ) {
        	syslog_info("Failed installing SIGTERM handler\n");
        	return -1;
     	}

    	if( signal( SIGABRT, sig_baracus_exit ) == SIG_ERR ) {
        	syslog_info("Failed installing SIGABRT handler\n");
          	return -1;
     	}

    	if( signal( SIGHUP, sig_baracus_config ) == SIG_ERR ) {
          	syslog_info("Failed installing SIGHUP handler\n");
          	return -1;
        }

    return 0;
}

int main(int argc, char ** argv, char **env) 
{
	appname = argv[0];
	syslog_info("Appd %s running as %s\n", baracus_appname, appname);
	
	if (strstr(appname, baracus_appname)) 
		baracus_systemd_main(argc, argv, env);
	else
		linuxrc_main(argc, argv, env);

	return 0;
}


int baracus_systemd_main(int argc, char ** argv, char **env) {
	int s;
	pid_t pid;

	char runlevel = '0';

	debug = 0; /* use '-d' to enable debug output */
	nofork = 0; /* no fork. */

	/* get the current runlevel */
	//runlevel = get_system_runlevel();

	config_ignore = 0;
	config_file = (char *) default_cfg_file;

	baracus_systemd_init();

	/* suppress fork on debug or nofork */
	if (!(debug || nofork)) {
		pid = fork();
		if (pid) {
         		if (pid == -1) {
               			syslog_info("fork\n");
                		exit (-1);
                	}
           		else {
                   		syslog_info("parent exit\n");
                   		exit (-1);
              		}
        	}
	}

	pid =  getpid();
	syslog_info("%s pid %d\n", appname, pid);

	if (sig_setup()) {
        	syslog_info(LOG_ERR, "Failed installing sig handler(s)");
		return -1;
    	}

	return 0;
}

int baracus_systemd_init() {

	PGconn	 *conn;

	baracus_ssl_init();

	baracus_query_t *bq;
	if ((conn = baracus_connect()) == NULL) {
		syslog_info("baracus_connect fails.");
		return -1;
	}


	if ((bq = baracus_query(conn, "SELECT * FROM pg_roles")) == NULL) {
		syslog_info("select fails.");
		return -1;
	}
	
	print_table(bq);

	return 0;
}


