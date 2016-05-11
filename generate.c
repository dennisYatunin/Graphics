#include "png.h"
#include "screen.h"
#include "matrix.h"
#include "stack.h"
#include "draw.h"
#include "generate.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

char first_pass() {
	int has_frames = 0;
	int has_basename = 0;
	int has_vary = 0;

	int i;
	for (i = 0; i < lastop; i++) {
		switch (op[i].opcode) {
			case FRAMES:
			num_frames = op[i].op.frames.num_frames;
			has_frames = 1;
			break;
			case BASENAME:
			has_basename = 1;
			strcpy(name, op[i].op.basename.p->name);
			break;
			case VARY:
			has_vary = 1;
			break;
		}
	}

	if(has_vary && !has_frames){
		fprintf(
			stderr,
			"First Pass error: "
			"vary command given without frames being specified\n"
			);
		exit(EXIT_FAILURE);
	}

	if(has_frames && !has_basename){
		strcpy(name, "default");
		printf("Using basename \"default\".\n");
	}

	return has_frames && num_frames > 1;
}

vary_node *second_pass() {
	vary_node *knobs = (vary_node *) malloc(sizeof(vary_node) * num_frames);
	if (knobs == NULL) {
		perror("Second Pass error (malloc)");
		exit(EXIT_FAILURE);
	}

	vary_node knob;
	int i, start_frame, end_frame, num_vals, frame;
	double start_val, end_val, cur_val, increment;

	for (i = 0; i < lastop; i++){
		if (op[i].opcode == VARY){
			start_frame = op[i].op.vary.start_frame;
			if (start_frame < 0){
				fprintf(
					stderr,
					"Second Pass error: start frame must be non-negative\n"
					);
				exit(EXIT_FAILURE);
			}

			end_frame = op[i].op.vary.end_frame;
			if (end_frame >= num_frames){
				fprintf(
					stderr,
					"Second Pass error: "
					"end frame must be less than the number of frames\n"
					);
				exit(EXIT_FAILURE);
			}

			if (start_frame > end_frame){
				fprintf(
					stderr,
					"Second Pass error: "
					"start frame must be less than or equal to end frame\n"
					);
				exit(EXIT_FAILURE);
			}

			start_val = op[i].op.vary.start_val;
			end_val = op[i].op.vary.end_val;
			cur_val = start_val;
			num_vals = end_frame - start_frame;
			increment = (end_val - start_val) / num_vals;

			for (frame = start_frame; frame <= end_frame; frame++){
				knob = (vary_node) malloc(sizeof(vary_node));
				if (knob == NULL) {
					perror("Second Pass error (malloc)");
					exit(EXIT_FAILURE);
				}

				strcpy(knob->name, op[i].op.vary.p->name);
				knob->value = cur_val;
				knob->next = knobs[frame];
				knobs[frame] = knob;
				cur_val += increment;
			}
		}
	}
	return knobs;
}

void generate( int polygons ) {
	double knob_value;
	vary_node *knobs;
	vary_node vn;
	char frame_name[128];

	if (first_pass()) {
		knobs = second_pass();
	}
	else {
		num_frames = 1;
	}

	// Default values
	char color_type = PNG_RGB, perspective = PERSPECTIVE_OFF;
	int width = 1000, height = 1000, color = rgb(255, 255, 255), segments = 20;
	screen s = make_screen(width, height);
	vertex_list vlist = new_vlist(100);
	face_list flist = new_flist(100);
	edge_list elist = new_elist(100);
	stack coord_systems = new_stack(5);
	vector center = new_vector(0, 0, 0);
	vector eye = new_vector(0, 0, 1000);
	double distance = 600;

	int frame, i, j; double temp;
	for (frame = 0; frame < num_frames; frame++) {
		if (num_frames > 1) {
			for (vn = knobs[j]; vn != NULL; vn = vn->next) {
				set_value(lookup_symbol(vn->name), vn->value);
			}
		}
		for (i = 0; i < lastop; i++) {
			switch (op[i].opcode) {
				case SPHERE:
				add_sphere(
					vlist, flist,
					op[i].op.sphere.d[0],
					op[i].op.sphere.d[1],
					op[i].op.sphere.d[2],
					op[i].op.sphere.r, segments, color
					);
				transform(vlist, peek(coord_systems));
				draw_faces(s, vlist, flist, center, eye, distance);
				clear_vlist(vlist);
				clear_flist(flist);
				break;

				case BOX:
				add_box(
					vlist, flist,
					op[i].op.box.d0[0],
					op[i].op.box.d0[1],
					op[i].op.box.d0[2],
					op[i].op.box.d1[0],
					op[i].op.box.d1[1],
					op[i].op.box.d1[2],
					color
					);
				transform(vlist, peek(coord_systems));
				draw_faces(s, vlist, flist, center, eye, distance);
				clear_vlist(vlist);
				clear_flist(flist);
				break;

				case LINE:
				add_line(
					vlist, elist,
					op[i].op.line.p0[0],
					op[i].op.line.p0[1],
					op[i].op.line.p0[2],
					op[i].op.line.p1[0],
					op[i].op.line.p1[1],
					op[i].op.line.p1[2],
					color
					);
				transform(vlist, peek(coord_systems));
				draw_edges(s, vlist, elist, center, eye, distance);
				clear_vlist(vlist);
				clear_elist(elist);
				break;

				case MOVE:
				if (op[i].op.move.p == NULL){
					knob_value = 1;
				}
				else {
					knob_value = op[i].op.move.p->s.value;
				}
				translate(
					op[i].op.move.d[0] * knob_value,
					op[i].op.move.d[1] * knob_value,
					op[i].op.move.d[2] * knob_value,
					peek(coord_systems)
					);
				break;

				case SCALE:
				if (op[i].op.scale.p == NULL){
					knob_value = 1;
				}
				else {
					knob_value = op[i].op.scale.p->s.value;
				}
				translate(
					op[i].op.scale.d[0] * knob_value,
					op[i].op.scale.d[1] * knob_value,
					op[i].op.scale.d[2] * knob_value,
					peek(coord_systems)
					);
				break;

				case ROTATE:
				if (op[i].op.rotate.p == NULL){
					knob_value = 1;
				}
				else {
					knob_value = op[i].op.rotate.p->s.value;
				}
				temp = op[i].op.rotate.axis;
				if (temp == 0) {
					rotate_x(
						op[i].op.rotate.degrees * knob_value,
						peek(coord_systems)
						);
				}
				else if (temp == 1) {
					rotate_y(
						op[i].op.rotate.degrees * knob_value,
						peek(coord_systems)
						);
				}
				else {
					rotate_z(
						op[i].op.rotate.degrees * knob_value,
						peek(coord_systems)
						);
				}
				break;

				case SET:
				set_value(
					lookup_symbol(op[i].op.set.p->name),
					op[i].op.set.val
					);
				break;

				case SETKNOBS:
				for (j = 0; j < lastsym; j++) {
					set_value(&symtab[j], op[j].op.setknobs.value);
				}
				break;

				case PUSH:
				push(coord_systems);
				break;

				case POP:
				pop(coord_systems);
				break;

				case SAVE:
				make_png(op[i].op.save.p->name, s, color_type);
				break;

				case DISPLAY:
				display_png(s, color_type);
				break;
			}
		}
		if (num_frames > 1){
			sprintf(frame_name, "anim/%s%03d.png", name, frame);
			make_png(frame_name, s, color_type);
		}
	}

	free_screen(s);
	free_vlist(vlist);
	free_flist(flist);
	free_elist(elist);
	free(coord_systems);
}
