#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <json.h>

#include "element.h"

#define CHECK_NULL(X) (X == NULL ? 1 : 0)

char *window_dateparsing(const char *buf, struct tm *time)
{
    // time format
    // "time": "2022-01-07T07:45:00+09:00"

    int ret = sscanf(buf, "%d-%d-%dT%d:%d:%d", &(time->tm_yday), &(time->tm_mon), &(time->tm_mday), &(time->tm_hour), &(time->tm_min), &(time->tm_sec));
    // strftime(json_object_get_string(time), "%Y-%m-%dT%T%z", &(tracking_data->from.time)) == NULL

    if(ret == EOF || ret != 6)
        return NULL;
    else
        return buf;
}

int GetCarrierCount(const char * json){
    int carrier_count;

    json_object * jobj = json_tokener_parse(json);
    carrier_count = json_object_array_length(jobj);

    return carrier_count;
}

List * GetCarrierList(List * list, const char * json){
    // printf("carrier len : %d\r\n", GetCarrierCount(json));
    int len = GetCarrierCount(json);
    Carrier * carriers;
    
    carriers = (Carrier *)malloc(sizeof(Carrier) * len);
    memcpy(carriers, GetCarriers(json), sizeof(Carrier) * len);

    for(int i = 0; i < len; i++){
        // printf("test %d\n",i);
        if(!list_size(list)) list_insert_next(list, NULL, &(carriers[i]));
        else list_insert_next(list, list_tail(list), &(carriers[i]));
        // free(temp);
    }
    free(carriers);
    return list;
}

ListElmt * list_select(List * list, int index){
	ListElmt * element;
    Carrier * carrier;
	
    element = list_head(list);
    int count = 0;
    printf("index is %d\n", index);
    
    for(int i = 0; i < index; i++){
        carrier = list_data(element);
        element = list_next(element);
    }

    // puts(carrier->id);
		
	return element;
}

Carrier * GetCarriers(const char * json){
    Carrier * carriers;
    int carrier_count;

    json_object * jobj = json_tokener_parse(json);

    json_object * carrier_array_object;
    json_object * id, * name, * tel;

    // puts("\ncarriers");

    carrier_count = json_object_array_length(jobj);
    carriers = (Carrier *)malloc(sizeof(Carrier) * carrier_count);
    
    for(int i = 0; i < carrier_count; i++){
        carrier_array_object = json_object_array_get_idx(jobj, i);

        id = json_object_object_get(carrier_array_object, "id");
        name = json_object_object_get(carrier_array_object, "name");
        tel = json_object_object_get(carrier_array_object, "tel");

        // carriers[i].id = (char *)malloc(json_object_get_string_len(id));
        // carriers[i].name = (char *)malloc(json_object_get_string_len(name));
        // carriers[i].tel = (char *)malloc(json_object_get_string_len(tel));

        if(id != NULL)      strcpy(carriers[i].id, json_object_get_string(id));
        if(name != NULL)    strcpy(carriers[i].name, json_object_get_string(name));
        if(tel != NULL)     strcpy(carriers[i].tel, json_object_get_string(tel));
    }
    return carriers;
}

void PrintCarrierList(List * list){
    ListElmt * element;
    Carrier * carrier;

    element = list_head(list);
    puts("List Elements");
    while (1)
    {
        carrier = list_data(element);
        
        puts(carrier->id);
        puts(carrier->name);
        puts(carrier->tel);

        if(list_is_tail(element))
            break;
        else
            element = list_next(element);
    }
}

char * GetTrackingOptionJson(TrackingOption * options, int len){
    json_object * jobj;
    json_object * tracking_object;
    json_object * tracking_array;

    jobj = json_object_new_object();
    tracking_object = json_object_new_object();
    tracking_array = json_object_new_array();
    
    for(int i = 0; i < len; i++){
        json_object_object_add(tracking_object, "carrier_id", json_object_new_string(options[i].carrier_id));
        json_object_object_add(tracking_object, "tracking_number", json_object_new_string(options[i].tracking_number));

        json_object_array_add(tracking_array, tracking_object);
    }

    json_object_object_add(jobj, "tracking_option", tracking_array);

    return json_object_to_json_string(jobj);
}

