// This file contains lvgl drivers for mtk lk based bootloader, it provides basic functions such as framebuffer flush and buttons support
#include <lvgl.h>
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

void droidboot_mtk_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
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
    #ifdef FROIDBOOT_PLATFORM_DEBUG
    t = current_time();
    #endif
    mt_disp_update(area->x1, area->y1, area->x2, area->y2);
    #ifdef FROIDBOOT_PLATFORM_DEBUG
    t=current_time()-t;
    printf("mt disp update took\n", (uint)t);
    #endif

    // Inform the graphics library that we are ready with the flushing
    lv_disp_flush_ready(disp_drv);
}

//Read keys state
bool droidboot_mtk_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
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

//Init emmc and bio
void droidboot_mtk_settings_init(){
    int err;
    err=mmc_init(1, 1);
    u64 g_emmc_size = 0;
    u64 g_emmc_user_size = 0;
    
    struct mmc_host *host;
	struct mmc_card *card;
	bdev = malloc(sizeof(block_dev_desc_t));
	u8 *ext_csd;
	
	card=mmc_get_card(1);
    host=mmc_get_host(1);
	g_emmc_user_size = (u64)card->blklen * card->nblks;

	bdev->dev         = 1;
	bdev->type        = BOOTDEV_SDMMC;
	bdev->blksz       = (u64)card->blklen;
	ext_csd = &card->raw_ext_csd[0];
	if (ext_csd[EXT_CSD_ERASE_GRP_DEF] & EXT_CSD_ERASE_GRP_DEF_EN)
		bdev->erasesz = card->ext_csd.hc_erase_grp_sz * 512 * 1024;
	else
		bdev->erasesz = card->csd.erase_sctsz * 512;
	bdev->lba         = card->nblks * card->blklen / MMC_BLOCK_SIZE;
	bdev->blk_bits    = 9;
	bdev->block_read  = mmc_wrap_bread;
	bdev->block_write = mmc_wrap_bwrite;
	
	mmc_sdhci_bdev_t *bdev1 = malloc(sizeof(mmc_sdhci_bdev_t));
	/* set up the base device */
    bio_initialize_bdev(&bdev1->dev, "mmc1", card->blklen,card->nblks);
	/* our bits */
	bdev1->mmcdev = bdev;
	bdev1->dev.read_block = mmc_sdhci_bdev_read_block;
	/* register it */
	bio_register_device(&bdev1->dev);
	bio_dump_devices();
	
	
	err=partition_publish("mmc1", 0);
}

void droidboot_mtk_sd_check()
{
    // This function is not part of mtk-lk as it dose not support SD card ootb, so it must be implemented in lk.
    init_sd_card();
    
    int ret = fs_mount("/boot", "ext2", "mmc1p1"); //try to mount abm_settings
	if(ret != 0){
	    return false;
	}
	else{
	    fs_unmount("/boot");
	    return true;
	}
}

// This function must not be here, as lvgl already have its own fs support, but it needs fseek and ftell wich are not supported by our fs driver, due to that fact we handle image loading ourself.
struct lv_img_dsc_t* droidboot_mtk_load_image_from_fs(char* path){
    int ret=0;
    filehandle *entry_file_handle = NULL;
    ret=fs_mount("/boot", "ext2", "mmc1p1");
    
    off_t entry_file_size = fs_get_file_size(path);
    off_t header_len      = sizeof(lv_img_header_t);          // file header struct
    off_t buf_len         = entry_file_size - header_len;     // file size minus header size
    lv_img_dsc_t* img_dsc = malloc(sizeof(lv_img_dsc_t));     // image descriptor struct
    unsigned char *buf    = malloc(buf_len);                  // pixel data only

    ret=fs_open_file(path, &entry_file_handle);
    if(ret!=0)
        return NULL;

    fs_read_file(entry_file_handle, img_dsc, 0, header_len);  // read header struct (first 4 bytes)
    fs_read_file(entry_file_handle, buf , 0, buf_len);        // read pixel data (remainder)
    fs_close_file(entry_file_handle);

    img_dsc->data      = buf;
    img_dsc->data_size = buf_len;
    fs_unmount("/boot");
    return img_dsc;
}
