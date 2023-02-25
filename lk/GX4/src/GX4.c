#include <stdio.h>

char* get_codename(){
    return "GX4";
}

char* get_vendor(){
    return "Gigaset";
}

char* get_model(){
    return "GX 4";
}

int droidboot_internal_get_disp_buffer_height()
{
    return 720;
}

bool droidboot_internal_use_double_buffering()
{
    return false;
}
