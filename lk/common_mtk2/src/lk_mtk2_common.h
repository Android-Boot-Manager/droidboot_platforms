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
bool droidboot_internal_sd_exists()
// Verify if there is abm compatible sd card
void droidboot_mtk_sd_check();

// Get screen height and width
int droidboot_internal_get_display_height();
int droidboot_internal_get_display_width();

// Logging functions
void droidboot_internal_platform_on_screen_log(const char *buf);
void droidboot_internal_platform_system_log(const char *buf);

droidboot_error droidboot_internal_platform_init();

// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, char *options);
void droidboot_internal_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size);
void *droidboot_internal_get_kernel_load_addr();
void *droidboot_internal_get_ramdisk_load_addr();
bool droidboot_internal_append_ramdisk_to_kernel();
void *droidboot_internal_get_dtb_load_addr();

// Next functions implements gui functions used by target BOOTLOADER code, those are not part of ABM droidboot aoi, and should not be callesd from droidboot code
int droidboot_mtk_show_boot_mode_menu();
