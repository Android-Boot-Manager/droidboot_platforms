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
    size_t mem_pages=EFI_SIZE_TO_PAGES(0x04000000);
	size_t mem_size=EFI_PAGES_TO_SIZE(mem_pages);
    void *out;
	void *out1;
    if(!(out=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
		droidboot_log(DROIDBOOT_LOG_ERROR, "allocate for compress buffer failed");
	ZeroMem(out,mem_size);
    memcpy(out+0x00080000, kernel_raw, kernel_raw_size);
    droidboot_log(DROIDBOOT_LOG_INFO, "kernel reallocation done, old addr: %p new: %p\n", kernel_raw, out+0x00080000);
    droidboot_dump_hex(DROIDBOOT_LOG_TRACE, (void *)out+0x00080000, 16);

	mem_pages=EFI_SIZE_TO_PAGES(ramdisk_size);
	mem_size=EFI_PAGES_TO_SIZE(mem_pages);
	 if(!(out1=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
		droidboot_log(DROIDBOOT_LOG_ERROR, "allocate for initrd");
	ZeroMem(out1,mem_size);
    memcpy(out1, ramdisk_raw, ramdisk_size);
    droidboot_log(DROIDBOOT_LOG_INFO, "ramdisk reallocation done, old addr: %p new: %p\n", ramdisk_raw, out);


    droidboot_log(DROIDBOOT_LOG_INFO, "dtb:\n");
    droidboot_dump_hex(DROIDBOOT_LOG_TRACE, (void *)dtb_raw, 16);
    fdt_add_ramdisk_addr(out1, ramdisk_size, dtb_raw);
    update_from_kernel_fdt(dtb_raw);
	int node=0,r=-1;
	EFI_STATUS st;
	uint64_t base=0,size=0;
	while(fdt_get_memory(dtb_raw,node,&base,&size)){
		droidboot_log(DROIDBOOT_LOG_INFO, "ram base: %llx, size: %llx\n", base, size);
		node++,r=0;
	}
    boot_linux_arm(out+0x00080000, kernel_raw_size, dtb_raw, 1048576);
}

void *droidboot_internal_dtb_post_processing(void *dtb_raw, off_t dtb_raw_size)
{
    EFI_STATUS st;
	KERNEL_FDT_PROTOCOL*fdt=NULL;
	st=gBS->LocateProtocol(
		&gKernelFdtProtocolGuid,
		NULL,
		(VOID**)&fdt
		);
	//TODO: remove hardcoded size
	size_t mem_pages=EFI_SIZE_TO_PAGES(ALIGN_VALUE(1048576,MEM_ALIGN));
	size_t dtb_mem_size=EFI_PAGES_TO_SIZE(mem_pages);
    void *dtb_address;
	if(!(dtb_address=AllocateAlignedPages(mem_pages,MEM_ALIGN)))
		droidboot_log(DROIDBOOT_LOG_ERROR, "dtb alloc failed\n");

	ZeroMem(dtb_address,dtb_mem_size);
	CopyMem(dtb_address,dtb_raw,1048576);
	droidboot_log(DROIDBOOT_LOG_INFO, "dtb reallocation done, old addr: %p new: %p\n", dtb_raw, dtb_address);
	return dtb_address;
}
