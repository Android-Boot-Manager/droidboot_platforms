#include <string.h>

#include <droidboot_logging.h>
#include <droidboot_error.h>
#include <droidboot_libufdt.h>
#include <sys/types.h>
#include <droidboot_libfdt.h>

// We need device name from dtbo as it apparently not overlayed by ufdt
droidboot_error droidboot_uefi_dtb_device_patch(void*fdt, char *device_name)
{
    droidboot_error err;
    uint off;

    // Check fdt header, just in case
    err = fdt_check_header(fdt);
    if(err!=0){
        droidboot_log(DROIDBOOT_LOG_ERROR, "dtb header check failed: %d\n", err);
        return err;
    }

    // On some devices BL disable firmware node, if left enable confuses android's init
   /* if (strstr(device_name, "pipa") != NULL || strstr(device_name, "vayu") != NULL){
        droidboot_log(DROIDBOOT_LOG_INFO, "device quirks: Patching dtb, disable fstab/vendor\n");
        err=fdt_path_offset(fdt,"/firmware/android/fstab/vendor");
        if(err<0){
            droidboot_log(DROIDBOOT_LOG_ERROR, "get chosen node failed: %s\n", fdt_strerror(err));
            return err;
        }
        off=err;
        err=fdt_setprop_string(fdt, off, "status", "disabled");
        if(err<0){
            droidboot_log(DROIDBOOT_LOG_ERROR, "Setting status failed: %s\n", fdt_strerror(err));
            return err;
        }
    }*/

    if (strstr(device_name, "pipa") != NULL){
        droidboot_log(DROIDBOOT_LOG_INFO, "device quirks: Patching dtb, adding ddr type\n");
        err=fdt_path_offset(fdt,"/memory");
        if(err<0){
            droidboot_log(DROIDBOOT_LOG_ERROR, "get memory node failed: %s\n", fdt_strerror(err));
            return err;
        }
        off=err;
        err=fdt_setprop_u32(fdt,off,"ddr_device_type",0x88);
        if(err<0){
            droidboot_log(DROIDBOOT_LOG_ERROR, "Setting status failed: %s\n", fdt_strerror(err));
            return err;
        }
    }
    return DROIDBOOT_EOK;
}
