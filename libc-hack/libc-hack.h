#ifndef LIBC_HACK_H
#define  LIBC_HACK_H
#if defined (PLATFORM_UBOOT)
#include <linux/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdint-hack.h>
typedef int status_t;
#elif defined (PLATFORM_UEFI)
#endif

// Common defenition (always safeguard with ifndef)
#ifndef INT32_MAX
#define INT32_MAX   0x7fffffff
#endif
#endif //  LIBC_HACK_H
