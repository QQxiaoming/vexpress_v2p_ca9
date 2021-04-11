#include "core_ca.h"
#include "v2p_ca9_system.h"

#define __TTB_BASE                  (V2P_CA9_MP_USER_SRAM_BASE)
#define __TTB_SIZE                  (0x6000)

#define TTB_L1_SIZE                 (0x4000)                           
#define TTB_L2_SIZE                 (0x400)   
#define TTB_BASE                    ((uint32_t*)__TTB_BASE)
#define TTB_L2_BASE_4k_NUM0         (__TTB_BASE + TTB_L1_SIZE + 0*TTB_L2_SIZE)
#define TTB_L2_BASE_64k_NUM1        (__TTB_BASE + TTB_L1_SIZE + 1*TTB_L2_SIZE)

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

    //Create 4GB of faulting entries
    MMU_TTSection(TTB_BASE,0,0,4096,DESCRIPTOR_FAULT);

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
     * Define MMU flat-map regions and attributes
     */
    MMU_TTSection(TTB_BASE,V2P_CA9_MP_USER_SRAM_BASE,V2P_CA9_MP_USER_SRAM_BASE,32,Sect_Normal_RW);  //0x48000000 - 0x48ffffff
    MMU_TTSection(TTB_BASE,V2P_CA9_MP_DDR2_LOW_BASE,V2P_CA9_MP_DDR2_LOW_BASE,512,Sect_Normal_RW);  //0x60000000 - 0x7fffffff
    MMU_TTSection(TTB_BASE,V2P_CA9_MP_DDR2_HIGHT_BASE,V2P_CA9_MP_DDR2_HIGHT_BASE,512,Sect_Normal_RW);  //0x80000000 - 0x9fffffff

    MMU_TTPage64k(TTB_BASE,V2P_CA9_MP_PERIPH_BASE_CS7,V2P_CA9_MP_PERIPH_BASE_CS7,16,Page_L1_64k,(uint32_t *)TTB_L2_BASE_64k_NUM1,DESCRIPTOR_FAULT);
    MMU_TTPage64k(TTB_BASE,V2P_CA9_MP_UART0_BASE,V2P_CA9_MP_UART0_BASE,1,Page_L1_64k,(uint32_t *)TTB_L2_BASE_64k_NUM1,Page_64k_Device_RW);

    // Create (256 * 4k)=1MB faulting entries to cover private address space. Needs to be marked as Device memory
    MMU_TTPage4k(TTB_BASE,__get_CBAR(),__get_CBAR(),256,Page_L1_4k,(uint32_t *)TTB_L2_BASE_4k_NUM0,DESCRIPTOR_FAULT);
    // Define private address space entry.
    MMU_TTPage4k(TTB_BASE,__get_CBAR(),__get_CBAR(),3,Page_L1_4k,(uint32_t *)TTB_L2_BASE_4k_NUM0,Page_4k_Device_RW);
    // Define L2CC entry.  Uncomment if PL310 is present
    MMU_TTPage4k(TTB_BASE,V2P_CA9_MP_PL310_BASE,V2P_CA9_MP_PL310_BASE,1,Page_L1_4k,(uint32_t *)TTB_L2_BASE_4k_NUM0,Page_4k_Device_RW);

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
    __set_TTBR0(__TTB_BASE | 0x48);
    __ISB();

    /* 
     * Set up domain access control register
     * We set domain 0 to Client and all other domains to No Access.
     * All translation table entries specify domain 0 
     */
    __set_DACR(1);
    __ISB();
}
