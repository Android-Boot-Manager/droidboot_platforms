#include <lvgl.h>
#include <kernel/thread.h>

//lvgl thread
static int droidboot_lv_tick_inc_thread(void * arg) {
  /*Handle LitlevGL tick*/
  while (1) {
    thread_sleep(1);
    lv_tick_inc(1);
    //lv_timer_handler();
  }
  return 0;
}

//lvgl thread
static int droidboot_lv_timer_handler_thread(void * arg) {
  /*Handle LitlevGL tick*/
  while (1) {
    thread_sleep(1);
    lv_timer_handler();
  }
  return 0;
}

void droidboot_internal_lvgl_threads_init()
{
    //Create threads for LVGL
    thread_t *lvgl_thr1;
    lvgl_thr1=thread_create("lvgl_thr1", & droidboot_lv_tick_inc_thread, NULL, HIGHEST_PRIORITY, 16*1024);
    thread_resume(lvgl_thr1);
    thread_t *lvgl_thr2;
    lvgl_thr2=thread_create("lvgl_thr2", & droidboot_lv_timer_handler_thread, NULL, HIGHEST_PRIORITY, 16*1024);
    thread_resume(lvgl_thr2);
}

// Nothing to do here, we have threads
void droidboot_internal_platform_tasks()
{
  thread_sleep(200);
}

// No lk targets do implement dtbo yet
void *droidboot_internal_get_dtbo_load_addr()
{
    return NULL;
}

