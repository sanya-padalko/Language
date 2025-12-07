#ifndef _LIST_H_
#define _LIST_H_

#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#include "dump.h"
#include "parsing.h"
#include "errors.h"
#include "vars.h"
#include "calc.h"

#define DUMP(tree, error_code, add_info) Dump(tree, VarInfo{#tree, __FILE__, __FUNCTION__, __LINE__, error_code, add_info})
#define NUM_NODE(x) NodeCtor(NUM, ValueNumCtor(x), NULL, NULL)
#define VAR_NODE(name) NodeCtor(VAR, ValueVarCtor(name), NULL, NULL)
#define OP_NODE(OPNAME, left, right) NodeCtor(OPER, ValueOperCtor(OPNAME), left, right)

#define c(node) CopyNode(node)

const int MAX_VAR_SIZE = 100;
const int MAX_NODES_CNT = 10000;
const int MAX_DIFFER = 1000;
const int MAX_VARS_CNT = 100;

enum TYPES {
    OPER    =   0,
    VAR     =   1,
    NUM     =   2, 
};

enum OPERATIONS {
    OP_ADD       =   0,
    OP_SUB       =   1,
    OP_MUL       =   2,
    OP_DIV       =   3,
    OP_COS       =   4,
    OP_SIN       =   5,
    OP_POW       =   6,
    OP_LN        =   7,
    OP_LOG       =   8,
    OP_EXP       =   9,
    OP_TAN       =  10,
    OP_COT       =  11,
    OP_ASIN      =  12,
    OP_ACOS      =  13,
    OP_ATAN      =  14,
    OP_ACOT      =  15,
    OP_EQ        =  16,
    OP_IF        =  17,
    OP_OPER      =  18,
    OP_WHILE     =  19,
    OP_FUNC      =  20,
    OP_PARAM     =  21,
    OP_COMMA     =  22,
    OP_FUNC_INFO =  23,

    OPER_CNT
};

union ValueType {
    int type;
    char* name;
    double value;
};

struct Node_t {
    TYPES type;

    ValueType* value = NULL;

    Node_t* left = NULL;
    Node_t* right = NULL;
};

struct Var_t {
    char* name = NULL;
    double value = NAN;
};

struct Tree_t {
    Node_t* root = NULL;

    const char* dot_file_name = NULL;
    const char* html_file_name = NULL;

    char* buf = NULL;

    int var_cnt = 0;
    Var_t* vars[MAX_VARS_CNT] = {NULL};

    size_t nodes_cnt = 0;
};

struct Operation_t {
    int type;

    const char* dump_view;

    double (*func) (Node_t*, Node_t*);
};

const Operation_t opers[] = {
    { .type = OP_ADD,       .dump_view = "+",        .func = Sum   },
    
    { .type = OP_SUB,       .dump_view = "-",        .func = Sub   },
    
    { .type = OP_MUL,       .dump_view = "*",        .func = Mul   },
    
    { .type = OP_DIV,       .dump_view = "/",        .func = Div   },
    
    { .type = OP_COS,       .dump_view = "cos",      .func = Cos   },
    
    { .type = OP_SIN,       .dump_view = "sin",      .func = Sin   },
    
    { .type = OP_POW,       .dump_view = "pow",      .func = Pow   },
    
    { .type = OP_LN,        .dump_view = "ln",       .func = Ln    },
    
    { .type = OP_LOG,       .dump_view = "log",      .func = Log   },
    
    { .type = OP_EXP,       .dump_view = "exp",      .func = Exp   },
    
    { .type = OP_TAN,       .dump_view = "tan",      .func = Tan   },

    { .type = OP_COT,       .dump_view = "cot",      .func = Cot   },

    { .type = OP_ASIN,      .dump_view = "arcsin",   .func = Asin  },

    { .type = OP_ACOS,      .dump_view = "arccos",   .func = Acos  },

    { .type = OP_ATAN,      .dump_view = "arctan",   .func = Atan  },

    { .type = OP_ACOT,      .dump_view = "arccot",   .func = Acot  },

    { .type = OP_EQ,        .dump_view = "=",        .func = NULL  },

    { .type = OP_IF,        .dump_view = "if",       .func = NULL  },

    { .type = OP_OPER,      .dump_view = ";",        .func = NULL  },

    { .type = OP_WHILE,     .dump_view = "while",    .func = NULL  },

    { .type = OP_FUNC,      .dump_view = "func",     .func = NULL  },

    { .type = OP_PARAM,     .dump_view = "param",    .func = NULL  },

    { .type = OP_COMMA,     .dump_view = ",",        .func = NULL  },
    
    { .type = OP_FUNC_INFO, .dump_view = "info",     .func = NULL  },
};

Tree_t* TreeCtor();
Node_t* NodeCtor(TYPES type, ValueType* val_type, Node_t* left, Node_t* right);

CodeError_t TreeDtor(Node_t* root);
CodeError_t TreeVerify(Tree_t* tree);
int GetSize(Node_t* root);

CodeError_t ReadBase(Tree_t* tree, const char* file_name);
int get_file_size(const char* file_name);
Tree_t* ParseTree(Tree_t* tree);

void SelectTreeVars(Node_t* node, Tree_t* tree);
Var_t* VarCtor(char* name);
bool CheckVars(Tree_t* forest, char* var_name);

Node_t* CopyNode(Node_t* node);

ValueType* ValueOperCtor(int type);
ValueType* ValueVarCtor(char* name);
ValueType* ValueNumCtor(double value);

double CalculatingTree(Tree_t* tree);
double CalculatingNode(Node_t* node, Tree_t* tree);
double CalcConstNode(Node_t* node, double value);

double GetValue(Node_t* node);
void ConvolConst(Node_t** node);
void RemovingNeutral(Node_t** node);

CodeError_t Optimization(Node_t** node_ptr);

#endif