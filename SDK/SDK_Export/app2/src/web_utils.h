#ifndef __WEB_UTILS_H_
#define __WEB_UTILS_H_

	#include <string.h>
	#include "lwip/sys.h"
	#include "mfs_config.h"
	#include "webserver.h"
	#include "platform_fs.h"

	void print_http_request(char *buf, int n);
	int is_cmd_led(char *buf);
	int is_cmd_switch(char *buf);
	void extract_file_name(char *filename, char *req, int rlen, int maxlen);
	char *get_file_extension(char *fname);
	int generate_http_header(char *buf, char *fext, int fsize);
	http_arg *palloc_arg();
	void pfree_arg(http_arg *arg);
	void print_ip(char *msg, struct ip_addr *ip);
	void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw);

#endif //__WEB_UTILS_H_
