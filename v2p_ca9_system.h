#ifndef V2P_CA9_SYSTEM_H
#define V2P_CA9_SYSTEM_H

#define __CHECK_DEVICE_DEFINES

/* Peripheral and RAM base address */
#define V2P_CA9_MP_CLCDC_BASE               (0x10020000UL) /*!< CLCDC configuration registers (PL111)            */
#define V2P_CA9_MP_AXISRAM_BASE             (0x10060000UL) /*!< AXI RAM (2KB)                                    */
#define V2P_CA9_MP_DMC_BASE                 (0x100E0000UL) /*!< Dynamic Memory Controller (PL341)                */
#define V2P_CA9_MP_SMC_BASE                 (0x100E1000UL) /*!< Static Memory Controller (PL354)                 */
#define V2P_CA9_MP_SCC_BASE                 (0x100E2000UL) /*!< System Configuration Controller                  */
#define V2P_CA9_MP_DTIM_BASE                (0x100E4000UL) /*!< Dual Timer module (SP804)                        */
#define V2P_CA9_MP_WDG_BASE                 (0x100E5000UL) /*!< Watchdog module (SP805)                          */
#define V2P_CA9_MP_TZPC_BASE                (0x100E6000UL) /*!< TrustZone Protection Controller (BP147)          */
#define V2P_CA9_MP_FAXIM_BASE               (0x100E9000UL) /*!< 'Fast' AXI matrix (PL301)                        */
#define V2P_CA9_MP_SAXIM_BASE               (0x100EA000UL) /*!< 'Slow' AXI matrix (PL301)                        */
#define V2P_CA9_MP_SMC_TZASC_BASE           (0x100EC000UL) /*!< SMC_TZASC                                        */
#define V2P_CA9_MP_CORESIGHT_APB_BASE       (0x10200000UL) /*!< CoreSight debug APB                              */
#define V2P_CA9_MP_CORE0_DEBUG_APB_BASE     (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00110000UL) /*!<            */
#define V2P_CA9_MP_CORE0_PMU_APB_BASE       (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00111000UL) /*!<            */
#define V2P_CA9_MP_CORE1_DEBUG_APB_BASE     (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00112000UL) /*!<            */
#define V2P_CA9_MP_CORE1_PMU_APB_BASE       (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00113000UL) /*!<            */
#define V2P_CA9_MP_CORE2_DEBUG_APB_BASE     (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00114000UL) /*!<            */
#define V2P_CA9_MP_CORE2_PMU_APB_BASE       (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00115000UL) /*!<            */
#define V2P_CA9_MP_CORE3_DEBUG_APB_BASE     (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00116000UL) /*!<            */
#define V2P_CA9_MP_CORE3_PMU_APB_BASE       (V2P_CA9_MP_CORESIGHT_APB_BASE + 0x00117000UL) /*!<            */
#define V2P_CA9_MP_SCU_BASE                 (0x1E000000UL) /*!< Cortex-A9 MPCore (SCU) private memory region     */
#define V2P_CA9_MP_GIC_INTERFACE_BASE       (V2P_CA9_MP_SCU_BASE + 0x00000100UL) /*!< (GIC CPU IF ) Base Address */
#define V2P_CA9_MP_GLOBA_TIMER_BASE         (V2P_CA9_MP_SCU_BASE + 0x00000200UL) /*!< (GBTIM      ) Base Address */
#define V2P_CA9_MP_PRIVATE_TIMER            (V2P_CA9_MP_SCU_BASE + 0x00000600UL) /*!< (PTIM       ) Base Address */
#define V2P_CA9_MP_GIC_DISTRIBUTOR_BASE     (V2P_CA9_MP_SCU_BASE + 0x00001000UL) /*!< (GIC DIST   ) Base Address */
#define V2P_CA9_MP_PL310_BASE               (0x1E00A000UL) /*!< L2CC config (PL310)                              */

