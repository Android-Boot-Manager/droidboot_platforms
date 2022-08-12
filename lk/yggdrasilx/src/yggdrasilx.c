#include <stdio.h>

char* get_codename(){
    return "yggdrasilx";
}

char* get_vendor(){
    return "Hallo Welt Systeme";
}

char* get_model(){
    return "Volla Phone X";
}

int droidboot_internal_get_disp_buffer_height()
{
    return droidboot_get_display_height()/6;
}

bool droidboot_internal_use_double_buffering()
{
    return false;
}
