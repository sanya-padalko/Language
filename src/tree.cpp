#include "tree.h"

#define READ(name)  scanf("%[^\n]", name); \
                    getchar();

Tree_t* TreeCtor() {
    Tree_t* tree = (Tree_t*)calloc(1, sizeof(Tree_t));
    my_assert(tree, CALLOC_ERR, NULL);

    tree->html_file_name = "dump.html";
    tree->dot_file_name = "tree.dot";

    tree->var_cnt = 0;

    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->value = 0;
    tree->root = NodeCtor(NUM, value, NULL, NULL);
    tree->nodes_cnt = 1;

    return tree;
}

Node_t* NodeCtor(TYPES type, ValueType* value, Node_t* left, Node_t* right) {
    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    my_assert(node, CALLOC_ERR, NULL);

    node->type = type;
    node->value = value;
    node->left = left;
    node->right = right;

    return node;
}

CodeError_t TreeDtor(Node_t* root) {
    my_assert(root, NULLPTR, NULLPTR);

    if (root->left) 
        TreeDtor(root->left);

    if (root->right) 
        TreeDtor(root->right);

    free(root);

    return NOTHING;
}

CodeError_t TreeVerify(Tree_t* tree) {
    if (!tree)
        return NULLPTR;

    if (!tree->root)
        return NULLPTR;

    if (tree->nodes_cnt <= 0)
        return SIZE_ERR;

    int real_size = GetSize(tree->root);

    if (real_size != tree->nodes_cnt)
        return CYCLE_ERR;
    
    return NOTHING;
}

int GetSize(Node_t* root) {
    my_assert(root, NULLPTR, MAX_NODES_CNT);

    int sz = 1;
    if (root->left)
        sz += GetSize(root->left);
    
    if (sz > MAX_NODES_CNT)
        return sz;

    if (root->right)
        sz += GetSize(root->right);

    return sz;
}

CodeError_t ReadBase(Tree_t* tree, const char* file_name) {
    my_assert(tree, NULLPTR, NULLPTR);
    my_assert(file_name, NULLPTR, NULLPTR);

    int file_size = get_file_size(file_name);
    tree->buf = (char*)calloc(file_size + 1, sizeof(char));

    FILE* file_base = fopen(file_name, "r");
    fread(tree->buf, sizeof(char), file_size, file_base);
    fclose(file_base);

    return NOTHING;
}

int get_file_size(const char* file_name) {
    my_assert(file_name, NULLPTR, 0);

    struct stat file_stat; 
    int stat_result = stat(file_name, &file_stat);
    my_assert(!stat_result, FILE_ERR, 0);

    return file_stat.st_size;
}

Tree_t* ParseTree(Tree_t* tree) {
    if (!tree || !tree->buf)
        return NULL;

    const char* cur_pos = tree->buf;
    char* old_buf = tree->buf;
    tree = TreeCtor();
    tree->buf = old_buf;
    tree->root = GetG(&cur_pos);

    return tree;
}

void SelectTreeVars(Node_t* node, Tree_t* tree) {
    if (!node || !tree) 
        return;

    if (node->type == NUM)
        return;

    if (node->type == VAR) {
        if (!CheckVars(tree, node->value->name))
            tree->vars[tree->var_cnt++] = VarCtor(node->value->name);
        return;
    }

    SelectTreeVars(node->left, tree);
    SelectTreeVars(node->right, tree);

    return;
}

bool CheckVars(Tree_t* tree, char* var_name) {
    if (!tree || !var_name)
        return false;

    for (int i = 0; i < tree->var_cnt; ++i) {
        if (!strcmp(tree->vars[i]->name, var_name))
            return true;
    }
    
    return false;
}

Var_t* VarCtor(char* name) {
    Var_t* new_var = (Var_t*)calloc(1, sizeof(Var_t));
    new_var->name = name;
    new_var->value = NAN;
    
    return new_var;
}

ValueType* ValueOperCtor(int type) {
    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->type = type;
    return value;
}

ValueType* ValueVarCtor(char* name) {
    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->name = name;
    return value;
}

ValueType* ValueNumCtor(double value) {
    ValueType* val_type = (ValueType*)calloc(1, sizeof(ValueType));
    val_type->value = value;
    return val_type;
}

