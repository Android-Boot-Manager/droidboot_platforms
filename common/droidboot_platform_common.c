#include "droidboot_platform_common.h"

void droidboot_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    droidboot_internal_fb_flush(disp_drv, area, color_p);
}

int droidboot_get_display_height()
{
    return droidboot_internal_get_display_height();
}

int droidboot_get_display_width()
{
    return droidboot_internal_get_display_width();
}

void droidboot_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    droidboot_internal_key_read(drv, data);
}


int droidboot_platform_settings_dev_open(struct ext4_blockdev *bdev)
{
    return EOK;
}

int droidboot_platform_settings_dev_close(struct ext4_blockdev *bdev)
{
    return EOK;
}

int droidboot_platform_settings_dev_bwrite(struct ext4_blockdev *bdev, const void *buf,
			  uint32_t blk_id, uint32_t blk_cnt)
			  {
  return EOK;
}

droidboot_error droidboot_platform_init()
{
 return droidboot_internal_platform_init();
}

ssize_t dridboot_sd_read_block(void *buf, uint32_t block, uint count)
{
    return dridboot_internal_sd_read_block(buf, block, count);
}

ssize_t dridboot_sd_write_block(const void *buf, uint32_t block, uint count)
{
    return dridboot_internal_sd_write_block(buf, block, count);
}

uint32_t droidboot_sd_blklen()
{
    return droidboot_internal_sd_blklen();
}

uint64_t droidboot_sd_blkcnt()
{
    return droidboot_internal_sd_blkcnt();
}

bool droidboot_sd_exists()
{
    return droidboot_internal_sd_exists();
}


int droidboot_get_disp_buffer_height()
{
    return droidboot_internal_get_disp_buffer_height();
}

bool droidboot_use_double_buffering()
{
    return droidboot_internal_use_double_buffering();
}

void droidboot_lvgl_threads_init()
{
    droidboot_internal_lvgl_threads_init();
}


void droidboot_platform_on_screen_log(const char *buf)
{
    droidboot_internal_platform_on_screen_log(buf);
}

void droidboot_platform_system_log(const char *buf)
{
    droidboot_internal_platform_system_log(buf);
}

void droidboot_delay(unsigned int time)
{
    droidboot_internal_delay(time);
}

void droidboot_platform_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, char *options)
{
    droidboot_internal_boot_linux_from_ram(kernel_raw, kernel_raw_size, ramdisk_raw, ramdisk_size, dtb_raw, dtb_raw_size, options);
}

void droidboot_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size)
{
    droidboot_internal_pre_ramdisk_load(kernel_raw, kernel_raw_size);
}

void *droidboot_get_kernel_load_addr()
{
    return droidboot_internal_get_kernel_load_addr();
}

void *droidboot_get_ramdisk_load_addr()
{
    return droidboot_internal_get_ramdisk_load_addr();
}

void *droidboot_get_dtb_load_addr()
{
    return droidboot_internal_get_dtb_load_addr();
}

void *droidboot_dtb_post_processing(void *dtb_raw, off_t dtb_raw_size)
{
    return droidboot_internal_dtb_post_processing(dtb_raw, dtb_raw_size);
}
bool droidboot_append_ramdisk_to_kernel()
{
    return droidboot_internal_append_ramdisk_to_kernel();
}

void droidboot_platform_tasks()
{
    droidboot_internal_platform_tasks();
}
