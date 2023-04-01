#include <lvgl.h>
#include <droidboot_drivers.h>
#include <droidboot_logging.h>
#include <droidboot_main.h>

#include <common.h>
#include <command.h>
#include <dm.h>
#include <env.h>
#include <lmb.h>
#include <net.h>
#include <stdbool.h>
#include <video.h>
#include <blk.h>
#include <vsprintf.h>
#include <asm/cache.h>
#include <asm/global_data.h>
#include <display_options.h>
#include <linux/delay.h>

struct udevice *dev_fb;
struct udevice *dev_mmc;
struct stdio_dev *dev_stdio;
uint32_t last_pressed_key;

droidboot_error droidboot_internal_platform_init()
{
    const struct udevice *dev;
	struct uclass *uc;

	uclass_id_foreach_dev(UCLASS_VIDEO, dev, uc) {
		droidboot_log(DROIDBOOT_LOG_INFO,"%-12s= %s %sactive\n", "Video", dev->name,
		       device_active(dev) ? "" : "in");
		if (device_active(dev)) {
			struct video_priv *upriv = dev_get_uclass_priv(dev);
            dev_fb=dev;
			droidboot_log(DROIDBOOT_LOG_INFO,"%-12s= %dx%dx%d\n", "FB size", upriv->xsize,
			       upriv->ysize, 1 << upriv->bpix);
		}
	}

	uclass_id_foreach_dev(UCLASS_BLK, dev, uc) {
		droidboot_log(DROIDBOOT_LOG_INFO,"%-12s= %s %sactive\n", "mmc", dev->name,
		       device_active(dev) ? "" : "in");
		if (device_active(dev)) {
			struct blk_desc *upriv =dev_get_uclass_plat(dev);
            dev_mmc=dev;
			droidboot_log(DROIDBOOT_LOG_INFO,"BLK device, type: %s, blocks count: %d, block size: %d is active\n", upriv->type, upriv->lba, upriv->blksz);
		}
	}

	int l;
	struct list_head *list = stdio_get_list();
	struct list_head *pos;
	struct stdio_dev *stdio_dev;

	list_for_each(pos, list) {
		stdio_dev = list_entry(pos, struct stdio_dev, list);
		for (l = 0; l < MAX_FILES; l++) {
			if (stdio_devices[l] == stdio_dev) {
                if(stdio_names[l]=="stdin")
                    droidboot_log(DROIDBOOT_LOG_INFO,"Found stdin dev\n");
                    dev_stdio=stdio_dev;
			}
		}
	}

	return DROIDBOOT_EOK;
}

//lvgl thread: u-boot is single-thread, so not much we can do here
void droidboot_internal_lvgl_threads_init()
{
}

