EXTRA_CFLAGS+=-I$(VPATH)/lib/droidboot/droidboot_platforms/common -I$(VPATH)/lib/droidboot/droidboot_platforms/libc-hack -I$(VPATH)/lib/droidboot/droidboot_platforms/u-boot -DPLATFORM_UBOOT=1
obj-y += u-boot/u-boot.o
obj-y += common/droidboot_platform_common.o
