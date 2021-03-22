#include "core_ca.h"
#include "irq_ctrl.h"
#include "debug_log.h"
#include "v2p_uart.h"
#include <math.h>
#include <string.h>

static inline void delay(uint32_t loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b" : "=r" (loops) : "0" (loops));
}

int main()
{
    uint32_t mpidr = __get_MPIDR();

    pl01x_init(V2P_CA9_MP_UART0_BASE,115200);

    debug_logdebug(LOG_SYS_INFO,"current MPIDR 0x%x\n",mpidr&0xFFF);

    //*(volatile uint32_t *)(0x10000000+0x34) = ~0x0;
    //*(volatile uint32_t *)(0x10000000+0x30) = (uint32_t)SMPLowLiveInit;
    //debug_logdebug(LOG_SYS_INFO,"debug 0x%x\n",*(volatile uint32_t *)(0x10000000));
    //debug_logdebug(LOG_SYS_INFO,"debug 0x%x\n",*(volatile uint32_t *)(0x10000000+0x34));
    //debug_logdebug(LOG_SYS_INFO,"debug 0x%x\n",*(volatile uint32_t *)(0x10000000+0x30));
    
    for(;;)
    {
    }
}

void SMPLowLiveInit()
{
    uint32_t mpid = __get_MPIDR()&0xFFF;
    if(mpid == 0x0)
    {
        return;
    }
    else
    {
        while(1)
        {
		    asm("wfe\n");
        }
    }
}
