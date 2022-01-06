#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "element.h"

int main(int argc, char *argv[]){
    List list;
    list_init(&list);

    char * temp[10]={
        "test",
        "asdf",
        "qwer",
        "random",
        "carrier",
        "tel",
        "list",
        "element",
        "temp",
        "array"
    };

    for(int i=0; i<10; i++){
        Carrier * carrier = (Carrier *)malloc(sizeof(Carrier));
        sprintf(carrier->id, "%d", i);
        sprintf(carrier->name, "name%d", i);
        sprintf(carrier->tel, "%s", temp[i]);

        if(!list_size(&list)) list_insert_next(&list, NULL, carrier);
        else list_insert_next(&list, list_tail(&list), carrier);
    }

    return 0;
}