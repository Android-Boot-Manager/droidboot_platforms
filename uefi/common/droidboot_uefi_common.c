#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/BlockIo.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PrintLib.h>

#include <lvgl.h>
#include <droidboot_drivers.h>
#include <droidboot_logging.h>
#include <droidboot_main.h>

EFI_SYSTEM_TABLE                    *gST1;
EFI_BOOT_SERVICES             *gBS1;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
EFI_BLOCK_IO *blkio;

uint32_t last_pressed_key;
int ww, hh;
static int droidboot_internal_uefigop_init(){
	EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*mode=gop->Mode;
	if(!mode)
		return -1;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION*info=mode->Info;
	if(!info)
		return -1;
	ww=info->HorizontalResolution;
	hh=info->VerticalResolution;
	if(ww<=0||hh<=0)
		return -1;
	droidboot_log(DROIDBOOT_LOG_INFO, "uefigop resolution %dx%d\n",ww,hh);
	return 0;
}

droidboot_error droidboot_internal_sd_init()
{
    droidboot_error status;
    int i;
    EFI_HANDLE handles[128];
    int nparts=-1;
    status = gBS1->LocateHandle(ByProtocol,
                                &gEfiBlockIoProtocolGuid, NULL, &nparts, &handles);
    nparts /= sizeof(handles[0]);
    droidboot_log(DROIDBOOT_LOG_INFO, "Found %d parts\n",nparts);
    for (i = 0; i < nparts; i++) {
        status = gBS1->HandleProtocol(handles[i],
            &gEfiBlockIoProtocolGuid, (void **)&blkio);
        if (EFI_ERROR(status))
            continue;
        if (blkio->Media->LogicalPartition)
            continue;
        if(droidboot_parse_gpt_on_sd()==DROIDBOOT_EOK){
            droidboot_log(DROIDBOOT_LOG_ERROR, "abm_settings found, stop scanning devices\n");
            return DROIDBOOT_EOK;
        }
    }
    droidboot_log(DROIDBOOT_LOG_ERROR, "abm_settings not fpund!\n");
    return DROIDBOOT_ENOENT;
}
void droidboot_internal_setgST(EFI_SYSTEM_TABLE*st1)
{
    gST1 = st1;
}
droidboot_error droidboot_internal_platform_init()
{
    droidboot_log(DROIDBOOT_LOG_INFO, "droidboot internal init 1\n");
    droidboot_log(DROIDBOOT_LOG_INFO, "droidboot internal init 2\n");
    gBS1 = gST1->BootServices;
    droidboot_log(DROIDBOOT_LOG_INFO, "droidboot internal init 3\n");
    droidboot_log(DROIDBOOT_LOG_INFO, "droidboot internal init try to init gop\n");
    gBS1->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&gop);
    droidboot_internal_uefigop_init();
    droidboot_log(DROIDBOOT_LOG_INFO, "droidboot internal init gop init done\n");
    droidboot_internal_sd_init();
    droidboot_log(DROIDBOOT_LOG_INFO, "droidboot internal init sd init done\n");
	return DROIDBOOT_EOK;
}

//lvgl thread: uefi is single-thread, so not much we can do here
void droidboot_internal_lvgl_threads_init()
{
}

//FB flushing function
void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    gop->Blt(
      gop, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)color_p, EfiBltBufferToVideo, 0, 0,
      area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, 0);
    lv_disp_flush_ready(disp_drv);
}

// Get screen height and width
int droidboot_get_display_height()
{
    return hh;
}

int droidboot_get_display_width()
{
    return ww;
}

//Read keys state
void droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    EFI_INPUT_KEY  Key;

    //Read keys
    gST1->ConIn->ReadKeyStroke (gST1->ConIn, &Key);

    //Vol up
    if (Key.ScanCode==SCAN_UP){
        data->key = LV_KEY_PREV;
        last_pressed_key = LV_KEY_PREV;
        data->state = LV_INDEV_STATE_PRESSED;
    }

    //Vol down
    else if (Key.ScanCode==SCAN_DOWN){
        data->key = LV_KEY_NEXT;
        last_pressed_key = LV_KEY_NEXT;
        data->state = LV_INDEV_STATE_PRESSED;
    }

    //Pwr key
    else if (Key.ScanCode==SCAN_SUSPEND){
        data->key = LV_KEY_ENTER;
        last_pressed_key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        data->key=last_pressed_key;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
    blkio->ReadBlocks(blkio, blkio->Media->MediaId, block, count*(blkio->Media->BlockSize), buf);
    return count;
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
    return -1;
}

uint32_t droidboot_internal_sd_blklen()
{
    return blkio->Media->BlockSize;
}

uint64_t droidboot_internal_sd_blkcnt()
{
    return blkio->Media->LastBlock;
}

bool droidboot_internal_sd_exists()
{
    return true;
}

// Logging functions
void droidboot_internal_platform_on_screen_log(const char *buf)
{

}

void droidboot_internal_platform_system_log(const char *buf)
{
   AsciiPrint(buf);
}

void droidboot_internal_delay(unsigned int time)
{
    gBS1->Stall (time*1000);
}

void droidboot_internal_pre_ramdisk_load(unsigned char *kernel_raw, off_t kernel_raw_size)
{

}

uint32_t droidboot_internal_get_kernel_load_addr()
{
    return 0;
}

uint32_t droidboot_internal_get_ramdisk_load_addr()
{
    return 0;
}

uint32_t droidboot_internal_get_dtb_load_addr()
{
    return 0;
}


// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(unsigned char *kernel_raw, off_t kernel_raw_size, unsigned char *ramdisk_raw, off_t ramdisk_size, unsigned char *dtb_raw, off_t dtb_raw_size, char *options)
{

}

bool droidboot_internal_append_ramdisk_to_kernel()
{
    return false;
}

int droidboot_internal_get_disp_buffer_height()
{
    return 0;
}

bool droidboot_internal_use_double_buffering()
{
    return true;
}

void droidboot_internal_platform_tasks()
{
    lv_tick_inc(3);
    lv_timer_handler();
    gBS1->Stall (3000);
}
