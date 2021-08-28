#include "qemu_virio.h"
#include "debug_log.h"

typedef enum {
	invalid                    = 0,
	network_card               = 1,
	block_device               = 2,
	console                    = 3,
	entropy_source             = 4,
	memory_ballooning          = 5,
	ioMemory                   = 6,
	rpmsg                      = 7,
	SCSI_host                  = 8,
	transport_9p               = 9,
	mac80211_wlan              = 10,
	rproc_serial               = 11,
	virtio_CAIF                = 12,
	memory_balloon             = 13,
	GPU_device                 = 16,
	Timer_device               = 17,
	Input_device               = 18,
	Socket_device              = 19,
	Crypto_device              = 20,
	Signal_Distribution_Module = 21,
	pstore_device              = 22,
	IOMMU_device               = 23,
	Memory_device              = 24,
} Virtio_Device_ID;

typedef struct {
	__IOM uint32_t MagicValue;          /* 0x0  "virt"             */
	__IOM uint32_t Version;             /* 0x4                     */
	__IOM uint32_t DeviceID;            /* 0x8  Virtio_Device_ID   */
	__IOM uint32_t VendorID;            /* 0xC  "QEMU"             */
	__IOM uint32_t DeviceFeatures;      /* 0x10                    */
	__IOM uint32_t DeviceFeaturesSel;   /* 0x14                    */
	__IOM uint32_t _reserved0[2];
	__IOM uint32_t DriverFeatures;      /* 0x20                    */
	__IOM uint32_t DriverFeaturesSel;   /* 0x24                    */
	__IOM uint32_t _reserved1[2];
	__IOM uint32_t QueueSel;            /* 0x30                    */
	__IOM uint32_t QueueNumMax;         /* 0x34                    */
	__IOM uint32_t QueueNum;            /* 0x38                    */
	__IOM uint32_t _reserved2[2];
	__IOM uint32_t QueueReady;			/* 0x44                    */
	__IOM uint32_t _reserved3[2];
	__IOM uint32_t QueueNotify;         /* 0x50                    */
	__IOM uint32_t _reserved4[3];
	__IOM uint32_t InterruptStatus;     /* 0x60                    */
	__IOM uint32_t InterruptACK;        /* 0x64                    */
	__IOM uint32_t _reserved5[2];
	__IOM uint32_t Status;              /* 0x70                    */
	__IOM uint32_t _reserved6[3];
	__IOM uint32_t QueueDescLow;        /* 0x80                    */
	__IOM uint32_t QueueDescHigh;       /* 0x84                    */
	__IOM uint32_t _reserved7[2];
	__IOM uint32_t QueueAvailLow;       /* 0x90                    */
	__IOM uint32_t QueueAvailHigh;      /* 0x94                    */
	__IOM uint32_t _reserved8[2];
	__IOM uint32_t QueueUsedLow;        /* 0xa0                    */
	__IOM uint32_t QueueUsedHigh;       /* 0xa4                    */
	__IOM uint32_t _reserved9[21];
	__IOM uint32_t ConfigGeneration;    /* 0xfc                    */
	__IOM uint32_t Config[0];           /* 0x100+                  */
} virtio_regs;

#define VIRIO_MMIO0           ((virtio_regs *)PERIPH_PA_TO_VA(QEMU_VIRIO_MMIO_BASE))
#define VIRIO_MMIO1           ((virtio_regs *)PERIPH_PA_TO_VA(QEMU_VIRIO_MMIO_BASE+0x200))
#define VIRIO_MMIO2           ((virtio_regs *)PERIPH_PA_TO_VA(QEMU_VIRIO_MMIO_BASE+0x400))
#define VIRIO_MMIO3           ((virtio_regs *)PERIPH_PA_TO_VA(QEMU_VIRIO_MMIO_BASE+0x600))

void virio_mmio_info(void)
{
	virtio_regs *VIRIO_MMIO = VIRIO_MMIO3;
	uint32_t MagicValue = VIRIO_MMIO->MagicValue;
	char * MagicValuePtr = (char *)(&MagicValue);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->MagicValue %c%c%c%c \n",
			MagicValuePtr[0],MagicValuePtr[1], MagicValuePtr[2],MagicValuePtr[3]);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->Version 0x%x\n",VIRIO_MMIO->Version);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DeviceID 0x%x\n",VIRIO_MMIO->DeviceID);
	uint32_t VendorID = VIRIO_MMIO->VendorID;
	char * VendorIDPtr = (char *)(&VendorID);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->VendorID %c%c%c%c \n",
			VendorIDPtr[0],VendorIDPtr[1], VendorIDPtr[2],VendorIDPtr[3]);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DeviceFeatures 0x%x\n",VIRIO_MMIO->DeviceFeatures);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DeviceFeaturesSel 0x%x\n",VIRIO_MMIO->DeviceFeaturesSel);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DriverFeatures 0x%x\n",VIRIO_MMIO->DriverFeatures);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DriverFeaturesSel 0x%x\n",VIRIO_MMIO->DriverFeaturesSel);
	debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueSel 0x%x\n",VIRIO_MMIO->QueueSel);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueNumMax 0x%x\n",VIRIO_MMIO->QueueNumMax);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueNum 0x%x\n",VIRIO_MMIO->QueueNum);
	debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueReady 0x%x\n",VIRIO_MMIO->QueueReady);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueNotify 0x%x\n",VIRIO_MMIO->QueueNotify);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->InterruptStatus 0x%x\n",VIRIO_MMIO->InterruptStatus);
	debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->InterruptACK 0x%x\n",VIRIO_MMIO->InterruptACK);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->Status 0x%x\n",VIRIO_MMIO->Status);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueDescLow 0x%x\n",VIRIO_MMIO->QueueDescLow);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueDescHigh 0x%x\n",VIRIO_MMIO->QueueDescHigh);
	debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueAvailLow 0x%x\n",VIRIO_MMIO->QueueAvailLow);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueAvailHigh 0x%x\n",VIRIO_MMIO->QueueAvailHigh);
	debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueUsedLow 0x%x\n",VIRIO_MMIO->QueueUsedLow);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->QueueUsedHigh 0x%x\n",VIRIO_MMIO->QueueUsedHigh);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->ConfigGeneration 0x%x\n",VIRIO_MMIO->ConfigGeneration);
}