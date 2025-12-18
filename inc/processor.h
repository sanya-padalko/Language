#ifndef _PROC_H_
#define _PROC_H_

#define printerr(...) fprintf(stderr, __VA_ARGS__)

#define line_info __FILE__, __FUNCTION__, __LINE__

#define my_assert(comp, code_err, ret_value) \
    if (comp == 0) { \
        PrintErr(code_err, line_info); \
        return ret_value; \
    }

#define RESET_COLOR  "\033[0m"
#define RED_COLOR    "\033[31m"
#define GREEN_COLOR  "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define BLUE_COLOR   "\033[36m" 

void PrintErr(CodeError_t error_type, const char* file_name, const char* func_name, const int line_ind);

VarInfo VarInfoCtor(const char* object_name, const char* file_name, const char* func_name, const int line_ind);

CodeError_t PrintVarInfo(const VarInfo varinfo);

typedef int StackElem_t;

struct stack_t {
    StackElem_t* data = NULL;

    unsigned long hash = 0;

    ssize_t size      = 0;
    ssize_t capacity  = 0;
};

unsigned long calc_hash(stack_t *stack);
int get_size(stack_t *stack);
CodeError_t fill_poizon(stack_t *stack, int left, int right);
static int StackRealloc(stack_t *stack, ssize_t new_size);

const StackElem_t POIZON_VALUE = 146988;
const StackElem_t CANARY_LEFT  = 52954; // CEDA
const StackElem_t CANARY_RIGHT = 65242; // FEDA

#ifdef DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

#define make_stack(capacity) StackCtor(capacity ON_DEBUG(, VarInfoCtor("stack", line_info)))

#define stackdump(name) StackDump(name, VarInfoCtor(#name, line_info));

#define stackverify(stack)  CodeError_t code_error = StackVerify(stack); \
                            if (code_error != NOTHING) { \
                                ON_DEBUG(stackdump(stack)); \
                                PrintErr(code_error, line_info); \
                            }

const long mod = 998244353;

const ssize_t MaxCapacity = 1000000;
const int MaxPrintedCount = 15;

const int ExpandMn =  2;
const int CheckMn  =  4;
const int NarrowMn =  3;
const int BadSize  = -1;
const int BaseStackSize = 1;

int get_size(stack_t *stack);

stack_t* StackCtor(ssize_t capacity ON_DEBUG(, VarInfo varinfo));

CodeError_t StackDtor(stack_t *stack);

CodeError_t StackPush(stack_t *stack, StackElem_t new_value);

StackElem_t StackPop(stack_t *stack);

CodeError_t StackVerify(stack_t* stack);

void StackDump(stack_t *stack, VarInfo varinfo);

struct processor_t;

StackElem_t calc_sqrt(StackElem_t value);
StackElem_t pow(StackElem_t a, StackElem_t b);

CodeError_t ProcAdd(processor_t *proc);

CodeError_t ProcSub(processor_t *proc);

CodeError_t ProcMul(processor_t *proc);

CodeError_t ProcDiv(processor_t *proc);

CodeError_t ProcSqrt(processor_t *proc);

CodeError_t ProcPow(processor_t *proc);

CodeError_t ProcOut(processor_t *proc);

CodeError_t ProcTop(processor_t *proc);

CodeError_t ProcIn(processor_t *proc);

struct Text {
    const char *file_name = "";
    int file_size = 0;
    
    char* buf = 0;
};

Text* TextCtor(const char* file_name);

int GetFileSize(const char* file_name);

CodeError_t ReadFile(Text* text);

#define Number (1 << 0)
#define Label  (1 << 1)
#define Reg    (1 << 2)
#define Mem    (1 << 3)

#define make_processor(code_file) ProcCtor(code_file ON_DEBUG(, VarInfoCtor("processor", line_info)))

#define procdump(name) ProcDump(name, VarInfoCtor(#name, line_info))

const size_t RegsCount = 8;
const size_t RamSize = 40000;
const int MaxOperationSize = 20;

struct processor_t {
    stack_t *stack = NULL;
    stack_t *stack_ret = NULL;

    Text *code = NULL;

    int *bytecode = {};

    int ic = 0;
    int cmd_cnt = 0;
    
    int ram[RamSize] = {};
    StackElem_t regs[RegsCount] = {0};
};

