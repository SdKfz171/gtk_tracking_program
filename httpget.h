#pragma once

#include "element.h"

struct url_data {
    size_t size;
    char* data;
};

#if 0
char * HttpGet(const char * url, char * * response); 
#else
int HttpGet(const char * url, char * * response); 
#endif