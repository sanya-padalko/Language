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
        case OP_PUTM:
            PrintPutm(node, ex_file, tree);
            break;
        case OP_END:
            PrintEnd(node, ex_file, tree);
            break;
        case OP_DRAW:
            PrintDraw(node, ex_file, tree);
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
	double val = node->value->value;
    uint64_t bits;
    memcpy(&bits, &val, sizeof(bits));

    fprintf(ex_file, "    mov rax, %llu    	; загружаем double %lg в виде битов\n"
                     "    push rax			\n", bits, val);
}

// Predict:		rbp - указатель на начало переменных в стеке
// Destruction: rax, xmm0
void PrintVar(Node_t* node, FILE* ex_file, Tree_t* tree) {
    int ind = GetVarInd(tree, node->value->name);
    if (ind == -1) {
        printf(RED_COLOR "Wrong var's index\n" RESET_COLOR);
        return;
    }

	int offset = (ind + 1) * 8; // каждая переменная по 8 байт, нам нужная ind-ая в 0-индексации
    
    fprintf(ex_file, "    movsd xmm0, qword [rbp - %d]	; Сохраняем переменную %s\n"
                     "    movq rax, xmm0				\n"
                     "    push rax						\n", offset, node->value->name);
}

// Destruction: -
void PrintIf(Node_t* node, FILE* ex_file, Tree_t* tree) {
    static int if_cnt = 0;
    int if_ind = if_cnt++;

    Backend(node->left, ex_file, tree);
    int comp_oper = node->left->value->type;
    const char* jmp_type = "je";

	switch (comp_oper) {
        case OP_EQUAL: jmp_type = "jne"; break;
        case OP_ABOVE: jmp_type = "jbe"; break;
        case OP_LESS:  jmp_type = "jae"; break;
    }

	fprintf(ex_file, "    %s .endif_%d\n\n", jmp_type, if_ind);

    Backend(node->right, ex_file, tree);
	
	fprintf(ex_file, ".endif_%d:\n", if_ind);
}

// Destruction: -
void PrintWhile(Node_t* node, FILE* ex_file, Tree_t* tree) {
    static int while_cnt = 0;
    
    int while_ind = while_cnt++;
    fprintf(ex_file, "\n.beginwhile_%d:\n", while_ind);

    Backend(node->left, ex_file, tree);
    int comp_oper = node->left->value->type;
    const char* jmp_type = "je";

	switch (comp_oper) {
        case OP_EQUAL: jmp_type = "jne"; break;
        case OP_ABOVE: jmp_type = "jbe"; break;
        case OP_LESS:  jmp_type = "jae"; break;
    }

	fprintf(ex_file, "    %s .endwhile_%d	\n\n", jmp_type, while_ind);

    Backend(node->right, ex_file, tree);
	
	fprintf(ex_file, "jmp .beginwhile_%d	\n"
					 ".endwhile_%d:			\n\n", while_ind, while_ind);
}

// Добавить в PrintStart
/*
section .rodata
    msg_format_out: db "%lg", 10, 0
*/
void PrintInput(Node_t* node, FILE* ex_file, Tree_t* tree) {
    int ind = GetVarInd(tree, GetRight(node)->value->name);
    int offset = (ind + 1) * 8;

    fprintf(ex_file, 
        "    ; --- Ввод числа (IN) ---		\n"
        "    mov rbx, rsp                  ; cохраняем стек\n"
        "    and rsp, ~0xF                 ; стек по 16 байт\n"
        
        "    lea rsi, [rbp - %d]           ; RSI = адрес переменной %s\n"
        "    lea rdi, [rel msg_format_out] ; RDI = адрес строки \"%%lg\"\n"
        "    mov al, 0                     ; 0 вещ. аргументов\n"
        "    call _scanf                   ; системный scanf\n"
        "\n"
        "    mov rsp, rbx                  ; восстанавливаем оригинальный стек\n"
        "    ; ------------------------		\n\n", offset, GetRight(node)->value->name);
}

// Добавить в PrintStart
/*
section .rodata
    msg_format_out: db "%lg", 10, 0
*/
void PrintOutput(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(GetRight(node), ex_file, tree);
    
    fprintf(ex_file, 
        "    ; --- Вывод числа (OUT) ---\n"
        "    pop rax								\n"
        "    movq xmm0, rax                ; передаем наш double в xmm0\n"
        
        "    mov rbx, rsp                  ; сохраняем оригинальный стек в rbx\n"
        "    and rsp, ~0xF                 ; выравниваем стек по границе 16 байт\n"
        
        "    lea rdi, [rel msg_format_out] ; загружаем адрес строки формата \"%%lg\\n\"\n"
        "    mov al, 1                     ; 1 вещ. аргумент\n"
        "    call _printf                  ; системный printf\n"
        
        "    mov rsp, rbx                  ; восстанавливаем оригинальный стек\n"
        "    ; ------------------------\n\n");
}

