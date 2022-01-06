#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <json.h>

#include "element.h"

#define CHECK_NULL(X) (X == NULL ? 1 : 0)

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