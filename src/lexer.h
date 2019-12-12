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
  OP_ASSIGN_LSH,      /* <<= */
  OP_ASSIGN_RSH,      /* >>= */
  OP_LSH,             /* <<  */
  OP_RSH,             /* >>  */
  OP_NOT_EQUAL,       /* !=  */
  OP_EQUAL,           /* ==  */
  OP_ASSIGN_PLUS,     /* +=  */
  OP_ASSIGN_MINUS,    /* -=  */
  OP_ASSIGN_MULTIPLY, /* *=  */
  OP_ASSIGN_DIVIDE,   /* /=  */
  OP_ASSIGN_AND,      /* &=  */
  OP_ASSIGN_OR,       /* |=  */
  OP_ASSIGN_XOR,      /* ^=  */
  OP_INCREMENT,       /* ++  */
  OP_DECREMENT,       /* --  */
  OP_LOGICAL_AND,     /* &&  */
  OP_LOGICAL_OR,      /* ||  */
  OP_ARROW,           /* ->  */
  OP_LOGICAL_GTE,     /* >=  */
  OP_LOGICAL_LTE,     /* <=  */
  OP_LOGICAL_NOT,     /* !   */
  OP_BITWISE_NOT,     /* ~   */
  OP_SEMICOLON,       /* ;   */
  OP_COLON,           /* :   */
  OP_QUESTIONMARK,    /* ?   */
  OP_COMMA,           /* ,   */
  OP_DOT,             /* .   */
  OP_PLUS,            /* +   */
  OP_MINUS,           /* -   */
  OP_MULTIPLY,        /* *   */
  OP_DIVIDE,          /* /   */
  OP_LOGICAL_GT,      /* >   */
  OP_LOGICAL_LT,      /* <   */
  OP_BITWISE_AND,     /* &   */
  OP_BITWISE_OR,      /* |   */
  OP_BITWISE_XOR,     /* ^   */
  OP_MODULO,          /* %   */
  OP_ASSIGN,          /* =   */
  OP_LPAREN,          /* (   */
  OP_RPAREN,          /* )   */
  OP_LBRACE,          /* {   */
  OP_RBRACE,          /* }   */
  OP_LBRACKET,        /* [   */
  OP_RBRACKET,        /* ]   */

  /* The 32 reserved keywords of C: */
  KW_AUTO,            /* auto     */
  KW_BREAK,           /* break    */
  KW_CASE,            /* case     */
  KW_CHAR,            /* char     */
  KW_CONST,           /* const    */
  KW_CONTINUE,        /* continue */
  KW_DEFAULT,         /* default  */
  KW_DO,              /* do       */
  KW_DOUBLE,          /* double   */
  KW_ELSE,            /* else     */
  KW_ENUM,            /* enum     */
  KW_EXTERN,          /* extern   */
  KW_FLOAT,           /* float    */
  KW_FOR,             /* for      */
  KW_GOTO,            /* goto     */
  KW_IF,              /* if       */
  KW_INT,             /* int      */
  KW_LONG,            /* long     */
  KW_REGISTER,        /* register */
  KW_RETURN,          /* return   */
  KW_SHORT,           /* short    */
  KW_SIGNED,          /* signed   */
  KW_SIZEOF,          /* sizeof   */
  KW_STATIC,          /* static   */
  KW_STRUCT,          /* struct   */
  KW_SWITCH,          /* switch   */
  KW_TYPEDEF,         /* typedef  */
  KW_UNION,           /* union    */
  KW_UNSIGNED,        /* unsigned */
  KW_VOID,            /* void     */
  KW_VOLATILE,        /* volatile */
  KW_WHILE,           /* while    */

  CNST_CHAR,          /* const char, string literal and integers */
  CNST_STRING,
  CNST_INT,
  CNST_FLOAT,
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

