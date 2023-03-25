// This file contains lvgl drivers for mtk lk based bootloader, it provides basic functions such as framebuffer flush and buttons support
#include <lvgl.h>
#include <droidboot_drivers.h>
#include <droidboot_logging.h>

#include <mt_disp_drv.h>
#include <mtk_key.h>
#include <lib/bio.h>
#include <lib/partition.h>
#include <sys/types.h>
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <kernel/thread.h>

bool metadata_fail;
uint32_t last_pressed_key;

void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            mtkfb_draw_block(x, y, 1, 1, 0xff << 24 | color_p->ch.red << 16 | color_p->ch.green << 8 | color_p->ch.blue);
            color_p++;
        }
    }
    
	// Update display
    mt_disp_update(area->x1, area->y1, area->x2, area->y2);

    // Inform the graphics library that we are ready with the flushing
    lv_disp_flush_ready(disp_drv);
}

//Read keys state
bool droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    if (detect_key(MENU_SELECT_KEY)){
        data->key = LV_KEY_PREV;
        last_pressed_key = LV_KEY_PREV;
        data->state = LV_INDEV_STATE_PRESSED;
    } 

    else if (detect_key(MENU_OK_KEY)){
        data->key = LV_KEY_NEXT;
        last_pressed_key = LV_KEY_NEXT;
        data->state = LV_INDEV_STATE_PRESSED;
    } 
    
    else if (detect_key(PMIC_PWR_KEY)){
        data->key = LV_KEY_ENTER;
        last_pressed_key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    } 
    else {
        data->key=last_pressed_key;
        data->state = LV_INDEV_STATE_RELEASED;
    }  
}

//Init SD card
int dridboot_mtk_sd_card()
{
    return sdcard_init();
}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
    bdev_t *lk_bdev = bio_open("sdmmc1");
    return bio_read_block(lk_bdev, buf, block, count);
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
    bdev_t *lk_bdev = bio_open("sdmmc1");
    return bio_write_block(lk_bdev, buf, block, count);
}

uint32_t droidboot_internal_sd_blklen()
{
    bdev_t *bdev = bio_open("sdmmc1");
    return bdev->block_size;
}

uint64_t droidboot_internal_sd_blkcnt()
{
    bdev_t *bdev = bio_open("sdmmc1");
    return bdev->block_count;
}

bool droidboot_internal_sd_exists()
{
    bdev_t *bdev = bio_open("sdmmc1");
    return bdev!=NULL;
}

// Get screen height and width
int droidboot_internal_get_display_height()
{
    return DISP_GetScreenHeight();
}

int droidboot_internal_get_display_width()
{
    return DISP_GetScreenWidth();
}

// Logging functions
void droidboot_internal_platform_on_screen_log(const char *buf)
{
    video_printf(buf);
}

void droidboot_internal_platform_system_log(const char *buf)
{
    printf(buf);
}

droidboot_error droidboot_internal_platform_init()
{
    dridboot_mtk_sd_card();
    return DROIDBOOT_EOK;
}

void droidboot_internal_delay(unsigned int time)
{
    thread_sleep(time);
}

// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, char *options)
{
   // cmdline_append(options);
    // NOTE: next function is NOT a part of mtk-lk you should implement it yourself based on your boot_linux_from_storage
	//mtk_boot_linux_from_ram(kernel_raw, kernel_raw_size, ramdisk_raw, ramdisk_size);
	
}

void droidboot_internal_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size)
{

}

void *droidboot_internal_get_kernel_load_addr()
{
    return get_kernel_addr();
}

void *droidboot_internal_get_ramdisk_load_addr()
{
    return NULL;
}

bool droidboot_internal_append_ramdisk_to_kernel()
{
    return true;
}

void *droidboot_internal_get_dtb_load_addr()
{
    return NULL;
}

// Next functions implements gui functions used by target BOOTLOADER code, those are not part of ABM droidboot aoi, and should not be callesd from droidboot code
int exit_mtk;
static lv_obj_t* currentButton = NULL;
static void droidboot_mtk_menu_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        int index = lv_obj_get_child_id(obj);
        if(index==0)
        {   
            exit_mtk=2;
        }
        if(index==1)
        {   
            exit_mtk=99;
        }
        if(index==2)
        {   
            exit_mtk=9;
        }
        if(index==3)
        {
            exit_mtk=0;
        }
    }
}

void droidboot_mtk_main_menu_add_options(lv_obj_t * list1)
{
    lv_obj_t * list_btn;
    list_btn = lv_list_add_btn(list1,  NULL, "\nBoot recovery");
    lv_obj_add_event_cb(list_btn, droidboot_mtk_menu_event_handler, LV_EVENT_CLICKED, NULL);
    list_btn = lv_list_add_btn(list1,  NULL, "\nEnter fastboot menu");
    lv_obj_add_event_cb(list_btn, droidboot_mtk_menu_event_handler, LV_EVENT_CLICKED, NULL);
    list_btn = lv_list_add_btn(list1,  NULL, "\nNormal boot");
    lv_obj_add_event_cb(list_btn, droidboot_mtk_menu_event_handler, LV_EVENT_CLICKED, NULL);
    
    /*if(!lvgl_mtk_get_metadata_fail()){
        list_btn = lv_list_add_btn(list1,  &next_menu_logo, "\nEnter dualboot menu");
        lv_obj_add_style(list_btn, &lvgl_volla_list_button_style, 0);
        lv_obj_add_style(list_btn, &lvgl_volla_list_button_pressed_style, LV_STATE_FOCUS_KEY); 
        lv_obj_add_event_cb(list_btn, lvgl_menu_event_handler, LV_EVENT_CLICKED, NULL);
    }*/
}


int droidboot_mtk_show_boot_mode_menu()
{
    exit_mtk=-1;
    
    lv_group_t * lvgl_main_menu_group = lv_group_create();
    lv_group_set_default(lvgl_main_menu_group);

    lv_indev_set_group(droidboot_lvgl_indev, lvgl_main_menu_group);
    lv_obj_t * win = lv_win_create(lv_scr_act(), lv_pct(15));
    lv_obj_set_pos(win, 0, 0);
    lv_obj_set_size(win, lv_pct(100), lv_pct(90));
    lv_obj_t * win_title = lv_win_add_title(win, "  Recovery Menu"); 
    lv_obj_set_pos(win_title, 0, 0);
    lv_obj_t * list1 = lv_list_create(win); 
    lv_obj_set_size(list1, lv_pct(100), lv_pct(100));
    lv_obj_set_pos(list1, 0, 0);
    lv_obj_align(list1, LV_ALIGN_BOTTOM_MID, 0, 0);
    droidboot_mtk_main_menu_add_options(list1);
    lv_group_focus_obj(win);
    while(exit_mtk==-1){
        mdelay(50);
    }
    return exit;
}

