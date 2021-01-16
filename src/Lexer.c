
/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

#include <Lexer.h>
#include <Data.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * *    C H A R       S T R E AM     * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void ReturnCharToStream(int Char) {
    Overread = Char;
}

static int NextChar(void) {
    int Char;

    if(Overread) {
        Char = Overread;
        Overread = 0;
        return Char;
    }

    Char = fgetc(SourceFile);

    if(Char == '\n')
        Line++;
    
    return Char;
}


static int FindChar() {
    int Char;

    Char = NextChar();

    while(Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r') {
        Char = NextChar();
    }

    return Char;
}

static int FindDigitFromPos(char* String, char Char) {
    char* Result = strchr(String, Char);
    return(Result ? Result - String : -1);
}

void VerifyToken(int Type, char* TokenExpected) {
    if(CurrentToken.type == Type)
        Tokenise(&CurrentToken);
    else {
        printf("Expected %s on line %d\n", TokenExpected, Line);
        exit(1);
    }
}

static struct Token* RejectedToken = NULL;

void RejectToken(struct Token* Token) {
    if(RejectedToken != NULL)
        Die("Cannot reject two tokens in a row!");
    
    RejectedToken = Token;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * *     L I T E R A L S   A N D   I D E N T I F I E R S     * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int ReadInteger(int Char) {
    int CurrentChar = 0;
    int IntegerValue = 0;

    while((CurrentChar = FindDigitFromPos("0123456789", Char)) >= 0) {
        IntegerValue = IntegerValue * 10 + CurrentChar;
        Char = NextChar();
    }

    ReturnCharToStream(Char);

    return IntegerValue;
}

// Variable identifier, keyword, function.
static int ReadWord(int Char, char* Buffer, int Limit) {
    int ind = 0;   

    // This defines the valid chars in a keyword/variable/function.
    while(isalpha(Char) || isdigit(Char) || Char == '_') {
        if (ind >= Limit - 1) {
            printf("Identifier too long: %d\n", Line);
            exit(1);
        } else {
            Buffer[ind++] = Char;
        }

        Char = NextChar();
    }

    // At this point, we've reached a non-keyword character
    ReturnCharToStream(Char);
    Buffer[ind] = '\0';
    return ind;
}

static int ReadCharLiteral() {
    int Char;
    Char = NextChar();
    if(Char == '\\') {
        switch(Char = NextChar()) {
            case 'a': return '\a';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            case '\\': return '\\';
            case '"': return '"';
            case '\'': return '\'';
            default:
                DieChar("Unknown Escape: ", Char);
        }
    }

    return Char;
}


static int ReadStringLiteral(char* Buffer) {
    int Char;

    for(int i = 0; i < TEXTLEN - 1; i++) {
        if((Char = ReadCharLiteral()) == '"') {
            Buffer[i] = 0; return i;
        }
        
        Buffer[i] = Char;
    }

    Die("String Literal Too Long");
    return 0;
}

/*
 * This function is what defines the valid keywords for the language
 * //TODO: move this to a static list?
 * //TODO: More optimisations?
 * 
 */
static int ReadKeyword(char* Str) {
    // First, scan with reference intact.
    switch(*Str) {
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

        case 'A':
            // AND ADD
            if (!strcmp(Str, "ADD"))
                return AR_ADD;
            else if (!strcmp(Str, "AND"))
                return BIT_AND;
            break;
        case 'C':
            // CALL COMPARE-EQUALITY COMPARE-GREATER-THAN COMPARE-LESS-THAN
            if (!strcmp(Str, "CALL"))
                return META_CALL;

            if (!strcmp( Str, "COMPARE-EQUALITY"))
                return COMP_EQUAL;

            if (!strcmp( Str, "COMPARE-GREATER-THAN"))
                return COMP_GREATER;

            if (!strcmp(Str, "COMPARE-LESS-THAN"))
                return COMP_LESS;

            break;
        case 'D':
            // DIVIDE DATA DEBUG DEREF
            if (!strcmp(Str, "DATA"))
                return META_DATA;

            if (!strcmp(Str, "DEBUG"))
                return META_DEBUG;

            if (!strcmp(Str, "DEREF"))
                return MEM_DEREF;

            if (!strcmp(Str, "DIVIDE"))
                return AR_DIVIDE;

            break;
        case 'F':
            // FUNCTION
            if (!strcmp(Str, "FUNCTION"))
                return META_FUNCTION;
            break;

        case 'J':
            // JUMP JUMP-IF-EQUAL JUMP-IF-NOT-EQUAL JUMP-IF-ERROR
            if (!strcmp(Str, "JUMP"))
                return JUMP_UNCOND;
            
            if (!strcmp(Str, "JUMP-IF-EQUAL"))
                return JUMP_EQUAL;
            
            if (!strcmp(Str, "JUMP-IF-NOT-EQUAL"))
                return JUMP_INEQUAL;
        
            if (!strcmp(Str, "JUMP-IF-ERROR"))
                return JUMP_ERROR;
            
            break;
        
        case 'L':
            // LABEL
            if (!strcmp(Str, "LABEL"))
                return META_LABEL;
            break;
        case 'M':
            // META MULTIPLY
            if (!strcmp(Str, "META"))
                return META_KWD;
            
            if (!strcmp(Str, "MULTIPLY"))
                return AR_MULTIPLY;
            
            break;
        case 'N':
            // NOT
            if (!strcmp(Str, "NOT"))
                return BIT_NOT;
            break;
        case 'O':
            // OR
            if (!strcmp(Str, "OR"))
                return BIT_OR;
            break;

        case 'P':
            // POINTERTO
            if (!strcmp(Str, "POINTERTO"))
                return MEM_POINTER;
            break;
        
        case 'R':
            // RETURN
            if (!strcmp(Str, "RETURN"))
                return META_RETURN;
            break;
        case 'S':
            // STORE SUBTRACT
            if (!strcmp(Str, "STORE"))
                return REG_STORE;

            if (!strcmp(Str, "SUBTRACT"))
                return AR_SUBTRACT;

            break;
        case 'W':
            // WRITE
            if (!srcmp(Str, "WRITE"))
                return MEM_WRITE;
            break;
        case 'X':
            // XOR
            if (!strcmp(Str, "XOR"))
                return BIT_XOR;
            break;
        
    }

    return 0;
}

/* * * * * * * * * * * * * * * * * * * * *
 * * * *      T O K E N I S E R    * * * *
 * * * * * * * * * * * * * * * * * * * * */

int Tokenise(struct Token* Token) {
    int Char, TokenType;

    if(RejectedToken != NULL) {
        Token = RejectedToken;
        RejectedToken = NULL;
        return 1;
    }

    Char = FindChar();

    switch(Char) {
        case EOF:
            Token->type = LI_EOF;
            return 0;

        // Literals can be:
        // keywords (STORE)
        // execute (->)
        // register (r1)
        // declits ($15) / (0)
        // hexlits (x15)
        // names (PrintInteger)
        // type (:i8)
        

        // if ->, expect outputs
        case '-':
            Char = NextChar();
            if(Char == '>') 
                Token->type = LI_OUTPUTS;
            else 
                ReturnCharToStream(Char);
            
            break;

        // if r<x>, encode x
        case 'r':
            Char = NextChar();
            if(isdigit(Char)) {
                Token->type = LI_REG;
                Token->value = ReadInteger(Char);
            } else {
                ReturnCharToStream(Char);
            }
            break;
        
        // if $<x>, delegate to int
        case '$':
            Char = NextChar();
            if(isDigit(Char)) {
                Token->type = LI_INT;
                Token->value = ReadInteger(Char);
            } else {
                ReturnCharToStream(Char);
            }
            break;
        
        // if x<y>, delegate to hex
        case 'x':
            Char = NextChar();
            if(isDigit(Char)) {
                Token->type = LI_HEX;
                Token->value = ReadInteger(Char);
            } else {
                ReturnCharToStream(Char);
            }
            break;

        // if :i/u<x>, implement strict typing
        // TODO
        
        default:
            if(isdigit(Char)) {
                Token->value = ReadInteger(Char);
                Token->type = LI_INT;
                break;
            } else if(isalpha(Char)) {
                ReadWord(Char, CurrentIdentifier, TEXTLEN);

                if(TokenType = ReadKeyword(CurrentIdentifier)) {
                    Token->type = TokenType;
                    break;
                }
                printf("Line %d: Unrecognized symbol %s\n", CurrentIdentifier, Line);
                exit(3);
            }
            DieChar("Unrecognized character", Char);

    }

    return 1;
}

