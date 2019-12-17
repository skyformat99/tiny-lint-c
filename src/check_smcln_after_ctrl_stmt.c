#include "check_smcln_after_ctrl_stmt.h"
#include <stdio.h>
#include <string.h> /* strncmp */


static int paren_lvl = 0; /* balance of '(', ')' */
static int paren_lvl_s1 = 0; /* balance of '(', ')' */
static int state = 0;
static int if_while_for = 0;
static int seen_do = 0;
static int seen_while = 0;


/* reset balance counter when loading new file */
void check_smcln_after_ctrl_stmt_init(void)
{
  paren_lvl = 0;
  paren_lvl_s1 = 0;
  state = 0;
  if_while_for = 0;
  seen_do = 0;
  seen_while = 0;
}

void check_smcln_after_ctrl_stmt_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  int i = tok_idx;

  /* TODO: handle do { ... } while(); better */
  if (    (strncmp("do", toks[i].symbol, 2) == 0)
       && (toks[i].symlen == 2))
  {
    seen_do = 1;
  }

  if (    (    (strncmp("if", toks[i].symbol, 2) == 0)
            && (toks[i].symlen == 2))
       || (    (strncmp("while", toks[i].symbol, 5) == 0)
            && (toks[i].symlen == 5))
       || (    (strncmp("for", toks[i].symbol, 3) == 0)
            && (toks[i].symlen == 3)))
  {
    state = 1;
    paren_lvl_s1 = paren_lvl;
    switch (toks[i].symbol[0])
    {
      case 'i': if_while_for = 0;                 break;
      case 'w': if_while_for = 1; seen_while = 1; break;
      case 'f': if_while_for = 2;                 break;
    }
  }
  else if (state == 1)
  {
    if (toks[i].toktyp == OP_LPAREN)
    {
      switch (toks[i - 1].symbol[0])
      {
        case 'i': if (toks[i - 1].symlen != 2) state = 4; break;
        case 'w': if (toks[i - 1].symlen != 5) state = 4; break;
        case 'f': if (toks[i - 1].symlen != 3) state = 4; break;
      }
    }

    if (    (toks[i].toktyp == OP_RPAREN)
         && ((paren_lvl - paren_lvl_s1) == 1))
    {
      state = 2;
    }
    else if (toks[i].toktyp == OP_SEMICOLON && if_while_for != 2)
    {
      state = 4; /* goto 4->0 */
    }
  }
  else if (state == 2)
  {
    if (toks[i].toktyp == OP_SEMICOLON)
    {
      state = 3;
    }
    else
    {
      state = 4; /* goto 4->0 */
    }
  }
  else if (state == 3)
  {
    if (toks[i].toktyp == OP_LBRACE)
    {
      if (!(    (seen_do == 1)
             && (seen_while == 1)))
      {
        const char* str_ifw[] = { "if", "while", "for" };
        fprintf(stdout, "[%s:%d] (warning) Suspicious semicolon after %s-stmt.\n", s->file_path, toks[i - 1].lineno, str_ifw[if_while_for] );
      }
    }
    state = 4; /* goto 4->0 */
  }
  else if (state == 4)
  {
    state = 0; /* goto 0 */
    seen_while = 0;
    seen_do = 0;
  }

       if (toks[i].toktyp == OP_LPAREN) { paren_lvl += 1; }
  else if (toks[i].toktyp == OP_RPAREN) { paren_lvl -= 1; }

  if (paren_lvl < 0) { paren_lvl = 0; }

}




