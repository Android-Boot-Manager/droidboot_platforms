#include "droidboot_platform_common.h"

void droidboot_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    droidboot_internal_fb_flush(disp_drv, area, color_p);
}


bool droidboot_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
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

int droidboot_platform_settings_dev_bread(struct ext4_blockdev *bdev, void *buf, uint32_t blk_id, uint32_t blk_count)
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
 return DROIDBOOT_EOK;
}

droidboot_error droidboot_platform_get_storage_part_offset()
{
 return DROIDBOOT_EOK;
}

droidboot_error droidboot_platform_get_storage_block_count()
{
 return DROIDBOOT_EOK;
}

droidboot_error droidboot_platform_get_storage_size()
{
 return DROIDBOOT_EOK;
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


void droidboot_platform_on_screen_log(droidboot_log_level log_level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    droidboot_internal_platform_on_screen_log(fmt, args);
    va_end(args);
}

void droidboot_platform_system_log(droidboot_log_level log_level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    droidboot_internal_platform_system_log(fmt, args);
    va_end(args);
}
