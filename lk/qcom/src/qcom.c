// This file contains lvgl drivers for qcom lk based bootloader, it provides basic functions such as framebuffer flush and buttons support
#include <lvgl.h>
#include <droidboot_drivers.h>
#include <droidboot_logging.h>

#include <dev/fbcon.h>
#include <kernel/thread.h>
#include <lib/bio.h>
#include <pm8x41.h>

#include <platform/iomap.h>
#include <platform.h>
#include <decompress.h>

uint32_t last_pressed_key;

extern uint32_t target_volume_up();
extern uint32_t target_volume_down();

void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    time_t t;
	int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            fbcon_draw_pixel(x, y, 0xff << 24 | color_p->ch.red << 16 | color_p->ch.green << 8 | color_p->ch.blue  ); /* Put a pixel to the display.*/
            color_p++;
        }
    }
    
	// Update display
    fbcon_flush();
    // Inform the graphics library that we are ready with the flushing
    lv_disp_flush_ready(disp_drv);
}



// Get screen height and width
int droidboot_internal_get_display_height()
{
    return fbcon_get_height();
}

int droidboot_internal_get_display_width()
{
    return fbcon_get_width();
}

//Read keys state
bool droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    if (target_volume_up()){
        data->key = LV_KEY_PREV;
        last_pressed_key = LV_KEY_PREV;
        data->state = LV_INDEV_STATE_PRESSED;
    } 

    else if (target_volume_down()){
        data->key = LV_KEY_NEXT;
        last_pressed_key = LV_KEY_NEXT;
        data->state = LV_INDEV_STATE_PRESSED;
    } 
    
    else if (pm8x41_get_pwrkey_is_pressed()){
        data->key = LV_KEY_ENTER;
        last_pressed_key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    } 
    else {
        data->key=last_pressed_key;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
    bdev_t *lk_bdev = bio_open("hd2");
    return bio_read_block(lk_bdev, buf, block, count);
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
    bdev_t *lk_bdev = bio_open("hd2");
    return bio_write_block(lk_bdev, buf, block, count);
}

uint32_t droidboot_internal_sd_blklen()
{
    bdev_t *lk_bdev = bio_open("hd2");
    return lk_bdev->block_size;
}

uint64_t droidboot_internal_sd_blkcnt()
{
    bdev_t *lk_bdev = bio_open("hd2");
    return lk_bdev->block_count;
}

bool droidboot_internal_sd_exists()
{
    return bio_open("hd2")!=NULL;
}

// Logging functions
void droidboot_internal_platform_on_screen_log(const char *buf)
{

}

void droidboot_internal_platform_system_log(const char *buf)
{
    printf(buf);
}

droidboot_error droidboot_internal_platform_init()
{
    bio_dump_devices();
    return DROIDBOOT_EOK;
}

void droidboot_internal_delay(unsigned int time)
{
    thread_sleep(time);
}

// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, void *dtbo_raw, off_t dtbo_raw_size, char *options)
{
	void *kernel_addr = VA((addr_t)(ABOOT_FORCE_KERNEL64_ADDR));
    droidboot_log(DROIDBOOT_LOG_ERROR, "Going to boot linux\n");
	boot_linux(VA((addr_t)(ABOOT_FORCE_KERNEL64_ADDR)), dtb_raw, options, board_machtype(), ramdisk_raw, ramdisk_size);
    droidboot_log(DROIDBOOT_LOG_ERROR, "boot linux failed\n");
	return -1; //something went wrong
}

void droidboot_internal_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size)
{
    void *kernel_addr = VA((addr_t)(ABOOT_FORCE_KERNEL64_ADDR));
    unsigned int dev_null;
    if(is_gzip_package(kernel_raw, kernel_raw_size)) {
		if(decompress(kernel_raw, kernel_raw_size, kernel_addr, ABOOT_FORCE_RAMDISK_ADDR - ABOOT_FORCE_KERNEL64_ADDR, &dev_null, &dev_null)){
			droidboot_log(DROIDBOOT_LOG_ERROR, "kernel decompression failed\n");
			return -1;
		}
		droidboot_log(DROIDBOOT_LOG_INFO, "kernel decompression OK\n");
	} else {
		memmove(kernel_addr, kernel_raw, kernel_raw_size);
	}
}

void *droidboot_internal_get_kernel_load_addr()
{
    return VA((addr_t)(ABOOT_FORCE_RAMDISK_ADDR));
}

void *droidboot_internal_get_ramdisk_load_addr()
{
    return VA((addr_t)(ABOOT_FORCE_RAMDISK_ADDR));
}

void *droidboot_internal_get_dtb_load_addr()
{
    return VA((addr_t)(ABOOT_FORCE_TAGS_ADDR));
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
