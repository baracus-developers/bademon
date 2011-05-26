#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <pgsql/libpq-fe.h>

#include "baracus-pgsql.h"

PGconn	 *conn;
int baracus_db_connect() 
{
	PGresult *res;

//	conn = PQconnectdb("hostaddr = '127.0.0.1' port = '5162' dbname = 'baracus' user = 'dancer' password = 'baractopus' connect_timeout = '10'");
	conn = PQconnectdb("hostaddr = '172.17.1.251' port = '5162' dbname = 'baracus' user = 'dancer' password = 'baractopus' connect_timeout = '10'");
 
        if (PQstatus(conn) == CONNECTION_BAD) {
                puts("We were unable to connect to the database");
                return -1;
        }

	puts("Conncection successful.");
	return 0;
}


int baracus_query() {
	int             rec_count;
	int             row;
	int             col;

	PGresult *res;
        res = PQexec(conn,
                "SELECT * FROM pg_roles");

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


