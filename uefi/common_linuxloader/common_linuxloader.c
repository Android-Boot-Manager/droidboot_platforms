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
#include <string.h>
#include <droidboot_dtb.h>

#define MEM_ALIGN          0x00200000
#define MAX_KERNEL_SIZE    0x04000000
#define LINUX_ARM64_OFFSET 0x00080000

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

// fuction to boot linux from ram
void droidboot_internal_boot_linux_from_ram(void *kernel_raw, off_t kernel_raw_size, void *ramdisk_raw, off_t ramdisk_size, void *dtb_raw, off_t dtb_raw_size, char *options)
{
    size_t mem_pages=EFI_SIZE_TO_PAGES(MAX_KERNEL_SIZE);
    size_t mem_size=EFI_PAGES_TO_SIZE(mem_pages);
    void *kernel_reallocated;
    void *ramdisk_reallocated;
    void *dtb_address;

    // Reallocate kernel
    if(!(kernel_reallocated=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
        droidboot_log(DROIDBOOT_LOG_ERROR, "allocate for kernel reallocation failed");
    ZeroMem(kernel_reallocated,mem_size);
    memcpy(kernel_reallocated+LINUX_ARM64_OFFSET, kernel_raw, kernel_raw_size);
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
    // NOTE: Here we do change dtb size by adding 512 bytes, to make sure we have space for cmdline and initrd addr
    dtb_raw_size+=512;
    mem_pages=EFI_SIZE_TO_PAGES(ALIGN_VALUE(dtb_raw_size,MEM_ALIGN));
    mem_size=EFI_PAGES_TO_SIZE(mem_pages);
    if(!(dtb_address=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
        droidboot_log(DROIDBOOT_LOG_ERROR, "dtb alloc failed\n");
    ZeroMem(dtb_address,mem_size);
    CopyMem(dtb_address,dtb_raw,dtb_raw_size-512);
    droidboot_log(DROIDBOOT_LOG_INFO, "dtb reallocation done, old addr: %p new: %p, new size: %llx\n", dtb_raw, dtb_address, dtb_raw_size);

    // Update size in dtb itself
    fdt_set_totalsize(dtb_address, dtb_raw_size);

    // Update fdt
    fdt_add_ramdisk_addr(ramdisk_reallocated, ramdisk_size, dtb_address);
    update_from_kernel_fdt(dtb_address);

    // Boot new kernel
    boot_linux_arm(kernel_reallocated+LINUX_ARM64_OFFSET, kernel_raw_size, dtb_address, dtb_raw_size);
}
