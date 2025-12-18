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
#define FUNC_START(str) printf("%s:\n %s\n ------------\n", __FUNCTION__, str)
#define START() printf("Enter to func: %s\n", __FUNCTION__)
#define END()   printf(" --- Exit the func: %s\n", __FUNCTION__)
#else
#define FUNC_START(str)
#define START() printf("Enter to func: %s\n", __FUNCTION__)
#define END()   printf(" --- Exit the func: %s\n", __FUNCTION__)
#endif

Tokenizator_t* SelectTokens(const char** s) {
    Tokenizator_t* tok = (Tokenizator_t*)calloc(1, sizeof(Tokenizator_t));
    SkipSpaces(s);

    while (**s != '\0') {
        Node_t* node = NULL;
        node = SelectOper(s);

        if (!node) {
            if (('a' <= **s && **s <= 'z') || (**s == '\"'))
                node = GetV(s);
            else if ('0' <= **s && **s <= '9')
                node = GetN(s);
            else
                node = SelectBracket(s);
        }

        if (!node) {
            printf("Error:\n%s\n ------- \n", *s);
            return tok;
        }

        tok->tokens[tok->token_cnt++] = node;
        SkipSpaces(s);
    }

    return tok;
}

Node_t* SelectOper(const char** s) {
    for (int i = OP_ADD; i <= OP_COMMA; ++i) {
        if (EQUAL(*s, opers[i].dump_view)) {
            *s += strlen(opers[i].dump_view);
            return OP_NODE(opers[i].type, NULL, NULL);
        }
    }

    return NULL;
}

Node_t* SelectBracket(const char** s) {
    for (int i = OP_LBR; i <= OP_FRBR; ++i) {
        if (EQUAL(*s, opers[i].dump_view)) {
            *s += strlen(opers[i].dump_view);
            return OP_NODE(opers[i].type, NULL, NULL);
        }
    }
    return NULL;
}

