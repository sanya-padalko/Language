#include "dump.h"
#include "tree.h"

int dump_counter = 0;

CodeError_t Dump(Tree_t* tree, VarInfo varinfo) {
    my_assert(tree, NULLPTR, NULLPTR);

    FILE* dump_file = fopen(tree->html_file_name, "a");
    fprintf(dump_file, "<h2 style = \"color: blue\"> %s </h2>\n", varinfo.add_info);
    const char* color = "green";
    if (varinfo.error_code != NOTHING) {
        fprintf(dump_file, "<h2 style = \"color: red\"> ERROR: %s</h2>\n", ErrorType(varinfo.error_code));
        color = "red";
    }

    fprintf(dump_file, "<h2 style=\"color: %s\"> TreeDump called from %s, function: %s, line %d, tree name: %s</h2>\n", color, varinfo.file_name, varinfo.func_name, varinfo.line, varinfo.name);

    if (tree->root == NULL) {
        fprintf(dump_file, "<h3 style=\"color: red\">Root of tree pointer equal NULL</h3>\n");
        return NULLPTR;
    }

    TreeImgDump(tree->dot_file_name, tree->root);

    char img_path[100] = {};
    sprintf(img_path, "result%d.svg", dump_counter - 1);
    fprintf(dump_file, "<img src=\"%s\">\n", img_path);
    fprintf(dump_file, "<hr>\n");
    fclose(dump_file);

    return NOTHING;
}

void TreeImgDump(const char* dot_file_name, Node_t* root) {
    if (!root || !dot_file_name)
        return;

    FILE* dot_file = fopen(dot_file_name, "w");
    fprintf(dot_file, "digraph G {\n");

    RecDump(root, dot_file);

    fprintf(dot_file, "}");
    fclose(dot_file);

    char dot_str[100] = {};
    sprintf(dot_str, "\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" %s -T svg -o result%d.svg", dot_file_name, dump_counter++);
    system(dot_str);
}

void RecDump(Node_t* root, FILE* dot_file) {
    unsigned int color = (unsigned int)CalcHash((long long)root);
    fprintf(dot_file, "\tNode%llX[shape = Mrecord, style = \"filled\", fillcolor = \"#%06x\"", (ull)root, color);
    fprintf(dot_file, ", label = <\n\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"5\">\n");
    fprintf(dot_file, "\t\t<TR>\n\t\t\t<TD> ptr: 0x%llX </TD>\n\t\t</TR>\n", (ull)root);
    
    fprintf(dot_file, "\n\t\t<TR>\n\t\t\t<TD> ");
    if (root->type == OPER) {
        fprintf(dot_file, "OPER: ");
        fprintf(dot_file, "%s", opers[root->value->type].dump_view);
    } 
    else if (root->type == NUM) {
        fprintf(dot_file, "NUM: ");
        fprintf(dot_file, "%lg", root->value->value);
    }
    else {
        fprintf(dot_file, "VAR: ");
        fprintf(dot_file, "\"%s\"", root->value->name);
    }
    fprintf(dot_file, " </TD>\n\t\t</TR>\n\n");
    
    PrintDotPtr(dot_file, root->left, "left");
    PrintDotPtr(dot_file, root->right, "right");

    fprintf(dot_file, "\t</TABLE>>];\n\n");
    if (root->left) {
        RecDump(root->left, dot_file);
        fprintf(dot_file, "\tNode%llX->Node%llX;\n", (ull)root, (ull)root->left);
    }
    if (root->right) {
        RecDump(root->right, dot_file);
        fprintf(dot_file, "\tNode%llX->Node%llX;\n", (ull)root, (ull)root->right);
    }
}

int CalcHash(long long p) {
    int c = (int)p;

    c = (c ^ (c >> 16)) * 0xC4CEB9FE;
    c = (c ^ (c >> 13)) * 0xFF51AFD7;
    c = c ^ (c >> 16);

    return (c >> 8) | 0x00808080;
}

int DarkCalcHash(long long p) {
    int c = (int)p;

    c = (c ^ (c >> 16)) * 0xC4CEB9FE;
    c = (c ^ (c >> 13)) * 0xFF51AFD7;
    c = c ^ (c >> 16);

    return (c >> 8) | 0x9F9F9F9F;
}

void PrintDotPtr(FILE* dot_file, Node_t* node, const char* name) {
    unsigned int color = (unsigned int)CalcHash((long long)node);
    fprintf(dot_file, "\t\t<TR>\n\t\t\t<TD BGCOLOR = \"#%06x\"> %s: ", color, name);

    if (node) 
        fprintf(dot_file, "0x%llX", (ull)node);
    else
        fprintf(dot_file, "NULL");
    
    fprintf(dot_file, "</TD>\n\t\t</TR>\n");
}