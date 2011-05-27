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


baracus_query_t* baracus_query(PGconn *conn, char *query) {
	int     row;
	int     col;
	PGresult *res;
	baracus_query_t *bq;

	if (conn == NULL) 
		return NULL;

 	if ((bq = malloc(sizeof(baracus_query_t))) == NULL)
		return NULL;
	
	bq->query = query;
        res = PQexec(conn, bq->query);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                puts("We did not get any data!");
                return NULL;
        }

//	if (PQsetResultAttrs
	/*
         * Should PQclear PGresult whenever it is no longer needed to 
	 * avoid memory leaks
         */
//        PQclear(res);
	
	bq->cols = PQnfields(res);
        bq->rows = PQntuples(res);
	if ((bq->header = calloc(bq->cols, sizeof(char *))) == NULL)
		return NULL;
	if ((bq->data = calloc(bq->cols * bq->rows, sizeof(char *))) == NULL)
		return NULL;
	
    	for (col = 0; col < bq->cols; col++) 
		bq->header[col] = PQfname(res, col);

        for (row=0; row < bq->rows; row++) 
                for (col=0; col < bq->cols; col++) 
			bq->data[row*col+col] = PQgetvalue(res, row, col);

        PQclear(res);

        PQfinish(conn);
	
	return bq;
}


void print_table (baracus_query_t *bq) {
	int     row;
	int     col;

        puts("==========================");
	
	/* first, print out the attribute names */
    	for (col = 0; col < bq->cols; col++) {
        	printf("%-16s", bq->header[col]);
	}
    	printf("\n");
    	for (col = 0; col < ((bq->cols < 5) ? bq->cols:5); col++)
        	printf("===============");
    	printf("\n");

	/* print the data */
        for (row=0; row < bq->rows; row++) {
                for (col=0; col < bq->cols; col++) {
                        printf("%-16s", bq->data[row*col+col]);
                }
    		printf("\n");
    		for (col = 0; col < ((bq->cols < 5) ? bq->cols:5); col++)
        		printf("--------------");
    		printf("\n");
        }
        printf("%d records.\n", bq->rows);
        puts("==========================");
}

