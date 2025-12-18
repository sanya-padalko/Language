#include "middle.h"
#include "tree.h"
#include "parsing.h"

CodeError_t Optimization(Node_t** node_ptr) {
    if (!node_ptr || !(*node_ptr))
        return NULLPTR;

    Optimization(&((*node_ptr)->left));
    Optimization(&((*node_ptr)->right));

    ConvolConst(node_ptr);
    RemovingNeutral(node_ptr);
    
    return NOTHING;
}

void ConvolConst(Node_t** node) {
    if (!node || !(*node))
        return;

    if ((*node)->type == NUM || (*node)->type == VAR) 
        return;

    #define is_const(node, dir) ((node)->dir && (node)->dir->type == NUM) 
    int oper = (*node)->value->type;

    if (GetOperArgs(*node) == 1) {
        if (!is_const(*node, right))
            return;

        (*node)->type = NUM;
        (*node)->value->value = opers[oper].func(GetLeft(*node), GetRight(*node));

        (*node)->left = (*node)->right = NULL;
    }
    else if (is_const(*node, left) && is_const(*node, right)) {
        (*node)->type = NUM;
        (*node)->value->value = opers[oper].func(GetLeft(*node), GetRight(*node));

        (*node)->left = (*node)->right = NULL;
    }
    #undef is_const

    return;
}

int GetOperArgs(Node_t* node) {
    if (!node || node->type != OPER)
        return 0;

    int oper = node->value->type;
    switch (oper) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_POW:
        case OP_LOG:
        case OP_DIV:
            return 2;
    }

    return 1;
}

#define is_const(node, dir) ((node)->dir && (node)->dir->type == NUM)
#define is_null(node, dir) (is_const(node, dir) && (GetValue((node)->dir) == double(0)))
#define is_one(node, dir) (is_const(node, dir) && (GetValue((node)->dir) == double(1)))

void RemovingNeutral(Node_t** node) {
    if (!node || !(*node))
        return;

    if ((*node)->type == NUM || (*node)->type == VAR) 
        return;
    
    int oper = (*node)->value->type;
    switch (oper) {
        case OP_ADD:
            if (is_null(*node, right))
                *node = GetLeft(*node);
            else if (is_null(*node, left))
                *node = GetRight(*node);
            break;

        case OP_SUB:
            if (is_null(*node, right))
                *node = GetLeft(*node);
            break;
            
        case OP_DIV:
            if (is_one(*node, right))
                *node = GetLeft(*node);
            break;

        case OP_POW:
            if (is_one(*node, right))
                *node = GetLeft(*node);
            else if (is_null(*node, right) || is_one(*node, left))
                *node = NUM_NODE(1);
            break;

        case OP_MUL:
            RemovingMul(node);
    }

    return;
}

void RemovingMul(Node_t** node) {
    if ((*node)->type == OPER && (*node)->value->type == OP_MUL && is_const(*node, right)) {
        Node_t* add_son = (*node)->right;
        (*node)->right = (*node)->left;
        (*node)->left = add_son;
    }

    Node_t* right = GetRight(*node);
    Node_t* last_node = *node;
    if (is_const(*node, left) && right &&  right->type == OPER && right->value->type == OP_MUL && is_const(right, left)) {
        double value = GetValue((*node)->left);
        *node = right;
        (*node)->left->value->value *= value; 
        return;
    }

    if (is_one(*node, left)) {
        *node = GetRight(*node);
        return;
    }
    else if (is_one(*node, right)) {
        *node = GetLeft(*node);
        return;
    }

    if (is_null(*node, left) || is_null(*node, right))
        *node = NUM_NODE(0);
}

#undef is_num
#undef is_null
#undef is_one

Tree_t* GetTreeAST(const char* file_name) {
    Tree_t* tree = TreeCtor();
    ReadAST(tree, file_name);
    const char* cur_pos = tree->buf;
    Tokenizator_t* tok = SelectTokens(&cur_pos);

    int ind = 0;
    tree->root = ParseAST(tok->tokens, &ind);

    return tree;
}

CodeError_t ReadAST(Tree_t* tree, const char* file_name) {
    my_assert(tree, NULLPTR, NULLPTR);
    my_assert(file_name, NULLPTR, NULLPTR);

    int file_size = get_file_size(file_name);
    tree->buf = (char*)calloc(file_size + 1, sizeof(char));

    FILE* ast_file = fopen(file_name, "r");
    fread(tree->buf, sizeof(char), file_size, ast_file);
    fclose(ast_file);

    return NOTHING;
}

Node_t* ParseAST(Node_t** tokens, int* ind) {
    my_assert(tokens, NULLPTR, NULL);
    my_assert(*tokens, NULLPTR, NULL);
    my_assert(ind, NULLPTR, NULL);

    if (CHECK_TOKEN(tokens[*ind], OP_LBR)) {
        ++*ind;
        if (CHECK_TOKEN(tokens[*ind], OP_RBR)) {
            ++*ind;
            return NULL;
        }

        Node_t* node = tokens[*ind];
        ++*ind;

        if (node->type == VAR) {
            for (int i = 0; i < OPER_CNT; ++i) {
                if (!strcmp(node->value->name, opers[i].dump_view)) {
                    node->type = OPER;
                    node->value->type = opers[i].type;
                    break;
                }
            }
        }

        node->left = ParseAST(tokens, ind);
        node->right = ParseAST(tokens, ind);

        if (!CHECK_TOKEN(tokens[*ind], OP_RBR)) {
            printerr(RED_COLOR "ERRORS IN TEXT OF BASE\n" RESET_COLOR);
            return node;
        }

        ++*ind;
        return node;
    }

    printerr(RED_COLOR "ERRORS IN TEXT OF AST\n" RESET_COLOR);
    return NULL;
}