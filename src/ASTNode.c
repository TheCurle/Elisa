/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

#include <stdio.h>
#include <stdlib.h>
#include <Defs.h>
#include <Data.h>

struct ASTNode* ConstructASTNode(int Operation, int Type,
                                 struct ASTNode* Left,
                                 struct ASTNode* Middle,
                                 struct ASTNode* Right,
                                 int IntValue) {
    struct ASTNode* Node;

    Node = (struct ASTNode*) malloc(sizeof(struct ASTNode));

    if(!Node) {
        fprintf(stderr, "Unable to allocate node!");
        exit(1);
    }

    Node->Operation = Operation;
    Node->ExprType = Type;
    Node->Left = Left;
    Node->Middle = Middle;
    Node->Right = Right;
    Node->Value.IntValue = IntValue;

    return Node;
}


struct ASTNode* ConstructASTLeaf(int Operation, int Type, int IntValue) {
    return ConstructASTNode(Operation, Type, NULL, NULL, NULL, IntValue);
}

struct ASTNode* ConstructASTBranch(int Operation, int Type, struct ASTNode* Left, int IntValue) {
    return ConstructASTNode(Operation, Type, Left, NULL, NULL, IntValue);
}