#include <lvgl.h>
#include <droidboot_drivers.h>
#include <droidboot_logging.h>
#include <droidboot_main.h>

droidboot_error droidboot_internal_platform_init()
{
	return DROIDBOOT_EOK;
}

//lvgl thread: uefi is single-thread, so not much we can do here
void droidboot_internal_lvgl_threads_init()
{
}

//FB flushing function
void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_disp_flush_ready(disp_drv);
}

// Get screen height and width
int droidboot_get_display_height()
{
    return 0;
}

int droidboot_get_display_width()
{
    return 0;
}

//Read keys state
void droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{

}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
    return -1;
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
    return -1;
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
    //printf(buf);
}

void droidboot_internal_delay(unsigned int time)
{
    //mdelay(time);
}

void droidboot_internal_pre_ramdisk_load(unsigned char *kernel_raw, off_t kernel_raw_size)
{

}

uint32_t droidboot_internal_get_kernel_load_addr()
{
    return 0;
}

uint32_t droidboot_internal_get_ramdisk_load_addr()
{
    return 0;
}

uint32_t droidboot_internal_get_dtb_load_addr()
{
    return 0;
}


// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(unsigned char *kernel_raw, off_t kernel_raw_size, unsigned char *ramdisk_raw, off_t ramdisk_size, unsigned char *dtb_raw, off_t dtb_raw_size, char *options)
{

}

bool droidboot_internal_append_ramdisk_to_kernel()
{
    return false;
}

int droidboot_internal_get_disp_buffer_height()
{
    return 0;
}

bool droidboot_internal_use_double_buffering()
{
    return true;
}

void droidboot_internal_platform_tasks()
{
    lv_tick_inc(3);
    lv_timer_handler();
    //mdelay(3);
}
