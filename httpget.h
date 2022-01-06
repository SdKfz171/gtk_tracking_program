#pragma once

#include "element.h"

struct url_data {
    size_t size;
    char* data;
};

char * HttpGet(const char * url, char * * response); 