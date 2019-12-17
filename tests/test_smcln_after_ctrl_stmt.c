

int main(void)
{
  int x = rand();
  if (x > 42); /* HIT */
  {
    printf("then do something else!");
  }

  int i;
  for (i = 0; i < x; ++i); { /* HIT */
    printf("then do something else!");
  }

  while (x > 42); { /* HIT */
    printf("then do something else!");
  }
  return 0;
}


