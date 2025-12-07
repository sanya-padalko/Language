#include "folders.h"
#include "tree.h"
#include "parsing.h"

int main() {
    const char* real_data = GetTime();
    MakeDir(real_data);

    const char* program = "../program.txt";

    Tree_t* tree = TreeCtor();
    ReadBase(tree, program);
    const char* cur_pos = tree->buf;
    tree->root = GetG(&cur_pos);
    DUMP(tree, NOTHING, "Program");
}
