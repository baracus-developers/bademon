#include <pgsql/libpq-fe.h>


PGconn * baracus_db_connect(char *hostaddr, char *dbname, char *username, 
			    char *password, int port, int timeout);

int baracus_query(PGconn *conn, char *query);