double CalculatingTree(Tree_t* tree) {
    my_assert(tree, NULLPTR, NAN);

    printf("Enter the values of vars:\n");
    for (int i = 0; i < tree->var_cnt; ++i) {
        printf("%s = ", tree->vars[i]->name);
        scanf("%lg", &tree->vars[i]->value);
    }

    return CalculatingNode(tree->root, tree);
}

double CalculatingNode(Node_t* node, Tree_t* tree) {
    if (!node)
        return NAN;

    if (node->type == NUM)
        return node->value->value;

    if (node->type == VAR) {
        for (int i = 0; i < tree->var_cnt; ++i) {
            if (!strcmp(tree->vars[i]->name, node->value->name))
                return tree->vars[i]->value;
        }

        return NAN;
    }

    double left_res = CalculatingNode(node->left, tree);
    double right_res = CalculatingNode(node->right, tree);

    double result = opers[node->value->type].func(NUM_NODE(left_res), NUM_NODE(right_res));

    return result;
}

double CalcConstNode(Node_t* node, double value) {
    if (!node)
        return NAN;

    if (node->type == NUM)
        return node->value->value;

    if (node->type == VAR)
        return value;

    double left_res = CalcConstNode(node->left, value);
    double right_res = CalcConstNode(node->right, value);

    double result = opers[node->value->type].func(NUM_NODE(left_res), NUM_NODE(right_res));

    return result;
}

Node_t* CopyNode(Node_t* node) {
    if (!node)
        return NULL;

    Node_t* new_node = (Node_t*)calloc(1, sizeof(Node_t));
    new_node->type = node->type;
    new_node->value = node->value;

    new_node->left = CopyNode(node->left);
    new_node->right = CopyNode(node->right);

    return new_node;
}

double GetValue(Node_t* node) {
    if (!node || !node->value)
        return NAN;

    return node->value->value;
}

Node_t* GetLeft(Node_t* node) {
    if (!node)
        return NULL;

    return node->left;
}

Node_t* GetRight(Node_t* node) {
    if (!node)
        return NULL;

    return node->right;
}

void ConvolConst(Node_t** node) {
    if (!node || !(*node))
        return;

    if ((*node)->type == NUM) 
        return;

    if ((*node)->type == VAR)
        return;

    #define is_const(node, dir) ((node)->dir && (node)->dir->type == NUM) 
    int oper = (*node)->value->type;

    if (oper != OP_ADD && oper != OP_SUB && oper != OP_MUL && oper != OP_POW && oper != OP_LOG && oper != OP_DIV) {
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

void RemovingNeutral(Node_t** node) {
    if (!node || !(*node))
        return;

    if ((*node)->type == NUM) 
        return;

    if ((*node)->type == VAR)
        return;

    #define is_num(node, dir) ((node)->dir && (node)->dir->type == NUM)
    #define is_null(node, dir) (is_num(node, dir) && GetValue((node)->dir) == 0)
    #define is_one(node, dir) (is_num(node, dir) && GetValue((node)->dir) == 1)
    
    int oper = (*node)->value->type;
    
    if ((*node)->type == OPER && (*node)->value->type == OP_MUL && is_num(*node, right)) {
        Node_t* add_son = (*node)->right;
        (*node)->right = (*node)->left;
        (*node)->left = add_son;
    }

    if (oper == OP_MUL) {
        Node_t* right = GetRight(*node);
        if (is_num(*node, left) && right &&  right->type == OPER && right->value->type == OP_MUL && is_num(right, left)) {
            double value = GetValue((*node)->left);
            *node = right;
            (*node)->left->value->value *= value; 
            return;
        }

        if (is_one(*node, left))
            *node = GetRight(*node);
        else if (is_one(*node, right))
            *node = GetLeft(*node);

        if (is_null(*node, left) || is_null(*node, right)) {
            (*node)->type = NUM;
            (*node)->value->value = 0;
            return;
        }
    }

    if (oper == OP_ADD || oper == OP_SUB || oper == OP_DIV || oper == OP_POW) {
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
        }
    }

    #undef is_num
    #undef is_null
    #undef is_one

    return;
}

CodeError_t Optimization(Node_t** node_ptr) {
    Node_t* node = *node_ptr;

    if (!node || !node_ptr)
        return NULLPTR;

    Optimization(&node->left);
    Optimization(&node->right);

    ConvolConst(node_ptr);
    RemovingNeutral(node_ptr);

    return NOTHING;
}