#include <openssl/ssl.h>

#define KEYFILE "server.pem"
#define PASSWORD "password"
#define DHFILE "dh1024.pem"
#define CA_LIST "root.pem"

SSL_CTX *ssl_init_ctx(char *, char*);
