DROIDBOOT_PLATFORM_LOCAL_PATH := $(GET_LOCAL_DIR)
DROIDBOOT_PLATFORM ?= $(strip $(PLATFORM))

LK_DEVICES := qcom yggdrasil yggdrasilx astroslide mimameid cosmocom GX4

ifdef OBJS
INCLUDES += -I$(DROIDBOOT_PLATFORM_LOCAL_PATH)/common
INCLUDES += -I$(DROIDBOOT_PLATFORM_LOCAL_PATH)/libc-hack
INCLUDES += -I$(DROIDBOOT_PLATFORM_LOCAL_PATH)/libc-hack/lk
OBJS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/common/droidboot_platform_common.o $(DROIDBOOT_PLATFORM_LOCAL_PATH)/libc-hack/lk/stroq.o $(DROIDBOOT_PLATFORM_LOCAL_PATH)/libc-hack/lk/strtouq.o $(DROIDBOOT_PLATFORM_LOCAL_PATH)/libc-hack/lk/sscanf.o
else
GLOBAL_INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/common
GLOBAL_INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/libc-hack
GLOBAL_INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/libc-hack/lk
MODULE_SRCS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/common/droidboot_platform_common.c
endif

ifeq (,$(DROIDBOOT_PLATFORM))
    $(error Please define the target device)
endif

ifneq (,$(filter $(LK_DEVICES), $(DROIDBOOT_PLATFORM)))
    include $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/$(DROIDBOOT_PLATFORM)/product.mk
else
    $(error Unsupported target device)
endif
