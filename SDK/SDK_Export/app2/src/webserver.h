#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

	#include "lwip/tcp.h"
	#include <stdio.h>
	#include <string.h>
	#include "lwip/err.h"
	#include "xilmfs.h"
	#include "xil_printf.h"

	#define MAX_FILENAME 256

	/* initialize file system layer */
	int platform_init_fs();

	/* initialize device layer */
	int http_init_devices();

	/* web_utils.c utilities */
	void extract_file_name(char *filename, char *req, int rlen, int maxlen);
	char *get_file_extension(char *buf);
	int generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len);

	typedef struct
	{
		int count;
		int fd;
		int fsize;
	} http_arg;

	http_arg *palloc_arg();
	void pfree_arg(http_arg *);
	int transfer_web_data();
	err_t http_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
	err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
	err_t http_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
	int start_web_application();
	void print_headers(struct ip_addr *ip);

#endif //__WEBSERVER_H__
