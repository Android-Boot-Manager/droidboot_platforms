ifdef OBJS
INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common/src
OBJS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common/src/lk_common.o
else
GLOBAL_INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common/src
MODULE_SRCS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common/src/lk_common.c
endif
