#include "tree.h"

#define READ(name)  scanf("%[^\n]", name); \
                    getchar();

Tree_t* TreeCtor() {
    Tree_t* tree = (Tree_t*)calloc(1, sizeof(Tree_t));
    my_assert(tree, CALLOC_ERR, NULL);

    tree->html_file_name = "dump.html";
    tree->dot_file_name = "tree.dot";

    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->value = 0;
    tree->root = NodeCtor(NUM, value, NULL, NULL);
    tree->nodes_cnt = 1;
    
    tree->var_cnt = 0;
    tree->func_cnt = 0;

    tree->funcs = (char**)calloc(MAX_FUNC_SIZE, sizeof(char*));

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

    Tokenizator_t* tok = SelectTokens(&cur_pos);
    int ind = 0;
    tree->root = GetG(tok->tokens, &ind);

    return tree;
}

void SelectTreeFunc(Node_t* node, Tree_t* tree) {
    if (!node || !tree)
        return;

    if (node->type == OPER && (node->value->type == OP_FUNC || node->value->type == OP_PROCEDURE))
        AddFunc(GetLeft(GetLeft(node)), tree);

    SelectTreeFunc(GetLeft(node), tree);
    SelectTreeFunc(GetRight(node), tree);
}

void AddFunc(Node_t* node, Tree_t* tree) {
    if (!node || !tree)
        return;

    if (CheckTreeFunc(node, tree) == -1)
        tree->funcs[tree->func_cnt++] = node->value->name;

    return;
}

int CheckTreeFunc(Node_t* node, Tree_t* tree) {
    if (!node || !tree)
        return -1;

    for (int i = 0; i < tree->func_cnt; ++i) {
        if (!strcmp(tree->funcs[i], node->value->name))
            return i;
    }

    return -1;
}

void SelectTreeVars(Node_t* node, Tree_t* tree) {
    if (!node || !tree) 
        return;

    if (node->type == VAR) {
        if (GetVarInd(tree, node->value->name) == -1)
            tree->vars[tree->var_cnt++] = VarCtor(node->value->name);
        return;
    }

    SelectTreeVars(node->left, tree);
    SelectTreeVars(node->right, tree);

    return;
}

int GetVarInd(Tree_t* tree, char* var_name) {
    if (!tree || !var_name)
        return -1;

    for (int i = 0; i < tree->var_cnt; ++i) {
        if (!strcmp(tree->vars[i]->name, var_name))
            return i;
    }
    
    return -1;
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

Node_t* CopyNode(Node_t* node) {
    if (!node)
        return NULL;

    Node_t* new_node = (Node_t*)calloc(1, sizeof(Node_t));
    new_node->type = node->type;
    new_node->value = (ValueType*)calloc(1, sizeof(ValueType));
    *(new_node->value) = *(node->value);

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

void PrintValueNode(Node_t* node) {
    if (node->type == NUM)
        printf("NUM:  %lg\n", node->value->value);
    else if (node->type == VAR)
        printf("VAR:  %s\n", node->value->name);
    else 
        printf("OPER: %d\n", node->value->type);
}