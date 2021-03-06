#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <limits.h>

#include "httpget.h"

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;

    data->size += (size * nmemb);

    tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}

#if 0
char * HttpGet(const char * url, char * * response){
    puts(url);
    struct url_data data;
    data.size = 0;
    data.data = malloc(INT_MAX);

    CURL * ctx = curl_easy_init();
    CURLcode res;
    int response_code = 0;
    
    curl_easy_setopt(ctx, CURLOPT_URL, url);
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &data);

    res = curl_easy_perform(ctx);
    if(res != CURLE_OK){
        exit(0);
    }

    curl_easy_getinfo(ctx, CURLINFO_RESPONSE_CODE, &response_code);
    printf("response_code: %d\r\n", response_code);

    if(response_code == 404){
        puts("invoice not exist!!");
        // exit(0);
        return NULL;
    }
    
    curl_easy_cleanup(ctx);
    
    *response = (char *)malloc(data.size);
    strcpy(*response, data.data);

    free(data.data);

    return *response;
}
#else
int HttpGet(const char * url, char * * response){
    puts(url);
    struct url_data data;
    data.size = 0;
    data.data = malloc(INT_MAX);

    CURL * ctx = curl_easy_init();
    CURLcode res;
    int response_code = 0;
    
    curl_easy_setopt(ctx, CURLOPT_URL, url);
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &data);

    res = curl_easy_perform(ctx);
    if(res != CURLE_OK){
        exit(0);
    }

    curl_easy_getinfo(ctx, CURLINFO_RESPONSE_CODE, &response_code);
    printf("response_code: %d\r\n", response_code);

    if(response_code == 404){
        puts("invoice not exist!!");
    }
    
    curl_easy_cleanup(ctx);
    
    *response = (char *)malloc(data.size);
    strcpy(*response, data.data);

    free(data.data);

    return response_code;
}
#endif

#if 0
#define CARRIER 0
#define TRACKER 1
int main(int argc, char * argv[]){
    char * response;
#if CARRIER
    HttpGet("https://apis.tracker.delivery/carriers", &response);
    
    puts(response);

    List carrier_list;
    list_init(&carrier_list);

    GetCarrierList(&carrier_list, response);

    PrintCarrierList(&carrier_list);
#elif TRACKER 
    HttpGet("https://apis.tracker.delivery/carriers/kr.epost/tracks/6896770230128", &response);
    
    puts(response);

    TrackingData tracking_data;
    GetTrackingData(response, &tracking_data);

    PrintTrackingData(&tracking_data);
#endif
    return 0;
}
#endif