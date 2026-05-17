#include "reverse-end.h"

const char* TreeCodeGenerate(Tree_t* ast_tree) {
    const char* program_file = "ast_file.txt";
    FILE* prog = fopen(program_file, "w");

    NodeCodeGenerate(ast_tree->root, prog);

    fclose(prog);

    return program_file;
}

void NodeCodeGenerate(Node_t* node, FILE* prog) {
    if (!node) 
        return;

    if (node->type == NUM) {
        fprintf(prog, " %lg ", GetValue(node));
        return;
    }

    if (node->type == VAR) {
        fprintf(prog, "%s", node->value->name);
        return;
    }

    int ind = node->value->type;

    switch (ind) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_POW:
        case OP_EQUAL:
        case OP_LESS:
        case OP_ABOVE:
        case OP_EQ:
        case OP_COMMA:
            NodeCodeGenerate(node->left, prog);
            fprintf(prog, " %s ", opers[ind].dump_view);          // dump_view -> lang_view
            NodeCodeGenerate(node->right, prog);
            break;
        
        case OP_SQRT:
        case OP_LN:
        case OP_LOG:
        case OP_EXP:
        case OP_COS:
        case OP_SIN:
        case OP_TAN:
        case OP_COT:
        case OP_ASIN:
        case OP_ACOS:
        case OP_ATAN:
        case OP_ACOT:
        case OP_DRAW:
        case OP_RETURN:
        case OP_FINISH:
        case OP_PARAM:
        case OP_INPUT:
        case OP_OUTPUT:
        case OP_END:
            fprintf(prog, "%s ", opers[ind].dump_view);          //dump_view ->lang_view
            NodeCodeGenerate(GetRight(node), prog);
            break;
        
        case OP_IF:
        case OP_WHILE:
            if (ind == OP_IF)
                fprintf(prog, "if (");
            else 
                fprintf(prog, "while (");

            NodeCodeGenerate(GetLeft(node), prog);
            fprintf(prog, ") {\n");
            NodeCodeGenerate(GetRight(node), prog);
            fprintf(prog, "};\n\n");
            break;

        case OP_PROCEDURE:
        case OP_FUNC:
            if (ind == OP_FUNC)
                fprintf(prog, "function ");
            else 
                fprintf(prog, "procedure ");
            NodeCodeGenerate(GetLeft(GetLeft(node)), prog);
            fprintf(prog, "(");
            NodeCodeGenerate(GetRight(GetLeft(node)), prog);
            fprintf(prog, ") {\n");
            NodeCodeGenerate(GetRight(node), prog);
            fprintf(prog, "};\n\n");
            break;
        
        case OP_PUTM:
            fprintf(prog, "putm ");
            NodeCodeGenerate(GetLeft(node), prog);
            NodeCodeGenerate(GetRight(node), prog);
            break;
        
        case OP_CALL:
            fprintf(prog, "call ");
            NodeCodeGenerate(GetLeft(node), prog);
            fprintf(prog, "(");
            NodeCodeGenerate(GetRight(node), prog);
            fprintf(prog, ")");
            break;
        
        case OP_OPER:
            NodeCodeGenerate(GetLeft(node), prog);
            Node_t* left = GetLeft(node);
            if (left && (left->type != OPER || left->value->type != OP_OPER)) 
                fprintf(prog, ";\n");

            NodeCodeGenerate(GetRight(node), prog);
            Node_t* right = GetRight(node);
            if (right && (right->type != OPER || right->value->type != OP_OPER))
                fprintf(prog, ";\n");
            break;
    }
}