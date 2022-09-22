#include <stdio.h>

char* get_codename(){
    return "cosmocom";
}

char* get_vendor(){
    return "Planet Computers";
}

char* get_model(){
    return "Cosmo Communicator";
}

int droidboot_internal_get_disp_buffer_height()
{
    return 30;
}

bool droidboot_internal_use_double_buffering()
{
    return false;
}
