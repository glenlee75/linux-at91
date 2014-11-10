#include "NMI_WFI_NetDevice.h"

#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/host.h>



#if defined (NM73131_0_BOARD)
#define SDIO_MODALIAS "nmi_sdio"
#else
#define SDIO_MODALIAS "nmc1000_sdio"
#endif

#ifdef NMC_ASIC_A0
#if defined (NM73131_0_BOARD)
#define MAX_SPEED 50000000
#elif defined (PLAT_ALLWINNER_A20)
#define MAX_SPEED 45000000 //40000000 //50000000
#else
#define MAX_SPEED 50000000
#endif
#else /* NMC_ASIC_A0 */
/* Limit clk to 6MHz on FPGA. */
#define MAX_SPEED 6000000
#endif /* NMC_ASIC_A0 */


struct sdio_func* local_sdio_func = NULL;
extern linux_wlan_t* g_linux_wlan;
extern int nmc_netdev_init(void);
extern int sdio_clear_int(void);
extern void nmi_handle_isr(void);

static unsigned int sdio_default_speed=0;

#define SDIO_VENDOR_ID_NMI 0x0296
#define SDIO_DEVICE_ID_NMI 0x5347

static const struct sdio_device_id nmi_sdio_ids[] = {
	{ SDIO_DEVICE(SDIO_VENDOR_ID_NMI,SDIO_DEVICE_ID_NMI) },
};

#ifndef NMI_SDIO_IRQ_GPIO
// [[ added by rachel 13-07-18
typedef enum {
	NMI_SDIO_HOST_NO_TAKEN = 0,
	NMI_SDIO_HOST_IRQ_TAKEN = 1,
	NMI_SDIO_HOST_DIS_TAKEN = 2,
} sdio_host_lock;
static sdio_host_lock	sdio_intr_lock = NMI_SDIO_HOST_NO_TAKEN;
static wait_queue_head_t sdio_intr_waitqueue;
// ]]
#endif

static void nmi_sdio_interrupt(struct sdio_func *func)
{
#ifndef NMI_SDIO_IRQ_GPIO
	if(sdio_intr_lock == NMI_SDIO_HOST_DIS_TAKEN)
		return;
	sdio_intr_lock = NMI_SDIO_HOST_IRQ_TAKEN;
	
	sdio_release_host(func);
	nmi_handle_isr();
	sdio_claim_host(func);

	sdio_intr_lock = NMI_SDIO_HOST_NO_TAKEN;
	wake_up_interruptible(&sdio_intr_waitqueue);
#endif
}


int linux_sdio_cmd52(sdio_cmd52_t *cmd){
	struct sdio_func *func = local_sdio_func;
	int ret;
	u8 data;


	sdio_claim_host(func);

	func->num = cmd->function;
	if (cmd->read_write) {	/* write */
		if (cmd->raw) {
			sdio_writeb(func, cmd->data, cmd->address, &ret);
			data = sdio_readb(func, cmd->address, &ret);
			cmd->data = data;
		} else {
			sdio_writeb(func, cmd->data, cmd->address, &ret);
		}
	} else {	/* read */
		data = sdio_readb(func, cmd->address, &ret);
		cmd->data = data;
	}

	sdio_release_host(func);

	if (ret < 0) {
		printk("nmi_sdio_cmd52..failed, err(%d)\n", ret);
		return 0;
	}
	return 1;
}


 int linux_sdio_cmd53(sdio_cmd53_t *cmd){
	struct sdio_func *func = local_sdio_func;
	int size, ret;

	sdio_claim_host(func);

	func->num = cmd->function;
	func->cur_blksize = cmd->block_size;
	if (cmd->block_mode)
		size = cmd->count * cmd->block_size;
	else
		size = cmd->count;

	if (cmd->read_write) {	/* write */
		ret = sdio_memcpy_toio(func, cmd->address, (void *)cmd->buffer, size);		
	} else {	/* read */
		ret = sdio_memcpy_fromio(func, (void *)cmd->buffer, cmd->address,  size);		
	}

	sdio_release_host(func);


	if (ret < 0) {
		printk("nmi_sdio_cmd53..failed, err(%d)\n", ret);
		return 0;
	}

	return 1;
}

