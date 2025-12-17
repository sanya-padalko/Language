#include "frontend.h"
#include "tree.h"

void Frontend(Node_t* node, const char* file_name) {
    if (!node || !file_name)
        return;

    FILE* ast_file = fopen(file_name, "a");

    char* add_info = (char*)calloc(100, sizeof(char));
    PrintAST(node, ast_file, add_info);
    
    fclose(ast_file);
}

void PrintAST(Node_t* node, FILE* ast_file, char* add_info) {
    if (!ast_file)
        return;
    
    if (!node) {
        fprintf(ast_file, "()");

    } else if (node->type == NUM) {
        PrintNum(node, ast_file, add_info);

    } else if (node->type == VAR) {
        PrintVar(node, ast_file, add_info);

    } else {
        int oper = node->value->type;
        char old_c = add_info[0];
        switch (oper) {
            case OP_IF:
            case OP_FUNC:
            case OP_WHILE:
                PrintControlStruct(node, ast_file, add_info);
                break;

            case OP_OPER:
                PrintOper(node, ast_file, add_info);
                break;

            case OP_PARAM:
                PrintParam(node, ast_file, add_info);
                break;
            
            case OP_RETURN:
                PrintReturn(node, ast_file, add_info);
                break;

            case OP_CALL:
                PrintCall(node, ast_file, add_info);
                break;

            default:
                PrintDefault(node, ast_file, add_info);
                break;
        }
        add_info[0] = old_c;
    }
}

void PrintNum(Node_t* node, FILE* ast_file, char* add_info) {
    char old_c = add_info[0];
    add_info[0] = '\0';

    fprintf(ast_file, "%s( %lg ", add_info, node->value->value);
    PrintAST(node->left, ast_file, add_info);
    fprintf(ast_file, " ");
    PrintAST(node->right, ast_file, add_info);
    fprintf(ast_file, " )");

    add_info[0] = old_c;
}

void PrintVar(Node_t* node, FILE* ast_file, char* add_info) {
    char old_c = add_info[0];
    add_info[0] = '\0';

    fprintf(ast_file, "%s( \"%s\" ", add_info, node->value->name);
    PrintAST(node->left, ast_file, add_info);
    fprintf(ast_file, " ");
    PrintAST(node->right, ast_file, add_info);
    fprintf(ast_file, " )");

    add_info[0] = old_c;
}

void PrintReturn(Node_t* node, FILE* ast_file, char* add_info) {
    fprintf(ast_file, "%s( return ", add_info);
    add_info[0] = '\0';
    PrintAST(node->left, ast_file, add_info);
    fprintf(ast_file, " ");
    PrintAST(node->right, ast_file, add_info);
    fprintf(ast_file, " )");
}

void PrintOper(Node_t* node, FILE* ast_file, char* add_info) {
    int len = strlen(add_info);
    int oper = node->value->type;

    fprintf(ast_file, "%s( %s \n", add_info, opers[oper].dump_view);
    add_info[len] = '\t';
    PrintAST(node->left, ast_file, add_info);
    fprintf(ast_file, "\n");
    PrintAST(node->right, ast_file, add_info);
    add_info[len] = '\0';
    fprintf(ast_file, "\n%s)", add_info);
}

void PrintCall(Node_t* node, FILE* ast_file, char* add_info) {
    fprintf(ast_file, "%s( call ", add_info);
    add_info[0] = '\0';
    PrintAST(node->left, ast_file, add_info);
    fprintf(ast_file, " ");
    PrintAST(node->right, ast_file, add_info);
    fprintf(ast_file, " )");
}

void PrintControlStruct(Node_t* node, FILE* ast_file, char* add_info) {
    int len = strlen(add_info);
    int oper = node->value->type;

    fprintf(ast_file, "%s( %s ", add_info, opers[oper].dump_view);
    add_info[0] = '\0';
    PrintAST(node->left, ast_file, add_info);
    fprintf(ast_file, "\n");
    add_info[0] = '\t';
    add_info[len] = '\t';
    PrintAST(node->right, ast_file, add_info);
    add_info[len] = '\0';
    fprintf(ast_file, "\n%s)\n", add_info);
}

void PrintParam(Node_t* node, FILE* ast_file, char* add_info) {
    add_info[0] = '\0';
    PrintAST(node->right, ast_file, add_info);
}

void PrintDefault(Node_t* node, FILE* ast_file, char* add_info) {
    int oper = node->value->type;
    
    fprintf(ast_file, "%s( %s ", add_info, opers[oper].dump_view);
    add_info[0] = '\0';
    PrintAST(node->left, ast_file, add_info);
    fprintf(ast_file, " ");
    add_info[0] = '\0';
    PrintAST(node->right, ast_file, add_info);
    fprintf(ast_file, " )", opers);
}