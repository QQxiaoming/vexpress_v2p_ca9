#include "v2p_core.h"

typedef struct
{
    __IOM  uint32_t CONTROL;
    __IOM  uint32_t CONFIG;
    __IOM  uint32_t POWER_STATUS;
    __IOM  uint32_t INVALIDATE;
    RESERVED(0[0xc], uint32_t)
    __IOM  uint32_t FILTERING_START;
    __IOM  uint32_t FILTERING_END;
    RESERVED(1[0x2], uint32_t)
    __IOM  uint32_t NS_ACCESS_CONTROL;
    __IOM  uint32_t S_ACCESS_CONTROL;
} SCU_TypeDef;
#define SCU           ((SCU_TypeDef *)V2P_CA9_MP_SCU_BASE)

typedef struct
{
    __IOM  uint32_t COUNTER_L;
    __IOM  uint32_t COUNTER_H;
    __IOM  uint32_t CONTROL;
    __IOM  uint32_t ISR;
    __IOM  uint32_t COMPARATOR_L;
    __IOM  uint32_t COMPARATOR_H;
    __IOM  uint32_t AUTO_INCREMENT;
} GLOBA_TIMER_TypeDef;
#define GLOBA_TIMER      ((GLOBA_TIMER_TypeDef *)V2P_CA9_MP_GLOBA_TIMER_BASE)

typedef struct
{
    __IOM  uint32_t DIDR;    //0x00
    RESERVED(0[0x5], uint32_t)
    __IOM  uint32_t WFAR;    //0x18
    __IOM  uint32_t VCR;     //0x1c
    RESERVED(1[0x1], uint32_t)
    __IOM  uint32_t ECR;     //0x24
    __IOM  uint32_t DSCCR;   //0x28
    __IOM  uint32_t DSMCR;   //0x2c
    RESERVED(2[0x14], uint32_t)
    __IOM  uint32_t DTRRXext;//0x80
    __IOM  uint32_t ITR_PCSR;//0x84
    __IOM  uint32_t DSCRext; //0x88
    __IOM  uint32_t DTRTXext;//0x8c
    __IOM  uint32_t DRCR;    //0x90
    RESERVED(3[0x3], uint32_t)
    __IOM  uint32_t PCSR;    //0xa0
    __IOM  uint32_t CIDSR;   //0xa4
    RESERVED(4[0x16], uint32_t)
    __IOM  uint32_t BVR[16]; //0x100 - 0x13c
    __IOM  uint32_t BCR[16]; //0x140 - 0x17c
    __IOM  uint32_t WVR[16]; //0x180 - 0x1bc
    __IOM  uint32_t WCR[16]; //0x1c0 - 0x1fc
    RESERVED(5[0x40], uint32_t)
    __IOM  uint32_t OSLAR;   //0x300
    __IOM  uint32_t OSLSR;   //0x304
    __IOM  uint32_t OSSRR;   //0x308
    RESERVED(6[0x1], uint32_t)
    __IOM  uint32_t PRCR;    //0x310
    __IOM  uint32_t PRSR;    //0x314
    RESERVED(7[0x2ba], uint32_t)
    __IOM  uint32_t PID[64]; //0xd00 - 0xdfc
    RESERVED(8[0x7e], uint32_t)
    __IOM  uint32_t ITMISCOUT;//0xef8
    __IOM  uint32_t ITMISCIN;//0xefc
    __IOM  uint32_t ITCTRL;  //0xf00
    RESERVED(9[0x27], uint32_t)
    __IOM  uint32_t CLAIMSET;  //0xfa0
    __IOM  uint32_t CLAIMCLR;  //0xfa4
    RESERVED(10[0x2], uint32_t)
    __IOM  uint32_t LAR;  //0xfb0
    __IOM  uint32_t LSR;  //0xfb4
    __IOM  uint32_t GAUTHSTATUS;  //0xfb8
    RESERVED(11[0x3], uint32_t)
    __IOM  uint32_t DEVID;  //0xfc8
    __IOM  uint32_t DEVTYP; //0xfcc
    __IOM  uint32_t PERIPHERALID[5]; //0xfd0 - 0xfec
    RESERVED(12[0x3], uint32_t)
    __IOM  uint32_t COMPONETID[3]; //0xff0 - 0xffc

} CORE_DEBUG_TypeDef;
#define CORE0_DEBUG       ((CORE_DEBUG_TypeDef *)V2P_CA9_MP_CORE0_DEBUG_APB_BASE)

typedef struct
{
    __IOM  uint32_t XEVCNTR0;   //0x00
    __IOM  uint32_t XEVCNTR1;   //0x04
    RESERVED(0[0x1d], uint32_t)
    __IOM  uint32_t CCNTR;      //0x7c
    RESERVED(1[0xe0], uint32_t)
    __IOM  uint32_t XEVTYPER0;  //0x400
    __IOM  uint32_t XEVTYPER1;  //0x404
    RESERVED(2[0x1d], uint32_t)
    __IOM  uint32_t CCFILTR;    //0x47c
    RESERVED(3[0x1e0], uint32_t)
    __IOM  uint32_t CNTENSET;   //0xc00
    RESERVED(4[0x7], uint32_t)
    __IOM  uint32_t CNTENCLR;   //0xc20
    RESERVED(5[0x7], uint32_t)
    __IOM  uint32_t INTENSET;   //0xc40
    RESERVED(6[0x7], uint32_t)
    __IOM  uint32_t INTENCLR;   //0xc60
    RESERVED(7[0x7], uint32_t)
    __IOM  uint32_t OVSR;       //0xc80
    RESERVED(8[0x7], uint32_t)
    __IOM  uint32_t SWINC;      //0xca0
    RESERVED(9[0x57], uint32_t)
    __IOM  uint32_t CFGR;       //0xe00
    __IOM  uint32_t CR;         //0xe04
    __IOM  uint32_t USERENR;    //0xe08
    RESERVED(10[0x5], uint32_t)
    __IOM  uint32_t CEID0;      //0xe20
    __IOM  uint32_t CEID1;      //0xe24
    RESERVED(11[0x62], uint32_t)
    __IOM  uint32_t LAR;        //0xfb0
    __IOM  uint32_t LSR;        //0xfb4
    __IOM  uint32_t AUTHSTATUS; //0xfb8
    RESERVED(12[0x4], uint32_t)
    __IOM  uint32_t DEVTYPE;    //0xfcc
    __IOM  uint32_t PERIPHERALID[8]; //0xfd0 - 0xfec
    __IOM  uint32_t COMPONENTID[4];  //0xff0 - 0xffc
} CORE_PMU_TypeDef;
#define CORE0_PMU         ((CORE_PMU_TypeDef *)V2P_CA9_MP_CORE0_PMU_APB_BASE)