struct operation_t {
    const char* name;

    size_t hash;

    int code;

    CodeError_t (*func) (processor_t*);

    int args;
};

enum ProcOper {
    PUSH  ,
    POP   ,
    TOP   ,
    IN    ,
    OUT   ,

    ADD   ,
    SUB   ,
    MUL   ,
    DIV   ,
    SQRT  ,
    POW   ,

    JMP   ,
    JB    ,
    JBE   ,
    JA    ,
    JAE   ,
    JE    ,
    JNE   ,
    CALL  ,
    RET   ,
    
    PUSHM ,
    POPM  ,
    DRAW  ,

    PUSHR , 
    POPR  ,
    
    HLT   ,

    OPER_COUNT
};

processor_t* ProcCtor(const char* code_file ON_DEBUG(, VarInfo varinfo));
CodeError_t ProcDtor(processor_t* proc);
CodeError_t ProcVerify(processor_t* proc);
void ProcDump(processor_t* proc, VarInfo varinfo);
size_t StringHash(const char* str);

CodeError_t ProcStackPush(processor_t* proc);
CodeError_t ProcStackPop(processor_t* proc);
CodeError_t ProcStackWork(processor_t* proc);
CodeError_t ProcCall(processor_t* proc);
CodeError_t ProcRet(processor_t* proc);
CodeError_t ProcPushReg(processor_t* proc);
CodeError_t ProcPopReg(processor_t* proc);
CodeError_t ProcPushRam(processor_t* proc);
CodeError_t ProcPopRam(processor_t* proc);
CodeError_t ProcJmp(processor_t* proc);
CodeError_t ProcCmpJump(processor_t* proc);
CodeError_t ProcDraw(processor_t* proc);

CodeError_t ParsingFile(processor_t *proc);
CodeError_t LoadFile(processor_t *proc);
CodeError_t execution(processor_t *proc);

const operation_t operations[] = {
    {  .name = "PUSH"  ,  .hash =  StringHash("PUSH")   ,  .code =  PUSH  ,  .func = ProcStackPush,  .args = 0 + Number  },
    {  .name = "POP"   ,  .hash =  StringHash("POP")    ,  .code =   POP  ,  .func =  ProcStackPop,  .args = 0           },
    {  .name = "TOP"   ,  .hash =  StringHash("TOP")    ,  .code =   TOP  ,  .func =       ProcTop,  .args = 0           },
    {  .name = "IN"    ,  .hash =  StringHash("IN")     ,  .code =    IN  ,  .func =        ProcIn,  .args = 0           },
    {  .name = "OUT"   ,  .hash =  StringHash("OUT")    ,  .code =   OUT  ,  .func =       ProcOut,  .args = 0           },

    {  .name = "ADD"   ,  .hash =  StringHash("ADD")    ,  .code =   ADD  ,  .func =       ProcAdd,  .args = 0           },
    {  .name = "SUB"   ,  .hash =  StringHash("SUB")    ,  .code =   SUB  ,  .func =       ProcSub,  .args = 0           },
    {  .name = "MUL"   ,  .hash =  StringHash("MUL")    ,  .code =   MUL  ,  .func =       ProcMul,  .args = 0           },
    {  .name = "DIV"   ,  .hash =  StringHash("DIV")    ,  .code =   DIV  ,  .func =       ProcDiv,  .args = 0           },
    {  .name = "SQRT"  ,  .hash =  StringHash("SQRT")   ,  .code =  SQRT  ,  .func =      ProcSqrt,  .args = 0           },
    {  .name = "POW"   ,  .hash =  StringHash("POW")    ,  .code =   POW  ,  .func =       ProcPow,  .args = 0           },

    {  .name = "JMP"   ,  .hash =  StringHash("JMP")    ,  .code =   JMP  ,  .func =       ProcJmp,  .args = 0 + Label   },
    {  .name = "JB"    ,  .hash =  StringHash("JB")     ,  .code =    JB  ,  .func =   ProcCmpJump,  .args = 0 + Label   },
    {  .name = "JBE"   ,  .hash =  StringHash("JBE")    ,  .code =   JBE  ,  .func =   ProcCmpJump,  .args = 0 + Label   },
    {  .name = "JA"    ,  .hash =  StringHash("JA")     ,  .code =    JA  ,  .func =   ProcCmpJump,  .args = 0 + Label   },
    {  .name = "JAE"   ,  .hash =  StringHash("JAE")    ,  .code =   JAE  ,  .func =   ProcCmpJump,  .args = 0 + Label   },
    {  .name = "JE"    ,  .hash =  StringHash("JE")     ,  .code =    JE  ,  .func =   ProcCmpJump,  .args = 0 + Label   },
    {  .name = "JNE"   ,  .hash =  StringHash("JNE")    ,  .code =   JNE  ,  .func =   ProcCmpJump,  .args = 0 + Label   },
    {  .name = "CALL"  ,  .hash =  StringHash("CALL")   ,  .code =  CALL  ,  .func =      ProcCall,  .args = 0 + Label   },
    {  .name = "RET"   ,  .hash =  StringHash("RET")    ,  .code =   RET  ,  .func =       ProcRet,  .args = 0           },

    {  .name = "PUSHM" ,  .hash =  StringHash("PUSHM")  ,  .code = PUSHM  ,  .func =   ProcPushRam,  .args = 0 + Mem     },
    {  .name = "POPM"  ,  .hash =  StringHash("POPM")   ,  .code =  POPM  ,  .func =    ProcPopRam,  .args = 0 + Mem     },
    {  .name = "DRAW"  ,  .hash =  StringHash("DRAW")   ,  .code =  DRAW  ,  .func =      ProcDraw,  .args = 0           },

    {  .name = "PUSHR" ,  .hash =  StringHash("PUSHR")  ,  .code = PUSHR  ,  .func =   ProcPushReg,  .args = 0 + Reg     },
    {  .name = "POPR"  ,  .hash =  StringHash("POPR")   ,  .code =  POPR  ,  .func =    ProcPopReg,  .args = 0 + Reg     },

    {  .name = "HLT"   ,  .hash =  StringHash("HLT")    ,  .code =   HLT  ,  .func = ProcStackPush,  .args = 0           }
};

