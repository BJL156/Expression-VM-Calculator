#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STACK_MAX 64
#define TOKEN_MAX 127

typedef enum {
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_PRINT,
  OP_END
} Opcodes;

typedef struct {
  double *values;
  size_t count;
  size_t capacity;
} ProgramValues;

void program_values_initialize(ProgramValues *program_values) {
  program_values->count = 0;
  program_values->capacity = 8;
  program_values->values = (double *)malloc(program_values->capacity * sizeof(double));
}

void program_values_write_value(ProgramValues *program_values, double value) {
    if (program_values->capacity <= program_values->count) {
    uint8_t new_capacity = program_values->capacity * 2;
    double *new_values = (double *)realloc(program_values->values, new_capacity);
    if (new_values == NULL) {
      printf("Program value error: Failed to allocate more memory.\n");
      exit(1);
    }

    program_values->capacity = new_capacity;
    program_values->values = new_values;
  }

  program_values->values[program_values->count] = value;
  program_values->count++;
}

void program_values_free(ProgramValues *program_values) {
  free(program_values->values);
  program_values->count = 0;
  program_values->capacity = 0;
}

typedef struct {
  uint8_t *opcodes;
  size_t count;
  size_t capacity;
  ProgramValues *program_values;
} Program;

void program_initialize(Program *program, ProgramValues *program_values) {
  program->count = 0;
  program->capacity = 8;
  program->opcodes = (uint8_t *)malloc(program->capacity * sizeof(uint8_t));
  program->program_values = program_values;
}

void program_write_opcode(Program *program, Opcodes opcode) {
  if (program->capacity <= program->count) {
    uint8_t new_capacity = program->capacity * 2;
    uint8_t *new_opcodes = (uint8_t *)realloc(program->opcodes, new_capacity);
    if (new_opcodes == NULL) {
      printf("Program error: Failed to allocate more memory.\n");
      exit(1);
    }

    program->capacity = new_capacity;
    program->opcodes = new_opcodes;
  }

  program->opcodes[program->count] = opcode;
  program->count++;
}

void program_write_constant(Program *program, double value) {
  program_values_write_value(program->program_values, value);

  program_write_opcode(program, OP_CONSTANT);
  program_write_opcode(program, (uint8_t)(program->program_values->count - 1));
}

void program_free(Program *program) {
  free(program->opcodes);
  program->count = 0;
  program->capacity = 0;
}

typedef struct {
  uint8_t *opcode_pointer;
  Program *program;
  double stack[STACK_MAX];
  size_t stack_count;
} VirtualMachine;

void virtual_machine_initialize(VirtualMachine *virtual_machine, Program *program) {
  virtual_machine->opcode_pointer = program->opcodes;
  virtual_machine->program = program;
  virtual_machine->stack_count = 0;
}

void virtual_machine_push(VirtualMachine *virtual_machine, double value) {
  if (virtual_machine->stack_count >= STACK_MAX) {
    printf("Virtual machine error: stack overflow.");
    exit(1);
  }

  virtual_machine->stack[virtual_machine->stack_count++] = value;
}

double virtual_machine_pop(VirtualMachine *virtual_machine) {
  if (virtual_machine->stack_count <= 0) {
    printf("Virtual machine error: stack underflow.");
    exit(1);
  }

  return virtual_machine->stack[--virtual_machine->stack_count];
}

void virtual_machine_execute(VirtualMachine *virtual_machine) {
  while (1) {
    uint8_t current_opcode = *(virtual_machine->opcode_pointer++);
    switch (current_opcode) {
      case OP_CONSTANT: {
        uint8_t index = *(virtual_machine->opcode_pointer++);
        double value = virtual_machine->program->program_values->values[index];
        virtual_machine_push(virtual_machine, value);
        break;
      }
      case OP_ADD: {
        double b = virtual_machine_pop(virtual_machine);
        double a = virtual_machine_pop(virtual_machine);
        virtual_machine_push(virtual_machine, a + b);
        break;
      }
      case OP_SUBTRACT: {
        double b = virtual_machine_pop(virtual_machine);
        double a = virtual_machine_pop(virtual_machine);
        virtual_machine_push(virtual_machine, a - b);
        break;
      }
      case OP_MULTIPLY: {
        double b = virtual_machine_pop(virtual_machine);
        double a = virtual_machine_pop(virtual_machine);
        virtual_machine_push(virtual_machine, a * b);
        break;
      }
      case OP_DIVIDE: {
        double b = virtual_machine_pop(virtual_machine);
        double a = virtual_machine_pop(virtual_machine);
        virtual_machine_push(virtual_machine, a / b);
        break;
      }
      case OP_END: {
        return;
      }
      case OP_PRINT: {
        printf("%f\n", virtual_machine->stack[virtual_machine->stack_count - 1]);
        break;
      }
      default: {
        printf("Execution error: Unknown opcode: %u\n", current_opcode);
        return;
      }
    }
  }
}