void PrintDraw(Node_t* node, FILE* ex_file, Tree_t* tree) {
    fprintf(ex_file, "DRAW\n");
}

void PrintPutm(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(GetRight(node), ex_file, tree);
    Backend(GetLeft(node), ex_file, tree);
    fprintf(ex_file, "POPR RDX\n");
    fprintf(ex_file, "POPM [RDX]\n");
}
void PrintEnd(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(GetLeft(node), ex_file, tree);
    Backend(GetRight(node), ex_file, tree);
    fprintf(ex_file, "HLT\n");
}

void PrintFunc(Node_t* node, FILE* ex_file, Tree_t* tree) {
    const char* func_name = GetLeft(GetLeft(node))->value->name;
    fprintf(ex_file, "JMP :after_%s\n", func_name);
    fprintf(ex_file, "\n:%s\n", func_name);

    Tree_t* params = TreeCtor();
    params->root = GetRight(GetLeft(node));
    SelectTreeVars(GetRight(GetLeft(node)), params);

    for (int i = params->var_cnt - 1; i >= 0; --i) {
        PrintPopVar(ex_file, i);
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
                        "PUSH %d\n"
                        "ADD\n"
                        "POPR RBX\n"
                        "\n"
                        "CALL :%s\n"
                        "\n"
                        "PUSHR RBX\n"
                        "PUSH %d\n"
                        "SUB\n"
                        "POPR RBX\n\n", tree->var_cnt, GetLeft(node)->value->name, tree->var_cnt);
}

void PrintComma(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(GetLeft(node), ex_file, tree);
    Backend(GetRight(node), ex_file, tree);
}

void PrintReturn(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->right, ex_file, tree);

    fprintf(ex_file, "\nRET\n");
}

// Predict:		rbp - указатель на начало переменных в стеке
// Destruction: rax, xmm0
void PrintEq(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->right, ex_file, tree);
    char* var_name = node->left->value->name;
    
    int ind = GetVarInd(tree, var_name);
	int offset = (ind + 1) * 8;

    fprintf(ex_file, "    pop rax\n"
                     "    movq xmm0, rax\n"
                     "    movsd qword [rbp - %d], xmm0    ; %s = xmm0\n", offset, var_name);
}

// Destruction: rax
void PrintComp(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->left, ex_file, tree);
    Backend(node->right, ex_file, tree);

    fprintf(ex_file, "    pop rax				\n"
                     "    movq xmm1, rax		\n");

	fprintf(ex_file, "    pop rax				\n"
                     "    movq xmm0, rax		\n");
	
	fprintf(ex_file, "    ucomisd xmm0, xmm1 	\n");
}

// Destruction: rax, rbx, rcx, xmm0, xmm1
void PrintDefault(Node_t* node, FILE* ex_file, Tree_t* tree) {
    Backend(node->left, ex_file, tree);
    Backend(node->right, ex_file, tree);

	fprintf(ex_file, "    pop rcx			\n"
                     "    movq xmm1, rcx	\n");
	
	fprintf(ex_file, "    pop rbx			\n"
                     "    movq xmm0, rbx	\n");
	
	int oper = node->value->type;
	switch (oper) {
		case OP_ADD:
			fprintf(ex_file, "    addsd xmm0, xmm1    ; xmm0 = левый + правый\n");
            break;
		case OP_SUB:
			fprintf(ex_file, "    subsd xmm0, xmm1    ; xmm0 = левый - правый\n");
            break;
		case OP_MUL:
			fprintf(ex_file, "    mulsd xmm0, xmm1    ; xmm0 = левый * правый\n");
            break;
		case OP_DIV:
			fprintf(ex_file, "    divsd xmm0, xmm1    ; xmm0 = левый / правый\n");
            break;
		
		default:
            fprintf(stderr, "Unknown mathematical operator: %d, node: %llX\n", oper, (uint64_t)node);
	}
	
	fprintf(ex_file, "    movq rax, xmm0	\n"
                     "    push rax			\n\n");
}
