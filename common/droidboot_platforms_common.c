#if defined(PLATFORM_YGGDRASIL)
#include <lk_mtk_common.h>
#include <yggdrasil.h>
#elif defined(PLATFORM_YGGDRASILX)
#include <lk_mtk_common.h>
#include <yggdrasilx.h>
#elif defined(PLATFORM_CEDRIC)
#include <lk_qcom_common.h>
#include <yggdrasil.h>
#elif defined(PLATFORM_PRO1)
#include <droidboot_uefi_common.h>
#include <common_abl.h>
#include <pro1.h>
#elif defined(PLATFORM_PRO1X)
#include <droidboot_uefi_common.h>
#include <common_abl.h>
#include <pro1x.h>
#endif
