#include "folders.h"
#include "tree.h"
#include "parsing.h"
#include "processor.h"
#include "frontend.h"
#include "backend.h"

int main() {
    const char* real_data = GetTime();
    MakeDir(real_data);

    const char* program = "../program.txt";
    const char* ast_file = "ast.txt";
    const char* assem_file = "assem.txt";
    const char* ex_file = "ex_file.txt";
    const char* listing_file = "listing.txt";

    Tree_t* tree = TreeCtor();
    ReadBase(tree, program);
    const char* cur_pos = tree->buf;
    Tokenizator_t* tok = SelectTokens(&cur_pos);

    tree = ParseTree(tree);
    Frontend(tree->root, ast_file);
    DUMP(tree, NOTHING, "Program");

    SelectTreeVars(tree->root, tree);

    Tree_t* ast = GetTreeAST(ast_file);
    DUMP(ast, NOTHING, "AST");

    FILE* program_file = fopen(assem_file, "w");
    Backend(ast->root, program_file, ast);
    fprintf(program_file,   "PUSH 0\n"
                            "POPR RFX\n"
                            "PUSHM [RFX]\n"
                            "OUT\n"
                            "HLT\n");
    fclose(program_file);

    assembler_t assem = {};
    assem.text_file = assem_file;
    assem.commands_file = ex_file;
    assem.listing_file = listing_file;

    assembler(&assem);
    
    processor_t* proc = make_processor(ex_file);
    LoadFile(proc);

    execution(proc);
}
