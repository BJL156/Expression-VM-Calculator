# Expression VM Calculator
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)](https://www.linux.org/)

A single-file expression calculator that compiles an arithmetic expression into bytecode and then executes it on a built-in virtual machine. It behaves the way real language implementations do.

Written as a first step toward writing my own AArch64 toolchain from scratch. The goal for this project is to understand lexers and bytecode VMs before continuing to the next project: [CPU Simulator](https://github.com/BJL156/CPU-Simulator).

## Build
```bash
gcc main.c -o main
./main
```

## How It Works
Given input `2 + 2 * 3`:

1. Lexer - tokenizes the input: `NUMBER PLUS NUMBER STAR NUMBER`
2. RPN conversion - reorders by precedence level: `2 3 * 2 +`
3. Compiler - emits bytecode:
```
    OP_CONSTANT 0  ; 2
    OP_CONSTANT 1  ; 3
    OP_MUL
    OP_CONSTANT 2  ; 2
    OP_ADD
    OP_PRINT
    OP_END
```
4. VM - executes the bytecode which includes a print instruction to write out the result to standard output.

## Features
- [x] Operator precedence via RPN conversion.
- [x] Floating point support.
- [x] Whitespace handling.
- [x] Error handling for unknown tokens.
