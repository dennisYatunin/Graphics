#include "parser.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
	if (!--argc) {
		printf("Usage: %s input1 [input2 input3 ...]", argv[0]);
	}

	else {
		while (argc) {
			parse(argv[argc--]);
		}
	}

	return 0;
}