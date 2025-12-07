#ifndef _VARS_H_
#define _VARS_H_

#include "errors.h"

struct VarInfo {
    const char* name;
    
    const char* file_name;
    
    const char* func_name;

    int line;

    CodeError_t error_code;

    const char* add_info;
};

#endif // _VARS_H_