#define X_POS(N) fprintf(stdout, "\033[%dG", N);
#define PRINT_TREE(N, D, ...) X_POS(N)  printf(D, __VA_ARGS__);

void PrintTrackingData(TrackingData * data){
    printf("%10s─┬─name:%s\n", "From", data->from.name);
    PRINT_TREE(12, "└─time:%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", data->from.time.tm_year + 1900, data->from.time.tm_mon + 1, data->from.time.tm_mday, data->from.time.tm_hour, data->from.time.tm_min, data->from.time.tm_sec, 9, 0);
    printf("%10s─┬─name:%s\n", "To", data->to.name);
    PRINT_TREE(12, "└─time:%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", data->to.time.tm_year + 1900, data->to.time.tm_mon + 1, data->to.time.tm_mday, data->to.time.tm_hour, data->to.time.tm_min, data->to.time.tm_sec, 9, 0);
    printf("%10s─┬─id:%s\n", "State", data->state.id);
    PRINT_TREE(12, "└─text:%s\n", data->state.text);
    
    printf("%10s", "Progresses─┬─");
    for(int i = 0; i < data->progress_count; i++){
        if(i != data->progress_count - 1){
            if(i > 0) PRINT_TREE(12, "├─", NULL);

            PRINT_TREE(13, "─┬─time:%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", data->progresses[i].time.tm_year + 1900, data->progresses[i].time.tm_mon + 1, data->progresses[i].time.tm_mday, data->progresses[i].time.tm_hour, data->progresses[i].time.tm_min, data->progresses[i].time.tm_sec, 9, 0);    
            PRINT_TREE(12, "│ ├─location─name:%s\n", data->progresses[i].location.name);
            PRINT_TREE(12, "│ ├─status─id:%s\n", data->progresses[i].status.id);
            PRINT_TREE(12, "│ ├─status─text:%s\n", data->progresses[i].status.text);
            PRINT_TREE(12, "│ └─status─description:%s\n", data->progresses[i].description);
            if(i == data->progress_count - 2) {PRINT_TREE(12, "└", NULL);}
        }
        else{
            PRINT_TREE(13, "─┬─time:%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", data->progresses[i].time.tm_year + 1900, data->progresses[i].time.tm_mon + 1, data->progresses[i].time.tm_mday, data->progresses[i].time.tm_hour, data->progresses[i].time.tm_min, data->progresses[i].time.tm_sec, 9, 0);    
            PRINT_TREE(14, "├─location─name:%s\n", data->progresses[i].location.name);
            PRINT_TREE(14, "├─status─id:%s\n", data->progresses[i].status.id);
            PRINT_TREE(14, "├─status─text:%s\n",  data->progresses[i].status.text);
            PRINT_TREE(14, "└─status─description:%s\n", data->progresses[i].description);
        }
    }
    printf("%10s─┬─id:%s\n", "Carrier", data->carrier.id);
    PRINT_TREE(12, "├─name:%s\n", data->carrier.name);
    PRINT_TREE(12, "└─tel:%s\n", data->carrier.tel);
}

