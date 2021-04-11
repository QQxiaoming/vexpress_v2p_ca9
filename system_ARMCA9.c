#include "core_ca.h"
#include "irq_ctrl.h"

/*----------------------------------------------------------------------------
    System Initialization
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{
    /* do not use global variables because this function is called before
     reaching pre-main. RW section may be overwritten afterwards.          */
    // Invalidate entire Unified TLB
    __set_TLBIALL(0);

    // Invalidate entire branch predictor array
    __set_BPIALL(0);
    __DSB();
    __ISB();

    //  Invalidate instruction cache and flush branch target cache
    __set_ICIALLU(0);
    __DSB();
    __ISB();

    //  Invalidate data cache
    L1C_InvalidateDCacheAll();

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    // Enable FPU
    __FPU_Enable();
#endif

    // Create Translation Table
    extern void MMU_CreateTranslationTable(void);
    MMU_CreateTranslationTable();

    // Enable MMU
    MMU_Enable();

    // Enable Caches
    L1C_EnableCaches();
    L1C_EnableBTAC();

#if (__L2C_PRESENT == 1) 
    // Enable GIC
    //L2C_Enable();
#endif
    
    // IRQ Initialize
    IRQ_Initialize();
}
