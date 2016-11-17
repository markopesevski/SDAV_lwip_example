#ifndef __PLATFORM_H_
#define __PLATFORM_H_

	#include "arch/cc.h"
	#include "lwipopts.h"
	#include "xenv_standalone.h"
	#include "xparameters.h"
	#include "xintc.h"
	#include "xil_exception.h"
	#include "mb_interface.h"
	#include "xtmrctr_l.h"
	#include "lwip/tcp.h"
	#include "xspi.h"

	#define VDD_ADC 3.3

	#define PLATFORM_EMAC_BASEADDR XPAR_EMACLITE_0_BASEADDR
	#define PLATFORM_TIMER_BASEADDR XPAR_TMRCTR_0_BASEADDR
	#define PLATFORM_TIMER_INTERRUPT_INTR XPAR_INTC_0_TMRCTR_0_VEC_ID
	#define PLATFORM_TIMER_INTERRUPT_MASK (1 << PLATFORM_TIMER_INTERRUPT_INTR)

	int platform_init_fs();
	int init_platform();
	void cleanup_platform();
	void platform_enable_interrupts();
	void timer_callback();
	void xadapter_timer_handler(void *p);
	void platform_setup_timer();
	void platform_enable_interrupts();
	void platform_setup_interrupts();
	void enable_caches();
	void disable_caches();
	int init_platform();
	void cleanup_platform();
	int init_spi(XSpi * spi);
	u32 read_spi(XSpi * spi);

	#if LWIP_DHCP==1
		void dhcp_fine_tmr();
		void dhcp_coarse_tmr();
	#endif //LWIP_DHCP

#endif //__PLATFORM_H_
