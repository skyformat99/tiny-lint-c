/*

  A simple lexer for C-like languages
  ===================================

    Matches:
    --------
      - single- and multi-line comments
      - string literals
      - char literals
      - integer numerals
      - ANSI C89 keywords
      - ANSI C89 operators


    Work-flow:
    ----------
      - define list of reserved keywords + operators
      - read a file
      - try treating input as (in prioritized sequence):
          comment
          whitespace
          pre-processor directive
          escape-characters
          char literal
          string literal
          integer numeral
          operator (arithmeric, logical, bitwise, etc.)
          reserved keyword
          identifier / symbol


  WORK NOTES:
 =============

    - when times comes, to implement support for '#line XX "file_yy.c" ... ' etc.,
      just make the name point into the source code and use the file_path_length
 
    

*/

#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>


#define MAXNTOKENS     1024 /* max number of keywords/lexemes to support - set high enough to accomodate typedef'd types etc. */
#define MAXTOKENLEN   65536 /* max supported token_length - this is the maximum supported token (and string) length. */




/* Token kinds - a kind covers a category of types. */
enum
{
/*
  TOK_COMMENT,
  TOK_PREPROCESSOR,
*/
  TOK_EOF,
  TOK_CONST,
  TOK_OPERATOR,
  TOK_KEYWORD,
  TOK_IDENTIFIER,
};



/* Token types */
enum
{
  OP_ASSIGN_LSH,      /*  0 : <<= */
  OP_ASSIGN_RSH,      /*  1 : >>= */
  OP_LSH,             /*  2 : <<  */
  OP_RSH,             /*  3 : >>  */
  OP_NOT_EQUAL,       /*  4 : !=  */
  OP_EQUAL,           /*  5 : ==  */
  OP_ASSIGN_PLUS,     /*  6 : +=  */
  OP_ASSIGN_MINUS,    /*  7 : -=  */
  OP_ASSIGN_MULTIPLY, /*  8 : *=  */
  OP_ASSIGN_DIVIDE,   /*  9 : /=  */
  OP_ASSIGN_AND,      /* 10 : &=  */
  OP_ASSIGN_OR,       /* 11 : |=  */
  OP_ASSIGN_XOR,      /* 12 : ^=  */
  OP_INCREMENT,       /* 13 : ++  */
  OP_DECREMENT,       /* 14 : --  */
  OP_LOGICAL_AND,     /* 15 : &&  */
  OP_LOGICAL_OR,      /* 16 : ||  */
  OP_ARROW,           /* 17 : ->  */
  OP_LOGICAL_GTE,     /* 18 : >=  */
  OP_LOGICAL_LTE,     /* 19 : <=  */
  OP_LOGICAL_NOT,     /* 20 : !   */
  OP_BITWISE_NOT,     /* 21 : ~   */
  OP_SEMICOLON,       /* 22 : ;   */
  OP_COLON,           /* 23 : :   */
  OP_QUESTIONMARK,    /* 24 : ?   */
  OP_COMMA,           /* 25 : ,   */
  OP_DOT,             /* 26 : .   */
  OP_PLUS,            /* 27 : +   */
  OP_MINUS,           /* 28 : -   */
  OP_MULTIPLY,        /* 29 : *   */
  OP_DIVIDE,          /* 30 : /   */
  OP_LOGICAL_GT,      /* 31 : >   */
  OP_LOGICAL_LT,      /* 32 : <   */
  OP_BITWISE_AND,     /* 33 : &   */
  OP_BITWISE_OR,      /* 34 : |   */
  OP_BITWISE_XOR,     /* 35 : ^   */
  OP_MODULO,          /* 36 : %   */
  OP_ASSIGN,          /* 37 : =   */
  OP_LPAREN,          /* 38 : (   */
  OP_RPAREN,          /* 39 : )   */
  OP_LBRACE,          /* 40 : {   */
  OP_RBRACE,          /* 41 : }   */
  OP_LBRACKET,        /* 42 : [   */
  OP_RBRACKET,        /* 43 : ]   */

