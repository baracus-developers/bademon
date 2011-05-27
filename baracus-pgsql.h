#include <pgsql/libpq-fe.h>

typedef struct {
	int rows;
	int cols;
	char **data;
} baracus_query_rpt;

PGconn * baracus_db_connect(char *hostaddr, char *dbname, char *username, 
			    char *password, int port, int timeout);

int baracus_query(PGconn *conn, char *query);

