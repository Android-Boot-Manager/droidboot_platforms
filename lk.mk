DROIDBOOT_PLATFORM_LOCAL_PATH := $(GET_LOCAL_DIR)
DROIDBOOT_PLATFORM ?= $(strip $(PLATFORM))

LK_DEVICES := cedric yggdrasil yggdrasilx astroslide

INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/common
OBJS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/common/droidboot_platform_common.o

ifeq (,$(DROIDBOOT_PLATFORM))
    $(error Please define the target device)
endif

ifneq (,$(filter $(LK_DEVICES), $(DROIDBOOT_PLATFORM)))
    include $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/$(DROIDBOOT_PLATFORM)/product.mk
else
    $(error Unsupported target device)
endif
