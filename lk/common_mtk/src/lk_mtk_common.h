#pragma once
#include <lvgl.h>

// Display flush 
void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// Read buttons state in lvgl style
bool droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

// Initi mmc (sd) stuff
void droidboot_mtk_settings_init();

// Verify if there is abm compatible sd card
void droidboot_mtk_sd_check();

// Get screen height and width
int droidboot_get_display_height();
int droidboot_get_display_width();

// HACK: image loading for lvgl in hacky manner
struct lv_img_dsc_t* droidboot_mtk_load_image_from_fs(char* path);

// Next functions implements gui functions used by target BOOTLOADER code, those are not part of ABM droidboot aoi, and should not be callesd from droidboot code
int droidboot_mtk_show_boot_mode_menu();
