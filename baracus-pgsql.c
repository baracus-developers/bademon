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

PGconn * baracus_db_connect(char *hostaddr, char *dbname, 
			    char *username, char *password, 
			    int port, int timeout) 
{
	
	PGconn	 *conn;
	PGresult *res;
	const char conninfo[conninfo_strlen];
	
	snprintf (conninfo, conninfo_strlen, "hostaddr = '%s' port = '%d' dbname = '%s' user = '%s' password = '%s' connect_timeout = '%d0'", hostaddr, port, dbname, username, password, timeout);
	printf("conninfo is %s\n", conninfo);
	
	PQinitOpenSSL(1, 1);
	conn = PQconnectdb(conninfo);
 
        if (PQstatus(conn) == CONNECTION_BAD) {
                puts("We were unable to connect to the database");
        }

	return conn;
}


int baracus_query(PGconn *conn, char *query) {
	int     rec_count;
	int	field_count;
	int     row;
	int     col;

	PGresult *res;

	if (conn == NULL) 
		return -1;

        res = PQexec(conn, query);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                puts("We did not get any data!");
                return -1;
        }

//	if (PQsetResultAttrs
	/*
         * Should PQclear PGresult whenever it is no longer needed to 
	 * avoid memory leaks
         */
//        PQclear(res);
	
        puts("==========================");
	/* first, print out the attribute names */
	field_count = PQnfields(res);
    	for (col = 0; col < field_count; col++)
        	printf("%-16s", PQfname(res, col));
    	printf("\n");
    	for (col = 0; col < ((field_count < 5) ? field_count:5); col++)
        	printf("===============");
    	printf("\n");

        rec_count = PQntuples(res);

        for (row=0; row<rec_count; row++) {
                for (col=0; col < field_count; col++) {
                        printf("%-16s", PQgetvalue(res, row, col));
                }
    		printf("\n");
    		for (col = 0; col < ((field_count < 5) ? field_count:5); col++)
        		printf("--------------");
    		printf("\n");
        }
        printf("%d records.\n", rec_count);
        puts("==========================");

        PQclear(res);

        PQfinish(conn);
	
	return 0;
}


