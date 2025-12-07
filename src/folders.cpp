#include "folders.h"

void MakeDir(const char* folder_name) {
    char create_dir[100] = {};
    sprintf(create_dir, "mkdir %s", folder_name);
    system(create_dir);

    chdir(folder_name);
}

const char* GetTime() {
    time_t sec_time = time(NULL);
    struct tm* real_time = localtime(&sec_time);
    
    char* real_data = (char*)calloc(40, sizeof(char));
    strftime(real_data, 40, "%d.%m.%Y__%H_%M_%S", real_time);

    return real_data;
}