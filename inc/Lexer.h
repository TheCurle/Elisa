/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

/**
 * Elisa v3.5 keyword list:
 * ADD SUBTRACT MULTIPLY DIVIDE
 * AND OR XOR NOT
 * DATA LABEL
 * COMPARE-EQUALITY
 * COMPARE-GREATER-THAN
 * COMPARE-LESS-THAN
 * JUMP-IF-EQUAL
 * JUMP-IF-NOT-EQUAL
 * JUMP-IF-ERROR
 * JUMP FUNCTION RETURN
 * DEREF POINTERTO CALL
 * STORE WRITE DEBUG
 * META
 */
enum TokenTypes {
    LI_EOF,         // EOF
    LI_INT,         // 5 $5
    LI_HEX,         // 0x5 x5
    LI_BAR,         // |
    LI_OUTPUTS,     // ->
    LI_TYPE,        // :
    LI_REG,         // r5

    AR_ADD,         // +
    AR_SUBTRACT,    // -
    AR_MULTIPLY,    // *
    AR_DIVIDE,      // /

    BIT_AND,        // &
    BIT_OR,         // |
    BIT_XOR,        // ^ 
    BIT_NOT,        // !

    META_DATA,      // DATA
    META_LABEL,     // LABEL
    META_FUNCTION,  // FUNCTION 
    META_CALL,      // CALL
    META_RETURN,    // RETURN
    META_DEBUG,     // DEBUG
    META_KWD,       // META

    COMP_EQUAL,     // == 
    COMP_GREATER,   // >
    COMP_LESS,      // <

    JUMP_UNCOND,    // JUMP
    JUMP_EQUAL,     // JUMP-IF-EQUAL
    JUMP_INEQUAL,   // JUMP-IF-NOT-EQUAL
    JUMP_ERROR,     // JUMP-IF-ERROR

    MEM_DEREF,      // DEREF
    MEM_POINTER,    // POINTERTO
    MEM_WRITE,      // WRITE

    REG_STORE,      // STORE
};

struct Token {
    int type;
    int value;
};

int Tokenise(struct Token* Token);

void VerifyToken(int Type, char* TokenExpected);
void RejectToken(struct Token* Token);

static int ReadIdentifier(int Char, char* Buffer, int Limit);
static int ReadKeyword(char* Str);
