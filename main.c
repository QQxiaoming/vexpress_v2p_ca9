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

void task1(void *p_arg)
{ 
    int time = 0;
    for(;;)
    {
        debug_logdebug(LOG_SYS_INFO,"task1 0x%x\n",time++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task2(void *p_arg)
{ 
    int time = 0;
    for(;;)
    {
        debug_logdebug(LOG_SYS_INFO,"task2 0x%x\n",time++);
        extern uint32_t rust_add(uint32_t,uint32_t);
        extern void test_rust_uart(void);
        debug_logdebug(LOG_SYS_INFO,"rust test %d\n",rust_add(10,time));
        test_rust_uart();
        debug_logdebug(LOG_SYS_INFO,"0x80008000 %x\n",*(volatile uint32_t *)(0x80008000));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void vTaskCreate (void *p_arg)
{ 
    xTaskCreate(task1,"task1",256,NULL,4,NULL);
    xTaskCreate(task2,"task2",256,NULL,4,NULL);

    vTaskDelete(NULL);
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
    //TODO:待确认时钟是否配置正确
    PTIM_SetLoadValue(configPERIPHERAL_CLOCK_HZ/configTICK_RATE_HZ);
    PTIM_SetControl((2<<8)|(1<<2)|(1<<1)|(1<<0));
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
    InterruptHandler = IRQ_GetHandler((IRQn_ID_t)ulInterruptID);
    if(InterruptHandler != NULL)
    {
        InterruptHandler();
    }
}
