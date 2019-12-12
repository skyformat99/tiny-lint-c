/*

Static analysis at the lexical abstraction level


*/


#include <assert.h>              /* for assert            */
#include <stdlib.h>
#include <stdio.h>               /* for printf + fgetc    */
#include <string.h>
#include "lexer.h"
#include "source.h"
#include "check_missing_void.h"        /* checks fundecls for missing (void), e.g. f() vs f(void) <-- correct */
#include "check_misleading_var_name.h" /* checks if variable names are misleading, e.g. if u32var is of type int8_t */


/* max size of token-buffer for each file */
#define MAXTOKENBUFSIZE (16*1024*1024)


static struct lexer l;
static struct source_file s;
static struct token toks[MAXTOKENBUFSIZE];


static void scan_file(char* src_file);



/* Main driver: */
int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "\nError: No file-list given as input\n\nUsage: %s <input-file>\n\n", argv[0]);
    return 1;
  }

  FILE* f = fopen(argv[1], "rb");
  if (f != 0)
  {
    lexer_init(&l);
    lexer_setup_alphabet(&l);

    l.continue_on_error = 1;

    char file_path[1024];
    int nbytes = 0;
    char c;
    while ((c = fgetc(f)) != EOF)
    {
      if (c == '\n')
      {
        file_path[nbytes] = 0;
        scan_file(file_path);
        nbytes = 0;
      }
      else
      {
        file_path[nbytes++] = c;
      }
      lexer_reset_state(&l);
    }
    fclose(f);
    lexer_free(&l);
    return 0;
  }

  return 1;
}




static void scan_file(char* src_file)
{
  /* Determine input source and test if it can be found and read.
     If src_file can be found, src_init() dynamically allocates memory. */
  assert(src_init(&s, src_file));

  /* Read file, null-terminate buffer and close file again */
  if (src_read_content(&s) > 0)
  {
    /* Initialize lexer and pass source file */
    lexer_set_char_buf(&l, s.file_content);

    /* ====================================== */
    /* Tokenize file and build token-stream:  */
    /* ====================================== */

    /* Turn characters into tokens / lexemes: */
    int ntokens = 0;

    /* Initialize checker(s) */
    check_missing_void_init();
    check_misleading_var_name_init();

    while (l.buffer[0] != 0)
    {
      struct token t = lexer_next_token(&l);
      if (ntokens < MAXTOKENBUFSIZE)
      {
        toks[ntokens] = t;
      }
      ntokens += 1;

      if (t.tokknd == TOK_EOF)
      {
        break;
      }

      int tok_idx = (ntokens - 1);
      check_missing_void_new_token(&s, toks, tok_idx);
      check_misleading_var_name_new_token(&s, toks, tok_idx);
    }
  }
  /* Clean up memory dynamically allocated by src_init(). */
  src_free(&s);
}





