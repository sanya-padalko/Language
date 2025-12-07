#include "errors.h"

void PrintErr(CodeError_t error_type, const char* file_name, const char* func_name, const int line_ind) {
    printerr(RED_COLOR "error in %s: function -> %s, line %d: " RESET_COLOR, file_name, func_name, line_ind);
    switch (error_type) {
        case NOTHING:
            printerr(GREEN_COLOR "Everything is OK\n");
            break;
        case NULLPTR:
            printerr(RED_COLOR "Pointer went equal NULL\n");
            break;
        case SIZE_ERR:
            printerr(RED_COLOR "Unavailable size\n");
            break;
        case HASH_ERR:
            printerr(RED_COLOR "Hash went wrong\n");
            break;
        case VALUE_ERR:
            printerr(RED_COLOR "Trying to work with wrong value\n");
            break;
        case IND_ERR:
            printerr(RED_COLOR "Tree index out of range\n");
            break;
        case CALLOC_ERR:
            printerr(RED_COLOR "Calloc went wrong\n");
            break;
        case FILE_ERR:
            printerr(RED_COLOR "Working with file went wrong\n");
            break;
        default:
            printerr(YELLOW_COLOR "Unknown error\n");
    }

    printerr(RESET_COLOR);
}

const char* ErrorType(CodeError_t error_code) {
    switch (error_code) {
        case NOTHING:
            return "Everything is OK\n";
        case NULLPTR:
            return "Pointer went equal NULL\n";
        case SIZE_ERR:
            return "Unavailable size\n";
        case REALLOC_ERR:
            return "Realloc went wrong\n";
        case HASH_ERR:
            return "Hash went wrong\n";
        case VALUE_ERR:
            return "Trying to work with wrong value\n";
        case IND_ERR:
            return "Tree index out of range\n";
        case CALLOC_ERR:
            return "Calloc went wrong\n";
    }
    return "Unknown error\n";
}
