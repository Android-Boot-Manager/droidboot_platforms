#include <stdio.h>

char* get_codename(){
    return "astroslide";
}

char* get_vendor(){
    return "Planet Computers";
}

char* get_model(){
    return "Astro Slide";
}

int droidboot_internal_get_disp_buffer_height()
{
    return 30;
}

bool droidboot_internal_use_double_buffering()
{
    return false;
}
