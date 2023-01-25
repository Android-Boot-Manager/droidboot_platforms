/*
 *
 * Copyright (C) 2021 BigfootACA <bigfoot@classfun.cn>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include <stdarg.h>
#include <Library/DebugLib.h>


int printf(const char *format, ...)
{
  char printbuffer[512];
  va_list args;
  va_start(args, format);
  vsnprintf(printbuffer, 512, format, args);
  va_end(args);
  DEBUG ((EFI_D_ERROR, printbuffer));

  return 0;
}
