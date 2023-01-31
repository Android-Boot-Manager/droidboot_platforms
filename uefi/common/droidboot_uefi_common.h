#pragma once
#include  <Uefi.h>
#include <lvgl.h>
#include <droidboot_drivers.h>
#include <droidboot_logging.h>

extern EFI_BOOT_SERVICES             *gBS1;

droidboot_error droidboot_internal_platform_init(void);

//lvgl thread: u-boot is single-thread, so not much we can do here
void droidboot_internal_lvgl_threads_init(void);

//FB flushing function
void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// Get screen height and width
int droidboot_internal_get_display_height(void);
int droidboot_internal_get_display_width(void);

//Read keys state
void droidboot_internal_key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count);
ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count);
uint32_t droidboot_internal_sd_blklen(void);
uint64_t droidboot_internal_sd_blkcnt(void);
bool droidboot_internal_sd_exists(void);

// Logging functions
void droidboot_internal_platform_on_screen_log(const char *buf);
void droidboot_internal_platform_system_log(const char *buf);
void droidboot_internal_delay(unsigned int time);

// fuction to boot linux from ram
void droidboot_internal_pre_ramdisk_load(void *kernel_raw, off_t kernel_raw_size);
void *droidboot_internal_get_kernel_load_addr(void);
void *droidboot_internal_get_ramdisk_load_addr(void);
void *droidboot_internal_get_dtb_load_addr(void);
bool droidboot_internal_append_ramdisk_to_kernel(void);

int droidboot_internal_get_disp_buffer_height(void);
bool droidboot_internal_use_double_buffering(void);

void droidboot_internal_platform_tasks();
