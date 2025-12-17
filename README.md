# Expression VM Calculator
This is a single file tiny expression calculator that compiles expressions into bytecode then executes them using a built-in virtual machine.

### Screenshot
<img width="385" height="213" alt="image" src="https://github.com/user-attachments/assets/24d1e20a-1a4e-487f-bf15-32b9acdc0650" />

*Calculates a variety of different expression which includes:
- Using basic opcodes.
- Precedence of opcodes.
- Floating point calculation.
- Handling spaces.
- Error handling for unknown tokens.

## How to Build
1. Clone the repository and change into its directory:
```
git clone https://github.com/BJL156/Expression-VM-Calculator
cd Expression-VM-Calculator
```
2. Compile the program:
```
gcc main.c -o main
```
3. Run in the terminal:
```
./main
```
  Or for Windows:
```
.\main
```

## How It Works
1. Reads user input: `2 + 2`
2. Performs a lexical analyst: `TOKEN_NUMBER`, `TOKEN_PLUS`, `TOKEN_NUMBER`
4. Converts it to Reverse Polish notation (RPN): `TOKEN_NUMBER`, `TOKEN_NUMBER`, `TOKEN_PLUS`
5. Creates a small bytecode program (`Program` and `ProgramValues`).
6. Compiles the RPN:

    a. Creates a dynamic array to store values on the stack: index `0` = value `2` and index `1` = value `2`.

    b. Converts the source to a bytecode representation:
  ```
  OP_CONSTANT 0
  OP_CONSTANT 1
  OP_ADD
  OP_PRINT
  OP_END
  ```
7. Creates the virutal machine (`VirtualMachine`) and executes the compiled bytecode.
8. Resets `Program` and `ProgramValues` by resetting variables and freeing allocated memory.
