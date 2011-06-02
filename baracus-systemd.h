/* Baracus - Systemd
 * (c) 2011 Sven-Thorsten Dietrich 
 */

#ifndef _BARACUS_SYSTEMD_H
#define _BARACUS_SYSTEMD_H

extern int systemd_run;
void syslog_info(const char *fmt, ...);
int baracus_systemd_main(int argc, char ** argv, char **env);
#endif
