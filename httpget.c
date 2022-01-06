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

char * HttpGet(const char * url, char * * response){
    struct url_data data;
    data.size = 0;
    data.data = malloc(INT_MAX);

    CURL * ctx = curl_easy_init();
    CURLcode res;
    
    curl_easy_setopt(ctx, CURLOPT_URL, url);
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, &data);

    res = curl_easy_perform(ctx);
    if(res != CURLE_OK){
        exit(0);
    }
    
    curl_easy_cleanup(ctx);
    
    *response = (char *)malloc(data.size);
    strcpy(*response, data.data);

    free(data.data);

    return *response;
}

int main(int argc, char * argv[]){
    char * response;

    HttpGet("https://apis.tracker.delivery/carriers", &response);
    
    puts(response);

    List carrier_list;
    list_init(&carrier_list);

    GetCarrierList(&carrier_list, response);

    PrintCarrierList(&carrier_list);

    return 0;
}