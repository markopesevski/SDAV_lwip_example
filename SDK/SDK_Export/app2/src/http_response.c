#include "http_response.h"

struct tcp_pcb * WSpcb;
u8 WS_ok = 0;
extern u32 spi_ADC_reading;

char *notfound_header =
	"<html> \
	<head> \
		<title>404</title> \
		<style type=\"text/css\"> \
		div#request {background: #eeeeee} \
		</style> \
	</head> \
	<body> \
	<h1>404 Page Not Found</h1> \
	<div id=\"request\">";

char *notfound_footer =
	"</div> \
	</body> \
	</html>";

/* dynamically generate 404 response:
 *	this inserts the original request string in betwween the notfound_header & footer strings
 */
int do_404(struct tcp_pcb *pcb, char *req, int rlen)
{
	int len, hlen;
	int BUFSIZE = 1024;
	char buf[BUFSIZE];
	err_t err;

	len = strlen(notfound_header) + strlen(notfound_footer) + rlen;
	hlen = generate_http_header((char *)buf, "html", len);

	if (tcp_sndbuf(pcb) < hlen)
	{
		xil_printf("cannot send 404 message, tcp_sndbuf = %d bytes, message length = %d bytes\r\n", tcp_sndbuf(pcb), hlen);
		return -1;
	}

	if ((err = tcp_write(pcb, buf, hlen, 1)) != ERR_OK)
	{
		xil_printf("%s: error (%d) writing 404 http header\r\n", __FUNCTION__, err);
		return -1;
	}

	tcp_write(pcb, notfound_header, strlen(notfound_header), 1);
	tcp_write(pcb, req, rlen, 1);
	tcp_write(pcb, notfound_footer, strlen(notfound_footer), 1);

	return 0;
}

