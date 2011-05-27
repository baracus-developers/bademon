#include <pgsql/libpq-fe.h>

typedef struct {
	int rows;
	int cols;
	char **header;
	char **data;
	char *query;
} baracus_query_t;

PGconn * baracus_db_connect(char *hostaddr, char *dbname, char *username, 
			    char *password, int port, int timeout);

baracus_query_t *baracus_query(PGconn *conn, char* query);

void print_table(baracus_query_t* table);
