#include "stack.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

stack new_stack(size_t max_num_matrices) {
	stack st = (stack) malloc(sizeof(struct stack_struct));
	if (st == NULL) {
		perror("New Stack error (malloc)");
		exit(EXIT_FAILURE);
	}
	st->size = 0;
	st->capacity = max_num_matrices;
	st->matrices = (matrix *) malloc(max_num_matrices * sizeof(matrix));
	if (st->matrices == NULL) {
		perror("New Stack error (malloc)");
		exit(EXIT_FAILURE);
	}
	st->matrices[st->size++] = identity_matrix();
	return st;
}

void clear_stack(stack st) {
	int counter = st->size;
	while (counter > 1) {
		free(st->matrices[counter--]);
	}
	st->size = 1;
}

void push(stack st) {
	if (st->size == st->capacity) {
		st->capacity *= 2;
		st->matrices = realloc(st->matrices, st->capacity * sizeof(matrix));
		if (st->matrices == NULL) {
			perror("Push error (realloc)");
			exit(EXIT_FAILURE);
		}
	}
	matrix m = new_matrix();
	memcpy(m, st->matrices[st->size - 1], 16 * sizeof(double));
	st->matrices[st->size++] = m;
}

void pop(stack st) {
	if (st->size == 1) {
		fprintf(stderr, "Pop error: cannot pop primary matrix\n");
		exit(EXIT_FAILURE);
	}
	free(st->matrices[--st->size]);
}

matrix peek(stack st) {
	return st->matrices[st->size - 1];
}
