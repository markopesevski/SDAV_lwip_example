#include "web_utils.h"

void print_ip(char *msg, struct ip_addr *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
}

void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{
	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}

void print_http_request(char *buf, int n)
{
	printf("%s\r\n", buf);
}

int is_cmd_led(char *buf)
{
	/* skip past 'POST /' */
	buf += 6;

	/* then check for cmd/ledxhr */
	return (!strncmp(buf, "cmd", 3) && !strncmp(buf + 4, "ledxhr", 6));
}

int is_cmd_switch(char *buf)
{
	/* skip past 'POST /' */
	buf += 6;

	/* then check for cmd/ledxhr */
	return (!strncmp(buf, "cmd", 3) && !strncmp(buf + 4, "switchxhr", 9));
}

void extract_file_name(char *filename, char *req, int rlen, int maxlen)
{
	char *fstart, *fend;

	/* first locate the file name in the request */
	/* requests are of the form GET /path/to/filename HTTP... */
	req += strlen("GET ");
	if (*req == '/')
	{
		req++;
	}

	/* start marker */
	fstart = req;

	/* file name finally ends in a space */
	while (*req != ' ')
		req++;

	/* end marker */
	fend = req-1;

	if (fend < fstart)
	{
		strcpy(filename, "index.html");
		return;
	}

	/* make sure filename is of reasonable size */
	if (fend - fstart > maxlen)
	{
		*fend = 0;
		strcpy(filename, "404.html");
		printf("Request filename is too long\r\n");
		return;
	}

	/* copy over the filename */
	strncpy(filename, fstart, fend-fstart+1);
	filename[fend-fstart+1] = 0;

	/* if last character is a '/', append index.html */
	if (*fend == '/')
	{
		strcat(filename, "index.html");
	}
}

char *get_file_extension(char *fname)
{
	char *fext = fname + strlen(fname) - 1;

	while (fext > fname)
	{
		if (*fext == '.')
		{
			return fext + 1;
		}
		fext--;
	}

	return NULL;
}

int generate_http_header(char *buf, char *fext, int fsize)
{
	char lbuf[40];

	strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: ");

	if (fext == NULL) /* for unknown types */
	{
		strcat(buf, "text/html");
	}
	else if (!strncmp(fext, "htm", 3)) /* html */
	{
		strcat(buf, "text/html");
	}
	else if (!strncmp(fext, "jpg", 3))
	{
		strcat(buf, "image/jpeg");
	}
	else if (!strncmp(fext, "gif", 3))
	{
		strcat(buf, "image/gif");
	}
	else if (!strncmp(fext, "json", 4))
	{
		strcat(buf, "application/json");
	}
	else if (!strncmp(fext, "js", 2))
	{
		strcat(buf, "application/javascript");
	}
	else if (!strncmp(fext, "pdf", 2))
	{
		strcat(buf, "application/pdf");
	}
	else if (!strncmp(fext, "css", 2))
	{
		strcat(buf, "text/css");
	}
	else /* for unknown types */
	{
		strcat(buf, "text/plain");
	}

	strcat(buf, "\r\n");

	sprintf(lbuf, "Content-length: %d", fsize);
	strcat(buf, lbuf);
	strcat(buf, "\r\n");

	strcat(buf, "Connection: close\r\n");
	strcat(buf, "\r\n");

	return strlen(buf);
}

int generate_ws_upgrade_header(char * buf, char * req, int reqlen)
{
	unsigned char key[128] = {'\0'};
	unsigned char key_sha1[128] = {'\0'};
	char key_sha1_base64[128] = {'\0'};
	u16 i = 0;
	u16 j = 0;

	//xil_printf("req:%s:req\r\n",req);

	for(i = 0; i < reqlen-19; i++)
	{
		if(!strncmp(&req[i], "Sec-WebSocket-Key: ", 19))
		{
			break;
		}
	}

	i = i + 19;

	for(j = i; j < reqlen; j++)
	{
		if(req[j] == '\r' || req[j] == '\n')
		{
			break;
		}
	}
	//xil_printf("req(%d): %c\r\n", i, req[i]);
	//xil_printf("req(%d): %c\r\n", j, req[i+j]);

	memcpy(key, &req[i], j-i);
	memcpy(&key[j-i], "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 36);
	xil_printf("key: %s\r\n", key);
	mbedtls_sha1(&key[0], (j-i)+36, &key_sha1[0]);
	xil_printf("sha1: ", key_sha1);
	for (i = 0; i < 20; i++)
	{
		xil_printf("%02x", key_sha1[i]);
	}
	xil_printf("\r\n");
	Base64encode((char *)&key_sha1_base64[0], (char *)&key_sha1[0], strlen((char *)key_sha1));
	xil_printf("base64: %s\r\n", key_sha1_base64);
	strcpy(buf, "HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
	strcat(buf, "Connection: Upgrade\r\n");
	strcat(buf, "Upgrade: WebSocket\r\n");
	strcat(buf, "Sec-WebSocket-Accept: %\r\n");
	strcat(buf, "\r\n");

	return strlen(buf);
}

static int p_arg_count = 0;
static http_arg httpArgArray[1000];
static int httpArgArrayIndex = 0;
http_arg *palloc_arg()
{
	http_arg *a;
	a = &(httpArgArray[httpArgArrayIndex]);
	httpArgArrayIndex++;
	if (httpArgArrayIndex == 1000)
	{
		httpArgArrayIndex = 0;
	}
	a->count = p_arg_count++;
	a->fd = -1;
	a->fsize = 0;

	return a;
}

void pfree_arg(http_arg *arg)
{
	;
}
