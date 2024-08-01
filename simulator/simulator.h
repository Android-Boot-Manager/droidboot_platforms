//
// Created by nick on 01.08.24.
//

#ifndef ABM_SIMULATOR_H
#define ABM_SIMULATOR_H

#include <lvgl.h>
#include "droidboot_error.h"

void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
int droidboot_internal_get_display_height();
int droidboot_internal_get_display_width();
void droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
droidboot_error droidboot_internal_platform_init();
ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count);
ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count);
uint32_t droidboot_internal_sd_blklen();
uint64_t droidboot_internal_sd_blkcnt();
bool droidboot_internal_sd_exists();
bool droidboot_internal_have_fallback();
int droidboot_internal_get_disp_buffer_height();
bool droidboot_internal_use_double_buffering();
void droidboot_internal_lvgl_threads_init();
void droidboot_internal_platform_on_screen_log(const char *buf);
void droidboot_internal_platform_system_log(const char *buf);
void droidboot_internal_delay(unsigned int time);
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, void *dtbo_raw, off_t dtbo_raw_size, char *options);
void droidboot_internal_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size);
void *droidboot_internal_get_kernel_load_addr();
void *droidboot_internal_get_ramdisk_load_addr();
void *droidboot_internal_get_dtb_load_addr();
void *droidboot_internal_get_dtbo_load_addr();
bool droidboot_internal_append_ramdisk_to_kernel();
void droidboot_internal_platform_tasks();


#endif //ABM_SIMULATOR_H
