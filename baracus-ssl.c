
#include <openssl/ssl.h>
#include "baracus-ssl.h"

BIO *bio_read;
BIO *bio_write;

int baracus_ssl_init() {

	SSL_load_error_strings();
	SSL_library_init();
}

int SSL_Connect() {
	int n;
	
	SSL_CTX *ctx;
	SSL *SSL;

	n = SSL_CTX_use_certificate_file(ctx,
         "/usr/share/baracus/certs/ssl.crt/server.crt", SSL_FILETYPE_PEM);
	n = SSL_CTX_use_PrivateKey_file(ctx,
         "/usr/share/baracus/certs/ssl.key/server.key", SSL_FILETYPE_PEM);

	SSL=SSL_new(ctx);
	bio_read = BIO_new(BIO_s_mem());
	bio_write= BIO_new(BIO_s_mem());

	SSL_set_bio(SSL, bio_read,bio_write);

	SSL_set_accept_state(SSL);
	return 0;


}

int SSL_shutdown(SSL * SSL) {
	SSL_free(SSL);
	return 0;

}


int SSLwrite(SSL *SSL, char * data) {
	int len;

	if (SSL_is_init_finished(SSL) && ( SSL_write(SSL, data, 1) < 0 ) ) {
         //       SSL_print_error("SSL_write error\n");
	}
	len = BIO_read(bio_write, data, sizeof(data));
	if (len == 0)
		return 0;

       	if (len < 0) {
		if (!BIO_should_retry(bio_write)) {
		//	SSL_print_error("BIO_read error");
		}
          	return 0;
	}
	return 0;
}


char * SSLread() {

	return NULL;

}

