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
#include <video.h>
#include <vsprintf.h>
#include <asm/cache.h>
#include <asm/global_data.h>
#include <display_options.h>
#include <linux/delay.h>

struct udevice *dev_fb;

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
	return DROIDBOOT_EOK;
}

//lvgl thread: u-boot is single-thread, so not much we can do here
void droidboot_internal_lvgl_threads_init()
{
}

//FB flushing function
void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    video_bmp_display(dev_fb, area, area->x1, area->y1, 0);
    // Inform the graphics library that we are ready with the flushing
    lv_disp_flush_ready(disp_drv);
}

// Get screen height and width
int droidboot_get_display_height()
{
    return video_get_xsize(dev_fb);
}

int droidboot_get_display_width()
{
    return video_get_ysize(dev_fb);
}

//Read keys state
bool droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
}

uint32_t droidboot_internal_sd_blklen()
{
    return 0;
}

uint64_t droidboot_internal_sd_blkcnt()
{
    return 0;
}

bool droidboot_internal_sd_exists()
{
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

// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(unsigned char *kernel_raw, off_t kernel_raw_size, unsigned char *ramdisk_raw, off_t ramdisk_size, unsigned char *dtb_raw, off_t dtb_raw_size, char *options)
{
	return -1; //something went wrong
}

void droidboot_internal_pre_ramdisk_load(unsigned char *kernel_raw, off_t kernel_raw_size)
{

}

uint32_t droidboot_internal_get_kernel_load_addr()
{
    return NULL;
}

uint32_t droidboot_internal_get_ramdisk_load_addr()
{
    return NULL;
}

uint32_t droidboot_internal_get_dtb_load_addr()
{
    return NULL;
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
