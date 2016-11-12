/*
 * main.c
 *
 *  Created on: Nov 12, 2016
 *      Author: root
 */

#include <stdio.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "lwipopts.h"
#include "dispatch.h"

void lwip_init(void);
void tcp_fasttmr(void);
void tcp_slowtmr(void);

#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
extern volatile int TxPerfConnMonCntr;
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

void print_ip(char *msg, struct ip_addr *ip)
{
    print(msg);
    xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{
    print_ip("Board IP:       ", ip);
    print_ip("Netmask :       ", mask);
    print_ip("Gateway :       ", gw);
}

int main(void)
{
	struct netif *netif, server_netif;
	struct ip_addr ipaddr, netmask, gw;

	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] = {0x00, 0x0a, 0x35, 0x00, 0x01, 0x02};

	netif = &server_netif;

	if (init_platform() < 0) {
		xil_printf("ERROR initializing platform.\r\n");
		return -1;
	}

	xil_printf("\r\n\r\n");
	xil_printf("-----lwIP RAW Mode Demo Application ------\r\n");

	/* initliaze IP addresses to be used */
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);
    print_ip_settings(&ipaddr, &netmask, &gw);

	xil_printf("Adding NETIF to list\r\n");
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\r\n");
		return -1;
	}
	xil_printf("Setting NETIF default\r\n");
	netif_set_default(netif);
	xil_printf("NETIF default set\r\n");

	/* specify that the network if is up */
	xil_printf("Setting NETIF up\r\n");
	netif_set_up(netif);
	xil_printf("NETIF up\r\n");

	/* now enable interrupts */
	xil_printf("Enabling interrupts\r\n");
	platform_enable_interrupts();
	xil_printf("Interrupts enabled\r\n");

	/* start the application (web server, rxtest, txtest, etc..) */
	xil_printf("Starting web app\r\n");
	start_applications();
	xil_printf("Web app started\r\n");
	print_headers();

	while (1) {
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		xemacif_input(netif);
		transfer_data();
	}

    /* never reached */
    cleanup_platform();

	return 0;
}
