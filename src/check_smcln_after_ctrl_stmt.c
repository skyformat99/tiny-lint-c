#include "check_missing_void.h"
#include <stdio.h>



static int paren_lvl = 0; /* balance of '(', ')' */
static int paren_lvl_s1 = 0; /* balance of '(', ')' */
static int state = 0;
static int if_while_for = 0;

/* reset balance counter when loading new file */
void check_smcln_after_ctrl_stmt_init(void)
{
  paren_lvl = 0;
  state = 0;
  if_while_for = 0;
  paren_lvl_s1 = 0;
}

void check_smcln_after_ctrl_stmt_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  int i = tok_idx;

  if ((strcmp("if", toks[i].symbol) == 0) || (strcmp("while", toks[i].symbol) == 0) || (strcmp("for", toks[i].symbol) == 0))
  {
    state = 1;
    paren_lvl_s1 = paren_lvl;
    switch (toks[i].symbol[0])
    {
      case 'i': if_while_for = 0; break;
      case 'w': if_while_for = 1; break;
      case 'f': if_while_for = 2; break;
    }
  }
  else if ((state == 1) && ((paren_lvl - paren_lvl_s1) == 1) && (toks[i].toktyp == OP_RPAREN))
  {
    state = 2;
  }
  else if ((state == 2) && (toks[i].toktyp == OP_SEMICOLON))
  {
    state = 3;
  }
  else if (state == 3)
  {
    if (toks[i].toktyp == OP_LBRACE)
    {
      const char* str_ifw[] = { "if", "while", "for" };
      fprintf(stderr, "[%s:%d] (warning) suspicious semicolon after %s-stmt.\n", s->file_path, toks[i].lineno, str_ifw[if_while_for] );
    }
    state = 0;
  }

  else if (toks[i].toktyp == OP_LPAREN) { paren_lvl += 1; }
  else if (toks[i].toktyp == OP_RPAREN) { paren_lvl -= 1; }

  if (paren_lvl < 0) { paren_lvl = 0; }

}




