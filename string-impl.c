#include "string-impl.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef STRINGDEBUG
#  ifndef STRDEBUGOUT
#    define STRDEBUGOUT stdout
#  endif // STRDEBUGOUT
#endif // STRINGDEBUG

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// flag byte:
//
// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//           ^  [-----------------]
//           |     remaining SSO
//           |
//         type
//

static char string_get_flag_byte(const string_t* ptr) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_get_flag_byte\n");
    #endif

    return ptr->small.buf[31];
}

static int string_get_internal_type(const string_t* ptr) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_get_internal_type\n");
    #endif

    // extract bit 5
    return (int)(( ptr->small.buf[31] >> 5) & 0x01);
}

static void string_promote_type(string_t* ptr) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_promote_type\n");
    #endif

    const int type = string_get_internal_type(ptr);
    ulong_t size = string_size(ptr);

    if(type == 1) // already dynamic type
        return;

    char* beg = (char*)malloc(64);
    char* end = beg + string_size(ptr);
    char* cap = beg + 64;

    for(ulong_t i = 0; i < size; i++) {
        beg[i] = ptr->small.buf[i];
    }

    beg[size] = '\0';

    ptr->ptrs.beg = beg;
    ptr->ptrs.end = end;
    ptr->ptrs.cap = cap;

    // set type bit in flag byte to 1
    ptr->small.buf[31] = 0x20;
}

static void string_realloc_dynamic_buffer(string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_realloc_dynamic_buffer\n");
    #endif

    const int type = string_get_internal_type(str);

    if(type == 0) // realloc'ing SSO strings is undefined behavior
        return;

    ulong_t str_size = string_size(str);

    // calculate new capacity from old capacity
    ulong_t current_cap = str->ptrs.cap - str->ptrs.beg;
    ulong_t new_cap = current_cap * 2;

    char* beg = (char*)malloc(new_cap);
    char* end = beg + str_size;
    char* cap = beg + new_cap;

    // copy existing data over to new allocation
    for(ulong_t i = 0; i < str_size; i++) {
        beg[i] = str->ptrs.beg[i];
    }

    beg[str_size] = '\0';

    // release old memory
    free((void*)str->ptrs.beg);

    // replace existing pointers
    str->ptrs.beg = beg;
    str->ptrs.end = end;
    str->ptrs.cap = cap;
}

static ulong_t string_remaining_free_space(const string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_remaining_free_space\n");
    #endif

    const int type = string_get_internal_type(str);

    switch(type) {
    case 0:
    {
        return (ulong_t)string_get_flag_byte(str);
    }
    case 1:
    {
        return str->ptrs.cap - str->ptrs.end;
    }
    }

}

string_t* string_new_default(void) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_new_default\n");
    #endif

    string_t* str = (string_t*)malloc(sizeof(string_t));
    return string_init_default(str); // default initialize
}

string_t* string_new_cptr(const char* char_ptr) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_new_cptr\n");
    #endif

    string_t* str = (string_t*)malloc(sizeof(string_t));
    return string_init_cptr(str, char_ptr);
}

string_t* string_new_string(string_t* str_ptr) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_new_string\n");
    #endif

    string_t* str = (string_t*)malloc(sizeof(string_t));
    return string_init_string(str, str_ptr);
}

string_t* string_init_default(string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_init_default\n");
    #endif

    str->ptrs.beg = 0;
    str->ptrs.end = 0;
    str->ptrs.cap = 0;

    // type bit implicitly set to zero
    str->small.buf[31] = 31;

    return str;
}

string_t* string_init_cptr(string_t* str, const char* char_ptr) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_init_cptr\n");
    #endif

    str = string_init_default(str);
    
    char* iter = (char*)char_ptr;

    while(*iter) {
        string_append(str, *iter);
        iter++;
    }

    return str;
}

