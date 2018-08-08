#include <stdio.h>
#include <stdlib.h>

#define SIZE 20
#define MAXVAL 10

int main() {

	FILE* matrix;
	FILE* vector;

	int i, j;
		
	matrix = fopen("matrix.txt", "w");
	vector = fopen("vector.txt", "w");

	for (i = 0; i < SIZE; i++) {
		fprintf(vector, "%d\n", rand() % MAXVAL);
		for (j = 0; j < SIZE; j++) {
			fprintf(matrix, "%d ", rand() % MAXVAL);
		}
		fprintf(matrix, "\n");
	}

	fclose(matrix);
	fclose(vector);

	return 0;
}
