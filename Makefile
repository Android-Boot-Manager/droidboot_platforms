LOCAL_PATH := $(shell pwd)
PLATFORM := $(strip $(PLATFORM))

LK_DEVICES := cedric yggdrasil yggdrasilx
UEFI_DEVICES := pro1

ifeq (,$(PLATFORM))
    $(error Please define the target device)
endif

ifneq (,$(filter $(LK_DEVICES), $(PLATFORM)))
    include $(LOCAL_PATH)/lk.mk
else
ifneq (,$(filter $(UEFI_DEVICES), $(PLATFORM)))
    include $(LOCAL_PATH)/uefi.mk
else
    $(error Unsupported target device)
endif
endif
