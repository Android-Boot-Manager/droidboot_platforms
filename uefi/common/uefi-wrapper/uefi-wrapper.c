#include<Library/PcdLib.h>
#include<Library/DebugLib.h>
#include<Library/UefiBootManagerLib.h>
#include<Library/ReportStatusCodeLib.h>
#include<droidboot_main.h>
#include <droidboot_logging.h>
#include<droidboot_uefi_common.h>

EFI_SYSTEM_TABLE                    *gST2;

EFI_SYSTEM_TABLE *getSystemTable()
{
    return gST2;
}
// droidboot uefi entry point
EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ih,IN EFI_SYSTEM_TABLE*st){
    droidboot_log(DROIDBOOT_LOG_ERROR, "Welcome to droidboot UEFI wrapper\n");
    REPORT_STATUS_CODE(EFI_PROGRESS_CODE,(EFI_SOFTWARE_DXE_BS_DRIVER|EFI_SW_PC_USER_SETUP));

    EfiBootManagerConnectAll();
    EfiBootManagerRefreshAllBootOption();
    gST2=st;
    droidboot_internal_setgST(st);
    droidboot_init();
    droidboot_log(DROIDBOOT_LOG_INFO, "droidboot_init done!\n");
    droidboot_show_dualboot_menu();
    return 0;
}
