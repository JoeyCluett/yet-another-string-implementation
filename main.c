#include <stdio.h>
#include "string-impl.h"
#include "main.h"

int main(int argc, char** argv) {

    string_t my_string;
    string_t* sptr = &my_string;
    
    string_init_cptr(sptr, "Hello World");

    printf("string size: %ld chars\n", string_size(sptr));
    printf("Type: %s\n", internal_type_str(sptr));

    int i = 0;
    for(; i < 20; i++)
        string_append(sptr, 'r');

    printf("string size: %ld chars\n", string_size(sptr));
    printf("Type: %s\n", internal_type_str(sptr));

    // should push string into dynamic string type
    string_append(sptr, 'q');

    printf("string size: %ld chars\n", string_size(sptr));
    printf("Type: %s\n", internal_type_str(sptr));

    return 0;
}