  /* The 32 reserved keywords of C: */
  KW_AUTO,            /* 44 : auto     */
  KW_BREAK,           /* 45 : break    */
  KW_CASE,            /* 46 : case     */
  KW_CHAR,            /* 47 : char     */
  KW_CONST,           /* 48 : const    */
  KW_CONTINUE,        /* 49 : continue */
  KW_DEFAULT,         /* 50 : default  */
  KW_DO,              /* 51 : do       */
  KW_DOUBLE,          /* 52 : double   */
  KW_ELSE,            /* 53 : else     */
  KW_ENUM,            /* 54 : enum     */
  KW_EXTERN,          /* 55 : extern   */
  KW_FLOAT,           /* 56 : float    */
  KW_FOR,             /* 57 : for      */
  KW_GOTO,            /* 58 : goto     */
  KW_IF,              /* 59 : if       */
  KW_INT,             /* 60 : int      */
  KW_LONG,            /* 61 : long     */
  KW_REGISTER,        /* 62 : register */
  KW_RETURN,          /* 63 : return   */
  KW_SHORT,           /* 64 : short    */
  KW_SIGNED,          /* 65 : signed   */
  KW_SIZEOF,          /* 66 : sizeof   */
  KW_STATIC,          /* 67 : static   */
  KW_STRUCT,          /* 68 : struct   */
  KW_SWITCH,          /* 69 : switch   */
  KW_TYPEDEF,         /* 70 : typedef  */
  KW_UNION,           /* 71 : union    */
  KW_UNSIGNED,        /* 72 : unsigned */
  KW_VOID,            /* 73 : void     */
  KW_VOLATILE,        /* 74 : volatile */
  KW_WHILE,           /* 75 : while    */

  CNST_CHAR,          /* 76 : const char      */
  CNST_STRING,        /* 77 : string literal  */
  CNST_INT,           /* 78 : integer numeral */
  CNST_FLOAT,         /* 79 : float numeral   */
};




/* Data structure defining tokens/lexemes that we want the lexer to match. */
struct token
{
  char*    symbol;   /* string symbol, e.g. "+" for the plus operator, or "goto" etc. */
  uint32_t symlen;   /* length of above string excluding null-termination. */
  uint32_t tokknd;   /* overall lexeme type differentiator. */
  uint32_t toktyp;   /* e.g. tokknd == TOK_OPERATOR && toktyp == OP_LSH. */
  uint32_t lineno;   /* line in buffer where token was lexed. First line is no. 1. */
  uint32_t byteno;   /* byte offset into line where token was lexed. First byte is no. 0. */
  uint32_t foffset;  /* byte offset into source file, for faster lookup. */
};



/* Lexer context object */
struct lexer 
{
  struct token lexemes[MAXNTOKENS];   /* Set of valid C tokens: operators + keywords (excluding trigraphs). */
  uint32_t nkeywords;                 /* Number of keywords in lexemes[]. */
  char*    buffer;                    /* Pointer to char buffer where tokens are read from (src file). */
  char*    buffer_original;           /* Pointer to start of buffer - 'buffer' points to next lex-point. */
  char     token_buffer[MAXTOKENLEN]; /* Buffer to accumulate chars to build current token from. */
  uint32_t token_length;              /* Length of current token. */
  uint32_t cur_lineno;                /* Line number in current input source code file. */
  uint32_t cur_byteno;                /* Byte/column number in current line. */
  int      continue_on_error;      
};



void lexer_init(struct lexer* l);
void lexer_free(struct lexer* l);
void lexer_reset_state(struct lexer* l);
void lexer_setup_alphabet(struct lexer* l);
void lexer_set_char_buf(struct lexer* l, char* char_buf);
void lexer_add_token(struct lexer* l, char* token_symbol, int token_strlen, int token_kind, int token_type);

struct token lexer_next_token(struct lexer* l);

#endif /* __LEXER_H__ */

