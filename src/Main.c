/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

#include <stdio.h>
#include <errno.h>
#define extern_
#include <Data.h>
#undef extern_
#include <ASTNode.h>

/**
 * 
 * Elisa Assembler Prototype v1
 * 
 * Experiment purpose:
 *  Determine whether the Elisa system is suitable for modelling an average computer
 * 
 *  General control flow:
 *   - Input (Likely the output of Erythro will be passed here)
 *      (in the form of a file, usually in /tmp/ or some similar platform-specific location)
 *     
 *     There are four input stages:
 *      - READING META
 *      - READING GLOBALS
 *      - (start loop) READ FUNCTION
 *      - READ FUNCTION CONTENTS (jump to loop)
 * 
 *   - Parsing (Read in the file (which means this contains lexing) and extract the context)
 *   - Optimisation
 *      (this is the only stage in the compiler toolchain that applies optimisations to the code)
 *   - Parsing again
 *      (we know that the output of optimisation is probably correct, but applying those changes in-tree
 *          is a massive pain, so we'll have to re-lex and re-parse the file)
 *   - Output
 *      (generated in a platform specific way - PECOFF32+, PECOFF32, ELF, ELF64, or Mach-O)

 * 
 * The lexer and parser system will be taken from Erythro, as this is literally just a prototype.
 * 
 */


/**
 * Exit code 1 = Error opening input file
 * Exit code 2 = Error opening output file
 * Exit code 3 = Unrecognized word
 * 
 */

int main(int argc, char* argv[]) {

    if((SourceFile = fopen(argv[1], "r")) == NULL) {
        printf("Reading %s generated error %s\r\n", argv[1], strerror(errno));
        exit(1);
    }

    if((OutputFile = fopen(argv[2], "w")) == NULL) {
        printf("Opening %s for writing generated error %s\r\n", argv[2], sterror(errno));
        exit(2);
    }

    struct ASTNode* ParseTree;

    ParseMeta(&ParseTree);
    ParseGlobals(&ParseTree);
    ParseFunctions(&ParseTree);

    OptimiseTree(&ParseTree);

    Link(&ParseTree);

    fclose(SourceFile);
    fclose(OutputFile);

    exit(0);

}