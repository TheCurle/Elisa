/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

/*
 * All Syntax Operations are prefixed OP.
 *  Terminal Operations are prefixed TERM.
 *  L-Values are prefixed LV.
 *  Reference Operations are prefixed REF.
 *
 * These represent the actions that a token will perform.
 * These are used exclusively in AST construction.
 * 
 * It is important that Tokens and Operations are logically separated,
 *  but that the Operation's index is the same as the Token that invokes it.
 */

enum SyntaxOps {
    OP_ASSIGN = 1,      // Assign an l-value

    OP_BOOLOR,          // Boolean OR two statements
    OP_BOOLAND,         // Boolean AND two statements
    OP_BITOR,           // Bitwise OR a number
    OP_BITXOR,          // Bitwise XOR a number
    OP_BITAND,          // Bitwise AND a number
    
    OP_EQUAL,           // Compare equality
    OP_INEQ,            // Compare inequality
    OP_LESS,            // Less than?
    OP_GREAT,           // Greater than?
    OP_LESSE,           // Less than or Equal to?
    OP_GREATE,          // Greater than or Equal to?

    OP_ADD,             // Add two numbers.
    OP_SUBTRACT,        // Subtract two numbers.
    OP_MULTIPLY,        // Multiply two numbers.
    OP_DIVIDE,          // Divide two numbers.

    OP_NEGATE,          // Negate a number (turn a positive number negative)

    OP_BOOLCONV,        // Convert an expression to a boolean.
    
    OP_ADDRESS,         // Fetch the address of a var
    OP_DEREF,           // Get the value of the address in a pointer

    TERM_INTLITERAL,    // Integer Literal. This is a virtual operation, so it's a terminal.
    TERM_STRLITERAL,    // String Literal. Also terminal.

    REF_IDENT,          // Reference (read) an identifier (variable).

    OP_WIDEN,           // Something contains a type that needs to be casted up
    OP_SCALE,           // We have a pointer that needs to be scaled!

    OP_CALL,            // Call a function
    OP_RET,             // Return from a function

    OP_COMP,            // Compound statements need a way to be "glued" together. This is one of those mechanisms
    OP_IF,              // If statement
    OP_LOOP,            // FOR, WHILE
    OP_PRINT,           // Print statement

    OP_FUNC,            // Define a function
};
