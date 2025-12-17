#ifndef _BACKEND_H_
#define _BACKEND_H_

#include <stdio.h>

struct Tree_t;
struct Node_t;

void Backend(Node_t* node, FILE* ex_file, Tree_t* tree);

void PrintNum(Node_t* node, FILE* ex_file);
void PrintVar(Node_t* node, FILE* ex_file, Tree_t* tree);
void PrintIf(Node_t* node, FILE* ex_file, Tree_t* tree);
void PrintWhile(Node_t* node, FILE* ex_file, Tree_t* tree);
void PrintFunc(Node_t* node, FILE* ex_file, Tree_t* tree);
void PrintReturn(Node_t* node, FILE* ex_file, Tree_t* tree);
void PrintCall(Node_t* node, FILE* ex_file, Tree_t* tree);
void PrintEq(Node_t* node, FILE* ex_file, Tree_t* tree);
void PrintDefault(Node_t* node, FILE* ex_file, Tree_t* tree);

void PrintPushVar(FILE* ex_file, int ind);
void PrintPopVar(FILE* ex_file, int ind);

#endif // _BACKEND_H_