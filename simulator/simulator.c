#include <stdio.h>
#include "simulator.h"
#include "droidboot_main.h"
#include "droidboot_boot_logo.h"
#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#include <android/bitmap.h>
#include <jni.h>
#include <stdlib.h>

static pthread_t t;
static bool s_simulator_running;
uint8_t s_simulator_keymask;
static JavaVM* s_simulator_jvm;
static jobject s_simulator_bitmap, s_simulator_thiz;
static lv_coord_t s_simulator_h, s_simulator_w;
extern int droidboot_exit;

lv_coord_t droidboot_internal_get_display_height()
{
	return s_simulator_h;
}

lv_coord_t droidboot_internal_get_display_width()
{
	return s_simulator_w;
}

JNIEXPORT void simulator_stop(JNIEnv* env)
{
	if (!s_simulator_running) return;
	__android_log_print(ANDROID_LOG_INFO, "droidboot", "closing");
	droidboot_exit = 0;
}

__attribute__((unused)) JNIEXPORT void simulator_start(JNIEnv* env, jobject thiz, jobject bitmap, jint w, jint h)
{
	(*env)->GetJavaVM(env, &s_simulator_jvm);
	s_simulator_bitmap = (*env)->NewGlobalRef(env, bitmap);
	s_simulator_thiz = (*env)->NewGlobalRef(env, thiz);
	s_simulator_h = (lv_coord_t)h;
	s_simulator_w = (lv_coord_t)w;
	droidboot_init();
	droidboot_show_boot_logo();
	droidboot_show_dualboot_menu();
	simulator_stop(env);
	s_simulator_running = false;
	pthread_join(t, NULL);
	(*env)->DeleteGlobalRef(env, s_simulator_bitmap);
	s_simulator_bitmap = NULL;
	__android_log_print(ANDROID_LOG_INFO, "droidboot", "goodbye");
	jclass cls = (*env)->GetObjectClass(env, s_simulator_thiz);
	jmethodID redraw = (*env)->GetMethodID(env, cls, "finish", "()V");
	(*env)->CallVoidMethod(env, s_simulator_thiz, redraw);
	(*env)->DeleteGlobalRef(env, s_simulator_thiz);
	s_simulator_thiz = NULL;
	s_simulator_jvm = NULL;
}

JNIEXPORT void simulator_key(jint key)
{
	s_simulator_keymask |= key;
}

void droidboot_internal_fb_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	if (s_simulator_bitmap != NULL) {
		JNIEnv* env;
		int ret;
		if ((ret = (*s_simulator_jvm)->GetEnv(s_simulator_jvm, (void **) &env, JNI_VERSION_1_6)) != JNI_OK) {
			__android_log_print(ANDROID_LOG_ERROR, "droidboot", "failed to get jni env: %d", ret);
		}
		uint32_t *addr;
		while ((ret = AndroidBitmap_lockPixels(env, s_simulator_bitmap, (void **) &addr)) != ANDROID_BITMAP_RESULT_SUCCESS) {
			__android_log_print(ANDROID_LOG_ERROR, "droidboot", "failed to lock bitmap (%d), trying again", ret);
			droidboot_internal_delay(10);
		}
		//__android_log_print(ANDROID_LOG_VERBOSE, "droidboot", "locked fb %p", addr);
		for (uint32_t y = area->y1; y <= area->y2; y++) {
			for (uint32_t x = area->x1; x <= area->x2; x++) {
				addr[(y*s_simulator_w)+x] = 0xffU << 24 | color_p->ch.blue << 16 | color_p->ch.green << 8 | color_p->ch.red;
				color_p++;
			}
		}

		while ((ret = AndroidBitmap_unlockPixels(env, s_simulator_bitmap)) != ANDROID_BITMAP_RESULT_SUCCESS) {
			__android_log_print(ANDROID_LOG_ERROR, "droidboot", "failed to unlock bitmap (%d), trying again", ret);
			droidboot_internal_delay(10);
		}

		jclass cls = (*env)->GetObjectClass(env, s_simulator_thiz);
		jmethodID redraw = (*env)->GetMethodID(env, cls, "redraw", "()V");
		(*env)->CallVoidMethod(env, s_simulator_thiz, redraw);
		//__android_log_print(ANDROID_LOG_VERBOSE, "droidboot", "unlocked fb %p", addr);
	}
	// Inform the graphics library that we are ready with the flushing
	lv_disp_flush_ready(disp_drv);
}

void droidboot_internal_key_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
	if (s_simulator_keymask & 1) {
		data->key = LV_KEY_PREV;
		data->state = LV_INDEV_STATE_PRESSED;
		s_simulator_keymask &= ~1;
	} else if (s_simulator_keymask & 2) {
		data->key = LV_KEY_NEXT;
		data->state = LV_INDEV_STATE_PRESSED;
		s_simulator_keymask &= ~2;
	} else if (s_simulator_keymask & 4) {
		data->key = LV_KEY_ENTER;
		data->state = LV_INDEV_STATE_PRESSED;
		s_simulator_keymask &= ~4;
	} else if (s_simulator_keymask & 8) {
		data->key = LV_KEY_PREV;
		data->state = LV_INDEV_STATE_RELEASED;
		s_simulator_keymask &= ~8;
	} else if (s_simulator_keymask & 16) {
		data->key = LV_KEY_NEXT;
		data->state = LV_INDEV_STATE_RELEASED;
		s_simulator_keymask &= ~16;
	} else if (s_simulator_keymask & 32) {
		data->key = LV_KEY_ENTER;
		data->state = LV_INDEV_STATE_RELEASED;
		s_simulator_keymask &= ~32;
	}
}

