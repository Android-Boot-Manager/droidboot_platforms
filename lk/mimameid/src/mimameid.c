#include <stdio.h>

char* get_codename(){
    return "mimameid";
}

char* get_vendor(){
    return "Hallo Welt Systeme";
}

char* get_model(){
    return "Volla Phone 22";
}

int droidboot_internal_get_disp_buffer_height()
{
    return 30;
}

bool droidboot_internal_use_double_buffering()
{
    return false;
}
