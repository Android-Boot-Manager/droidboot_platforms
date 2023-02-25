include $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common/common.mk
ifdef OBJS
INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk2/src
OBJS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk2/src/lk_mtk2_common.o
else
GLOBAL_INCLUDES += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk2/src
MODULE_SRCS += $(DROIDBOOT_PLATFORM_LOCAL_PATH)/lk/common_mtk2/src/lk_mtk2_common.c
endif
