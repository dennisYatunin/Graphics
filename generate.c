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
	vary_node *knobs =
	(vary_node *) calloc(sizeof(struct vary_node_struct), num_frames);
	if (knobs == NULL) {
		perror("Second Pass error (calloc)");
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
				knob = (vary_node) malloc(sizeof(struct vary_node_struct));
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
	char color_type = PNG_RGB, shading = GOROUD;
	int width = 1000, height = 1000, segments = 20;
	screen s = make_screen(width, height);
	vertex_list vlist = new_vlist(100);
	face_list flist = new_flist(100);
	edge_list elist = new_elist(100);
	stack coord_systems = new_stack(5);
	vector center = new_vector(0, 0, 0);
	vector eye = new_vector(0, 0, 200);
	double distance = 200;
	vertex text0, text1;
	text0.x = -100; text0.y = 0; text0.z = 0;
	text1.x =  100; text1.y = 0; text1.z = 0;
	text0.nx = text1.x - text0.x;
	text0.ny = text1.y - text0.y;
	text0.nz = text1.z - text0.z;
	text1.nx = text0.nx * text0.nx + text0.ny * text0.ny + text0.nz * text0.nz;
	text0.color = rgb(255, 255, 255);
	text0.shine = 3;
	text1.color = rgb(255, 0, 255);
	text1.shine = 3;
	uint32_t amb_color = rgb(255, 255, 255), light_color = rgb(0, 255, 255),
	wire_color = rgb(255, 255, 255);
	vector light_source = new_vector(0, 100, 200);
	double amb_ref_constant = 0.1, diff_ref_constant = 1, spec_ref_constant = 1;

	int frame, i, j; double temp;
	for (frame = 0; frame < num_frames; frame++) {
		if (num_frames > 1) {
			for (vn = knobs[frame]; vn != NULL; vn = vn->next) {
				set_value(lookup_symbol(vn->name), vn->value);
			}
		}
		for (i = 0; i < lastop; i++) {
			switch (op[i].opcode) {
				case TEXTURE:
				text0.x = op[lastop].op.texture.d0[0];
				text0.y = op[lastop].op.texture.d0[1];
				text0.z = op[lastop].op.texture.d0[2];
				text1.x = op[lastop].op.texture.d1[0];
				text1.y = op[lastop].op.texture.d1[1];
				text1.z = op[lastop].op.texture.d1[2];
				text0.nx = text1.x - text0.x;
				text0.ny = text1.y - text0.y;
				text0.nz = text1.z - text0.z;
				text1.nx =
				text0.nx * text0.nx + text0.ny * text0.ny + text0.nz * text0.nz;
				text0.color = rgb(
					(uint8_t) max(0, min(0xFF, op[lastop].op.texture.d2[0])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.texture.d2[1])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.texture.d2[2]))
 					);
				text0.shine = op[lastop].op.texture.d2[3];
				text1.color = rgb(
					(uint8_t) max(0, min(0xFF, op[lastop].op.texture.d3[0])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.texture.d3[1])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.texture.d3[2]))
 					);
				text1.shine = op[lastop].op.texture.d3[3];
				break;

				case CAMERA:
				eye.x = op[lastop].op.camera.eye[0];
				eye.y = op[lastop].op.camera.eye[1];
				eye.z = op[lastop].op.camera.eye[2];
				center.x = eye.x + op[lastop].op.camera.aim[0];
				center.y = eye.y + op[lastop].op.camera.aim[1];
				center.z = eye.z + op[lastop].op.camera.aim[2];
				break;

				case FOCAL:
				distance = op[lastop].op.focal.value;
				break;

				case AMBIENT:
				amb_color = rgb(
					(uint8_t) max(0, min(0xFF, op[lastop].op.ambient.c[0])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.ambient.c[1])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.ambient.c[2]))
					);
				break;

				case LIGHT:
				light_color = rgb(
					(uint8_t) max(0, min(0xFF, op[lastop].op.light.c[0])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.light.c[1])),
					(uint8_t) max(0, min(0xFF, op[lastop].op.light.c[2]))
					);
				light_source = new_vector(
					op[lastop].op.light.l[0],
					op[lastop].op.light.l[1],
					op[lastop].op.light.l[2]
					);
				break;

				case SHADING:
				if (strcmp(op[i].op.shading.p->name, "GOROUD") == 0) {
					shading = GOROUD;
				}
				else {
					shading = WIRE;
				}
				break;

				case SPHERE:
				add_sphere(
					vlist, elist, flist,
					op[i].op.sphere.d[0],
					op[i].op.sphere.d[1],
					op[i].op.sphere.d[2],
					op[i].op.sphere.r, segments, text0, text1
					);
				transform(vlist, peek(coord_systems));
				if (shading == GOROUD) {
					draw_faces(
						s, vlist, flist, center, eye, distance,
						amb_color, light_color, light_source,
						amb_ref_constant, diff_ref_constant, spec_ref_constant
						);
				}
				else {
					draw_edges(
						s, vlist, elist, center, eye, distance, wire_color
						);
				}
				clear_vlist(vlist);
				clear_elist(elist);
				clear_flist(flist);
				break;

				case BOX:
				add_box(
					vlist, elist, flist,
					op[i].op.box.d0[0],
					op[i].op.box.d0[1],
					op[i].op.box.d0[2],
					op[i].op.box.d1[0],
					op[i].op.box.d1[1],
					op[i].op.box.d1[2],
					text0, text1
					);
				transform(vlist, peek(coord_systems));
				if (shading == GOROUD) {
					draw_faces(
						s, vlist, flist, center, eye, distance,
						amb_color, light_color, light_source,
						amb_ref_constant, diff_ref_constant, spec_ref_constant
						);
				}
				else {
					draw_edges(
						s, vlist, elist, center, eye, distance, wire_color
						);
				}
				clear_vlist(vlist);
				clear_elist(elist);
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
					text0, text1
					);
				transform(vlist, peek(coord_systems));
				draw_edges(s, vlist, elist, center, eye, distance, wire_color);
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
				scale(
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
			clear_screen(s);
			clear_stack(coord_systems);
		}
	}

	free_screen(s);
	free_vlist(vlist);
	free_flist(flist);
	free_elist(elist);
	free(coord_systems);
}
