#include <lvgl.h>

lv_indev_drv_t indev_drv;
lv_indev_t * lvgl_mtk_indev;

// Display flush 
void droidboot_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

void droidboot_drivers_init();

bool droidboot_get_metadata_fail();
