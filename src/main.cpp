#include "folders.h"
#include "tree.h"
#include "parsing.h"
#include "frontend.h"
#include "backend.h"
#include "reverse-end.h"

int main(int argc, char* argv[]) {
    const char* real_data = GetTime();
    MakeDir(real_data);

    const char* program = "../../programs/program.txt";
    const char* ast_file = "ast.txt";
    const char* assem_file = "assem.txt";
    const char* ex_file = "ex_file.txt";
    const char* listing_file = "listing.txt";

    if (argc > 1) 
        program = argv[1];

    Tree_t* tree = TreeCtor();
    ReadBase(tree, program);
	fprintf(stderr, "%s", tree->buf);
    const char* cur_pos = tree->buf;
    Tokenizator_t* tok = SelectTokens(&cur_pos);

    tree = ParseTree(tree);
    Frontend(tree->root, ast_file);
    DUMP(tree, NOTHING, "Program");

    Tree_t* ast = GetTreeAST(ast_file);
    DUMP(ast, NOTHING, "AST");

    SelectTreeVars(ast->root, ast);
    SelectTreeFunc(ast->root, ast);

    TreeCodeGenerate(ast);

    FILE* program_file = fopen(assem_file, "w");
	PrintFileStart(program_file);
    Backend(ast->root, program_file, ast);
}
