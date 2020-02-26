# tiny-lint-c
## A small and simple static analysis tool for C implemented in C
### Description

A small ["linter"](https://en.wikipedia.org/wiki/Lint_(software)) for C written in C99, demonstrating shallow [static analysis](https://en.wikipedia.org/wiki/Static_program_analysis) at the [lexical](https://en.wikipedia.org/wiki/Lexical_analysis) level of abstraction.

There are currently four checks implemented:

- [A check](https://github.com/kokke/tiny-lint-c/blob/master/src/check_missing_void.c) for [nullary](https://en.wikipedia.org/wiki/Arity#Examples) function prototypes declared incorrectly, e.g. in the form of `f()` instead of `f(void)`.
- [A check](https://github.com/kokke/tiny-lint-c/blob/master/src/check_misleading_var_name.c) for variable names that have a misleading prefix, e.g. `uint32_t u8var` instead of `uint32_t u32var`.
- [A check](https://github.com/kokke/tiny-lint-c/blob/master/src/check_smcln_after_ctrl_stmt.c) for seemingly misplaced semi-colons after control statements (if, for, while), e.g. `if (x); { ... }`.
- [A check](https://github.com/kokke/tiny-lint-c/blob/master/src/check_assign_in_ctrl_stmt.c) for assignments in expressions deciding control-flow (if, for, while), e.g. `if (v = 0) { ... }`.



NOTE: This is very much a work in progress still. It should be fairly easy to hack on though.

