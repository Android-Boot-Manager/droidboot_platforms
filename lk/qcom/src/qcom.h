#pragma once
#include <lvgl.h>
#include <droidboot_logging.h>
#include <droidboot_error.h>

// Display flush 
void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// Read buttons state in lvgl style
bool droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

// Initi mmc (sd) stuff
//Init SD card
int dridboot_mtk_sd_card();
ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count);
ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count);
uint32_t droidboot_internal_sd_blklen();
uint64_t droidboot_internal_sd_blkcnt();
bool droidboot_internal_sd_exists();

// Get screen height and width
int droidboot_get_display_height();
int droidboot_get_display_width();

// Logging functions
void droidboot_internal_platform_on_screen_log(const char *buf);
void droidboot_internal_platform_system_log(const char *buf);

droidboot_error droidboot_internal_platform_init();

// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(unsigned char *kernel_raw, off_t kernel_raw_size, unsigned char *ramdisk_raw, off_t ramdisk_size, unsigned char *dtb_raw, off_t dtb_raw_size, char *options);
uint32_t droidboot_internal_get_kernel_load_addr();
uint32_t droidboot_internal_get_ramdisk_load_addr();
bool droidboot_internal_append_ramdisk_to_kernel();

int droidboot_internal_get_disp_buffer_height();
bool droidboot_internal_use_double_buffering();
