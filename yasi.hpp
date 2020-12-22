#pragma once

#include <stddef.h>
#include "string-impl.h"

#define EXPORT_STRING_TYPE

template<typename Dummy>
class yasi_t : public string_t {
public:

    // default constructor
    yasi_t(void) {
        string_init_default(static_cast<string_t*>(this));
    }

    // copy constructor
    yasi_t(const yasi_t& other) {
        string_init_string(
            static_cast<string_t*>(this), 
            static_cast<string_t*>(&other));
    }

    // move constructor
    yasi_t(yasi_t&& other) {
        string_move(
            static_cast<string_t*>(this),
            static_cast<string_t*>(&other));
    }

    // c-string constructor
    yasi_t(const char* cptr) {
        string_init_cptr(
            static_cast<string_t*>(this),
            cptr);
    }

    // destructor
    ~yasi_t() {
        string_deconstruct(static_cast<string_t*>(this));
    }

    size_t size(void) const { return string_size(static_cast<const string_t*>(this)); }
    void append(char c) { string_append(static_cast<string_t*>(this), c); }

    template<typename Iterable>
    void append(Iterable beg, Iterable end) {
        while(beg != end) {
            this->append(*beg);
            ++beg;
        }
    }

    char* begin(void) { return string_begin(static_cast<string_t*>(this)); }
    char* end(void) { return string_end(static_cast<string_t*>(this)); }

};

#ifdef EXPORT_STRING_TYPE
namespace std {
    typedef yasi_t<char> yasi;
}
#endif

//
// A haiku about coding
//
// This source code is crap.
// What moron wrote all of this?
// Oh no, it was me.
//
// -AegisToast
//