//FB flushing function
void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    struct video_priv *upriv = dev_get_uclass_priv(dev_fb);
    droidboot_log(DROIDBOOT_LOG_TRACE, "droidboot_internal_fb_flush was called");

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > (int32_t)upriv->xsize - 1 ? (int32_t)upriv->xsize - 1 : area->x2;
    int32_t act_y2 = area->y2 > (int32_t)upriv->ysize - 1 ? (int32_t)upriv->ysize - 1 : area->y2;


    lv_coord_t w = (act_x2 - act_x1 + 1);
    long int location = 0;
    long int byte_location = 0;
    unsigned char bit_location = 0;

    /*32 bit per pixel*/
    uint32_t * fbp32 = (uint32_t *)upriv->fb;
    int32_t y;
    for(y = act_y1; y <= act_y2; y++) {
        location = (act_x1 + 0) + (y + 0) * upriv->line_length / 4;
        memcpy(&fbp32[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
        color_p += w;
    }

    lv_disp_flush_ready(disp_drv);
}

// Get screen height and width
int droidboot_internal_get_display_height()
{
    struct video_priv *upriv = dev_get_uclass_priv(dev_fb);
    droidboot_log(DROIDBOOT_LOG_TRACE, "droidboot height is: %d\n", (int32_t)upriv->xsize);
    return (int32_t)upriv->ysize;
}

int droidboot_internal_get_display_width()
{
    struct video_priv *upriv = dev_get_uclass_priv(dev_fb);
    droidboot_log(DROIDBOOT_LOG_TRACE, "droidboot width is: %d\n", (int32_t)upriv->xsize);
    return (int32_t)upriv->xsize;
}

//Read keys state
bool droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    if(dev_stdio->tstc(dev_stdio)){
        if (dev_stdio->getc(dev_stdio)==65){
            data->key = LV_KEY_PREV;
            last_pressed_key = LV_KEY_PREV;
            data->state = LV_INDEV_STATE_PRESSED;
        }

        else if (dev_stdio->getc(dev_stdio)==66){
            data->key = LV_KEY_NEXT;
            last_pressed_key = LV_KEY_NEXT;
            data->state = LV_INDEV_STATE_PRESSED;
        }

        else if (dev_stdio->getc(dev_stdio)==13){
            data->key = LV_KEY_ENTER;
            last_pressed_key = LV_KEY_ENTER;
            data->state = LV_INDEV_STATE_PRESSED;
        }
        else {
            data->key=last_pressed_key;
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
    return false;
}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
    return blk_read(dev_mmc, block, count, buf);
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
}

uint32_t droidboot_internal_sd_blklen()
{
    struct blk_desc *upriv =dev_get_uclass_plat(dev_mmc);
    return upriv->blksz;
}

uint64_t droidboot_internal_sd_blkcnt()
{
    struct blk_desc *upriv =dev_get_uclass_plat(dev_mmc);
    return upriv->lba;
}

bool droidboot_internal_sd_exists()
{
    struct blk_desc *upriv =dev_get_uclass_plat(dev_mmc);
    if(upriv->lba>0)
        return true;
    return false;
}

// Logging functions
void droidboot_internal_platform_on_screen_log(const char *buf)
{

}

void droidboot_internal_platform_system_log(const char *buf)
{
    printf(buf);
}

void droidboot_internal_delay(unsigned int time)
{
    mdelay(time);
}

void droidboot_internal_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size)
{

}

void *droidboot_internal_get_kernel_load_addr()
{
    return env_get_ulong("kernel_addr_r", 16, 0);
}

void *droidboot_internal_get_ramdisk_load_addr()
{
    return env_get_ulong("ramdisk_addr_r", 16, 0);
}

void *droidboot_internal_get_dtb_load_addr()
{
    return env_get_ulong("fdt_addr_r", 16, 0);
}


// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, void *dtbo_raw, off_t dtbo_raw_size, char *options)
{
    env_set("bootargs", options);
    env_set("kernel_comp_addr_r", droidboot_internal_get_kernel_load_addr()+kernel_raw_size);
    env_set("kernel_comp_size", kernel_raw_size);
    char *argv[4] = {"booti", kernel_raw, ramdisk_raw, dtb_raw};
    struct cmd_tbl cmdtb;
    cmdtb.name="booti";
    cmdtb.maxargs=255;
    do_booti(&cmdtb, 0, 4, argv);
	return -1; //something went wrong
}

bool droidboot_internal_append_ramdisk_to_kernel()
{
    return false;
}

int droidboot_internal_get_disp_buffer_height()
{
    return droidboot_get_display_height();
}

bool droidboot_internal_use_double_buffering()
{
    return true;
}

void droidboot_internal_platform_tasks()
{
    lv_tick_inc(3);
    lv_timer_handler();
    mdelay(3);
}

int do_droidboot_init(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    droidboot_init();
}

int do_show_droidboot(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    droidboot_show_dualboot_menu();
}

U_BOOT_CMD(
	droidboot_init,	1,	0,	do_droidboot_init,
	"init droidboot gui",
	""
);

U_BOOT_CMD(
	droidboot_show,	1,	0,	do_show_droidboot,
	"init droidboot gui",
	""
);
