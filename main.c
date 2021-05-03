#include "core_ca.h"
#include "irq_ctrl.h"
#include "debug_log.h"
#include "v2p_uart.h"
#include "v2p_core.h"
#include "qemu_virio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <math.h>
#include <string.h>

unsigned int smp_start_flag = 0;

static inline void delay(uint32_t loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b" : "=r" (loops) : "0" (loops));
}

static void vTaskCreate (void *p_arg)
{ 
    int time = 0;
    for(;;)
    {
        //debug_logdebug(LOG_SYS_INFO,"debug 0x%x\n",time++);
        //vTaskDelay(pdMS_TO_TICKS(1000));
        debug_logdebug(LOG_SYS_INFO,"IRQ_GetPending 0x%x\n",IRQ_GetPending(PrivTimer_IRQn));
        debug_logdebug(LOG_SYS_INFO,"PTIM_GetEventFlag 0x%x\n",PTIM_GetEventFlag());
        for(int i=0;i<5000;i++) delay(10000);
    }
}

int main()
{
    uint32_t mpid = __get_MPIDR()&0xFFF;
    pl01x_init(V2P_CA9_MP_UART0_BASE,115200);
    debug_logdebug(LOG_SYS_INFO,"this core MPIDR 0x%x\n",mpid);
    for(int i=0;i<1000;i++) delay(100000);
    smp_start_flag = mpid+1;
    L1C_CleanDCacheAll();
    asm volatile("sev");
    for(int i=0;i<5000;i++) delay(100000);

    IRQ_SetPriorityGroupBits(7);

    xTaskCreate(vTaskCreate,"task creat",256,NULL,4,NULL);

    vTaskStartScheduler();
}

void SMPLowLiveInit(void)
{
    uint32_t mpid = __get_MPIDR()&0xFFF;
    if(mpid == 0x0)
    {
        return;
    }
    else
    {
        asm volatile("wfe");

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
}

void vConfigureTickInterrupt(void) 
{
    IRQ_SetHandler(PrivTimer_IRQn,FreeRTOS_Tick_Handler);
    uint32_t mode = (~IRQ_MODE_TRIG_Msk)&IRQ_GetMode(PrivTimer_IRQn);
    IRQ_SetMode(PrivTimer_IRQn, mode|IRQ_MODE_TRIG_EDGE_RISING);
    IRQ_Enable(PrivTimer_IRQn);
    PTIM_SetCurrentValue(0);
    PTIM_SetLoadValue(300000000);
    PTIM_SetControl((1<<8)|(1<<2)|(1<<1)|(1<<0));
}

void vClearTickInterrupt(void) 
{ 
    PTIM_ClearEventFlag(); 
}

void vApplicationFPUSafeIRQHandler( uint32_t ulICCIAR )
{
    uint32_t ulInterruptID;
    IRQHandler_t InterruptHandler = NULL;

	/* Re-enable interrupts. */
	__asm ( "cpsie i" );

	/* The ID of the interrupt is obtained by bitwise anding the ICCIAR value
	with 0x3FF. */
	ulInterruptID = ulICCIAR & 0x3FFUL;
    debug_logdebug(LOG_SYS_INFO,"ulInterruptID %d\n",ulInterruptID);
    InterruptHandler = IRQ_GetHandler((IRQn_ID_t)ulInterruptID);
    if(InterruptHandler != NULL)
    {
        InterruptHandler();
    }
}