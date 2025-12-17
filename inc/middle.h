#ifndef _MIDDLE_H_
#define _MIDDLE_H_

#include <stdio.h>
#include <sys/stat.h>
#include "errors.h"

struct Node_t;
struct Tree_t;

CodeError_t Optimization(Node_t** node_ptr);
void ConvolConst(Node_t** node);
void RemovingNeutral(Node_t** node);
void RemovingMul(Node_t** node);
int GetOperArgs(Node_t* node);

Tree_t* GetTreeAST(const char* file_name);
CodeError_t ReadAST(Tree_t* tree, const char* file_name);
Node_t* ParseAST(Node_t** tokens, int* ind);

#endif // _MIDDLE_H_