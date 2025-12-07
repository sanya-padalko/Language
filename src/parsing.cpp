#include "parsing.h"
#include "tree.h"

#define EQUAL(str, oper) !strncmp(str, oper, strlen(oper))

#define CHECK(ptr, symbol, add_info)    if (strncmp(*ptr, symbol, 1)) { \
                                            SyntaxError("( parsing " add_info " ), expected '" symbol "', but getting", **ptr); \
                                            return NULL; \
                                        } \
                                        else { \
                                            ++*ptr; \
                                            SkipSpaces(ptr); \
                                        }

#ifdef DEBUG
#define FUNC_START(str) printf("%s:\n %s\n ------------\n", __FUNCTION__, str);
#else
#define FUNC_START(str)
#endif

Tokenizator_t* SelectTokens(const char** s) {
    Tokenizator_t* tokenizator = (Tokenizator_t*)calloc(1, sizeof(Tokenizator_t));

    return tokenizator;
}

Node_t* GetG(const char** s) {
    SkipSpaces(s);

    Node_t* node = GetOp(s);

    while (**s != '\0') {
        Node_t* node2 = GetOp(s);
        if (!node2)
            return node;

        node = OP_NODE(OP_OPER, c(node), c(node2));
    }
    return node;
}

Node_t* GetOp(const char** s) {
    FUNC_START(*s);
    SkipSpaces(s);

    Node_t* node = NULL;

    if (EQUAL(*s, "if")) {
        node = GetIf(s);
    }
    else if (EQUAL(*s, "while")) {
        node = GetWhile(s);
    }
    else if (EQUAL(*s, "function")) {
        node = GetFunc(s);
    }
    else if ('a' <= **s && **s <= 'z') {
        node = GetA(s);
        CHECK(s, ";", "end of operation");
    }
    else {
        CHECK(s, "{", "operation");
        node = GetOp(s);

        while (**s != '}') {
            Node_t* node2 = GetOp(s);
            if (!node2)
                return node;

            node = OP_NODE(OP_OPER, c(node), c(node2));
        }

        CHECK(s, "}", "operation");
        CHECK(s, ";", "end of operation");
    }

    return node;
}

Node_t* GetFunc(const char** s) {
    SkipSpaces(s);
    FUNC_START(*s);

    if (!EQUAL(*s, "function")) {
        SyntaxError("(parsing function) expected \"function\", but getting", **s);
        return NULL;
    }

    *s += strlen("function");
    SkipSpaces(s);

    char* func_name = (char*)calloc(MAX_VAR_SIZE, sizeof(char));
    int ind = 0;

    while ('a' <= **s && **s <= 'z') {
        func_name[ind++] = **s;
        ++*s;
    }
    func_name[ind] = '\0';

    Node_t* name = VAR_NODE(func_name);
    Node_t* params = GetParams(s);

    Node_t* func_body = GetOp(s);

    SkipSpaces(s);
    return OP_NODE(OP_FUNC, OP_NODE(OP_FUNC_INFO, c(name), c(params)), c(func_body));
}

Node_t* GetParams(const char** s) {
    SkipSpaces(s);
    FUNC_START(*s);

    CHECK(s, "(", "params");
    Node_t* node = NULL;

    while (**s != ')') {
        if (node) {
            CHECK(s, ",", "params");
        }

        Node_t* node2 = GetParam(s);
        if (!node2)
            break;
        
        if (!node)
            node = node2;
        else
            node = OP_NODE(OP_COMMA, c(node), c(node2));
    }

    CHECK(s, ")", "params");

    return node;
}

Node_t* GetParam(const char** s) {
    SkipSpaces(s);
    FUNC_START(*s);

    if (!EQUAL(*s, "param")) {
        SyntaxError("(parsing param) expected \"param\", but getting", **s);
        return NULL;
    }

    *s += strlen("param");
    SkipSpaces(s);

    char* param_name = (char*)calloc(MAX_VAR_SIZE, sizeof(char));
    int ind = 0;
    
    while ('a' <= **s && **s <= 'z') {
        param_name[ind++] = **s;
        ++*s;
    }
    param_name[ind] = '\0';

    if (ind == 0) {
        printf("(parsing param) empty name of parameter\n");
        return NULL;
    }
    
    SkipSpaces(s);
    return OP_NODE(OP_PARAM, NULL, VAR_NODE(param_name));
}

Node_t* GetWhile(const char** s) {
    SkipSpaces(s);
    FUNC_START(*s);

    if (!EQUAL(*s, "while")) {
        SyntaxError("(parsing while) expected \"while\", but getting", **s);
        return NULL;
    }

    *s += strlen("while");
    SkipSpaces(s);

    CHECK(s, "(", "while");
    Node_t* node = GetE(s);
    CHECK(s, ")", "while");

    Node_t* node2 = GetOp(s);

    return OP_NODE(OP_WHILE, c(node), c(node2));
}

Node_t* GetIf(const char** s) {
    SkipSpaces(s);
    FUNC_START(*s);

    if (!EQUAL(*s, "if")) {
        SyntaxError("(parsing if) expected \"if\", but getting", **s);
        return NULL;
    }
    
    *s += strlen("if");
    SkipSpaces(s);

    CHECK(s, "(", "if");
    Node_t* node = GetE(s);
    CHECK(s, ")", "if");
    Node_t* node2 = GetOp(s);

    return OP_NODE(OP_IF, c(node), c(node2));
}

