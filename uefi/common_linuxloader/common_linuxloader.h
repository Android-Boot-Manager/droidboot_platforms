#pragma once
void *droidboot_internal_dtb_post_processing(void *dtb_raw, off_t dtb_raw_size);
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, char *options);
