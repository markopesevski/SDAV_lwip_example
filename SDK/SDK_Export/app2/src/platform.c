#include "platform.h"

volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;

#if LWIP_DHCP==1
	volatile int dhcp_timoutcntr = 24;
#endif //LWIP_DHCP

void timer_callback()
{
	static int odd = 1;
	#if LWIP_DHCP==1
		static int dhcp_timer = 0;
	#endif //LWIP_DHCP
	TcpFastTmrFlag = 1;

	odd = !odd;
	if (odd)
	{
		#if LWIP_DHCP==1
			dhcp_timer++;
			dhcp_timoutcntr--;
		#endif //LWIP_DHCP

		TcpSlowTmrFlag = 1;

		#if LWIP_DHCP==1
			dhcp_fine_tmr();
			if (dhcp_timer >= 120)
			{
				dhcp_coarse_tmr();
				dhcp_timer = 0;
			}
		#endif //LWIP_DHCP
	}
}

void xadapter_timer_handler(void *p)
{
	timer_callback();

	/* Load timer, clear interrupt bit */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK);
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK | XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

	/* Clear interrupt bit */
	XIntc_AckIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}

#define TIMER_TLR (XPAR_TMRCTR_0_CLOCK_FREQ_HZ / 4)
void platform_setup_timer()
{
	/* set the number of cycles the timer counts before interrupting */
	/* 100 Mhz clock => .01us for 1 clk tick. For 100ms, 10000000 clk ticks need to elapse  */
	XTmrCtr_SetLoadReg(PLATFORM_TIMER_BASEADDR, 0, TIMER_TLR);

	/* reset the timers, and clear interrupts */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK );

	/* start the timers */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK | XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

	/* Register Timer handler */
	XIntc_RegisterHandler(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_INTR, (XInterruptHandler)xadapter_timer_handler, 0);
	XIntc_EnableIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}

void platform_enable_interrupts()
{
	Xil_ExceptionEnable();
}

static XIntc intc;

void platform_setup_interrupts()
{
	XIntc *intcp;
	intcp = &intc;

	XIntc_Initialize(intcp, XPAR_INTC_0_DEVICE_ID);
	XIntc_Start(intcp, XIN_REAL_MODE);

	platform_setup_timer();
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XIntc_InterruptHandler, intcp);
	XIntc_EnableIntr(XPAR_MICROBLAZE_0_INTC_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK | XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK);
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
	XIntc_Enable(intcp, XPAR_INTC_0_EMACLITE_0_VEC_ID);
}

void enable_caches()
{
	microblaze_invalidate_icache();
	microblaze_enable_icache();
	microblaze_invalidate_dcache();
	microblaze_enable_dcache();
}

void disable_caches()
{
	microblaze_invalidate_dcache();
	microblaze_disable_dcache();
	microblaze_invalidate_icache();
	microblaze_disable_icache();
}

int init_platform()
{
	enable_caches();

	platform_setup_interrupts();

	/* initialize file system layer */
	if (platform_init_fs() < 0)
	{
		return -1;
	}
	return 0;
}

void cleanup_platform()
{
	disable_caches();
}

int init_spi(XSpi * spi)
{
	XSpi_Initialize(spi, XPAR_SPI_DEVICE_ID);
	XSpi_SetOptions(spi, XSP_MANUAL_SSELECT_OPTION | XSP_MASTER_OPTION);
	XSpi_Start(spi);
	XSpi_IntrGlobalDisable(spi);
	XSpi_SetSlaveSelect(spi, 1);
	return 0;
}

u32 read_spi(XSpi * spi)
{
	u8 dataTx[2] = {0x68, 0};
	u8 dataRx[2] = {0};
	u32 ret_value = 0;

	XSpi_Transfer(spi, dataTx, dataRx, 2);
	ret_value = ((dataRx[0] << 8 | dataRx[1]) & 0x3FF);
	if(ret_value > 1024)
	{
		ret_value = 1024;
	}
	else if(ret_value < 0)
	{
		ret_value = 0;
	}

	return (u32) ret_value;
}

