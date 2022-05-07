LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += $(LOCAL_PATH)/lk/common

ifeq ($(PLATFORM),yggdrasil)
	include $(LOCAL_PATH)/lk/common/common.mk
	include $(LOCAL_PATH)/lk/common_mtk/common_mtk.mk
	include $(LOCAL_PATH)/lk/yggdrasil/yggdrasil.mk
	INCLUDES += $(LOCAL_PATH)/lk/common_mtk
	INCLUDES += $(LOCAL_PATH)/lk/yggdrasil
else ifeq ($(PLATFORM),yggdrasilx)
    include $(LOCAL_PATH)/lk/common/common.mk
	include $(LOCAL_PATH)/lk/common_mtk/common_mtk.mk
	include $(LOCAL_PATH)/lk/yggdrasilx/yggdrasilx.mk
	INCLUDES += $(LOCAL_PATH)/lk/common_mtk
	INCLUDES += $(LOCAL_PATH)/lk/yggdrasilx
else ifeq ($(PLATFORM),cedric)
    include $(LOCAL_PATH)/lk/common/common.mk
	include $(LOCAL_PATH)/lk/common_mtk/common_mtk.mk
	include $(LOCAL_PATH)/lk/yggdrasil/yggdrasil.mk
	INCLUDES += $(LOCAL_PATH)/lk/common_qcom
	INCLUDES += $(LOCAL_PATH)/lk/cedric
else
    $(error Your target is either not set or is not supported)
endif
