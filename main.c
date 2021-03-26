#include "core_ca.h"
#include "irq_ctrl.h"
#include "debug_log.h"
#include "v2p_uart.h"
#include "qemu_virio.h"
#include <math.h>
#include <string.h>

unsigned int smp_start_flag = 0;

static inline void delay(uint32_t loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b" : "=r" (loops) : "0" (loops));
}

int main()
{
    uint32_t mpid = __get_MPIDR()&0xFFF;
    if(mpid == smp_start_flag)
    {
        pl01x_init(V2P_CA9_MP_UART0_BASE,115200);
        debug_logdebug(LOG_SYS_INFO,"this core MPIDR 0x%x\n",mpid);
        for(int i=0;i<1000;i++) delay(100000);
        smp_start_flag = mpid+1;
        asm volatile("sev");
    }
    else
    {
        while (1)
        {
            if(smp_start_flag == mpid)
            {
                debug_logdebug(LOG_SYS_INFO,"this core MPIDR 0x%x\n",mpid);
                for(int i=0;i<1000;i++) delay(100000);
                if(mpid+1 != 4)
                    smp_start_flag = mpid+1;
                else
                    smp_start_flag = 0;
                asm volatile("sev");
                break;
            }
            else
            {
                asm volatile("wfe");
            }
        }
    }

    for(;;)
    {
        if(smp_start_flag == mpid)
        {
            debug_logdebug(LOG_SYS_INFO,"this core %x\n",mpid);
            for(int i=0;i<1000;i++) delay(100000);
            if(mpid+1 != 4)
                smp_start_flag = mpid+1;
            else
                smp_start_flag = 0;
        }
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
        asm volatile("wfe");
    }
}
