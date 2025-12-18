#ifndef _FRONTEND_H_
#define _FRONTEND_H_

#include <stdio.h>

struct Node_t;

void Frontend(Node_t* node, const char* file_name);
void PrintAST(Node_t* node, FILE* ast_file, char* add_info);

void PrintNum(Node_t* node, FILE* ast_file, char* add_info);
void PrintVar(Node_t* node, FILE* ast_file, char* add_info);
void PrintOper(Node_t* node, FILE* ast_file, char* add_info);
void PrintControlStruct(Node_t* node, FILE* ast_file, char* add_info);
void PrintParam(Node_t* node, FILE* ast_file, char* add_info);
void PrintDefault(Node_t* node, FILE* ast_file, char* add_info);


#endif