string_t* string_init_string(string_t* new_str, string_t* old_str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_init_string\n");
    #endif

    const int type = string_get_internal_type(old_str);

    switch(type) {
    case 0:
        {
            new_str->ptrs.beg     = old_str->ptrs.beg;
            new_str->ptrs.end     = old_str->ptrs.end;
            new_str->ptrs.cap     = old_str->ptrs.cap;

            // in union, this also contains the flag byte
            new_str->ptrs._unused = old_str->ptrs._unused;

            return new_str;
        }
        break;
    case 1:
        {
            const ulong_t old_str_cap  = old_str->ptrs.cap - old_str->ptrs.beg;
            const ulong_t old_str_size = string_size(old_str);

            new_str->ptrs.beg = (char*)malloc(old_str_cap);
            new_str->ptrs.end = new_str->ptrs.beg + old_str_size;
            new_str->ptrs.cap = new_str->ptrs.beg + old_str_cap;

            for(ulong_t i = 0; i < old_str_size; i++)
                new_str->ptrs.beg[i] = old_str->ptrs.beg[i];

            new_str->ptrs.beg[old_str_size] = '\0';

            return new_str;
        }
        break;
    }
    
}

void string_move(string_t* new_str, string_t* old_str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_move\n");
    #endif

    new_str->ptrs.beg     = old_str->ptrs.beg;
    new_str->ptrs.end     = old_str->ptrs.end;
    new_str->ptrs.cap     = old_str->ptrs.cap;
    new_str->ptrs._unused = old_str->ptrs._unused;

    string_init_default(old_str);
}

void string_delete(string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_delete\n");
    #endif

    string_deconstruct(str);
    free((void*)str);
}

void string_deconstruct(string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_deconstruct\n");
    #endif

    int type = string_get_internal_type(str);

    if(type == 1) {
        // delete dynamic buffer space
        free((void*)str->ptrs.beg);
    }

    string_init_default(str);
}

ulong_t string_size(const string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_size\n");
    #endif

    int type = string_get_internal_type(str);

    switch(type) {
    case 0:
        {
            int remaining_space = (int)str->small.buf[31];
            return (ulong_t)(31 - remaining_space);
        }
        break; // these breaks arent strictly necessary
    case 1:
        return (ulong_t)(str->ptrs.end - str->ptrs.beg);
        break;
    }

}

void string_append(string_t* str, char c) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_append\n");
    #endif

    const int type = string_get_internal_type(str);
    const ulong_t free_space = string_remaining_free_space(str);
    const ulong_t size       = string_size(str);

    switch(type) {
    case 0:
        if(free_space == 0) {
            string_promote_type(str);
            string_append(str, c);
            return;
        }
        else {
            str->small.buf[size] = c;
            str->small.buf[31]--;
            str->small.buf[size+1] = '\0';
            return;
        }
        break;
    case 1:
        if(free_space == 0) {
            string_realloc_dynamic_buffer(str);
            string_append(str, c);
            return;
        }
        else {
            str->ptrs.end[0] = c;
            str->ptrs.end++;
            str->ptrs.end[0] = '\0';
            return;
        }
        break;
    }

}

void string_append_iter(string_t* str, char* beg, char* end) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_append_iter\n");
    #endif

    while(beg < end) {
        string_append(str, *beg);
        beg++;
    }
}

void string_append_string(string_t* to, string_t* from) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_append_string\n");
    #endif

    string_append_iter(
        to,
        string_begin(from),
        string_end(from));
}

char* string_begin(string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_begin\n");
    #endif

    const int type = string_get_internal_type(str);

    switch(type) {
    case 0:
        return str->small.buf;
    case 1:
        return str->ptrs.beg;
    }
}

char* string_end(string_t* str) {

    #ifdef STRINGDEBUG
    fprintf(STRDEBUGOUT, "string_end\n");
    #endif

    const int type = string_get_internal_type(str);

    switch(type) {
    case 0:
        return str->small.buf + string_size(str);
    case 1:
        return str->ptrs.end;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

