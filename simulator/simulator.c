#include <stdio.h>
#include "simulator.h"
#include <android/log.h>
#include <unistd.h>

void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	// TODO
}

int droidboot_internal_get_display_height()
{
	return 1920; // TODO
}

int droidboot_internal_get_display_width()
{
	return 1080; // TODO
}

void droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
	// TODO
}

droidboot_error droidboot_internal_platform_init()
{
	return DROIDBOOT_EOK;
}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
	return 0; // TODO
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
	return 0; // TODO
}

uint32_t droidboot_internal_sd_blklen()
{
	return 0; // TODO
}

uint64_t droidboot_internal_sd_blkcnt()
{
	return 0; // TODO
}

bool droidboot_internal_sd_exists()
{
	return false; // TODO
}

bool droidboot_internal_have_fallback()
{
	return true;
}

int droidboot_internal_get_disp_buffer_height()
{
	return 0;
}

bool droidboot_internal_use_double_buffering()
{
	return true;
}

void droidboot_internal_lvgl_threads_init()
{
	// TODO
}


void droidboot_internal_platform_on_screen_log(const char *buf)
{
	__android_log_print(ANDROID_LOG_ERROR, "droidboot", "%s", buf); // TODO
}

void droidboot_internal_platform_system_log(const char *buf)
{
	__android_log_print(ANDROID_LOG_VERBOSE, "droidboot", "%s", buf);
}

void droidboot_internal_delay(unsigned int time)
{
	sleep(time);
}

void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, void *dtbo_raw, off_t dtbo_raw_size, char *options)
{
	// nothing happens
}

void droidboot_internal_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size)
{
	// nothing happens
}

void *droidboot_internal_get_kernel_load_addr()
{
	return NULL;
}

void *droidboot_internal_get_ramdisk_load_addr()
{
	return NULL;
}

void *droidboot_internal_get_dtb_load_addr()
{
	return NULL;
}

void *droidboot_internal_get_dtbo_load_addr()
{
	return NULL;
}
bool droidboot_internal_append_ramdisk_to_kernel()
{
	return true;
}

void droidboot_internal_platform_tasks()
{
}
