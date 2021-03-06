# Comment/uncomment the following line to disable/enable debugging
MAKE	= make
RM	= rm
MV  	= mv
ECHO	= echo
CP	= cp

EXTRA_CFLAGS += -DNMC1000_BT_COEXISTENCE

#DEBUG = y
# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
DEBFLAGS = -O -g 
else
DEBFLAGS = -O2
endif

EXTRA_CFLAGS += -DDISABLE_PWRSAVE_AND_SCAN_DURING_IP
EXTRA_CFLAGS += -DAGING_ALG

#Default to NMI=EVAL if NMI isn't defined
ifeq ($(strip $(NMI)),)
NMI=EVAL
endif

ifeq ($(NMI),EVAL)
EXTRA_CFLAGS += -DNMC_ASIC_A0
else ifeq ($(NMI),FPGA)
endif

EXTRA_CFLAGS += -DCONNECT_DIRECT -DNMI_PARSE_SCAN_IN_HOST

ifeq ($(BUS),SDIO)
EXTRA_CFLAGS += -DNMI_SDIO
endif

ifeq ($(SDIO_IRQ),GPIO)
EXTRA_CFLAGS += -DNMI_SDIO_IRQ_GPIO
endif

ifeq ($(DISABLE_PMU),TRUE)
EXTRA_CFLAGS += -DNMI_DISABLE_PMU
endif

EXTRA_CFLAGS += -DDOWNLOAD_BT_FW
#EXTRA_CFLAGS += -DRESCAN_SDIO

ifeq ($(SLEEP_CLK_SRC),XO)
EXTRA_CFLAGS += -DNMI_SLEEP_CLK_SRC_XO
endif

ifeq ($(SLEEP_CLK_SRC),RTC)
EXTRA_CFLAGS += -DNMI_SLEEP_CLK_SRC_RTC
endif

ifeq ($(EXT_PA_INV_TX_RX),TRUE)
EXTRA_CFLAGS += -DNMI_EXT_PA_INV_TX_RX
endif

ifeq ($(TARGET),SIMULATION)
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
TARGET=SIMULATION
EXTRA_CFLAGS += -DSIMULATION -g
OUT_ARCH = x86
else ifeq ($(TARGET),PANDA)
EXTRA_CFLAGS += -DPANDA_BOARD -DUSE_WIRELESS
ifeq ($(ANDR_VER),4.3)
KERNELDIR ?= $(DEV_TREE_PANDA_4_3)/kernel/omap
MAKE_FLAGS := CROSS_COMPILE=$(DEV_TREE_PANDA_4_3)/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/arm-eabi- ARCH=arm
else ifeq ($(ANDR_VER),4.2)
KERNELDIR ?= $(DEV_TREE_PANDA_4_2)/kernel/omap
MAKE_FLAGS := CROSS_COMPILE=$(DEV_TREE_PANDA_4_2)/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi- ARCH=arm
else
KERNELDIR ?= $(DEV_TREE)/kernel/omap
MAKE_FLAGS := CROSS_COMPILE=$(DEV_TREE)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi- ARCH=arm
endif
OUT_ARCH = ARM-3.0
#EXTRA_CFLAGS += -DSIMULATION
else ifeq ($(TARGET),BEAGLE)
KERNELDIR ?= $(DEV_TREE)
OUT_ARCH = ARM-2.6.39
MAKE_FLAGS := CROSS_COMPILE=arm-arago-linux-gnueabi- ARCH=arm
EXTRA_CFLAGS += -DBEAGLE_BOARD
KERNELDIR ?= $(DEV_TREE)
else ifeq ($(TARGET),NM73131)
KERNELDIR ?= $(DEV_TREE_LPC)
OUT_ARCH = ARM-2.6.28.2
MAKE_FLAGS := CROSS_COMPILE=arm-linux- ARCH=arm
EXTRA_CFLAGS += -DNM73131_0_BOARD
else ifeq ($(TARGET),ALLWINNER)
KERNELDIR ?= $(DEV_TREE)/linux-3.0
MAKE_FLAGS := CROSS_COMPILE=$(DEV_TREE)/buildroot/output/external-toolchain/bin/arm-none-linux-gnueabi- ARCH=arm
EXTRA_CFLAGS += -DALLWINNER_BOARD -DUSE_WIRELESS
OUT_ARCH = ARM-3.0
else ifeq ($(TARGET),ALLWINNER_A20)
KERNELDIR ?= $(DEV_TREE)/linux-3.3
MAKE_FLAGS := CROSS_COMPILE=$(DEV_TREE)/out/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi- ARCH=arm
EXTRA_CFLAGS += -DPLAT_ALLWINNER_A20 -DUSE_WIRELESS
OUT_ARCH = ARM-3.3
else ifeq ($(TARGET),ALLWINNER_A23)
KERNELDIR ?= $(DEV_TREE)/linux-3.4
MAKE_FLAGS := CROSS_COMPILE=$(DEV_TREE)/out/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi- ARCH=arm
EXTRA_CFLAGS += -DPLAT_ALLWINNER_A23 -DUSE_WIRELESS
OUT_ARCH = ARM-3.4
else ifeq ($(TARGET),ALLWINNER_A31)
KERNELDIR ?= $(DEV_TREE)/linux-3.3
MAKE_FLAGS := CROSS_COMPILE=$(DEV_TREE)/buildroot/output/external-toolchain/bin/arm-linux-gnueabi- ARCH=arm
EXTRA_CFLAGS += -DPLAT_ALLWINNER_A31 -DUSE_WIRELESS
OUT_ARCH = ARM-3.3
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
OUT_ARCH = x86
TARGET=SIMULATION
EXTRA_CFLAGS += -DSIMULATION
endif

