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
#include "middle.h"

#define DUMP(tree, error_code, add_info) Dump(tree, VarInfo{#tree, __FILE__, __FUNCTION__, __LINE__, error_code, add_info})
#define NUM_NODE(x) NodeCtor(NUM, ValueNumCtor(x), NULL, NULL)
#define VAR_NODE(name) NodeCtor(VAR, ValueVarCtor(name), NULL, NULL)
#define OP_NODE(OPNAME, left, right) NodeCtor(OPER, ValueOperCtor(OPNAME), left, right)

#define c(node) CopyNode(node)

const int MAX_VAR_SIZE = 100;
const int MAX_FUNC_SIZE = 50;
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
    OP_POW       =   4,
    OP_LN        =   5,
    OP_LOG       =   6,
    OP_EXP       =   7,
    OP_COS       =   8,
    OP_SIN       =   9,
    OP_TAN       =  10,
    OP_COT       =  11,
    OP_ASIN      =  12,
    OP_ACOS      =  13,
    OP_ATAN      =  14,
    OP_ACOT      =  15,
    OP_EQUAL     =  16,
    OP_LESS      =  17,
    OP_ABOVE     =  18,
    OP_EQ        =  19,
    OP_IF        =  20,
    OP_OPER      =  21,
    OP_WHILE     =  22,
    OP_FUNC      =  23,
    OP_PARAM     =  24,
    OP_RETURN    =  25,
    OP_CALL      =  26,
    OP_INPUT     =  27,
    OP_OUTPUT    =  28,
    OP_PROCEDURE =  29,
    OP_FINISH    =  30,
    OP_COMMA     =  31,

    OP_LBR       =  32,
    OP_RBR       =  33,
    OP_FLBR      =  34,
    OP_FRBR      =  35,

    OP_INFO      =  36,

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

    int func_cnt = 0;
    char** funcs = NULL;

    size_t nodes_cnt = 0;
};

struct Operation_t {
    int type;

    const char* dump_view;

    double (*func) (Node_t*, Node_t*);

    const char* proc_view;
};

const Operation_t opers[] = {
    { .type = OP_ADD,         .dump_view = "+",          .func = Sum   ,    .proc_view = "ADD" },
         
    { .type = OP_SUB,         .dump_view = "-",          .func = Sub   ,    .proc_view = "SUB" },
         
    { .type = OP_MUL,         .dump_view = "*",          .func = Mul   ,    .proc_view = "MUL" },
         
    { .type = OP_DIV,         .dump_view = "/",          .func = Div   ,    .proc_view = "DIV" },
         
    { .type = OP_POW,         .dump_view = "^",          .func = Pow   ,    .proc_view = "POW" },
         
    { .type = OP_LN,          .dump_view = "ln",         .func = Ln    ,    .proc_view = ""    },
         
    { .type = OP_LOG,         .dump_view = "log",        .func = Log   ,    .proc_view = ""    },
         
    { .type = OP_EXP,         .dump_view = "e ^",        .func = Exp   ,    .proc_view = ""    },
         
    { .type = OP_COS,         .dump_view = "cos",        .func = Cos   ,    .proc_view = ""    },
         
    { .type = OP_SIN,         .dump_view = "sin",        .func = Sin   ,    .proc_view = ""    },
         
    { .type = OP_TAN,         .dump_view = "tan",        .func = Tan   ,    .proc_view = ""    },
     
    { .type = OP_COT,         .dump_view = "cot",        .func = Cot   ,    .proc_view = ""    },
     
    { .type = OP_ASIN,        .dump_view = "arcsin",     .func = Asin  ,    .proc_view = ""    },
     
    { .type = OP_ACOS,        .dump_view = "arccos",     .func = Acos  ,    .proc_view = ""    },
     
    { .type = OP_ATAN,        .dump_view = "arctan",     .func = Atan  ,    .proc_view = ""    },
     
    { .type = OP_ACOT,        .dump_view = "arccot",     .func = Acot  ,    .proc_view = ""    },
     
    { .type = OP_EQUAL,       .dump_view = "==",         .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_LESS,        .dump_view = "<",          .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_ABOVE,       .dump_view = ">",          .func = NULL  ,    .proc_view = ""    },
         
    { .type = OP_EQ,          .dump_view = "=",          .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_IF,          .dump_view = "if",         .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_OPER,        .dump_view = ";",          .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_WHILE,       .dump_view = "while",      .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_FUNC,        .dump_view = "function",   .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_PARAM,       .dump_view = "param",      .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_RETURN,      .dump_view = "return",     .func = NULL  ,    .proc_view = "RET" },
     
    { .type = OP_CALL,        .dump_view = "call",       .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_INPUT,       .dump_view = "input",      .func = NULL  ,    .proc_view = "IN"  },
         
    { .type = OP_OUTPUT,      .dump_view = "output",     .func = NULL  ,    .proc_view = "OUT" },
         
    { .type = OP_PROCEDURE,   .dump_view = "procedure",  .func = NULL  ,    .proc_view = ""    }, 
         
    { .type = OP_FINISH,      .dump_view = "finish",     .func = NULL  ,    .proc_view = "RET" },
     
    { .type = OP_COMMA,       .dump_view = ",",          .func = NULL  ,    .proc_view = ""    },
         
    { .type = OP_LBR,         .dump_view = "(",          .func = NULL  ,    .proc_view = ""    },
    { .type = OP_RBR,         .dump_view = ")",          .func = NULL  ,    .proc_view = ""    },
    { .type = OP_FLBR,        .dump_view = "{",          .func = NULL  ,    .proc_view = ""    },
    { .type = OP_FRBR,        .dump_view = "}",          .func = NULL  ,    .proc_view = ""    },
     
    { .type = OP_INFO,        .dump_view = "info",       .func = NULL  ,    .proc_view = ""    },
};

Tree_t* TreeCtor();
Node_t* NodeCtor(TYPES type, ValueType* val_type, Node_t* left, Node_t* right);

CodeError_t TreeDtor(Node_t* root);
CodeError_t TreeVerify(Tree_t* tree);
int GetSize(Node_t* root);

CodeError_t ReadBase(Tree_t* tree, const char* file_name);
int get_file_size(const char* file_name);
Tree_t* ParseTree(Tree_t* tree);

void SelectTreeFunc(Node_t* node, Tree_t* tree);
void AddFunc(Node_t* node, Tree_t* tree);
int CheckTreeFunc(Node_t* node, Tree_t* tree);

void SelectTreeVars(Node_t* node, Tree_t* tree);
Var_t* VarCtor(char* name);
int GetVarInd(Tree_t* tree, char* var_name);

Node_t* CopyNode(Node_t* node);

ValueType* ValueOperCtor(int type);
ValueType* ValueVarCtor(char* name);
ValueType* ValueNumCtor(double value);

double GetValue(Node_t* node);
Node_t* GetLeft(Node_t* node);
Node_t* GetRight(Node_t* node);

void PrintValueNode(Node_t* node);

#endif