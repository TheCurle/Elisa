/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

#include <stddef.h>

// A node in a Binary Tree that forms the syntax of Erythro
struct ASTNode {
    int Operation;      // SyntaxOps Index
    int ExprType;       // Value->IntValue's DataType
    int RVal;           // True if this node is an Rval, false if Lval
    struct ASTNode* Left;
    struct ASTNode* Middle;
    struct ASTNode* Right;
    union {
        size_t IntValue; // TERM_INTLIT's Value
        size_t Address;  // Function's address
    } Value;
};


struct ASTNode* ConstructASTNode(int Operation, int Type,
                                 struct ASTNode* Left,
                                 struct ASTNode* Middle,
                                 struct ASTNode* Right,
                                 int IntValue);

struct ASTNode* ConstructASTLeaf(int Operation, int Type, int IntValue);

struct ASTNode* ConstructASTBranch(int Operation, int Type, struct ASTNode* Left, int IntValue);
