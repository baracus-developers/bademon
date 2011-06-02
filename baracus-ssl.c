
#include "baracus-ssl.h"
#include "baracus-systemd.h"

BIO *bio_err = 0;

BIO *bio_read;
BIO *bio_write;

static char *pass;
static int password_cb(char *buf,int num, int rwflag,void *userdata);

SSL_CTX *ssl_init_ctx(char *keyfile, char *password) {
	SSL_METHOD *meth;
	SSL_CTX *ctx;

	SSL_library_init();
	SSL_load_error_strings();

	/* An error write context */
        bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

        /* Create our context*/
        meth = SSLv23_method();
        ctx = SSL_CTX_new(meth);

        /* Load our keys and certificates*/
        if(!(SSL_CTX_use_certificate_chain_file(ctx, keyfile))) {
                syslog_info("Can't read certificate file %s", keyfile);
                return NULL;
        }

        pass = password;
        SSL_CTX_set_default_passwd_cb(ctx, password_cb);
        if(!(SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM))) {
                syslog_info("Can't read key file %s", keyfile);
                return NULL;
        }

	/* Load the CAs we trust*/
        if (!(SSL_CTX_load_verify_locations(ctx, CA_LIST, 0))) {
                syslog_info("Can't read CA list %s", CA_LIST);
                return NULL;
        }

        return ctx;
}


int process_oob_channel(fd_set * er, int s )
{
  char c;
  int r;
  if (FD_ISSET(s, er))
        {
 //       r = recv (s, &c, 1, msg_oob);
        if (r < 1)
          {
               // syslog(log_err,"oob channel error socket %d", s);
                return -1;
          }
        }
  return 0;
}

/*The password code is not thread safe*/
static int password_cb(char *buf,int num,
  int rwflag,void *userdata)
  {
        if (num < strlen(pass)+1)
                return(0);

        strcpy(buf,pass);
        return(strlen(pass));
}

int ssl_accept(int s, SSL_CTX *ctx) {
	SSL *ssl;
	BIO *sbio;
	sbio = BIO_new_socket(s, BIO_NOCLOSE);

        ssl = SSL_new(ctx);
#if 0
        SSL_set_bio(ssl, sbio, sbio);

        if ((SSL_accept(ssl) <= 0)) {
                syslog_info("SSL accept ");
                //close (s);
                goto server_run;
        }

        //destroy_ctx(ctx);
#endif


}


int ssl_connect() {
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

