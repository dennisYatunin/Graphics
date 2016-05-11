#ifndef STACK_H
#define STACK_H
#include "matrix.h"
#include <stddef.h>
#include <stdint.h>

typedef struct stack_struct {
	size_t size;
	size_t capacity;
	matrix *matrices;
} *stack;

stack new_stack(size_t max_num_matrices);

void clear_stack(stack st);

void push(stack st);

void pop(stack st);

matrix peek(stack st);
#endif
