LOCAL_PATH := $(shell pwd)
PLATFORM := $(strip $(PLATFORM))

LK_DEVICES := cedric yggdrasil yggdrasilx

ifeq (,$(PLATFORM))
    $(error Please define the target device)
endif

ifneq (,$(filter $(LK_DEVICES), $(PLATFORM)))
    include $(LOCAL_PATH)/lk/$(PLATFORM)/product.mk
else
    $(error Unsupported target device)
endif