#define PUTS 1
TrackingData * GetTrackingData_(const char * json){
    int progress_count = 0;

    TrackingData * tracking_data = (TrackingData *)malloc(sizeof(TrackingData));

    json_object * jobj = json_tokener_parse(json);
    
    json_object * message_object = json_object_object_get(jobj, "message");
    
    if(!CHECK_NULL(message_object)) return NULL;

    json_object * from_object = json_object_object_get(jobj, "from");
    json_object * to_object = json_object_object_get(jobj, "to");
    json_object * name = json_object_object_get(from_object, "name");
    json_object * time = json_object_object_get(from_object, "time");
    json_object * state_object = json_object_object_get(jobj, "state");
    json_object * id = json_object_object_get(state_object, "id");
    json_object * text = json_object_object_get(state_object, "text");
    json_object * progress_object = json_object_object_get(jobj, "progresses");
    json_object * progress_array_object;
    json_object * carrier_object = json_object_object_get(jobj, "carrier");

    if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->from.name, json_object_get_string(name));
    #if defined(_WIN32) || defined(_WIN64)
    if(!CHECK_NULL(json_object_get_string(time))) if(window_dateparsing(json_object_get_string(time), &(tracking_data->from.time)) == NULL) puts("format error");
    #else
    if(!CHECK_NULL(json_object_get_string(time))) if(strptime(json_object_get_string(time), "%Y-%m-%dT%T%z", &(tracking_data->from.time)) == NULL) puts("format error");
    #endif
    
#if PUTS
    puts("\nfrom");
    puts(tracking_data->from.name);
    printf("%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", tracking_data->from.time.tm_year + 1900, tracking_data->from.time.tm_mon + 1, tracking_data->from.time.tm_mday, tracking_data->from.time.tm_hour, tracking_data->from.time.tm_min, tracking_data->from.time.tm_sec, 9, 0);
#endif

    name = json_object_object_get(to_object, "name");
    time = json_object_object_get(to_object, "time");

    if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->to.name, json_object_get_string(name));
    #if defined(_WIN32) || defined(_WIN64)
    if(!CHECK_NULL(json_object_get_string(time))) if(window_dateparsing(json_object_get_string(time), &(tracking_data->to.time)) == NULL) puts("format error");
    #else
    if(!CHECK_NULL(json_object_get_string(time))) if(strptime(json_object_get_string(time), "%Y-%m-%dT%T%z", &(tracking_data->to.time)) == NULL) puts("format error");
    #endif
    
#if PUTS
    puts("\nto");
    puts(tracking_data->to.name);
    printf("%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", tracking_data->to.time.tm_year + 1900, tracking_data->to.time.tm_mon + 1, tracking_data->to.time.tm_mday, tracking_data->to.time.tm_hour, tracking_data->to.time.tm_min, tracking_data->to.time.tm_sec, 9, 0);
#endif

    if(!CHECK_NULL(json_object_get_string(id))) strcpy(tracking_data->state.id, json_object_get_string(id));
    if(!CHECK_NULL(json_object_get_string(text))) strcpy(tracking_data->state.text, json_object_get_string(text));

#if PUTS
    puts("\nstate");
    puts(tracking_data->state.id);
    puts(tracking_data->state.text);
    
    puts("\nprogresses");
#endif

    progress_count = json_object_array_length(progress_object);
    
    for(int i = 0; i < progress_count; i++){
        progress_array_object = json_object_array_get_idx(progress_object, i);

        time = json_object_object_get(progress_array_object, "time");
        json_object * location_object = json_object_object_get(progress_array_object, "location");
        name = json_object_object_get(location_object, "name");
        state_object = json_object_object_get(progress_array_object, "status");
        id = json_object_object_get(state_object, "id");
        text = json_object_object_get(state_object, "text");
        json_object * description = json_object_object_get(progress_array_object, "description");

        #if defined(_WIN32) || defined(_WIN64)
        if(!CHECK_NULL(json_object_get_string(time))) if(window_dateparsing(json_object_get_string(time), &(tracking_data->progresses[i].time)) == NULL) puts("format error");
        #else
        if(!CHECK_NULL(json_object_get_string(time))) if(strptime(json_object_get_string(time), "%Y-%m-%dT%T%z", &(tracking_data->progresses[i].time)) == NULL) puts("format error");
        #endif
        if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->progresses[i].location.name, json_object_get_string(name));
        if(!CHECK_NULL(json_object_get_string(id))) strcpy(tracking_data->progresses[i].status.id, json_object_get_string(id));
        if(!CHECK_NULL(json_object_get_string(text))) strcpy(tracking_data->progresses[i].status.text, json_object_get_string(text));
        if(!CHECK_NULL(json_object_get_string(description))) strcpy(tracking_data->progresses[i].description, json_object_get_string(description));
        
