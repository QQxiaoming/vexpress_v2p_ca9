#include "qemu_virio.h"
#include "debug_log.h"

typedef struct {
	__IOM uint32_t MagicValue;
	__IOM uint32_t Version;
	__IOM uint32_t DeviceID;
	__IOM uint32_t VendorID;
	__IOM uint32_t DeviceFeatures;
	__IOM uint32_t DeviceFeaturesSel;
	__IOM uint32_t _reserved0[2];
	__IOM uint32_t DriverFeatures;
	__IOM uint32_t DriverFeaturesSel;
	__IOM uint32_t _reserved1[2];
	__IOM uint32_t QueueSel;
	__IOM uint32_t QueueNumMax;
	__IOM uint32_t QueueNum;
	__IOM uint32_t _reserved2[2];
	__IOM uint32_t QueueReady;
	__IOM uint32_t _reserved3[2];
	__IOM uint32_t QueueNotify;
	__IOM uint32_t _reserved4[3];
	__IOM uint32_t InterruptStatus;
	__IOM uint32_t InterruptACK;
	__IOM uint32_t _reserved5[2];
	__IOM uint32_t Status;
	__IOM uint32_t _reserved6[3];
	__IOM uint32_t QueueDescLow;
	__IOM uint32_t QueueDescHigh;
	__IOM uint32_t _reserved7[2];
	__IOM uint32_t QueueAvailLow;
	__IOM uint32_t QueueAvailHigh;
	__IOM uint32_t _reserved8[2];
	__IOM uint32_t QueueUsedLow;
	__IOM uint32_t QueueUsedHigh;
	__IOM uint32_t _reserved9[21];
	__IOM uint32_t ConfigGeneration;
	__IOM uint32_t Config[0];
} virtio_regs;

#define VIRIO_MMIO           ((virtio_regs *)PERIPH_PA_TO_VA(QEMU_VIRIO_MMIO_BASE))

void virio_mmio_info(void)
{
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->MagicValue 0x%x\n",VIRIO_MMIO->MagicValue);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->Version 0x%x\n",VIRIO_MMIO->Version);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DeviceID 0x%x\n",VIRIO_MMIO->DeviceID);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->VendorID 0x%x\n",VIRIO_MMIO->VendorID);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DeviceFeatures 0x%x\n",VIRIO_MMIO->DeviceFeatures);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DeviceFeaturesSel 0x%x\n",VIRIO_MMIO->DeviceFeaturesSel);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DriverFeatures 0x%x\n",VIRIO_MMIO->DriverFeatures);
    debug_logdebug(LOG_SYS_INFO,"VIRIO_MMIO->DriverFeaturesSel 0x%x\n",VIRIO_MMIO->DriverFeaturesSel);
}