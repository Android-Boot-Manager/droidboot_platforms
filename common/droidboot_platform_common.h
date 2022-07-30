#ifndef DROIDBOOT_PLATFORMS_COMMON_H
#define DROIDBOOT_PLATFORMS_COMMON_H

#include <lvgl.h>
#include <ext4.h>

#if defined(PLATFORM_YGGDRASIL)
#include <lk_mtk_common.h>
#include <yggdrasil.h>
#elif defined(PLATFORM_YGGDRASILX)
#include <lk_mtk_common.h>
#include <yggdrasilx.h>
#elif defined(PLATFORM_CEDRIC)
#include <lk_qcom_common.h>
#include <yggdrasil.h>
#elif defined(PLATFORM_PRO1)
#include <droidboot_uefi_common.h>
#include <common_abl.h>
#include <pro1.h>
#elif defined(PLATFORM_PRO1X)
#include <droidboot_uefi_common.h>
#include <common_abl.h>
#include <pro1x.h>
#endif

void droidboot_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
bool droidboot_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

int droidboot_platform_settings_dev_open(struct ext4_blockdev *bdev);
int droidboot_platform_settings_dev_close(struct ext4_blockdev *bdev);
int droidboot_platform_settings_dev_bread(struct ext4_blockdev *bdev, void *buf, uint32_t blk_id, uint32_t blk_count);
int droidboot_platform_settings_dev_bwrite(struct ext4_blockdev *bdev, const void *buf,
			  uint32_t blk_id, uint32_t blk_cnt);
#endif
