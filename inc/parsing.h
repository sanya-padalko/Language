#ifndef _PARSING_H_
#define _PARSING_H_

#include "tree.h"

#define SyntaxError(str, c) printf("Syntax error in %s, in line %d: %s %c\n", __FUNCTION__, __LINE__, str, c);

#define CHECK_TOKEN(node, OP_NAME)    ((node)->type == OPER && (node)->value->type == OP_NAME)

const int MAX_TOKEN_CNT = 1000;
struct Node_t;

struct Tokenizator_t {
    int token_cnt = 0;

    Node_t* tokens[MAX_TOKEN_CNT] = {NULL};
};

Tokenizator_t* SelectTokens(const char** s);
Node_t* SelectBracket(const char** s);
Node_t* SelectOper(const char** s);

Node_t* GetV(const char** s);
Node_t* GetN(const char** s);
void SkipSpaces(const char** s);

Node_t* GetG(Node_t** tokens, int* ind);
Node_t* GetOp(Node_t** tokens, int* ind);

Node_t* GetFunc(Node_t** tokens, int* ind);
Node_t* GetParams(Node_t** tokens, int* ind);
Node_t* GetParam(Node_t** tokens, int* ind);
Node_t* GetReturn(Node_t** tokens, int* ind);
Node_t* GetCall(Node_t** tokens, int* ind);
Node_t* GetArgues(Node_t** tokens, int* ind);

Node_t* GetWhile(Node_t** tokens, int* ind);
Node_t* GetIf(Node_t** tokens, int* ind);

Node_t* GetA(Node_t** tokens, int* ind);
Node_t* GetE(Node_t** tokens, int* ind);
Node_t* GetT(Node_t** tokens, int* ind);
Node_t* GetPower(Node_t** tokens, int* ind);
Node_t* GetP(Node_t** tokens, int* ind);

#endif