volatile int probe = 0;
static int linux_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id){	

	printk("probe function\n");
	
	if(local_sdio_func != NULL)
	{		
		local_sdio_func = func;
		probe = 1;
		return 0;
	}
	PRINT_D(INIT_DBG,"Initializing netdev\n");
	local_sdio_func = func;
	if(nmc_netdev_init()){
		PRINT_ER("Couldn't initialize netdev\n");
		return -1;
	}
	return 0;
}

static void linux_sdio_remove(struct sdio_func *func)
{
	/**
		TODO
	**/
	
}

struct sdio_driver nmc_bus = {
	.name		= SDIO_MODALIAS,
	.id_table	= nmi_sdio_ids,
	.probe		= linux_sdio_probe,
	.remove		= linux_sdio_remove,
};

int enable_sdio_interrupt(void){
	int ret = 0;
#ifndef NMI_SDIO_IRQ_GPIO
	
	sdio_intr_lock  = NMI_SDIO_HOST_NO_TAKEN;
	
	sdio_claim_host(local_sdio_func);
	ret = sdio_claim_irq(local_sdio_func, nmi_sdio_interrupt);
	sdio_release_host(local_sdio_func);

	if (ret < 0) {
		PRINT_ER("can't claim sdio_irq, err(%d)\n", ret);
		ret = -EIO;
	}	
#endif
	return ret;
}

void disable_sdio_interrupt(void){

#ifndef NMI_SDIO_IRQ_GPIO
	int ret;

	if(sdio_intr_lock  == NMI_SDIO_HOST_IRQ_TAKEN ) 
		wait_event_interruptible(sdio_intr_waitqueue, sdio_intr_lock == NMI_SDIO_HOST_NO_TAKEN );
	sdio_intr_lock  = NMI_SDIO_HOST_DIS_TAKEN;

	sdio_claim_host(local_sdio_func);
	ret = sdio_release_irq(local_sdio_func);
	if (ret < 0) {
			PRINT_ER("can't release sdio_irq, err(%d)\n", ret);
		}
	sdio_release_host(local_sdio_func);
	
	sdio_intr_lock  = NMI_SDIO_HOST_NO_TAKEN;
#endif
}

static int linux_sdio_set_speed(int speed)
{
#if 1
	struct mmc_ios ios;
	sdio_claim_host(local_sdio_func);
	
	memcpy((void *)&ios,(void *)&local_sdio_func->card->host->ios,sizeof(struct mmc_ios));
	local_sdio_func->card->host->ios.clock = speed;
	ios.clock = speed;
	local_sdio_func->card->host->ops->set_ios(local_sdio_func->card->host,&ios);
	sdio_release_host(local_sdio_func);
	PRINT_D(INIT_DBG,"@@@@@@@@@@@@ change SDIO speed to %d @@@@@@@@@\n", speed);
#endif
	return 1;
}

static int linux_sdio_get_speed(void)
{
	return local_sdio_func->card->host->ios.clock;
}

int linux_sdio_init(void* pv){
#ifndef NMI_SDIO_IRQ_GPIO
	init_waitqueue_head(&sdio_intr_waitqueue);
#endif
	sdio_default_speed = linux_sdio_get_speed();
	return 1;
}

void linux_sdio_deinit(void *pv){
	sdio_unregister_driver(&nmc_bus);
}

int linux_sdio_set_max_speed(void)
{
	return linux_sdio_set_speed(MAX_SPEED);
}

int linux_sdio_set_default_speed(void)
{
	return linux_sdio_set_speed(sdio_default_speed);
}



