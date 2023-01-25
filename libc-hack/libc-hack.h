#if defined (PLATFORM_UBOOT)
#include <linux/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdint-hack.h>
typedef int status_t;
#elif defined (PLATFORM_UEFI)
#endif
