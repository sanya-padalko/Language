#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>

#define printerr(...) fprintf(stderr, __VA_ARGS__)

#define line_info __FILE__, __FUNCTION__, __LINE__

#define my_assert(comp, code_err, ret_value) \
    if (comp == 0) { \
        PrintErr(code_err, line_info); \
        return ret_value; \
    }

#define RESET_COLOR  "\033[0m"
#define RED_COLOR    "\033[31m"
#define GREEN_COLOR  "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define BLUE_COLOR   "\033[36m"

enum CodeError_t {
    NOTHING        =    0,
    NULLPTR        =    1,
    REALLOC_ERR    =    2,
    HASH_ERR       =    3,
    VALUE_ERR      =    4,
    IND_ERR        =    5,
    CALLOC_ERR     =    6,
    CYCLE_ERR      =    7,
    FILE_ERR       =    8,
    SIZE_ERR       =    9,
    ERROR_COUNTS
};

void PrintErr(CodeError_t error_type, const char* file_name, const char* func_name, const int line_ind);

const char* ErrorType(CodeError_t error_code);

#endif // _ERRORS_H
