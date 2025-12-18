#include "backend.h"
#include "tree.h"

void Backend(Node_t* node, FILE* ex_file, Tree_t* tree) {
    if (!node)
        return;

    if (node->type == NUM) {
        PrintNum(node, ex_file);
        return;
    }
    
    if (node->type == VAR) {
        PrintVar(node, ex_file, tree);
        return;
    }

    int oper = node->value->type;
    switch (oper) {
        case OP_IF:
            PrintIf(node, ex_file, tree);
            break;
        case OP_WHILE:
            PrintWhile(node, ex_file, tree);
            break;
        case OP_PROCEDURE:
        case OP_FUNC:
            PrintFunc(node, ex_file, tree);
            break;
        case OP_FINISH:
        case OP_RETURN:
            PrintReturn(node, ex_file, tree);
            break;
        case OP_CALL:
            PrintCall(node, ex_file, tree);
            break;
        case OP_EQ:
            PrintEq(node, ex_file, tree);
            break;
        case OP_INPUT:
            PrintInput(node, ex_file, tree);
            break;
        case OP_OUTPUT:
            PrintOutput(node, ex_file, tree);
            break;
        case OP_COMMA:
            PrintComma(node, ex_file, tree);
            break;
        case OP_EQUAL:
        case OP_LESS:
        case OP_ABOVE:
            PrintComp(node, ex_file, tree);
            break;
        
        default:
            PrintDefault(node, ex_file, tree);
    }
}

void PrintNum(Node_t* node, FILE* ex_file) {
    fprintf(ex_file, "PUSH %lg\n", node->value->value);
}

void PrintVar(Node_t* node, FILE* ex_file, Tree_t* tree) {
    int ind = GetVarInd(tree, node->value->name);
    if (ind == -1) {
        printf(RED_COLOR "Wrong var's index\n" RESET_COLOR);
        return;
    }

    fprintf(ex_file,    "PUSH %d\n"
                        "PUSHR RBX\n"
                        "ADD\n"
                        "POPR RFX\n"
                        "PUSHM [RFX]\n", ind);
}

void PrintIf(Node_t* node, FILE* ex_file, Tree_t* tree) {
    static int if_cnt = 0;
    int if_ind = if_cnt++;

    Backend(node->left, ex_file, tree);
    fprintf(ex_file,    "\n"
                        "PUSH 0\n"
                        "JNE :endif_%d\n", if_ind);

    Backend(node->right, ex_file, tree);
    fprintf(ex_file,    ":endif_%d\n", if_ind);
}

void PrintWhile(Node_t* node, FILE* ex_file, Tree_t* tree) {
    static int while_cnt = 0;
    
    int while_ind = while_cnt++;
    fprintf(ex_file, "\n:beginwhile_%d\n", while_ind);

    Backend(node->left, ex_file, tree);
    fprintf(ex_file,    "PUSH 0\n"
                        "JNE :endwhile_%d\n"
                        "JMP :beginwhile_%d\n"
                        ":endwhile_%d\n\n", while_ind, while_ind, while_ind);
}

void PrintInput(Node_t* node, FILE* ex_file, Tree_t* tree) {
    fprintf(ex_file, "IN\n");

    int ind = GetVarInd(tree, GetRight(node)->value->name);
    PrintPopVar(ex_file, ind);
}

void PrintOutput(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(GetRight(node), ex_file, tree);
    
    fprintf(ex_file, "OUT\n");
}

void PrintFunc(Node_t* node, FILE* ex_file, Tree_t* tree) {
    const char* func_name = GetLeft(GetLeft(node))->value->name;
    fprintf(ex_file, "JMP :after_%s\n", func_name);
    fprintf(ex_file, "\n:%s\n", func_name);

    Tree_t* params = TreeCtor();
    params->root = GetRight(GetLeft(node));
    SelectTreeVars(GetRight(GetLeft(node)), params);

    for (int i = params->var_cnt - 1; i >= 0; --i) {
        PrintPopVar(ex_file, i + 1);
    }

    Tree_t* subtree = TreeCtor();
    subtree->root = node;
    SelectTreeVars(node, subtree);
    subtree->func_cnt = tree->func_cnt;
    subtree->funcs = tree->funcs;

    Backend(node->right, ex_file, subtree);
    fprintf(ex_file, "\n:after_%s\n", func_name);
}

void PrintCall(Node_t* node, FILE* ex_file, Tree_t* tree) {
    if (CheckTreeFunc(GetLeft(node), tree) == -1) {
        printf("Calling a non-existent function or procedure: %s\n", GetLeft(node)->value->name);
        return;
    }

    Backend(GetRight(node), ex_file, tree);

    fprintf(ex_file,    "\n"
                        "PUSHR RBX\n"
                        "PUSH 20\n"
                        "ADD\n"
                        "POPR RBX\n"
                        "\n"
                        "CALL :%s\n"
                        "\n"
                        "PUSHR RBX\n"
                        "PUSH 20\n"
                        "SUB\n"
                        "POPR RBX\n\n", GetLeft(node)->value->name);
}

void PrintComma(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(GetLeft(node), ex_file, tree);
    Backend(GetRight(node), ex_file, tree);
}

void PrintPushVar(FILE* ex_file, int ind) {
    fprintf(ex_file,    "PUSH %d\n"
                        "PUSHR RBX\n"
                        "ADD\n"
                        "POPR RFX\n"
                        "PUSHM [RFX]\n", ind);
} 

void PrintPopVar(FILE* ex_file, int ind) {
    fprintf(ex_file,    "PUSH %d\n"
                        "PUSHR RBX\n"
                        "ADD\n"
                        "POPR RFX\n"
                        "POPM [RFX]\n", ind);
}

void PrintReturn(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->right, ex_file, tree);

    fprintf(ex_file, "\nRET\n");
}

void PrintEq(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->right, ex_file, tree);
    char* var_name = node->left->value->name;
    
    int ind = GetVarInd(tree, var_name);
    fprintf(ex_file,    "PUSH %d\n"
                        "PUSHR RBX\n"
                        "ADD\n"
                        "POPR RFX\n"
                        "POPM [RFX]\n", ind);
}

void PrintComp(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->left, ex_file, tree);
    Backend(node->right, ex_file, tree);

    static int comp_cnt = 0;
    
    int oper = node->value->type;
    const char* comp_type = "JMP";
    const char* label_name = "jmp";

    switch (oper) {
        case OP_EQUAL:
            comp_type = "JE";
            label_name = "je";
            break;
        case OP_ABOVE:
            comp_type = "JA";
            label_name = "ja";
            break;
        case OP_LESS:
            comp_type = "JB";
            label_name = "jb";
            break;
    }

    fprintf(ex_file,    "\n%s :%s_%d\n"
                        "PUSH 0\n"
                        "JMP :endcomp_%d\n\n"
                        ":%s_%d\n"
                        "PUSH 1\n"
                        ":endcomp_%d\n\n", comp_type, label_name, comp_cnt, comp_cnt, 
                        label_name, comp_cnt, comp_cnt);

    ++comp_cnt;
}

void PrintDefault(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->left, ex_file, tree);
    Backend(node->right, ex_file, tree);
    fprintf(ex_file, "%s\n", opers[node->value->type].proc_view);
}
