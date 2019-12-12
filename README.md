# tiny-lint-c
# A small and simple static analysis tool in C
### Description

A small [linter](https://en.wikipedia.org/wiki/Lint_(software)) written in C99, demonstrating [static analysis](https://en.wikipedia.org/wiki/Static_program_analysis) at the [lexical](https://en.wikipedia.org/wiki/Lexical_analysis) level of abstraction.

There are currently two checks implemented:

- Check for nullary function prototypes declared incorrectly, e.g. in the form of `f()` instead of `f(void)`.
- Check for variable names that have a misleading name-prefix, e.g. `uint32_t u8var` instead of `uint32_t u32var`.


NOTE: This is very much a work in progress still. It should be fairly easy to hack on though.

