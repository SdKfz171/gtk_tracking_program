#pragma once
#include <time.h>

#include "list.h"

// Carrier Data
typedef struct Carrier_{
    char id[64];
    char name[64];
    char tel[64];
} Carrier;

// Tracking Data
struct From
{
    char name[512];
    struct tm time;
};

struct To
{
    char name[512];
    struct tm time;
};

struct State
{
    char id[512];
    char text[512];
};

struct Location
{
    char name[512];
};

struct Status
{
    char id[512];
    char text[512];
};

struct Progresses
{
    struct tm time;
    struct Location location;
    struct State status;
    char description[1024];
};

typedef struct _TrackingData {
    struct From from;
    struct To to;
    struct State state;
    struct Progresses progresses[64];
    int progress_count;
    Carrier carrier;
} TrackingData;

typedef struct _TrackingOption {
    char * carrier_id;
    char * tracking_number;
} TrackingOption;


List * GetCarrierList(List * list, const char * json);
ListElmt * list_select(List * list, int index);
Carrier * GetCarriers(const char * json);
int GetCarrierCount(const char * json);
void PrintCarrierList(List * list);

char * GetTrackingOptionJson(TrackingOption * options, int len);
TrackingData * GetTrackingData_(const char * json);
int GetTrackingData(const char * json, TrackingData * tracking_data);
void PrintTrackingData(TrackingData * data);