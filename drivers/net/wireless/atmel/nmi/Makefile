CONFIG_NMC1XXX=m
CONFIG_NMC1XXX_SDIO=y
#CONFIG_NMC1XXX_PREALLOCATE_DURING_SYSTEM_BOOT=y

MODULE_NAME = nmc1000

NMC1000_STA_FW_VERSION = STA_FIRMWARE=\"wilc1000_fw.bin\"
NMC1000_AP_FW_VERSION = AP_FIRMWARE=\"wilc1000_ap_fw.bin\"
NMC1000_P2P_FIRMWARE = P2P_CONCURRENCY_FIRMWARE=\"wilc1000_p2p_fw.bin\"

EXTRA_CFLAGS += -D$(NMC1000_STA_FW_VERSION)
EXTRA_CFLAGS += -D$(NMC1000_AP_FW_VERSION)
EXTRA_CFLAGS += -D$(NMC1000_P2P_FIRMWARE)

obj-$(CONFIG_NMC1XXX) += $(MODULE_NAME).o
obj-$(CONFIG_NMC1XXX_PREALLOCATE_DURING_SYSTEM_BOOT) += src/nmi_memory/nmc1000_exported_buf.o

EXTRA_CFLAGS += -DEXPORT_SYMTAB
EXTRA_CFLAGS += -D__CHECK_ENDIAN__

ifeq ($(strip $(CONFIG_NMC1XXX_SDIO)), y)
	EXTRA_CFLAGS += -DNMI_SDIO
	#EXTRA_CFLAGS += -DNMI_OPTIMIZE_SLEEP_INT	## uncommented in wilc3000 release1.0
	$(MODULE_NAME)-objs 	+= src/driver/linux/source/linux_wlan_sdio.o

	ifeq ($(strip $(CONFIG_NMC1XXX_HW_OOB_INTR)),y)
		EXTRA_CFLAGS += -DNMI_SDIO_IRQ_GPIO
	endif
else
	EXTRA_CFLAGS += -DNMI_SPI
	$(MODULE_NAME)-objs 	+= src/driver/linux/source/linux_wlan_spi.o
endif

EXTRA_CFLAGS += -DNMC_ASIC_A0
EXTRA_CFLAGS += -DPLL_WORKAROUND
EXTRA_CFLAGS += -DCONNECT_DIRECT
EXTRA_CFLAGS += -DNMI_PARSE_SCAN_IN_HOST

#EXTRA_CFLAGS += -DTCP_ACK_FILTER
EXTRA_CFLAGS += -DAGING_ALG
EXTRA_CFLAGS += -DDISABLE_PWRSAVE_AND_SCAN_DURING_IP

EXTRA_CFLAGS += -DNMI_PLATFORM=NMI_LINUXKERNEL  
EXTRA_CFLAGS += -Wno-unused-function
EXTRA_CFLAGS += -DUSE_WIRELESS

EXTRA_CFLAGS += -DSTATIC_MACADDRESS
EXTRA_CFLAGS += -DCOMPLEMENT_BOOT

################################################## uncertain
EXTRA_CFLAGS += -DNMI_SLEEP_CLK_SRC_XO
#EXTRA_CFLAGS += -DNMI_SLEEP_CLK_SRC_RTC
EXTRA_CFLAGS += -DNMI_EXT_PA_INV_TX_RX
EXTRA_CFLAGS += -DNMI_DISABLE_PMU
############################################################# related on BLUETOOTH
EXTRA_CFLAGS += -DNMC1000_BT_COEXISTENCE
EXTRA_CFLAGS += -DDOWNLOAD_BT_FW
#############################################################

ifeq ($(strip $(CONFIG_NMC1XXX_PREALLOCATE_DURING_SYSTEM_BOOT)),y)
	EXTRA_CFLAGS += -DNMC_PREALLOC_AT_BOOT
	EXTRA_CFLAGS += -DMEMORY_STATIC
else
	ifeq ($(strip $(CONFIG_NMC1XXX_PREALLOCATE_AT_LOADING_DRIVER)),y)
		EXTRA_CFLAGS += -DMEMORY_STATIC
		EXTRA_CFLAGS += -DNMC_PREALLOC_AT_INSMOD
	else
		ifeq ($(strip $(CONFIG_NMC1XXX_DYNAMICALLY_ALLOCATE_MEMROY)),y)
			EXTRA_CFLAGS += -DNMC_NORMAL_ALLOC
		endif
	endif
endif


EXTRA_CFLAGS += -I$(src)/src/NMI_OsWrapper/include/ 
EXTRA_CFLAGS += -I$(src)/src/
EXTRA_CFLAGS += -I$(src)/src/driver/include/
EXTRA_CFLAGS += -I$(src)/src/driver/
EXTRA_CFLAGS += -I$(src)/src/driver/linux/include/

$(MODULE_NAME)-objs += src/driver/linux/source/NMI_WFI_NetDevice.o
$(MODULE_NAME)-objs += src/driver/linux/source/NMI_WFI_CfgOperations.o
$(MODULE_NAME)-objs += src/driver/linux/source/linux_wlan.o
$(MODULE_NAME)-objs += src/driver/linux/source/linux_mon.o

$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_Memory.o
$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_MsgQueue.o
$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_Semaphore.o
$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_Sleep.o
$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_StrUtils.o
$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_Thread.o
$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_Time.o
$(MODULE_NAME)-objs += src/NMI_OsWrapper/source/linuxkernel/source/NMI_Timer.o

$(MODULE_NAME)-objs += src/driver/core_configurator/CoreConfigurator.o
$(MODULE_NAME)-objs += src/driver/nmi_driver_interface/host_interface.o
$(MODULE_NAME)-objs += src/driver/nmi_sdio.o
$(MODULE_NAME)-objs += src/driver/nmi_spi.o
$(MODULE_NAME)-objs += src/driver/nmi_wlan_cfg.o

WILC1000_SRC_VERSION = 10.0
PATCHLEVEL = 0
WILC1000_FW_VERSION = 0

EXTRA_CFLAGS += -D__DRIVER_VERSION__=\"$(WILC1000_SRC_VERSION).$(PATCHLEVEL)\"