/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 1,
     THEN = 2,
     ELSE = 3,
     FI = 4,
     FOR = 5,
     TO = 6,
     DOWNTO = 7,
     IN = 8,
     ROW = 9,
     WHILE = 10,
     DO = 11,
     OD = 12,
     LOC = 13,
     RETURN = 14,
     BREAK = 15,
     MAKE = 16,
     APPLY = 17,
     ON = 20,
     OFF = 21,
     SETDEFAULT = 22,
     SAVESTATE = 23,
     RESTORESTATE = 24,
     TYPEID = 25,
     HELP = 31,
     WHATTYPE = 32,
     EDIT = 33,
     READ = 34,
     WRITE = 35,
     MONFIL = 36,
     LEARN = 37,
     EXEC = 38,
     QUIT = 39,
     LISTCOMMAND = 41,
     MAXNODES = 44,
     MAXOBJECTS = 45,
     WRITEHELP = 18,
     APPENDHELP = 19,
     INT = 26,
     NUMBER = 27,
     GROUPID = 28,
     IDENT = 29,
     DOLLAR = 30,
     TEXT = 40,
     ANY_STRING = 42,
     ENTER = 43,
     SEMICOLON = 46,
     COMMA = 47,
     BAR = 48,
     COLON = 49,
     LPAR = 50,
     RPAR = 51,
     LBRACE = 52,
     RBRACE = 53,
     LBRACK = 54,
     RBRACK = 55,
     OR = 56,
     AND = 57,
     NOT = 58,
     TIMES = 59,
     POWER = 60,
     X = 61,
     BECOMES = 62,
     PLUSAB = 63,
     RELOP = 64,
     ADDOP = 65,
     DIVOP = 66
   };
#endif
/* Tokens.  */
#define IF 1
#define THEN 2
#define ELSE 3
#define FI 4
#define FOR 5
#define TO 6
#define DOWNTO 7
#define IN 8
#define ROW 9
#define WHILE 10
#define DO 11
#define OD 12
#define LOC 13
#define RETURN 14
#define BREAK 15
#define MAKE 16
#define APPLY 17
#define ON 20
#define OFF 21
#define SETDEFAULT 22
#define SAVESTATE 23
#define RESTORESTATE 24
#define TYPEID 25
#define HELP 31
#define WHATTYPE 32
#define EDIT 33
#define READ 34
#define WRITE 35
#define MONFIL 36
#define LEARN 37
#define EXEC 38
#define QUIT 39
#define LISTCOMMAND 41
#define MAXNODES 44
#define MAXOBJECTS 45
#define WRITEHELP 18
#define APPENDHELP 19
#define INT 26
#define NUMBER 27
#define GROUPID 28
#define IDENT 29
#define DOLLAR 30
#define TEXT 40
#define ANY_STRING 42
#define ENTER 43
#define SEMICOLON 46
#define COMMA 47
#define BAR 48
#define COLON 49
#define LPAR 50
#define RPAR 51
#define LBRACE 52
#define RBRACE 53
#define LBRACK 54
#define RBRACK 55
#define OR 56
#define AND 57
#define NOT 58
#define TIMES 59
#define POWER 60
#define X 61
#define BECOMES 62
#define PLUSAB 63
#define RELOP 64
#define ADDOP 65
#define DIVOP 66




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 7 "parser.y"
{
	  short sub;
	  long val;
	  bigint* num;
	  symblst sym;
	  char* text;
	}
/* Line 1529 of yacc.c.  */
#line 193 "parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

