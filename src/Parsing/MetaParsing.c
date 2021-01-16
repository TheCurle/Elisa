/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

#include <Data.h>
#include <ASTNode.h>
#include <Lexer.h>

/**
 * 
 * Elisa Interpreter Prototype v1
 * 
 * Meta Parsing Functions
 * 
 * As per the Elisa Specification v3.5, the META keyword
 *  is to be used to convey extra information to the interpreter
 *   about how the program is to be treated.
 * 
 * It also serves to allow some of the parsing work to be
 *  saved from the compiler - so that things like the maximum word
 *   size and the amount of registers used can be pre-determined.
 * 
 * Specifically, the META keyword comes in 3 parts;
 *  - KEYWORD PROFILE DATA
 *  The KEYWORD here, is obviously META.
 *  The PROFILE can be any of the elements in the State structure.
 *   The State Structure takes its form from the Elisa Specification.
 *  The DATA can be anything, so long as the Specification says it is
 *   a valid input.
 *   For example, the BITNESS profile takes any valid integer.
 *    The TRACKING profile takes any pair of integers.
 * 
 * Note that the parsing of this metadata does not affect the AST tree.
 * 
 */


void ParseMeta(struct ASTNode* Tree) {

}