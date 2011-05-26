#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <pgsql/libpq-fe.h>

#include "baracus-pgsql.h"

const int conninfo_strlen=255;

PGconn * baracus_db_connect(char *hostaddr, char *dbname, char *username, char *password, int port, int timeout) 
{
	
	PGconn	 *conn;
	PGresult *res;
	const char conninfo[conninfo_strlen];
	
	snprintf (conninfo, conninfo_strlen, "hostaddr = '%s' port = '%d' dbname = '%s' user = '%s' password = '%s' connect_timeout = '%d0'", hostaddr, port, dbname, username, password, timeout);
	printf("conninfo is %s\n", conninfo);
	conn = PQconnectdb(conninfo);
 
        if (PQstatus(conn) == CONNECTION_BAD) {
                puts("We were unable to connect to the database");
        }

	return conn;
}


int baracus_query(PGconn *conn, char *query) {
	int             rec_count;
	int             row;
	int             col;

	PGresult *res;
        res = PQexec(conn, query);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                puts("We did not get any data!");
                return -1;
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