#if PUTS
        printf("%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", tracking_data->progresses[i].time.tm_year + 1900, tracking_data->progresses[i].time.tm_mon + 1, tracking_data->progresses[i].time.tm_mday, tracking_data->progresses[i].time.tm_hour, tracking_data->progresses[i].time.tm_min, tracking_data->progresses[i].time.tm_sec, 9, 0);
        puts(tracking_data->progresses[i].location.name);
        puts(tracking_data->progresses[i].status.id);
        puts(tracking_data->progresses[i].status.text);
        puts(tracking_data->progresses[i].description);
#endif
    }

    tracking_data->progress_count = progress_count;

    id = json_object_object_get(carrier_object, "id");
    name = json_object_object_get(carrier_object, "name");
    json_object * tel = json_object_object_get(carrier_object, "tel");

    if(!CHECK_NULL(json_object_get_string(id))) strcpy(tracking_data->carrier.id, json_object_get_string(id));
    if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->carrier.name, json_object_get_string(name));
    if(!CHECK_NULL(json_object_get_string(tel))) strcpy(tracking_data->carrier.tel, json_object_get_string(tel));

#if PUTS
    puts("\ncarrier");
    puts(tracking_data->carrier.id);
    puts(tracking_data->carrier.name);
    puts(tracking_data->carrier.tel);

    puts("\n==========================================================\n");
#endif
    return tracking_data;
}

int GetTrackingData(const char * json, TrackingData * tracking_data){
    int progress_count = 0;

    json_object * jobj = json_tokener_parse(json);
    
    json_object * message_object = json_object_object_get(jobj, "message");
    
    if(!CHECK_NULL(message_object)) return 0;

    json_object * from_object = json_object_object_get(jobj, "from");
    json_object * to_object = json_object_object_get(jobj, "to");
    json_object * name = json_object_object_get(from_object, "name");
    json_object * time = json_object_object_get(from_object, "time");
    json_object * state_object = json_object_object_get(jobj, "state");
    json_object * id = json_object_object_get(state_object, "id");
    json_object * text = json_object_object_get(state_object, "text");
    json_object * progress_object = json_object_object_get(jobj, "progresses");
    json_object * progress_array_object;
    json_object * carrier_object = json_object_object_get(jobj, "carrier");

    if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->from.name, json_object_get_string(name));
    #if defined(_WIN32) || defined(_WIN64)
    if(!CHECK_NULL(json_object_get_string(time))) if(window_dateparsing(json_object_get_string(time), &(tracking_data->from.time)) == NULL) puts("format error");
    #else
    if(!CHECK_NULL(json_object_get_string(time))) if(strptime(json_object_get_string(time), "%Y-%m-%dT%T%z", &(tracking_data->from.time)) == NULL) puts("format error");
    #endif
    
#if PUTS
    puts("\nfrom");
    puts(tracking_data->from.name);
    printf("%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", tracking_data->from.time.tm_year + 1900, tracking_data->from.time.tm_mon + 1, tracking_data->from.time.tm_mday, tracking_data->from.time.tm_hour, tracking_data->from.time.tm_min, tracking_data->from.time.tm_sec, 9, 0);
#endif

    name = json_object_object_get(to_object, "name");
    time = json_object_object_get(to_object, "time");

    if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->to.name, json_object_get_string(name));
    #if defined(_WIN32) || defined(_WIN64)
    if(!CHECK_NULL(json_object_get_string(time))) if(window_dateparsing(json_object_get_string(time), &(tracking_data->to.time)) == NULL) puts("format error");
    #else
    if(!CHECK_NULL(json_object_get_string(time))) if(strptime(json_object_get_string(time), "%Y-%m-%dT%T%z", &(tracking_data->to.time)) == NULL) puts("format error");
    #endif

