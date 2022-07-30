#include <droidboot_platforms_common.h>

void droidboot_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    #if defined(PLATFORM_YGGDRASIL) || defined(PLATFORM_YGGDRASILX)
    droidboot_mtk_fb_flush(disp_drv, area, color_p);
    #endif
}

bool droidboot_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    #if defined(PLATFORM_YGGDRASIL) || defined(PLATFORM_YGGDRASILX)
    droidboot_mtk_key_read(drv, data);
    #endif
}

int droidboot_platform_settings_dev_open(struct ext4_blockdev *bdev)
{

}

int droidboot_platform_settings_dev_close(struct ext4_blockdev *bdev)
{

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