ifeq ($(FIRMLOG),HOST)
	EXTRA_CFLAGS += -DSWITCH_LOG_TERMINAL
endif

EXTRA_CFLAGS += $(DEBFLAGS) -I $(PWD)/../../NMI_OsWrapper/include/ 
EXTRA_CFLAGS += -I $(PWD)/../../ -I $(PWD)/../include -I $(PWD)/../ -I $(PWD)/include
EXTRA_CFLAGS += -DNMI_PLATFORM=NMI_LINUXKERNEL  
EXTRA_CFLAGS += -Wno-unused-function



obj-m	 		:= nmi_wifi.o
ifeq ($(TARGET),NM73131)
ifeq ($(BUS),SDIO)
nmi_wifi-objs 	:= source/linux_wlan.o source/linux_wlan_sdio.o\
source/nmi_debugfs.o source/nmi_queue.o\
../binary/linux/$(OUT_ARCH)/NMI_WiFi_Driver.a\
../../NMI_OsWrapper/binary/linux/$(OUT_ARCH)/NMI_OsWrapper.a
else
nmi_wifi-objs 	:= source/linux_wlan.o source/linux_wlan_spi.o\
source/nmi_debugfs.o source/nmi_queue.o\
../binary/linux/$(OUT_ARCH)/NMI_WiFi_Driver.a\
../../NMI_OsWrapper/binary/linux/$(OUT_ARCH)/NMI_OsWrapper.a
endif

else

ifeq ($(BUS),SDIO)
nmi_wifi-objs 	:= source/NMI_WFI_NetDevice.o source/NMI_WFI_CfgOperations.o\
source/linux_wlan.o source/linux_wlan_sdio.o source/linux_mon.o\
source/nmi_debugfs.o source/nmi_queue.o\
../binary/linux/$(OUT_ARCH)/NMI_WiFi_Driver.a\
../../NMI_OsWrapper/binary/linux/$(OUT_ARCH)/NMI_OsWrapper.a
else
nmi_wifi-objs 	:= source/NMI_WFI_NetDevice.o source/NMI_WFI_CfgOperations.o\
source/linux_wlan.o source/linux_wlan_spi.o source/linux_mon.o\
source/nmi_debugfs.o source/nmi_queue.o\
../binary/linux/$(OUT_ARCH)/NMI_WiFi_Driver.a\
../../NMI_OsWrapper/binary/linux/$(OUT_ARCH)/NMI_OsWrapper.a
endif

endif

ifneq ($(KERNELRELEASE),)
# call from kernel build system
  

else
  
#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)
  
default:
	echo KERNELDIR = $(KERNELDIR)
	$(MAKE) TARGET=$(TARGET) NMI=$(NMI) BUS=$(BUS) FULLY_HOSTING_AP=$(FULLY_HOSTING_AP) -C ../
	@$(ECHO) "###########################################"
	@$(ECHO) "##### Building NMI WiFi for Linux ######"
	@$(ECHO) "##### TARGET =" $(TARGET) "######"
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules $(MAKE_FLAGS)
	mkdir -p binary/linux/$(OUT_ARCH)
	$(MV) -f $(PWD)/nmi_wifi.ko binary/linux/$(OUT_ARCH)/nmi_wifi.ko
  
endif
  
clean:
	$(MAKE) TARGET=$(TARGET) NMI=$(NMI) BUS=$(BUS) -C ../ clean
	@$(ECHO) "###########################################"
	@$(ECHO) "##### Cleaning NMI WiFi for Linux ######"
	@$(ECHO) "##### TARGET =" $(TARGET) "######"
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) clean
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
	rm -rf binary/linux/$(OUT_ARCH)/nmi_wifi.ko
  
depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend
  
  
ifeq (.depend,$(wildcard .depend))
include .depend
endif
