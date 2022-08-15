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

// Verify if there is abm compatible sd card
void droidboot_mtk_sd_check();

// Get screen height and width
int droidboot_get_display_height();
int droidboot_get_display_width();

// HACK: image loading for lvgl in hacky manner
struct lv_img_dsc_t* droidboot_mtk_load_image_from_fs(char* path);

// Logging functions
void droidboot_internal_platform_on_screen_log(droidboot_log_level log_level, const char *fmt, ...);
void droidboot_internal_platform_system_log(droidboot_log_level log_level, const char *fmt, ...);

droidboot_error droidboot_internal_platform_init();

// Next functions implements gui functions used by target BOOTLOADER code, those are not part of ABM droidboot aoi, and should not be callesd from droidboot code
int droidboot_mtk_show_boot_mode_menu();