const int NumberCnt = 10;
const int AlphaCnt = 26;
const int FirstPass = 1;
const int SecondPass = 2;
const int LabelCount = 200;

struct label_t {
    int ic = -1;

    size_t hash = 0;
};

struct reg_t {
    const char* name;

    size_t hash;
};

struct assembler_t {
    const char* text_file = NULL;
    const char* commands_file = NULL;

    const char* listing_file = NULL;
    FILE* listing = NULL;

    Text* program = NULL;
    char* buf = NULL;
    char* ex_ptr = NULL;

    label_t* labels = NULL;
    int label_cnt = 0;

    int ic = 0;
};

enum Comparison {
    LESS  = -1,
    EQUAL =  0,
    ABOVE =  1,
};

const reg_t available_regs[RegsCount] = {{"RAX"}, {"RBX"}, {"RCX"}, {"RDX"}, {"REX"}, {"RFX"}, {"RGX"}, {"RHX"}};

CodeError_t assembler(assembler_t* assem);
CodeError_t PassingCode(assembler_t* assem, int pass_num);
CodeError_t ReadCodeFile(assembler_t* assem);
CodeError_t WriteToExFile(assembler_t* assem, const char* buf);
CodeError_t ZeroOper(char* operation);
CodeError_t ParseOper(assembler_t* assem, const operation_t* operation, int pass_num);
CodeError_t ParseNumber(assembler_t* assem, const int* value);
CodeError_t ParseLabel(assembler_t* assem, const char* label);
CodeError_t ParseMem(assembler_t* assem, const char* reg);
CodeError_t ParseString(assembler_t* assem, const char* str, int pass_num);
CodeError_t PrintNumber(assembler_t* assem, const int value);
CodeError_t AddLabel(assembler_t* assem, const char* str, const int ic);

int CheckHash(assembler_t *assem, size_t hash);
int GetLabel(assembler_t* assem, const char* str);
size_t GetLabelHash(label_t* label);
int GetLabelIc(label_t* label);
bool CheckLabelsHash(assembler_t* assem);

int RegComp(const void* param1, const void* param2);
int LabelComp(const void* param1, const void* param2);
CodeError_t CalcRegsHash();
bool CheckReg(const char* reg);

#endif