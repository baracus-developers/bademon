#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <pgsql/libpq-fe.h>

#include "baracus-pgsql.h"

PGconn * baracus_db_connect(char *hostaddr, char *dbname, 
			    char *username, char *password, 
			    int port, int timeout) 
{
	PGresult *res;
	char * conninfo;
	PGconn	 *conn = NULL;
	int conninfo_len = 128 + strlen(hostaddr) + strlen(dbname) + 
				 strlen(username) + strlen(password) + 6 + 3;

	conninfo = malloc(conninfo_len);
	if (conninfo == NULL) 
		return NULL;
	
	snprintf (conninfo, conninfo_len, "hostaddr = '%s' port = '%6d' dbname = '%s' user = '%s' password = '%s' connect_timeout = '%3d'", hostaddr, port, dbname, username, password, timeout);
	printf("conninfo is %s\n", conninfo);
	
	PQinitOpenSSL(1, 1);
	conn = PQconnectdb(conninfo);
 
        if (PQstatus(conn) == CONNECTION_BAD) 
                puts("We were unable to connect to the database");

	free (conninfo);
	return conn;
}


int baracus_query(PGconn *conn, char *query) {
	int     rec_count;
	int	field_count;
	int     row;
	int     col;
	char **  data;
	char *x;

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
        rec_count = PQntuples(res);
	data = (char **) calloc(field_count * (rec_count+1), sizeof(char *));
	if (data == NULL) {
		printf("error allocating data\n");
		return -1;
	}
    	for (col = 0; col < field_count; col++) {
		data[col] = PQfname(res, col);
        	printf("%-16s", data[col]);
	}
    	printf("\n");
    	for (col = 0; col < ((field_count < 5) ? field_count:5); col++)
        	printf("===============");
    	printf("\n");

        rec_count = PQntuples(res);

        for (row=0; row<rec_count; row++) {
                for (col=0; col < field_count; col++) {
			data[(row+1)*col+col] = PQgetvalue(res, row, col);
                        printf("%-16s", data[(row+1)*col+col]);
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


