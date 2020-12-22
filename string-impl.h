#pragma once

//#define STRINGDEBUG

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {

    union {

        struct {
            char buf[32];
        } small;

        struct {
            char* beg;
            char* end;
            char* cap;
            void* _unused; // used to ensure struct[ptrs] is also 32 byte aligned
        } ptrs;

    };

} string_t;

typedef unsigned long int ulong_t;

// dynamically allocate and initialize string
string_t* string_new_default(void); // default string
string_t* string_new_cptr(const char* char_ptr);
string_t* string_new_string(string_t* str_ptr);

// initialize pre-allocated string_t
string_t* string_init_default(string_t* str);
string_t* string_init_cptr(string_t* str, const char* char_ptr);
string_t* string_init_string(string_t* new_str, string_t* old_str); // "copy constructor"

void string_move(string_t* new_str, string_t* old_str); // move semantics are explicit
void string_delete(string_t* str);      // memory associated with str object is removed
void string_deconstruct(string_t* str); // internal buffer is deallocated if there is one

void string_append(string_t* str, char c);
void string_append_iter(string_t* str, char* beg, char* end);
void string_append_string(string_t* to, string_t* from);

ulong_t string_size(const string_t* str);
char* string_data(string_t* str);

// iterators
char* string_begin(string_t* str);
char* string_end(string_t* str);

#ifdef __cplusplus
} // extern "C"
static_assert(sizeof(string_t) == 32);
#endif // __cplusplus
