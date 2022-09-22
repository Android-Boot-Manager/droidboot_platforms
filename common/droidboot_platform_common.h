#ifndef DROIDBOOT_PLATFORMS_COMMON_H
#define DROIDBOOT_PLATFORMS_COMMON_H

#include <lvgl.h>
#include <ext4.h>
#include <droidboot_error.h>
#include <droidboot_logging.h>

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
#elif defined(PLATFORM_ASTROSLIDE)
#include <lk_mtk_common.h>
#include <astroslide.h>
#elif defined(PLATFORM_MIMAMEID)
#include <lk_mtk_common.h>
#include <mimameid.h>
#elif defined(PLATFORM_COSMOCOM)
#include <lk_mtk_common.h>
#include <cosmocom.h>
#endif

void droidboot_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
bool droidboot_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

int droidboot_platform_settings_dev_open(struct ext4_blockdev *bdev);
int droidboot_platform_settings_dev_close(struct ext4_blockdev *bdev);

droidboot_error droidboot_platform_init();
ssize_t dridboot_sd_read_block(void *buf, uint32_t block, uint count);
ssize_t dridboot_sd_write_block(const void *buf, uint32_t block, uint count);
uint32_t droidboot_sd_blklen();
uint64_t droidboot_sd_blkcnt();

int droidboot_get_disp_buffer_height();
bool droidboot_use_double_buffering();

void droidboot_lvgl_threads_init();

void droidboot_platform_on_screen_log(const char *buf);
void droidboot_platform_system_log(const char *buf);

void droidboot_delay(unsigned int time);

void droidboot_platform_boot_linux_from_ram(unsigned char *kernel_raw, off_t kernel_raw_size, unsigned char *ramdisk_raw, off_t ramdisk_size, unsigned char *dtb_raw, off_t dtb_raw_size, char *options);
uint32_t droidboot_get_kernel_load_addr();
uint32_t droidboot_get_ramdisk_load_addr();
bool droidboot_append_ramdisk_to_kernel();
#endif
