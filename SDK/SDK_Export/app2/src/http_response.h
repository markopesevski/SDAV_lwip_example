#ifndef __HTTP_RESPONSE_H_
#define __HTTP_RESPONSE_H_
	#include <string.h>
	#include "mfs_config.h"
	#include "lwip/inet.h"
	#include "webserver.h"
	#include "xil_printf.h"
	#include "web_utils.h"

	enum http_req_type
	{
		HTTP_GET,
		HTTP_POST,
		HTTP_NEW_WS,
		HTTP_UNKNOWN
	};

	int generate_http_header(char *buf, char *fext, int fsize);
	int toggle_leds();
	unsigned int get_switch_state();
	int mfs_file_read(int fd, char *buf, int buflen);
	int do_404(struct tcp_pcb *pcb, char *req, int rlen);
	int do_http_post(struct tcp_pcb *pcb, char *req, int rlen);
	int do_http_get(struct tcp_pcb *pcb, char *req, int rlen);
	enum http_req_type decode_http_request(char *req, int l);
	void dump_payload(char *p, int len);
	int generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len);

#endif //__HTTP_RESPONSE_H_