typedef enum {
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN
} TokenType;

typedef struct {
  TokenType token_type;
  double value;
} Token;

size_t scan_tokens(const char *user_input, Token *tokens_output) {
  size_t count = 0;

  while (*user_input) {
    if (*user_input == ' ' || *user_input == '\n') {
      user_input++;
      continue;
    }

    if ((*user_input >= '0' && *user_input <= '9') || *user_input == '.') {
      char *value_end;
      double value = strtod(user_input, &value_end);
      tokens_output[count++] = (Token){ TOKEN_NUMBER, value };
      user_input = value_end;
      continue;
    }

    switch (*user_input) {
      case '+': tokens_output[count++] = (Token){ TOKEN_PLUS }; break;
      case '-': tokens_output[count++] = (Token){ TOKEN_MINUS }; break;
      case '*': tokens_output[count++] = (Token){ TOKEN_STAR }; break;
      case '/': tokens_output[count++] = (Token){ TOKEN_SLASH }; break;
      case '(': tokens_output[count++] = (Token){ TOKEN_LEFT_PAREN }; break;
      case ')': tokens_output[count++] = (Token){ TOKEN_RIGHT_PAREN }; break;
      default: {
        printf("Scan tokens: unknown character: %c", *user_input);
        exit(1);
      }
    }

    user_input++;
  }

  return count;
}

int precedence(TokenType token_type) {
  if (token_type == TOKEN_STAR || token_type == TOKEN_SLASH) {
    return 2;
  }

  if (token_type == TOKEN_PLUS || token_type == TOKEN_MINUS) {
    return 1;
  }

  return 0;
}

size_t infix_to_postfix(Token *tokens_infix, size_t count, Token *tokens_postfix) {
  Token stack[STACK_MAX];
  size_t stack_top = 0;
  size_t postfix_count = 0;

  for (size_t i = 0; i < count; i++) {
    Token token = tokens_infix[i];

    if (token.token_type == TOKEN_NUMBER) {
      tokens_postfix[postfix_count++] = token;
    } else if (token.token_type == TOKEN_LEFT_PAREN) {
      stack[stack_top++] = token;
    } else if (token.token_type == TOKEN_RIGHT_PAREN) {
      while (stack_top && stack[stack_top - 1].token_type != TOKEN_LEFT_PAREN) {
        tokens_postfix[postfix_count++] = stack[--stack_top];
      }
      stack_top--;
    } else {
      while (stack_top && precedence(stack[stack_top - 1].token_type) >= precedence(token.token_type)) {
        tokens_postfix[postfix_count++] = stack[--stack_top];
      }

      stack[stack_top++] = token;
    }
  }

  while (stack_top) {
    tokens_postfix[postfix_count++] = stack[--stack_top];
  }

  return postfix_count;
}

void compile_tokens(Token *tokens, size_t count, Program *program) {
  for (size_t i = 0; i < count; i++) {
    switch (tokens[i].token_type) {
      case TOKEN_NUMBER: {
        program_write_constant(program, tokens[i].value);
        break;
      }
      case TOKEN_PLUS: {
        program_write_opcode(program, OP_ADD);
        break;
      }
      case TOKEN_MINUS: {
        program_write_opcode(program, OP_SUBTRACT);
        break;
      }
      case TOKEN_STAR: {
        program_write_opcode(program, OP_MULTIPLY);
        break;
      }
      case TOKEN_SLASH: {
        program_write_opcode(program, OP_DIVIDE);
        break;
      }
      default: {
        break;
      }
    }
  }

  program_write_opcode(program, OP_PRINT);
  program_write_opcode(program, OP_END);
}

int main(void) {
  while (1) {
    printf("> ");
    char user_input[127];
    fgets(user_input, sizeof(user_input), stdin);

    Token tokens[TOKEN_MAX];
    size_t token_count = scan_tokens(user_input, tokens);

    Token postfix[TOKEN_MAX];
    size_t postfix_count = infix_to_postfix(tokens, token_count, postfix);

    ProgramValues program_values;
    program_values_initialize(&program_values);

    Program program;
    program_initialize(&program, &program_values);

    compile_tokens(postfix, postfix_count, &program);

    VirtualMachine virtual_machine;
    virtual_machine_initialize(&virtual_machine, &program);
    virtual_machine_execute(&virtual_machine);

    program_free(&program);
    program_values_free(&program_values);
  }

  return 0;
}