Node_t* GetV(const char** s) {
    SkipSpaces(s);

    char* name = (char*)calloc(MAX_VAR_SIZE, sizeof(char));
    int ind = 0;

    while (('a' <= **s && **s <= 'z') || (**s == '\"')) {
        if (**s != '\"')
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

Node_t* GetG(Node_t** tokens, int* ind) {
    START();
    Node_t* node = GetOp(tokens, ind);

    while (tokens[*ind]) {
        Node_t* node2 = GetOp(tokens, ind);
        if (!node2)
            return NULL;

        node = OP_NODE(OP_OPER, c(node), c(node2));
    }

    END();
    return node;
}

Node_t* GetOp(Node_t** tokens, int* ind) {
    START();
    Node_t* node = NULL;
    Node_t* node2 = NULL;

    int type = tokens[*ind]->type;
    int oper = -1;
    if (type == OPER)
        oper = tokens[*ind]->value->type;

    switch (oper) {
        case OP_IF:
            node = GetIf(tokens, ind);
            break;
        case OP_WHILE:
            node = GetWhile(tokens, ind);
            break;
        case OP_FUNC:
            node = GetFunc(tokens, ind);
            break;
        case OP_PROCEDURE:
            node = GetProcedure(tokens, ind);
            break;
        case OP_RETURN:
            node = GetReturn(tokens, ind);
            break;
        case OP_FINISH:
            node = GetFinish(tokens, ind);
            break;
        case OP_CALL:
            node = GetCall(tokens, ind);
            break;
        case OP_INPUT:
            node = GetInput(tokens, ind);
            break;
        case OP_OUTPUT:
            node = GetOutput(tokens, ind);
            break;
        case OP_FLBR:
            ++*ind;

            node = GetOp(tokens, ind);

            while (!CHECK_TOKEN(tokens[*ind], OP_FRBR)) {
                node2 = GetOp(tokens, ind);
                if (!node2) 
                    return node;

                node = OP_NODE(OP_OPER, c(node), c(node2));
            }
            ++*ind;

            if (!CHECK_TOKEN(tokens[*ind], OP_OPER)) {
                printf("There isn't operation's ended symbol\n");
                return NULL;
            }
            ++*ind;

            break;
        case -1:
            node = GetA(tokens, ind);
            if (!CHECK_TOKEN(tokens[*ind], OP_OPER)) {
                printf("There isn't operation's ended symbol\n");
                return NULL;
            }
            ++*ind;
                
            return node;
        
        default:
            printf("Wrong token finded: ");
            PrintValueNode(tokens[*ind]);
            return NULL;
    }

    return node;
}

Node_t* GetFunc(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_FUNC)) {
        printf("(parsing function) expected \"function\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* name = tokens[*ind];
    if (name->type != VAR) {
        printf("wrong token: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* params = GetParams(tokens, ind);

    Node_t* func_body = GetOp(tokens, ind);
    if (!func_body)
        return NULL;

    END();
    return OP_NODE(OP_FUNC, OP_NODE(OP_INFO, c(name), c(params)), c(func_body));
}

Node_t* GetProcedure(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_PROCEDURE)) {
        printf("(parsing procedure) expected \"procedure\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* name = tokens[*ind];
    if (name->type != VAR) {
        printf("wrong token: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* params = GetParams(tokens, ind);

    Node_t* proc_body = GetOp(tokens, ind);
    if (!proc_body)
        return NULL;

    END();
    return OP_NODE(OP_PROCEDURE, OP_NODE(OP_INFO, c(name), c(params)), c(proc_body));
}

Node_t* GetReturn(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_RETURN)) {
        printf("(parsing return) expected \"return\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* node = GetE(tokens, ind);
    if (!node)
        return NULL;

    if (!CHECK_TOKEN(tokens[*ind], OP_OPER)) {
        printf("(parsing return) missing \';\': ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    return OP_NODE(OP_RETURN, NULL, c(node));
}

Node_t* GetFinish(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_FINISH)) {
        printf("(parsing finish) expected \"finish\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    if (!CHECK_TOKEN(tokens[*ind], OP_OPER)) {
        printf("(parsing finish) missing \';\': ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    END();
    return OP_NODE(OP_FINISH, NULL, NULL);
}

Node_t* GetParams(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_LBR)) {
        printf("(parsing params) expected \"function\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* node = NULL;

    while (!CHECK_TOKEN(tokens[*ind], OP_RBR)) {
        if (node) {
            if (!CHECK_TOKEN(tokens[*ind], OP_COMMA)) {
                printf("Missing \',\' between parametres\n");
                return NULL;
            }
            ++*ind;
        }

        Node_t* node2 = GetParam(tokens, ind);
        if (!node2)
            return NULL;
        
        if (!node)
            node = node2;
        else
            node = OP_NODE(OP_COMMA, c(node), c(node2));
    }

    ++*ind;

    END();
    return node;
}

Node_t* GetParam(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_PARAM)) {
        printf("(parsing param) expected \"param\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* name = NULL;
    if (tokens[*ind]->type != VAR)
        return NULL;

    name = tokens[*ind];
    ++*ind;
    
    END();
    return OP_NODE(OP_PARAM, NULL, name);
}

Node_t* GetCall(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_CALL)) {
        printf("(parsing call) expected \"call\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* name = tokens[*ind];
    if (name->type != VAR)
        return NULL;
    ++*ind;

    Node_t* args = GetArgues(tokens, ind);

    if (!CHECK_TOKEN(tokens[*ind], OP_OPER)) {
        printf("There isn't call's ended symbol\n");
        return NULL;
    }
    ++*ind;

    END();
    return OP_NODE(OP_CALL, name, args);
}

Node_t* GetArgues(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_LBR)) {
        printf("(parsing argues) expected \"(\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* node = GetE(tokens, ind);

    while (CHECK_TOKEN(tokens[*ind], OP_COMMA)) {
        ++*ind;

        Node_t* node2 = GetE(tokens, ind);

        node = OP_NODE(OP_COMMA, c(node), c(node2));
    }

    if (!CHECK_TOKEN(tokens[*ind], OP_RBR)) {
        printf("(parsing argues) expected \")\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    END();
    return node;
}

Node_t* GetInput(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_INPUT)) {
        printf("(parsing input) expected \"input\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* node = tokens[*ind];
    if (node->type != VAR) {
        printf("(parsing input) expected \"input\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    if (!CHECK_TOKEN(tokens[*ind], OP_OPER)) {
        printf("There isn't operation's ended symbol\n");
        return NULL;
    }
    ++*ind;

    END();
    return OP_NODE(OP_INPUT, NULL, c(node));
}

Node_t* GetOutput(Node_t** tokens, int* ind) {
    START();
    if (!CHECK_TOKEN(tokens[*ind], OP_OUTPUT)) {
        printf("(parsing output) expected \"output\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    Node_t* node = GetE(tokens, ind);
    
    if (!CHECK_TOKEN(tokens[*ind], OP_OPER)) {
        printf("There isn't operation's ended symbol\n");
        return NULL;
    }
    ++*ind;

    END();
    return OP_NODE(OP_OUTPUT, NULL, c(node));
}

Node_t* GetWhile(Node_t** tokens, int* ind) {
    START();

    if (!CHECK_TOKEN(tokens[*ind], OP_WHILE)) {
        printf("(parsing while) expected \"while\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    if (!CHECK_TOKEN(tokens[*ind], OP_LBR))
        return NULL;
    ++*ind;

    Node_t* node = GetE(tokens, ind);
    if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
        return NULL;
    ++*ind;

    Node_t* node2 = GetOp(tokens, ind);

    END();
    return OP_NODE(OP_WHILE, c(node), c(node2));
}

Node_t* GetIf(Node_t** tokens, int* ind) {
    START();

    if (!CHECK_TOKEN(tokens[*ind], OP_IF)) {
        printf("(parsing if) expected \"if\", but getting: ");
        PrintValueNode(tokens[*ind]);
        return NULL;
    }
    ++*ind;

    if (!CHECK_TOKEN(tokens[*ind], OP_LBR))
        return NULL;
    ++*ind;

    Node_t* node = GetE(tokens, ind);

    if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
        return NULL;
    ++*ind;

    Node_t* node2 = GetOp(tokens, ind);

    END();
    return OP_NODE(OP_IF, c(node), c(node2));
}

Node_t* GetA(Node_t** tokens, int* ind) {
    START();
    Node_t* node = tokens[*ind];
    if (node->type != VAR)
        return NULL;

    ++*ind;

    if (!CHECK_TOKEN(tokens[*ind], OP_EQ))
        return NULL;
    ++*ind;

    Node_t* node2 = GetE(tokens, ind);

    END();
    return OP_NODE(OP_EQ, c(node), c(node2));
}

Node_t* GetE(Node_t** tokens, int* ind) {
    START();
    Node_t* node = GetSimple(tokens, ind);

    while (CHECK_TOKEN(tokens[*ind], OP_EQUAL) || CHECK_TOKEN(tokens[*ind], OP_LESS) || CHECK_TOKEN(tokens[*ind], OP_ABOVE)) {
        int op = tokens[*ind]->value->type;
        ++*ind;

        Node_t* node2 = GetSimple(tokens, ind);
        if (op == OP_EQUAL)
            node = OP_NODE(OP_EQUAL, c(node), c(node2));
        else if (op == OP_LESS)
            node = OP_NODE(OP_LESS, c(node), c(node2));
        else 
            node = OP_NODE(OP_ABOVE, c(node), c(node2));
    }

    END();
    return node;
}

Node_t* GetSimple(Node_t** tokens, int* ind) {
    START();
    Node_t* node = GetT(tokens, ind);

    while (CHECK_TOKEN(tokens[*ind], OP_ADD) || CHECK_TOKEN(tokens[*ind], OP_SUB) || CHECK_TOKEN(tokens[*ind], OP_POW)) {
        int op = tokens[*ind]->value->type;
        ++*ind;

        Node_t* node2 = GetT(tokens, ind);
        if (op == OP_POW)
            node = OP_NODE(OP_POW, c(node), c(node2));
        else if (op == OP_ADD)
            node = OP_NODE(OP_ADD, c(node), c(node2));
        else 
            node = OP_NODE(OP_SUB, c(node), c(node2));
    }

    END();
    return node;
}

Node_t* GetT(Node_t** tokens, int* ind) {
    START();
    Node_t* node = GetPower(tokens, ind);

    while (CHECK_TOKEN(tokens[*ind], OP_MUL) || CHECK_TOKEN(tokens[*ind], OP_DIV)) {
        int op = tokens[*ind]->value->type;
        ++*ind;

        Node_t* node2 = GetPower(tokens, ind);
        if (op == OP_MUL)
            node = OP_NODE(OP_MUL, c(node), c(node2));
        else 
            node = OP_NODE(OP_DIV, c(node), c(node2));
    }

    END();
    return node;
}

Node_t* GetPower(Node_t** tokens, int* ind) {
    START();
    Node_t* node = GetP(tokens, ind);

    while (CHECK_TOKEN(tokens[*ind], OP_POW)) {
        ++*ind;
        Node_t* node2 = GetP(tokens, ind);
        
        node = OP_NODE(OP_POW, c(node), c(node2));
    }

    END();
    return node;
}

Node_t* GetP(Node_t** tokens, int* ind) {
    START();

    if (CHECK_TOKEN(tokens[*ind], OP_LBR)) {
        ++*ind;
        Node_t* node = GetE(tokens, ind);

        if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
            return NULL;
        ++*ind;

        END();
        return node;
    }

    if (tokens[*ind]->type != NUM) {
        int oper = -1;
        if (tokens[*ind]->type == OPER) {
            oper = tokens[*ind]->value->type;
        }
        else {
            Node_t* node = tokens[*ind];
            ++*ind;
            END();
            return node;
        }

        Node_t* node = NULL;
        Node_t* argue = NULL;
        Node_t* base = NULL;
        Node_t* name = NULL;

        switch (oper) {
            case OP_SQRT:
                ++*ind;
                if (!CHECK_TOKEN(tokens[*ind], OP_LBR))
                    return NULL;
                ++*ind;

                node = GetE(tokens, ind);
                if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
                    return NULL;
                ++*ind;

                END();
                return OP_NODE(OP_SQRT, NULL, node);
                
            case OP_LN:
                ++*ind;
                if (!CHECK_TOKEN(tokens[*ind], OP_LBR))
                    return NULL;
                ++*ind;

                node = GetE(tokens, ind);
                if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
                    return NULL;
                ++*ind;

                END();
                return node;
            
            case OP_LOG:
                ++*ind;

                if (!CHECK_TOKEN(tokens[*ind], OP_LBR))
                    return NULL;
                ++*ind;

                argue = GetE(tokens, ind);
                if (!CHECK_TOKEN(tokens[*ind], OP_COMMA))
                    return NULL;
                ++*ind;

                base = GetE(tokens, ind);
                if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
                    return NULL;
                ++*ind;

                END();
                return OP_NODE(OP_LOG, c(base), c(argue));
            
            case OP_EXP:
                ++*ind;

                if (!CHECK_TOKEN(tokens[*ind], OP_LBR))
                    return NULL;
                ++*ind;

                node = GetE(tokens, ind);
                if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
                    return NULL;
                ++*ind;

                END();
                return OP_NODE(OP_EXP, NULL, c(node));
            
            case OP_CALL:
                ++*ind;
                if (tokens[*ind]->type != VAR)
                    return NULL;

                name = tokens[*ind];
                ++*ind;

                END();
                return OP_NODE(OP_CALL, name, GetArgues(tokens, ind));
                
            default:

                for (int op_ind = OP_COS; op_ind <= OP_ACOT; ++op_ind) {
                    if (oper == op_ind) {
                        ++*ind;
                        
                        if (!CHECK_TOKEN(tokens[*ind], OP_LBR))
                            return NULL;
                        ++*ind;

                        node = GetE(tokens, ind);
                        if (!CHECK_TOKEN(tokens[*ind], OP_RBR))
                            return NULL;
                        ++*ind;

                        return node;
                    }
                }
                break;
        }

        return NULL;
    }
    
    Node_t* node = tokens[*ind];
    ++*ind;

    END();
    return node;
}

void SkipSpaces(const char** s) {
    while (**s == ' ' || **s == '\n' || **s == '\t')
        ++*s;
}