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

    // insert
    for(int i=0; i<10; i++){
        Carrier * carrier = (Carrier *)malloc(sizeof(Carrier));
        sprintf(carrier->id, "%d", i);
        sprintf(carrier->name, "name%d", i);
        sprintf(carrier->tel, "%s", temp[i]);

        if(!list_size(&list)) list_insert_next(&list, NULL, carrier);
        else list_insert_next(&list, list_tail(&list), carrier);
    }

    // print
    ListElmt * element;
    Carrier * carrier;

    element = list_head(&list);
    
    while (1)
    {
        carrier = list_data(element);
        
        printf("%s: %s, %s\r\n", carrier->id, carrier->name, carrier->tel);
        // puts("");

        if(list_is_tail(element))
            break;
        else
            element = list_next(element);
    }

    return 0;
}