droidboot_error droidboot_internal_platform_init()
{
	s_simulator_running = true;
	return DROIDBOOT_EOK;
}

ssize_t dridboot_internal_sd_read_block(void *buf, uint32_t block, uint count)
{
	if (!droidboot_internal_sd_exists())
		return 0;
	JNIEnv* env;
	int ret;
	if ((ret = (*s_simulator_jvm)->GetEnv(s_simulator_jvm, (void **) &env, JNI_VERSION_1_6)) != JNI_OK) {
		__android_log_print(ANDROID_LOG_ERROR, "droidboot", "failed to get jni env: %d", ret);
	}

	jclass cls = (*env)->GetObjectClass(env, s_simulator_thiz);
	jmethodID readBlk = (*env)->GetMethodID(env, cls, "readBlocks", "(JI)[B");
	jbyteArray arr = (*env)->CallObjectMethod(env, s_simulator_thiz, readBlk, (jlong)block*droidboot_internal_sd_blklen(), (jint)count*droidboot_internal_sd_blklen());
	jbyte* bufferPtr = (*env)->GetByteArrayElements(env, arr, NULL);
	jsize lengthOfArray = (*env)->GetArrayLength(env, arr);
	memcpy(buf, bufferPtr, lengthOfArray);
	(*env)->ReleaseByteArrayElements(env, arr, bufferPtr, JNI_ABORT);
	(*env)->DeleteLocalRef(env, arr);
	return lengthOfArray;
}

ssize_t dridboot_internal_sd_write_block(const void *buf, uint32_t block, uint count)
{
	return 0; // do not break our sd card
}

uint32_t droidboot_internal_sd_blklen()
{
	if (!droidboot_internal_sd_exists())
		return 0;
	return 512;
}

uint64_t droidboot_internal_sd_blkcnt()
{
	if (!droidboot_internal_sd_exists())
		return 0;
	JNIEnv* env;
	int ret;
	if ((ret = (*s_simulator_jvm)->GetEnv(s_simulator_jvm, (void **) &env, JNI_VERSION_1_6)) != JNI_OK) {
		__android_log_print(ANDROID_LOG_ERROR, "droidboot", "failed to get jni env: %d", ret);
	}

	jclass cls = (*env)->GetObjectClass(env, s_simulator_thiz);
	jmethodID countBlk = (*env)->GetMethodID(env, cls, "blockCount", "()J");
	return (uint64_t)(*env)->CallLongMethod(env, s_simulator_thiz, countBlk);
}

bool droidboot_internal_sd_exists()
{
	return s_simulator_running;
}

bool droidboot_internal_have_fallback()
{
	return true;
}

bool droidboot_internal_use_double_buffering()
{
	return true;
}

//lvgl thread
static void* droidboot_lv_tick_inc_thread(void * arg) {
	/*Handle LitlevGL tick*/
	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_6;
	args.name = "lv_tick_inc_thread";
	args.group = NULL;
	JNIEnv* env;
	(*s_simulator_jvm)->AttachCurrentThread(s_simulator_jvm, &env, &args);
	while (s_simulator_running) {
		droidboot_internal_delay(1);
		lv_tick_inc(1);
	}
	if (s_simulator_jvm != NULL)
		(*s_simulator_jvm)->DetachCurrentThread(s_simulator_jvm);
	return 0;
}

void droidboot_internal_lvgl_threads_init()
{
	pthread_create(&t, NULL, droidboot_lv_tick_inc_thread, NULL);
}

void droidboot_internal_platform_on_screen_log(const char *buf)
{
	JNIEnv* env;
	int ret;
	if ((ret = (*s_simulator_jvm)->GetEnv(s_simulator_jvm, (void **) &env, JNI_VERSION_1_6)) != JNI_OK) {
		__android_log_print(ANDROID_LOG_ERROR, "droidboot", "failed to get jni env: %d", ret);
	}

	jclass cls = (*env)->GetObjectClass(env, s_simulator_thiz);
	jmethodID screenPrint = (*env)->GetMethodID(env, cls, "screenPrint", "(Ljava/lang/String;)V");
	jstring str = (*env)->NewStringUTF(env, buf);
	(*env)->CallVoidMethod(env, s_simulator_thiz, screenPrint, str);
	(*env)->DeleteLocalRef(env, str);
}

void droidboot_internal_platform_system_log(const char *buf)
{
	__android_log_print(ANDROID_LOG_VERBOSE, "droidboot", "%s", buf);
}

void droidboot_internal_delay(unsigned int time)
{
	usleep(time*1000);
}

void droidboot_internal_boot_linux_from_ram(void *kernel_raw, uint64_t kernel_raw_size, void *ramdisk_raw, uint64_t ramdisk_size, void *dtb_raw, uint64_t dtb_raw_size, void *dtbo_raw, uint64_t dtbo_raw_size, char *options)
{
	// nothing happens
}

void droidboot_internal_pre_ramdisk_load(void *kernel_raw, uint64_t kernel_raw_size)
{
	// nothing happens
}

void *droidboot_internal_get_kernel_load_addr()
{
	return NULL;
}

void *droidboot_internal_get_ramdisk_load_addr()
{
	return NULL;
}

void *droidboot_internal_get_dtb_load_addr()
{
	return NULL;
}

void *droidboot_internal_get_dtbo_load_addr()
{
	return NULL;
}

bool droidboot_internal_append_ramdisk_to_kernel()
{
	return false;
}

void droidboot_internal_platform_tasks()
{
	droidboot_internal_delay(lv_timer_handler());
}