#define V2P_CA9_MP_PERIPH_BASE_CS7          (0x10000000UL) 
#define V2P_CA9_MP_SYSTEM_REG_BASE          (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00000000UL) /*!< System registers Custom */
#define V2P_CA9_MP_SYSTEM_CTL_BASE          (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00001000UL) /*!< System control ARM SP810 */
#define V2P_CA9_MP_PCI_BASE                 (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00002000UL) /*!< Serial Bus PCI Custom */
#define V2P_CA9_MP_AACI_BASE                (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00004000UL) /*!< AACI ARM PL041 */
#define V2P_CA9_MP_MMCI_BASE                (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00005000UL) /*!< MMCI ARM PL180 */
#define V2P_CA9_MP_KMI0_BASE                (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00006000UL) /*!< KMI ARM PL050 */
#define V2P_CA9_MP_KMI1_BASE                (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00007000UL) /*!< KMI ARM PL050 */
#define V2P_CA9_MP_UART0_BASE               (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00009000UL) /*!< UART ARM PL011 */
#define V2P_CA9_MP_UART1_BASE               (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x0000A000UL) /*!< UART ARM PL011 */
#define V2P_CA9_MP_UART2_BASE               (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x0000B000UL) /*!< UART ARM PL011 */
#define V2P_CA9_MP_UART3_BASE               (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x0000C000UL) /*!< UART ARM PL011 */
#define V2P_CA9_MP_WDT_BASE                 (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x0000F000UL) /*!< WDT SP805 */
#define V2P_CA9_MP_TIMER0_1_BASE            (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00011000UL) /*!< TIMER ARM SP804 */
#define V2P_CA9_MP_TIMER2_3_BASE            (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00012000UL) /*!< TIMER ARM SP804 */
#define V2P_CA9_MP_DVI_BASE                 (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00016000UL) /*!< Serial Bus DVI Custom */
#define V2P_CA9_MP_RTC_BASE                 (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x00017000UL) /*!< RTC ARM PL031 */
#define V2P_CA9_MP_COMPACT_FLASH_BASE       (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x0001A000UL) /*!< Compact Flash Custom */
#define V2P_CA9_MP_CLCD_BASE                (V2P_CA9_MP_PERIPH_BASE_CS7 + 0x0001F000UL) /*!< CLCD control ARM PL111 */

#define V2P_CA9_MP_NOR_FLASH0_BASE          (0x40000000UL) /*!< NOR Flash */
#define V2P_CA9_MP_NOR_FLASH1_BASE          (0x44000000UL) /*!< NOR Flash */
#define V2P_CA9_MP_USER_SRAM_BASE           (0x48000000UL) /*!< User SRAM */
#define V2P_CA9_MP_VIDEO_SRAM_BASE          (0x4C000000UL) /*!< Video SRAM */
#define V2P_CA9_MP_ETHERNET_BASE            (0x4E000000UL) /*!< Ethernet SMSC LAN9118 */
#define V2P_CA9_MP_USB_BASE                 (0x4F000000UL) /*!< USB Philips ISP1761 */

#define V2P_CA9_MP_DDR2_LOW_BASE            (0x60000000UL) /*!<  */
#define V2P_CA9_MP_DDR2_HIGHT_BASE          (0x80000000UL) /*!<  */
#define V2P_CA9_MP_EXT_HSB_AXI_BASE         (0xE0000000UL) /*!< External HSB AXI */

#define GIC_DISTRIBUTOR_BASE                V2P_CA9_MP_GIC_DISTRIBUTOR_BASE
#define GIC_INTERFACE_BASE                  V2P_CA9_MP_GIC_INTERFACE_BASE
#define TIMER_BASE                          V2P_CA9_MP_PRIVATE_TIMER
#define L2C_310_BASE                        V2P_CA9_MP_PL310_BASE
#define IRQ_GIC_LINE_COUNT                  (64U)//TODO

/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV        0x0000U    /*!< Core revision r0p0                          */
#define __CORTEX_A           9U    /*!< Cortex-A9 Core                              */
#define __FPU_PRESENT        1U    /* FPU present                                   */
#define __GIC_PRESENT        1U    /* GIC present                                   */
#define __TIM_PRESENT        1U    /* TIM present                                   */
#define __L2C_PRESENT        1U    /* L2C present                                   */


