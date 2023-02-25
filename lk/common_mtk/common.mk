include $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common/common.mk
ifdef OBJS
INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk/src
OBJS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk/src/lk_mtk_common.o
else
GLOBAL_INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk/src
MODULE_SRCS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk/src/lk_mtk_common.c
endif