#if PUTS
    puts("\nto");
    puts(tracking_data->to.name);
    printf("%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", tracking_data->to.time.tm_year + 1900, tracking_data->to.time.tm_mon + 1, tracking_data->to.time.tm_mday, tracking_data->to.time.tm_hour, tracking_data->to.time.tm_min, tracking_data->to.time.tm_sec, 9, 0);
#endif

    if(!CHECK_NULL(json_object_get_string(id))) strcpy(tracking_data->state.id, json_object_get_string(id));
    if(!CHECK_NULL(json_object_get_string(text))) strcpy(tracking_data->state.text, json_object_get_string(text));

#if PUTS
    puts("\nstate");
    puts(tracking_data->state.id);
    puts(tracking_data->state.text);
    
    puts("\nprogresses");
#endif

    progress_count = json_object_array_length(progress_object);
    
    for(int i = 0; i < progress_count; i++){
        progress_array_object = json_object_array_get_idx(progress_object, i);

        time = json_object_object_get(progress_array_object, "time");
        json_object * location_object = json_object_object_get(progress_array_object, "location");
        name = json_object_object_get(location_object, "name");
        state_object = json_object_object_get(progress_array_object, "status");
        id = json_object_object_get(state_object, "id");
        text = json_object_object_get(state_object, "text");
        json_object * description = json_object_object_get(progress_array_object, "description");

        #if defined(_WIN32) || defined(_WIN64)
        if(!CHECK_NULL(json_object_get_string(time))) if(window_dateparsing(json_object_get_string(time), &(tracking_data->progresses[i].time)) == NULL) puts("format error");
        #else
        if(!CHECK_NULL(json_object_get_string(time))) if(strptime(json_object_get_string(time), "%Y-%m-%dT%T%z", &(tracking_data->progresses[i].time)) == NULL) puts("format error");
        #endif
        if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->progresses[i].location.name, json_object_get_string(name));
        if(!CHECK_NULL(json_object_get_string(id))) strcpy(tracking_data->progresses[i].status.id, json_object_get_string(id));
        if(!CHECK_NULL(json_object_get_string(text))) strcpy(tracking_data->progresses[i].status.text, json_object_get_string(text));
        if(!CHECK_NULL(json_object_get_string(description))) strcpy(tracking_data->progresses[i].description, json_object_get_string(description));
        
#if PUTS
        // printf("%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", tracking_data->progresses[i].time.tm_year + 1900, tracking_data->progresses[i].time.tm_mon + 1, tracking_data->progresses[i].time.tm_mday, tracking_data->progresses[i].time.tm_hour, tracking_data->progresses[i].time.tm_min, tracking_data->progresses[i].time.tm_sec, tracking_data->progresses[i].time.tm_gmtoff / (60 * 60), tracking_data->progresses[i].time.tm_gmtoff % (60 * 60));
        printf("%4d-%02d-%02dT%02d:%02d:%02d+%02d:%02d\n", tracking_data->progresses[i].time.tm_year + 1900, tracking_data->progresses[i].time.tm_mon + 1, tracking_data->progresses[i].time.tm_mday, tracking_data->progresses[i].time.tm_hour, tracking_data->progresses[i].time.tm_min, tracking_data->progresses[i].time.tm_sec, 9, 0);
        puts(tracking_data->progresses[i].location.name);
        puts(tracking_data->progresses[i].status.id);
        puts(tracking_data->progresses[i].status.text);
        puts(tracking_data->progresses[i].description);
