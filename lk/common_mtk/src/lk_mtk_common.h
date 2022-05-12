#include <lvgl.h>

// Display flush 
void droidboot_mtk_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// Read buttons state in lvgl style
bool droidboot_mtk_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

// Initi mmc (sd) stuff
void droidboot_mtk_settings_init();

// Verify if there is abm compatible sd card
void droidboot_mtk_sd_check();

// HACK: image loading for lvgl in hacky manner
struct lv_img_dsc_t* droidboot_mtk_load_image_from_fs(char* path);
