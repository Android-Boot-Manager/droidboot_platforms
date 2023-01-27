/*
 *
 * Copyright (C) 2021 BigfootACA <bigfoot@classfun.cn>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#if defined (__aarch64__) || defined(__arm__)
#include<Uefi.h>
#include<Library/ArmLib.h>
#include<Library/UefiLib.h>
#include<Library/DebugLib.h>
#include<Library/ReportStatusCodeLib.h>
#include<Library/MemoryAllocationLib.h>
#include<Library/UefiBootServicesTableLib.h>
#include<Library/CacheMaintenanceLib.h>
#include"str.h"

#include <droidboot_logging.h>

typedef VOID (*ARM_V7_CACHE_OPERATION)(UINTN);
typedef VOID (*AARCH64_CACHE_OPERATION)(UINTN);
extern VOID ArmV7DataCacheOperation(IN ARM_V7_CACHE_OPERATION DataCacheOperation);
extern VOID AArch64DataCacheOperation(IN AARCH64_CACHE_OPERATION DataCacheOperation);
extern VOID EFIAPI ArmInvalidateDataCacheEntryBySetWay(IN UINTN SetWayFormat);
extern VOID EFIAPI ArmCleanDataCacheEntryBySetWay(IN UINTN SetWayFormat);
#if defined(__arm__)
#define DataCacheOperation ArmV7DataCacheOperation
#elif defined(__aarch64__)
#define DataCacheOperation AArch64DataCacheOperation
#endif

// linux kernel arm64 boot main
typedef void(*linux_kernel_arm64_main)(
        UINTN fdt_address,
        UINT64 unknown0,
        UINT64 unknown1,
        UINT64 unknown2
);

// linux kernel arm32 boot main
typedef void(*linux_kernel_arm32_main)(
        UINT32 zero,
        UINT32 arch,
        UINTN fdt_address
);

// linux kernel main
typedef union linux_kernel_main{
        UINTN address;
        void*pointer;
        linux_kernel_arm32_main arm32;
        linux_kernel_arm64_main arm64;
}linux_kernel_main;

#define LINUX_ARM32_OFFSET 0x00008000
#define LINUX_ARM64_OFFSET 0x00080000
#define MEM_ALIGN          0x00200000
#define MAX_DTB_SIZE       0x00200000
#define MAX_DTBO_SIZE      0x01800000
#define MAX_KERNEL_SIZE    0x04000000
#define MAGIC_DTBO         0x1EABB7D7
#define MAGIC_KERNEL_ARM32 0x016f2818
#define MAGIC_KERNEL_ARM64 0x644d5241

static VOID EFIAPI ArmInvalidateDataCacheInternal(VOID){
	// FUCK ASSERT
	ArmDataSynchronizationBarrier();
	DataCacheOperation(ArmInvalidateDataCacheEntryBySetWay);
}

static VOID EFIAPI ArmCleanDataCacheInternal(VOID){
	// FUCK ASSERT
	ArmDataSynchronizationBarrier();
	DataCacheOperation(ArmCleanDataCacheEntryBySetWay);
}

static void platform_cleanup(){
	DEBUG((EFI_D_INFO,"boot: cleanup platform...\n"));

	ArmCleanDataCacheInternal();
	ArmDisableDataCache();
	ArmInvalidateDataCacheInternal();

	ArmDisableInstructionCache();
	ArmInvalidateInstructionCache();

	ArmDisableMmu();
}

static void exit_boot_services(void){
	UINT32 dv;
	EFI_STATUS st;
	EFI_MEMORY_DESCRIPTOR *map=NULL;
	UINTN size=0,pages=0,mk,ds;

	DEBUG((EFI_D_INFO,"boot: shutdown uefi boot services...\n"));
	do{
		st=gBS->GetMemoryMap(&size,map,&mk,&ds,&dv);
		if(st==EFI_BUFFER_TOO_SMALL){
			pages=EFI_SIZE_TO_PAGES(size)+1;
			map=AllocatePages(pages);
			ASSERT((map!=NULL));
			if(!map)abort();
			st=gBS->GetMemoryMap(&size,map,&mk,&ds,&dv);
		}
		if(!EFI_ERROR(st)){
			st=gBS->ExitBootServices(gImageHandle,mk);
			if(EFI_ERROR(st)){
				FreePages(map,pages);
				map=NULL,size=0;
			}
		}
	}while(EFI_ERROR(st));
}

int boot_linux_arm(unsigned char *kernel_raw, off_t kernel_raw_size, unsigned char *ramdisk_raw, off_t ramdisk_size, unsigned char *dtb_raw, off_t dtb_raw_size){
	linux_kernel_main main;

	if(dtb_raw_size>MAX_DTB_SIZE){
		char buf[64];
		droidboot_log(DROIDBOOT_LOG_ERROR,
			"dtb too large: %zu bytes (%s)",dtb_raw_size,
			make_readable_str_buf(buf,sizeof(buf),dtb_raw_size,1,0)
		);
		return -1;
	}

	droidboot_log(DROIDBOOT_LOG_TRACE,"kernel main at 0x%llx",(unsigned long long)(UINTN)kernel_raw);

	EfiSignalEventReadyToBoot();
	REPORT_STATUS_CODE(
		EFI_PROGRESS_CODE,(
			EFI_SOFTWARE_DXE_BS_DRIVER|
			EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT
		)
	);

	droidboot_log(DROIDBOOT_LOG_INFO,"prepare platform for arm linux boot...");
	exit_boot_services();
	platform_cleanup();

	// FIXME: cannot boot qualcomm android kernel now
	main.pointer=kernel_raw;
	DEBUG((EFI_D_INFO,"boot: run arm linux kernel main at 0x%llx\n",main.address));
	main.arm64((UINTN)dtb_raw,0,0,0);

	DEBUG((EFI_D_ERROR,"Boot Failed\n"));
	ASSERT(FALSE);
	while(1)CpuDeadLoop();
	return 0;
}
#else
#include"internal.h"
int boot_linux_arm(linux_boot*boot __attribute__((unused))){return -1;}
#endif
