/*************/
/*GEMWIRE    */
/*    ERYTHRO*/
/*************/

#pragma once
#include <stdio.h>
#include <Lexer.h>
#include <Meta.h>

#ifndef extern_
#define extern_ extern
#endif

#define TEXTLEN 512
#define SYMBOLS 1024

extern_ int CurrentFunction;
extern_ int Line;
extern_ int Overread;

extern_ FILE* SourceFile;
extern_ FILE* OutputFile;

extern_ struct Token CurrentToken;
extern_ char CurrentIdentifier[TEXTLEN + 1];

extern_ State CurrentState;
