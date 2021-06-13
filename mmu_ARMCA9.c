#include "core_ca.h"
#include "v2p_ca9_system.h"

#define TTB_L2_SIZE                 (0x400)   
#define TTB_L2_BASE                 (V2P_CA9_MP_USER_SRAM_BASE)
#define TTB_L2_BASE_4k_NUM0         (TTB_L2_BASE + 0*TTB_L2_SIZE)
#define TTB_L2_BASE_64k_NUM1        (TTB_L2_BASE + 1*TTB_L2_SIZE)

static uint32_t Sect_Normal;        //outer & inner wb/wa, non-shareable, executable, rw, domain 0, base addr 0
static uint32_t Sect_Normal_Cod;    //outer & inner wb/wa, non-shareable, executable, ro, domain 0, base addr 0
static uint32_t Sect_Normal_RO;     //as Sect_Normal_Cod, but not executable
static uint32_t Sect_Normal_RW;     //as Sect_Normal_Cod, but writeable and not executable
static uint32_t Sect_Device_RO;     //device, non-shareable, non-executable, ro, domain 0, base addr 0
static uint32_t Sect_Device_RW;     //as Sect_Device_RO, but writeable

/* Define global descriptors */
static uint32_t Page_L1_4k  = 0x0;  //generic
static uint32_t Page_L1_64k = 0x0;  //generic
static uint32_t Page_4k_Device_RW;  //Shared device, not executable, rw, domain 0
static uint32_t Page_64k_Device_RW; //Shared device, not executable, rw, domain 0

static uint32_t Page_L1_Normal_64k = 0x0;
static uint32_t Page_64k_Normal_RW;

void MMU_CreateTranslationTable(void)
{
    mmu_region_attributes_Type region;

    uint32_t *ttbr0 = (uint32_t *)(DDRMEM_PA_TO_VA(__get_TTBR0()));
    /*
     * Generate descriptors. Refer to core_ca.h to get information about attributes
     */
    //Create descriptors for Vectors, RO, RW, ZI sections
    section_normal(Sect_Normal,region);
    section_normal_cod(Sect_Normal_Cod,region);
    section_normal_ro(Sect_Normal_RO,region);
    section_normal_rw(Sect_Normal_RW,region);
    //Create descriptors for peripherals
    section_device_ro(Sect_Device_RO,region);
    section_device_rw(Sect_Device_RW,region);
    //Create descriptors for 64k pages
    page64k_device_rw(Page_L1_64k,Page_64k_Device_RW,region);
    //Create descriptors for 4k pages
    page4k_device_rw(Page_L1_4k,Page_4k_Device_RW,region);
    page64k_normal_rw(Page_L1_Normal_64k,Page_64k_Normal_RW,region);

    /*
     * remap MMU flat-map regions and attributes
     */
    //清理启动的map
    MMU_TTSection(ttbr0,V2P_CA9_MP_DDR2_LOW_BASE,0,1,DESCRIPTOR_FAULT);    //0x60000000 - 0x600fffff
    
    //外设映射到0xf0000000开始处
    MMU_TTPage64k(ttbr0,
                  PERIPH_PA_TO_VA(V2P_CA9_MP_PERIPH_BASE_CS7),V2P_CA9_MP_PERIPH_BASE_CS7,16,
                  Page_L1_64k,
                  (uint32_t *)TTB_L2_BASE_64k_NUM1,(uint32_t *)SRAMMEM_PA_TO_VA(TTB_L2_BASE_64k_NUM1),
                  DESCRIPTOR_FAULT);
    MMU_TTPage64k(ttbr0,
                  PERIPH_PA_TO_VA(V2P_CA9_MP_UART0_BASE),V2P_CA9_MP_UART0_BASE,1,
                  Page_L1_64k,
                  (uint32_t *)TTB_L2_BASE_64k_NUM1,(uint32_t *)SRAMMEM_PA_TO_VA(TTB_L2_BASE_64k_NUM1),
                  Page_64k_Device_RW);

    MMU_TTPage4k(ttbr0,
                 PERIPH_PA_TO_VA(__get_CBAR()),__get_CBAR(),256,
                 Page_L1_4k,
                 (uint32_t *)TTB_L2_BASE_4k_NUM0,(uint32_t *)SRAMMEM_PA_TO_VA(TTB_L2_BASE_4k_NUM0),
                 DESCRIPTOR_FAULT);
    MMU_TTPage4k(ttbr0,
                 PERIPH_PA_TO_VA(__get_CBAR()),__get_CBAR(),3,
                 Page_L1_4k,
                 (uint32_t *)TTB_L2_BASE_4k_NUM0,(uint32_t *)SRAMMEM_PA_TO_VA(TTB_L2_BASE_4k_NUM0),
                 Page_4k_Device_RW);
    MMU_TTPage4k(ttbr0,
                 PERIPH_PA_TO_VA(V2P_CA9_MP_PL310_BASE),V2P_CA9_MP_PL310_BASE,1,
                 Page_L1_4k,
                 (uint32_t *)TTB_L2_BASE_4k_NUM0,(uint32_t *)SRAMMEM_PA_TO_VA(TTB_L2_BASE_4k_NUM0),
                 Page_4k_Device_RW);

    /* 
     * Set location of level 1 page table
     * 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
     * 13:7  - 0x0
     * 6     - IRGN[0] 0x1  (Inner WB WA)
     * 5     - NOS     0x0  (Non-shared)
     * 4:3   - RGN     0x01 (Outer WB WA)
     * 2     - IMP     0x0  (Implementation Defined)
     * 1     - S       0x0  (Non-shared)
     * 0     - IRGN[1] 0x0  (Inner WB WA) 
     */
    __set_TTBR0((DDRMEM_VA_TO_PA((uint32_t)ttbr0)) | 0x48);
    __ISB();

    /* 
     * Set up domain access control register
     * We set domain 0 to Client and all other domains to No Access.
     * All translation table entries specify domain 0 
     */
    __set_DACR(1);
    __ISB();
}