int do_http_post(struct tcp_pcb *pcb, char *req, int rlen)
{
	int BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	if (tcp_write(pcb, buf, rlen, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	return 0;
}

/* respond for a file GET request */
int do_http_get(struct tcp_pcb *pcb, char *req, int rlen)
{
	int BUFSIZE = 1400;
	char filename[MAX_FILENAME];
	unsigned char buf[BUFSIZE];
	signed int fsize, hlen, n;
	int fd;
	char *fext;
	err_t err;

	/* determine file name */
	extract_file_name(filename, req, rlen, MAX_FILENAME);

	if(!strncmp(filename, "new_ws", strlen("new_ws")))
	{
		xil_printf("Upgrading socket!\r\n");
		hlen = generate_ws_upgrade_header((char *)buf, req, rlen);
		/* Turn on TCP Keepalive for the given pcb */
		pcb->so_options |= SOF_KEEPALIVE|SOF_REUSEADDR|SOF_ACCEPTCONN;
		pcb->keep_idle = 10000;
		pcb->keep_cnt_sent = 255;
		if ((err = tcp_write(pcb, buf, hlen, 3)) != ERR_OK)
		{
			xil_printf("Error creating WS handshake\r\n");
			xil_printf("error (%d) writing http header to socket\r\n", err);
			xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", hlen, tcp_sndbuf(pcb));
			xil_printf("http header = %s\r\n", buf);
			return -1;
		}
		if ((err = tcp_output(pcb)) != ERR_OK)
		{
			xil_printf("Error creating WS handshake\r\n");
			xil_printf("error (%d) sending http header to socket\r\n", err);
			xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", hlen, tcp_sndbuf(pcb));
			xil_printf("http header = %s\r\n", buf);
			return -1;
		}
		WSpcb = pcb;
		WS_ok = 1;
		return 0;
	}

	/* respond with 404 if not present */
	if (mfs_exists_file(filename) != 1)
	{
		xil_printf("requested file %s not found, returning 404\r\n", filename);
		do_404(pcb, req, rlen);
		return -1;
	}

	/* respond with correct file */
	/* debug statement on UART */
	xil_printf("http GET: %s\r\n", filename);

	/* get a pointer to file extension */
	fext = get_file_extension(filename);

	fd = mfs_file_open(filename, MFS_MODE_READ);
	if (fd == -1)
	{
		platform_init_fs();
		extract_file_name(filename, req, rlen, MAX_FILENAME);
		if (mfs_exists_file(filename) != 1)
		{
			xil_printf("requested file %s not found, returning 404\r\n", filename);
			do_404(pcb, req, rlen);
			return -1;
		}
		fext = get_file_extension(filename);
		fd = mfs_file_open(filename, MFS_MODE_READ);
		return -1;
	}


	/* obtain file size,
	 * note that lseek with offset 0, MFS_SEEK_END does not move file pointer */
	fsize = mfs_file_lseek(fd, 0, MFS_SEEK_END);
	if (fsize == -1)
	{
		xil_printf("\r\nFile Read Error\r\n");
		return -1;
	}

	/* write the http headers */
	hlen = generate_http_header((char *)buf, fext, fsize);
	if ((err = tcp_write(pcb, buf, hlen, 3)) != ERR_OK)
	{
		xil_printf("Error generating http header\r\n");
		xil_printf("error (%d) writing http header to socket\r\n", err);
		xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", hlen, tcp_sndbuf(pcb));
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	/* now write the file */
	while (fsize > 0)
	{
		int sndbuf;
		sndbuf = tcp_sndbuf(pcb);

		if (sndbuf < BUFSIZE)
		{
			/* not enough space in sndbuf, so send remaining bytes when there is space */
			/* this is done by storing the fd in as part of the tcp_arg, so that the sent
			   callback handler knows to send data */
			http_arg *a = (http_arg *)pcb->callback_arg;
			a->fd = fd;
			a->fsize = fsize;
			return -1;
		}

		n = mfs_file_read(fd, (char *)buf, BUFSIZE);

		if ((err = tcp_write(pcb, buf, n, 3)) != ERR_OK)
		{
			xil_printf("error writing file (%s) to socket, remaining unwritten bytes = %d\r\n", filename, fsize - n);
			xil_printf("attempted to lwip_write %d bytes, tcp write error = %d\r\n", n, err);
			break;
		}
		if (fsize >= n)
		{
			fsize -= n;
		}
		else
		{
			fsize = 0;
		}
	}

	mfs_file_close(fd);
	return 0;
}

enum http_req_type decode_http_request(char *req, int l)
{
	char *new_ws_str = "GET /new_ws";
	char *get_str = "GET";
	char *post_str = "POST";


	if(!strncmp(req, new_ws_str, strlen(new_ws_str)))
	{
		return HTTP_NEW_WS;
	}
	else if (!strncmp(req, get_str, strlen(get_str)))
	{
		return HTTP_GET;
	}
	else if (!strncmp(req, post_str, strlen(post_str)))
	{
		return HTTP_POST;
	}

	return HTTP_UNKNOWN;
}

void dump_payload(char *p, int len)
{
	int i, j;

	for (i = 0; i < len; i+=16)
	{
		for (j = 0; j < 16; j++)
		{
			xil_printf("%c ", p[i+j]);
		}

		xil_printf("\r\n");
	}

	xil_printf("total len = %d\r\n", len);
}

/* generate and write out an appropriate response for the http request
 * this assumes that tcp_sndbuf is high enough to send atleast 1 packet */
int generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len)
{
//	u8 payloadLength = http_req[1] & 0x7F;
//	u8 maskingKey[4] = {'\0'};
//	u8 * originalData = (u8 *) &http_req[6];
//	u8 transformedData[128] = {'\0'};
//	u8 txBuff[128] = {'\0'};
//	u8 level_ascii[5] = {'\0'};
//	err_t err;
	enum http_req_type request_type = decode_http_request(http_req, http_req_len);

	switch(request_type)
	{
		case HTTP_NEW_WS:
		case HTTP_GET:
			return do_http_get(pcb, http_req, http_req_len);
		case HTTP_POST:
			return do_http_post(pcb, http_req, http_req_len);
		default:
//			memcpy(maskingKey, &http_req[2], 4);
//			WSMaskUnmaskData(originalData, payloadLength, maskingKey, transformedData);
			return -1;
	}
}

void WSMaskUnmaskData(u8 * originalData, u8 len, u8 * maskingKey, u8 * transformedData)
{
	/*
		j                   = i MOD 4
		transformed-octet-i = original-octet-i XOR masking-key-octet-j
	*/
	int i = 0;
	for (i = 0; i < len; i++)
	{
		transformedData[i] = originalData[i] ^ maskingKey[i % 4];
	}
}

int updateWSWithWaterLevel(struct tcp_pcb * pcb, u32 level)
{
	u8 txBuff[128] = {'\0'};
	u8 level_ascii[5] = {'\0'};
	err_t err;
	myItoA((int) level, &level_ascii[0]);
	txBuff[0] = 0x81;
	txBuff[1] = strlen((char *) level_ascii);
	memcpy(&txBuff[2], &level_ascii[0], strlen((char *) level_ascii));
	if ((err = tcp_write(pcb, txBuff, strlen((char *) txBuff), 3)) != ERR_OK)
	{
		xil_printf("Error sending WS timed data\r\n");
		xil_printf("error (%d) writing http header to socket\r\n", err);
		xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", strlen((char *) txBuff), tcp_sndbuf(pcb));
		xil_printf("http header = %s\r\n", txBuff);
		return -1;
	}
	if ((err = tcp_output(pcb)) != ERR_OK)
	{
		xil_printf("Error sending WS timed data\r\n");
		xil_printf("error (%d) sending http header to socket\r\n", err);
		xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", strlen((char *) txBuff), tcp_sndbuf(pcb));
		xil_printf("http header = %s\r\n", txBuff);
		return -1;
	}
	xil_printf("WL: %d\r", level);
	return strlen((char *) txBuff);
}

int myItoA(int value, u8 * buffer)
{
	int i = 0;
	buffer[0] = ((value - (value % 1000))/1000) + '0';
	buffer[1] = (((value % 1000) - (value % 100))/100) + '0';
	buffer[2] = (((value % 100) - (value % 10))/10) + '0';
	buffer[3] = (value % 10) + '0';

	for(i = 0; i < 3; i++)
	{
		if(buffer[0] == '0')
		{
			memcpy(&buffer[0], &buffer[1], 4-i);
		}
		else
		{
			break;
		}
	}

	return 4-i;
}
