MODULE_NAME=wilc3000
obj-$(CONFIG_WILC3000) += ${MODULE_NAME}.o
${MODULE_NAME}-y := main.o test.o

ccflags-y := -I$(src)/

ifeq ($(strip $(CONFIG_WILC1000_PREALLOCATE_DURING_SYSTEM_BOOT)),y)
	ccflags-y += -DWILC_PREALLOC_AT_BOOT
    ccflags-y += -DMEMORY_STATIC
else
    ifeq ($(strip $(CONFIG_WILC1000_PREALLOCATE_AT_LOADING_DRIVER)),y)
    	ccflags-y += -DMEMORY_STATIC
		ccflags-y += -DWILC_PREALLOC_AT_INSMOD
	else
		ifeq ($(strip $(CONFIG_WILC1000_DYNAMICALLY_ALLOCATE_MEMROY)),y)
	 		ccflags-y += -DWILC_NORMAL_ALLOC
	 	endif
	endif
endif
