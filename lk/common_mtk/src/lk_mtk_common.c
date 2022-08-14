// This file contains lvgl drivers for mtk lk based bootloader, it provides basic functions such as framebuffer flush and buttons support
#include <lvgl.h>
#include <droidboot_drivers.h>

#include <platform/mt_disp_drv.h>
#include <platform/mtk_key.h>
#include <target/cust_key.h>
#include <lib/fs.h>
#include <lib/bio.h>
#include <part_interface.h>
#include <block_generic_interface.h>
#include <lib/partition.h>
#include <sys/types.h>
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <platform/mt_typedefs.h>
#include <platform/mt_gpt.h>
#include <msdc.h>
#include <mmc_common_inter.h>
#include <mmc_core.h>

bool metadata_fail;
block_dev_desc_t *bdev;

void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    time_t t;
	int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            t = current_time();
            mtkfb_draw_block(x, y, 1, 1, 0xff << 24 | color_p->ch.red << 16 | color_p->ch.green << 8 | color_p->ch.blue);
            color_p++;
            t=current_time()-t;
            printf("draw block took\n", (uint)t);
        }
    }
    
	// Update display
    #ifdef DROIDBOOT_PLATFORM_DEBUG
    t = current_time();
    #endif
    mt_disp_update(area->x1, area->y1, area->x2, area->y2);
    #ifdef DROIDBOOT_PLATFORM_DEBUG
    t=current_time()-t;
    printf("mt disp update took\n", (uint)t);
    #endif

    // Inform the graphics library that we are ready with the flushing
    lv_disp_flush_ready(disp_drv);
}

//Read keys state
bool droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    data->key = LV_KEY_PREV;
    if (mtk_detect_key(MT65XX_MENU_SELECT_KEY)){
        data->state = LV_INDEV_STATE_PR;
        return false; /*No buffering now so no more data read*/
    } 
    data->key = LV_KEY_NEXT;
    if (mtk_detect_key(MT65XX_MENU_OK_KEY)){
        data->state = LV_INDEV_STATE_PR;
         return false; /*No buffering now so no more data read*/
    } 
    data->key = LV_KEY_ENTER;
    if (mtk_detect_key(MTK_PMIC_PWR_KEY)){
        data->state = LV_INDEV_STATE_PR;
         return false; /*No buffering now so no more data read*/
    } 
   
}

//Init SD card
int dridboot_mtk_sd_card()
{
    return mmc_init(1, 1);
}

ssize_t dridboot_internal_sd_read_block(void *buf, bnum_t block, uint count)
{
    return mmc_block_read(1, block, count, (unsigned long*)buf);
}

ssize_t dridboot_internal_sd_write_block(struct bdev *_bdev, const void *buf, bnum_t block, uint count)
{
    return mmc_block_write(1, block, count, buf);
}

uint32_t droidboot_internal_sd_blklen()
{
    struct mmc_card *card;
    card=mmc_get_card(1);
    return card->blklen;
}

uint64_t droidboot_internal_sd_blkcnt()
{
    struct mmc_card *card;
    card=mmc_get_card(1);
    return card->nblks;
}

void droidboot_mtk_sd_check()
{
    // This function is not part of mtk-lk as it dose not support SD card ootb, so it must be implemented in lk.
    /*init_sd_card();
    
    int ret = fs_mount("/boot", "ext2", "mmc1p1"); //try to mount abm_settings
	if(ret != 0){
	    return false;
	}
	else{
	    fs_unmount("/boot");
	    return true;
	}*/
}

// Get screen height and width
int droidboot_get_display_height()
{
    return DISP_GetScreenHeight();
}

int droidboot_get_display_width()
{
    return DISP_GetScreenWidth();
}

// This function must not be here, as lvgl already have its own fs support, but it needs fseek and ftell wich are not supported by our fs driver, due to that fact we handle image loading ourself.
struct lv_img_dsc_t* droidboot_mtk_load_image_from_fs(char* path){
    /*int ret=0;
    filehandle *entry_file_handle = NULL;
    ret=fs_mount("/boot", "ext2", "mmc1p1");
    
    off_t entry_file_size = fs_get_file_size(path);
    off_t header_len      = sizeof(lv_img_header_t);          // file header struct
    off_t buf_len         = entry_file_size - header_len;     // file size minus header size*/
    lv_img_dsc_t* img_dsc = malloc(sizeof(lv_img_dsc_t));     // image descriptor struct
    /*unsigned char *buf    = malloc(buf_len);                  // pixel data only

    ret=fs_open_file(path, &entry_file_handle);
    if(ret!=0)
        return NULL;

    fs_read_file(entry_file_handle, img_dsc, 0, header_len);  // read header struct (first 4 bytes)
    fs_read_file(entry_file_handle, buf , 0, buf_len);        // read pixel data (remainder)
    fs_close_file(entry_file_handle);

    img_dsc->data      = buf;
    img_dsc->data_size = buf_len;
    fs_unmount("/boot");*/
    return img_dsc;
}

// Next functions implements gui functions used by target BOOTLOADER code, those are not part of ABM droidboot aoi, and should not be callesd from droidboot code
int exit;
static lv_obj_t* currentButton = NULL;
static void droidboot_mtk_menu_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        int index = lv_obj_get_child_id(obj);
        if(index==0)
        {   
            exit=2;
        }
        if(index==1)
        {   
            exit=99;
        }
        if(index==2)
        {   
            exit=9;
        }
        if(index==3)
        {
            exit=0;
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
    exit=-1;
    
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
    while(exit==-1){
        mdelay(50);
    }
    return exit;
}

