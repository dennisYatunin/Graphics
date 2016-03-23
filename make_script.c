#include <unistd.h>
#include <stdio.h>

#define WIDTH 1000
#define HEIGHT 1000

int main(int argc, char *argv[]) {
	FILE *fp = fopen("script", "w");
	if (fp == NULL) {
		fprintf(stderr, "Make Script error: unable to open script file\n");
		exit(1);
	}

	fprintf(fp, "setwh\n%d %d\nsetct\nRGB\nsetsegs\n5000\n", WIDTH, HEIGHT);
	int i;
	for (i = 0; i < 200; i++) {
		fprintf(fp, "setcolor\n%d %d %d\n", i, 255, i);
		fprintf(fp, "circle\n%d %d %d\n", WIDTH / 2, 3 * HEIGHT / 4, i);
	}
	for (i = 0; i < 148; i++) {
		fprintf(fp, "setcolor\n%d %d %d\n", 200, i + 100, 200);
		fprintf(
			fp, "bezier\n%d %d %d %d %d %d %d %d\n",
			0, 0,
			WIDTH / 4, i * HEIGHT / 200,
			3 * WIDTH / 4, i * HEIGHT / 200,
			WIDTH, 0
			);
	}
	fprintf(fp, "display\nquit\n");

	return 0;
}