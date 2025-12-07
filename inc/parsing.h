#ifndef _PARSING_H_
#define _PARSING_H_

#include "tree.h"

#define SyntaxError(str, c) printf("Syntax error in %s, in line %d: %s %c\n", __FUNCTION__, __LINE__, str, c);

const int MAX_TOKEN_CNT = 1000;

struct Node_t;

struct Tokenizator_t {
    int token_cnt = 0;

    Node_t* tokens[MAX_TOKEN_CNT] = {NULL};
};

Node_t* GetG(const char** s);
Node_t* GetOp(const char** s);
Node_t* GetFunc(const char** s);
Node_t* GetParams(const char** s);
Node_t* GetParam(const char** s);
Node_t* GetWhile(const char** s);
Node_t* GetIf(const char** s);
Node_t* GetA(const char** s);
Node_t* GetE(const char** s);
Node_t* GetT(const char** s);
Node_t* GetPower(const char** s);
Node_t* GetP(const char** s);
Node_t* GetV(const char** s);
Node_t* GetN(const char** s);
void SkipSpaces(const char** s);

#endif