#ifndef GENERATE_H
#define GENERATE_H

int num_frames;
char name[128];

typedef struct vary_node_struct {
	char name[128];
	double value;
	struct vary_node_struct *next;
} *vary_node;

void generate();
void print_knobs();
void process_knobs();
char first_pass();
vary_node *second_pass();

#define GOROUD 0
#define WIRE 1

#endif
