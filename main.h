#pragma once

#include "string-impl.h"

#ifdef __cplusplus

#include "yasi.hpp"

const char* internal_type_str(yasi_t<char>& s) {
    string_t* ptr = static_cast<string_t*>(&s);

    if(ptr->small.buf[31] & 0x20)
        return "(DYNAMIC_STRING)";
    else
        return "(SMALL_STRING)";

}

#else

const char* internal_type_str(string_t* ptr) {
    if(ptr->small.buf[31] & 0x20)
        return "(DYNAMIC_STRING)";
    else
        return "(SMALL_STRING)";
}

#endif