#endif
    }

    tracking_data->progress_count = progress_count;

    id = json_object_object_get(carrier_object, "id");
    name = json_object_object_get(carrier_object, "name");
    json_object * tel = json_object_object_get(carrier_object, "tel");

    if(!CHECK_NULL(json_object_get_string(id))) strcpy(tracking_data->carrier.id, json_object_get_string(id));
    if(!CHECK_NULL(json_object_get_string(name))) strcpy(tracking_data->carrier.name, json_object_get_string(name));
    if(!CHECK_NULL(json_object_get_string(tel))) strcpy(tracking_data->carrier.tel, json_object_get_string(tel));

#if PUTS
    puts("\ncarrier");
    puts(tracking_data->carrier.id);
    puts(tracking_data->carrier.name);
    puts(tracking_data->carrier.tel);

    puts("\n==========================================================\n");
#endif
    return 1;
}


#if 0
int main(int argc, char *argv[]){
    const char * json = "[{\"id\": \"de.dhl\",\"name\": \"DHL\",\"tel\": \"+8215880001\"},{\"id\": \"jp.sagawa\",\"name\": \"Sagawa\",\"tel\": \"+810120189595\"},{\"id\": \"jp.yamato\",\"name\": \"Kuroneko Yamato\",\"tel\": \"+810120189595\"},{\"id\": \"jp.yuubin\",\"name\": \"Japan Post\",\"tel\": \"+810570046111\"},{\"id\": \"kr.chunilps\",\"name\": \"천일택배\",\"tel\": \"+8218776606\"},{\"id\": \"kr.cjlogistics\",\"name\": \"CJ대한통운\",\"tel\": \"+8215881255\"},{\"id\": \"kr.cupost\",\"name\": \"CU 편의점택배\",\"tel\": \"+8215771287\"},{\"id\": \"kr.cvsnet\",\"name\": \"GS Postbox 택배\",\"tel\": \"+8215771287\"},{\"id\": \"kr.cway\",\"name\": \"CWAY (Woori Express)\",\"tel\": \"+8215884899\"},{\"id\": \"kr.daesin\",\"name\": \"대신택배\",\"tel\": \"+82314620100\"},{\"id\": \"kr.epost\",\"name\": \"우체국 택배\",\"tel\": \"+8215881300\"},{\"id\": \"kr.hanips\",\"name\": \"한의사랑택배\",\"tel\": \"+8216001055\"},{\"id\": \"kr.hanjin\",\"name\": \"한진택배\",\"tel\": \"+8215880011\"},{\"id\": \"kr.hdexp\",\"name\": \"합동택배\",\"tel\": \"+8218993392\"},{\"id\": \"kr.homepick\",\"name\": \"홈픽\",\"tel\": \"+8218000987\"},{\"id\": \"kr.honamlogis\",\"name\": \"한서호남택배\",\"tel\": \"+8218770572\"},{\"id\": \"kr.ilyanglogis\",\"name\": \"일양로지스\",\"tel\": \"+8215880002\"},{\"id\": \"kr.kdexp\",\"name\": \"경동택배\",\"tel\": \"+8218995368\"},{\"id\": \"kr.kunyoung\",\"name\": \"건영택배\",\"tel\": \"+82533543001\"},{\"id\": \"kr.logen\",\"name\": \"로젠택배\",\"tel\": \"+8215889988\"},{\"id\": \"kr.lotte\",\"name\": \"롯데택배\",\"tel\": \"+8215882121\"},{\"id\": \"kr.slx\",\"name\": \"SLX\",\"tel\": \"+82316375400\"},{\"id\": \"kr.swgexp\",\"name\": \"성원글로벌카고\",\"tel\": \"+82327469984\"},{\"id\": \"nl.tnt\",\"name\": \"TNT\"},{\"id\": \"un.upu.ems\",\"name\": \"EMS\"},{\"id\": \"us.fedex\",\"name\": \"Fedex\"},{\"id\": \"us.ups\",\"name\": \"UPS\"},{\"id\": \"us.usps\",\"name\": \"USPS\"}]";

    List list;
    list_init(&list);

    // insert
    GetCarrierList(&list, json);
    
    // print
    PrintCarrierList(&list);

    return 0;
}
#endif