Node_t* GetA(const char** s) {
    SkipSpaces(s);
    FUNC_START(*s);

    Node_t* node = GetV(s);
    CHECK(s, "=", "assigned");
    Node_t* node2 = GetE(s);

    return OP_NODE(OP_EQ, c(node), c(node2));
}

Node_t* GetE(const char** s) {
    SkipSpaces(s);

    Node_t* node = GetT(s);

    while (**s == '+' || **s == '-' || **s == '^') {
        int op = **s;
        ++*s;
        Node_t* node2 = GetT(s);
        if (op == '^')
            node = OP_NODE(OP_POW, c(node), c(node2));
        else if (op == '+')
            node = OP_NODE(OP_ADD, c(node), c(node2));
        else 
            node = OP_NODE(OP_SUB, c(node), c(node2));
    }

    SkipSpaces(s);
    return node;
}

Node_t* GetT(const char** s) {
    SkipSpaces(s);

    Node_t* node = GetPower(s);

    while (**s == '*' || **s == '/') {
        int op = **s;
        ++*s;
        Node_t* node2 = GetPower(s);
        if (op == '*')
            node = OP_NODE(OP_MUL, c(node), c(node2));
        else 
            node = OP_NODE(OP_DIV, c(node), c(node2));
    }

    SkipSpaces(s);
    return node;
}

Node_t* GetPower(const char** s) {
    SkipSpaces(s);

    Node_t* node = GetP(s);

    while (**s == '^') {
        ++*s;
        Node_t* node2 = GetP(s);
        
        node = OP_NODE(OP_POW, c(node), c(node2));
    }

    return node;
}

Node_t* GetP(const char** s) {
    SkipSpaces(s);

    if (**s == '(') {
        ++*s;
        Node_t* node = GetE(s);

        CHECK(s, ")", "parentheses");
        return node;
    }

    if ('a' <= **s && **s <= 'z') {
        if (EQUAL(*s, "ln")) {
            *s += strlen("ln");
            SkipSpaces(s);

            CHECK(s, "(", "ln");
            Node_t* node = GetE(s);
            CHECK(s, ")", "ln");

            return node;
        }

        if (EQUAL(*s, "log")) {
            *s += strlen("log");
            SkipSpaces(s);

            CHECK(s, "(", "log");
            Node_t* argue = GetE(s);
            CHECK(s, ",", "log");
            Node_t* base = GetE(s);
            CHECK(s, ")", "log");

            return OP_NODE(OP_LOG, c(base), c(argue));
        }

        if (EQUAL(*s, "e")) {
            *s += strlen("e");
            SkipSpaces(s);

            CHECK(s, "^", "exp");
            CHECK(s, "(", "exp");
            Node_t* power = GetE(s);
            CHECK(s, ")", "exp");

            return OP_NODE(OP_EXP, NULL, c(power));
        }
        
        if (EQUAL(*s, "sin") || EQUAL(*s, "cos")) {
            char first_char = **s;
            *s += strlen("sin");
            SkipSpaces(s);

            CHECK(s, "(", "sin/cos");
            Node_t* node = GetE(s);
            switch(first_char) {
                case 'c':
                    node = OP_NODE(OP_COS, NULL, c(node));
                    break;
                case 's':
                    node = OP_NODE(OP_SIN, NULL, c(node));
                    break;
            }
            CHECK(s, ")", "sin/cos");

            return node;
        }

        for (int op_ind = OP_TAN; op_ind <= OP_ACOT; ++op_ind) {
            if (EQUAL(*s, opers[op_ind].dump_view)) {
                *s += strlen(opers[op_ind].dump_view);
                SkipSpaces(s);

                CHECK(s, "(", "trigonometry");
                Node_t* node = GetE(s);
                node = OP_NODE(opers[op_ind].type, NULL, c(node));

                CHECK(s, ")", "trigonometry");

                return node;
            }
        }

        SkipSpaces(s);
        return GetV(s);
    }
    
    SkipSpaces(s);
    return GetN(s);
}

Node_t* GetV(const char** s) {
    SkipSpaces(s);

    char* name = (char*)calloc(MAX_VAR_SIZE, sizeof(char));
    int ind = 0;

    while ('a' <= **s && **s <= 'z') {
        name[ind++] = **s;
        ++*s;
    }
    name[ind] = '\0';

    SkipSpaces(s);
    return VAR_NODE(name);
}

Node_t* GetN(const char** s) {
    SkipSpaces(s);

    const char* old_s = *s;
    double val = 0;

    while ('0' <= **s && **s <= '9') {
        val = val * 10 + (double)(**s - '0');
        ++*s;
    }

    if (*s == old_s) {
        SyntaxError("(parsing number) expected digit, but getting", **s);
    }

    SkipSpaces(s);
    return NUM_NODE(val);
}

void SkipSpaces(const char** s) {
    while (**s == ' ' || **s == '\n' || **s == '\t')
        ++*s;
}