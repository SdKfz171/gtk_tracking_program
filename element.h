#pragma once
#include <time.h>

#include "list.h"

typedef struct Carrier_{
    char id[64];
    char name[64];
    char tel[64];
} Carrier;

List * GetCarrierList(List * list, const char * json);
ListElmt * list_select(List * list, int index);
Carrier * GetCarriers(const char * json);
int GetCarrierCount(const char * json);
void PrintCarrierList(List * list);