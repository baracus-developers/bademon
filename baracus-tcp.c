#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include "baracus-systemd.h"
#include "baracus-tcp.h"
#include "baracus-ssl.h"


inline void fd_set_clear(fd_set *rd, fd_set *wr, fd_set *er)
{
	if (rd) FD_ZERO(rd);
	if (wr) FD_ZERO(wr);
	if (er) FD_ZERO(er);
}


inline int fd_set_build(int s, fd_set * rd, fd_set * wr, fd_set * er)
{
	fd_set_clear(rd, wr, er);

	if (rd) FD_SET(s, rd);
	if (wr) FD_SET(s, wr);
	if (er) FD_SET(s, er);

	return 0;
}

int tcp_srv_stop(int fd) {

	/* clear buffers */
     	tcflush(fd, TCOFLUSH);
	close (fd);
	return 0;
}

int tcp_srv_init(int port)
{
	int ss;
	int len, rlen;
	int optval = 1;

	struct sockaddr_in addr_in, addr_nm;

	syslog_info("TCP server port %d", port);
  
	if ((ss = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		syslog_info("socket fails");
		return -1;
	}

	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_addr.s_addr=INADDR_ANY;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);
	setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	len = rlen = sizeof(addr_in);

	if (bind(ss, (struct sockaddr *) &addr_in, len) < 0) {
		syslog_info("bind");
		return -1;
	}
	
	if (getsockname(ss, (struct sockaddr *) &addr_nm, &rlen)) {
		syslog_info("TCP server getsockname");
		return -1;
	}
	if ( 1 || len != rlen) {
		syslog_info("getsockname len %d rlen %d", len, rlen);
	}

	if (listen(ss, 0) < 0) {
		syslog_info("TCP server listen");
		return -1;
	}

	return(ss);
}

int tcp_srv() {
	int s, ss, as;

	fd_set rd, wr, er;
	struct timeval tv;
	struct sockaddr addr;
	SSL_CTX *ssl_ctx;

	int len = sizeof(addr);

	if ((ss = tcp_srv_init(10000)) < 0) { 
		return -1;
	}

	ssl_ctx = ssl_init_ctx(KEYFILE, PASSWORD);

server_run: 
	fd_set_build(ss, &rd, &wr, &er);
#ifdef USE_SELECT
	s = select(ss + 1, &rd, &wr, &er, &tv);
	if (s < 0) {
		if (s == -1 && errno == EINTR)
			goto server_run;
		syslog_info("select");
	}
 	printf("select succeeds\n");
#endif
	if ((as = accept(ss, &addr, &len)) < 0) {
		syslog_info("accept");
		//close(s);
		goto server_run;
	}
	printf("accepted\n");

	ssl_accept(ss, ssl_ctx);

	if (tcp_srv_stop(ss)) {
		syslog_info("socket shutdown");
		return -1;
	}
	return 0;
}

