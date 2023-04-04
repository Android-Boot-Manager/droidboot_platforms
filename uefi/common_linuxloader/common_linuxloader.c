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
#include <stdlib.h>
#include <string.h>
#include <droidboot_dtb.h>
#include <droidboot_kernel_helper.h>
#include <droidboot_ufdt_overlay.h>

#include "linux-boot/arm.h"

#define MEM_ALIGN           0x00200000
#define MAX_KERNEL_SIZE     0x04000000
#define LINUX_ARM64_OFFSET  0x00080000

// Dtbo releated
#define MAX_DTBO_SIZE       0x01800000
#define MAGIC_QCOM_DTBO_IMG 0x1EABB7D7
#define MAGIC_DTBO          0xEDFE0DD0

// Function to update memory node in target dtb from dtb provided by 1st stage bootloader
static int update_from_kernel_fdt(void *dtb_raw){
	int node=0,r=-1;
	EFI_STATUS st;
	uint64_t base=0,size=0;
	KERNEL_FDT_PROTOCOL*fdt;
	st=gBS->LocateProtocol(
		&gKernelFdtProtocolGuid,
		NULL,
		(VOID**)&fdt
	);
	if(EFI_ERROR(st)||!fdt||!fdt->Fdt)return r;
	droidboot_log(DROIDBOOT_LOG_INFO,"update memory from kernel fdt");
	while(fdt_get_memory(fdt->Fdt,node,&base,&size)){
		fdt_add_memory(dtb_raw,(UINTN)base,(UINTN)size);
		node++,r=0;
	}
	return r;
}

// Get cmdline from kfdt
char *droidboot_uefi_get_cmdline_from_kfdt(){
    int r=-1, len=0;
    EFI_STATUS st;
    KERNEL_FDT_PROTOCOL*fdt;
    st=gBS->LocateProtocol(
        &gKernelFdtProtocolGuid,
        NULL,
        (VOID**)&fdt
    );
	if(EFI_ERROR(st)||!fdt||!fdt->Fdt)return (char *)r;
	r=fdt_path_offset(fdt->Fdt,"/chosen");
    if(r<0){
        droidboot_log(DROIDBOOT_LOG_ERROR, "get chosen node failed: %s\n", fdt_strerror(r));
        return (char *)r;
    }
    const char*fdt_str=fdt_getprop(fdt->Fdt,r,"bootargs",&len);
    droidboot_log(DROIDBOOT_LOG_INFO,"Got cmdline: %s from kfdt\n", fdt_str);
	return fdt_str;
}

// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, void *dtbo_raw, off_t dtbo_raw_size, char *options)
{
    int r;
    size_t mem_pages=EFI_SIZE_TO_PAGES(MAX_KERNEL_SIZE);
    size_t mem_size=EFI_PAGES_TO_SIZE(mem_pages);
    void *kernel_reallocated;
    void *ramdisk_reallocated;
    void *dtb_address;
    off_t kernel_actual_size;
    char cmdline[4096];

    // Reallocate kernel
    if(!(kernel_reallocated=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
        droidboot_log(DROIDBOOT_LOG_ERROR, "allocate for kernel reallocation failed");
    ZeroMem(kernel_reallocated,mem_size);
    droidboot_kernel_prepear(kernel_raw, kernel_raw_size, kernel_reallocated+LINUX_ARM64_OFFSET, MAX_KERNEL_SIZE, &kernel_actual_size);
    droidboot_log(DROIDBOOT_LOG_INFO, "kernel reallocation done, old addr: %p new: %p\n", kernel_raw, kernel_reallocated+LINUX_ARM64_OFFSET);
    droidboot_dump_hex(DROIDBOOT_LOG_TRACE, (void *)kernel_reallocated+LINUX_ARM64_OFFSET, 16);

    // Reallocate ramdisk
    mem_pages=EFI_SIZE_TO_PAGES(ramdisk_size);
    mem_size=EFI_PAGES_TO_SIZE(mem_pages);
    if(!(ramdisk_reallocated=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
        droidboot_log(DROIDBOOT_LOG_ERROR, "allocate for initrd");
    ZeroMem(ramdisk_reallocated,mem_size);
    memcpy(ramdisk_reallocated, ramdisk_raw, ramdisk_size);
    droidboot_log(DROIDBOOT_LOG_INFO, "ramdisk reallocation done, old addr: %p new: %p\n", ramdisk_raw, ramdisk_reallocated);

    // Reallocate DTB
    // NOTE: Here we do change dtb size by adding 512 bytes for our extras, 4096 for cmdline, to make sure we have space for cmdline, memory node and initrd addr
    if(dtbo_raw==NULL)
        dtb_raw_size+=512+4096;
    else
        dtb_raw_size+=512+fdt_totalsize(dtbo_raw)+4096;
    mem_pages=EFI_SIZE_TO_PAGES(ALIGN_VALUE(dtb_raw_size,MEM_ALIGN));
    mem_size=EFI_PAGES_TO_SIZE(mem_pages);
    if(!(dtb_address=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
        droidboot_log(DROIDBOOT_LOG_ERROR, "dtb alloc failed\n");
    ZeroMem(dtb_address,mem_size);
    // Append dtbo
    if(dtbo_raw==NULL)
        CopyMem(dtb_address,dtb_raw,dtb_raw_size);
    else {
        unsigned char *header_ptr = (unsigned char *)dtbo_raw;
        droidboot_dump_hex(DROIDBOOT_LOG_TRACE, dtbo_raw, 16);
        if(fdt_totalsize(dtbo_raw)<=4||fdt_totalsize(dtbo_raw)>MAX_DTBO_SIZE){
            CopyMem(dtb_address,dtb_raw,dtb_raw_size);
            droidboot_log(DROIDBOOT_LOG_ERROR, "invalid dtbo size\n");
        } else {
            const uint32_t magic_dtbo=0xd0dfeed;
            if(memcmp(dtbo_raw, &magic_dtbo, 4)){
                struct fdt_header*fdto=ufdt_apply_overlay(dtb_raw,fdt_totalsize(dtb_raw),dtbo_raw,fdt_totalsize(dtbo_raw));
                CopyMem(dtb_address,fdto,fdt_totalsize(fdto));
                droidboot_log(DROIDBOOT_LOG_INFO, "Dtb overlay aplied\n");
            } else {
                CopyMem(dtb_address,dtb_raw,dtb_raw_size);
                droidboot_log(DROIDBOOT_LOG_ERROR, "dtbo wrong signature, ecepted: %llx, got: %x%x%x %x\n", magic_dtbo, header_ptr[0], header_ptr[1], header_ptr[2], header_ptr[3]);
            }
        }
    }
    droidboot_log(DROIDBOOT_LOG_INFO, "dtb reallocation done, old addr: %p new: %p, new size: %llx\n", dtb_raw, dtb_address, dtb_raw_size);

    // Update size in dtb itself
    fdt_set_totalsize(dtb_address, dtb_raw_size);

    // Print some dtb/o info
    char buf[64],*model;
    r=fdt_check_header(dtb_address);
    if(r!=0){
        droidboot_log(DROIDBOOT_LOG_ERROR, "invalid dtb head: %s\n",fdt_strerror(r));
    }
    if(dtbo_raw!=NULL){
        r=fdt_check_header(dtbo_raw);
        if(r!=0){
            droidboot_log(DROIDBOOT_LOG_ERROR,"invalid dtbo head: %s\n",fdt_strerror(r));
        }
    }
    droidboot_log(DROIDBOOT_LOG_INFO,"dtb totalsize is: %d\n",fdt_totalsize(dtb_address));
    if(dtbo_raw!=NULL)
        droidboot_log(DROIDBOOT_LOG_INFO,"dtbo totalsize is: %d\n",fdt_totalsize(dtbo_raw));

    if((model=(char*)fdt_getprop(dtb_address,0,"model",NULL)))
        droidboot_log(DROIDBOOT_LOG_INFO, "dtb device model: %s\n",model);
    if(dtbo_raw!=NULL){
        if((model=(char*)fdt_getprop(dtbo_raw,0,"model",NULL)))
            droidboot_log(DROIDBOOT_LOG_INFO, "dtbo device model: %s\n",model);
    }

    // Update fdt
    fdt_add_ramdisk_addr(ramdisk_reallocated, ramdisk_size, dtb_address);
    update_from_kernel_fdt(dtb_address);
    strcpy(cmdline, options);
    strcat(cmdline, " ");
    const char *kfdt_cmdline=droidboot_uefi_get_cmdline_from_kfdt();
    if(kfdt_cmdline>=0){
        strcat(cmdline, kfdt_cmdline);
    }
    droidboot_dtb_update_cmdline(dtb_address, &cmdline);

    // Boot new kernel
    boot_linux_arm(kernel_reallocated+LINUX_ARM64_OFFSET, kernel_actual_size, dtb_address, dtb_raw_size);
}