typedef enum IRQn
{
/******  SGI Interrupts Numbers                 ****************************************/
  SGI0_IRQn            =  0,        /*!< Software Generated Interrupt 0 */
  SGI1_IRQn            =  1,        /*!< Software Generated Interrupt 1 */
  SGI2_IRQn            =  2,        /*!< Software Generated Interrupt 2 */
  SGI3_IRQn            =  3,        /*!< Software Generated Interrupt 3 */
  SGI4_IRQn            =  4,        /*!< Software Generated Interrupt 4 */
  SGI5_IRQn            =  5,        /*!< Software Generated Interrupt 5 */
  SGI6_IRQn            =  6,        /*!< Software Generated Interrupt 6 */
  SGI7_IRQn            =  7,        /*!< Software Generated Interrupt 7 */
  SGI8_IRQn            =  8,        /*!< Software Generated Interrupt 8 */
  SGI9_IRQn            =  9,        /*!< Software Generated Interrupt 9 */
  SGI10_IRQn           = 10,        /*!< Software Generated Interrupt 10 */
  SGI11_IRQn           = 11,        /*!< Software Generated Interrupt 11 */
  SGI12_IRQn           = 12,        /*!< Software Generated Interrupt 12 */
  SGI13_IRQn           = 13,        /*!< Software Generated Interrupt 13 */
  SGI14_IRQn           = 14,        /*!< Software Generated Interrupt 14 */
  SGI15_IRQn           = 15,        /*!< Software Generated Interrupt 15 */

/******  Cortex-A9 Processor Exceptions Numbers ****************************************/
  GlobalTimer_IRQn     = 27,        /*!< Global Timer Interrupt                        */
  PrivTimer_IRQn       = 29,        /*!< Private Timer Interrupt                       */
  PrivWatchdog_IRQn    = 30,        /*!< Private Watchdog Interrupt                    */

/******  Platform Exceptions Numbers ***************************************************/
  Watchdog_IRQn        = 32,        /*!< SP805 Interrupt        */
  Timer0_IRQn          = 34,        /*!< SP804 Interrupt        */
  Timer1_IRQn          = 35,        /*!< SP804 Interrupt        */
  RTClock_IRQn         = 36,        /*!< PL031 Interrupt        */
  UART0_IRQn           = 37,        /*!< PL011 Interrupt        */
  UART1_IRQn           = 38,        /*!< PL011 Interrupt        */
  UART2_IRQn           = 39,        /*!< PL011 Interrupt        */
  UART3_IRQn           = 40,        /*!< PL011 Interrupt        */
  MCI0_IRQn            = 41,        /*!< PL180 Interrupt (1st)  */
  MCI1_IRQn            = 42,        /*!< PL180 Interrupt (2nd)  */
  AACI_IRQn            = 43,        /*!< PL041 Interrupt        */
  Keyboard_IRQn        = 44,        /*!< PL050 Interrupt        */
  Mouse_IRQn           = 45,        /*!< PL050 Interrupt        */
  CLCD_IRQn            = 46,        /*!< PL111 Interrupt        */
  Ethernet_IRQn        = 47,        /*!< SMSC_91C111 Interrupt  */
  VFS2_IRQn            = 73,        /*!< VFS2 Interrupt         */
} IRQn_Type;

#define INT32_T_MAX (0x7fffffff)
#define INT32_T_MIN (0x80000000)

enum _retval
{
    RETURN_OK = 0,       RET_SUCCESS = RETURN_OK,
    PARA_ERROR = -1,     RET_INVALIDARGMENT = PARA_ERROR,
    RETURN_ERR = -2,     RET_FAIL = RETURN_ERR,
                         RET_MOMEM = -3,
                         RET_READONLY = -4,
                         RET_OUTOFRANGE = -5,
                         RET_TIMEOUT = -6,
                         RET_NOTRANSFEINPROGRESS = -7,
                         
                         RET_UNKNOW = INT32_T_MIN,
};

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;


#endif /* V2P_CA9_SYSTEM_H */
