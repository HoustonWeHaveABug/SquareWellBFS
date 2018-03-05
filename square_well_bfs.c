#include <stdio.h>
#include <stdlib.h>
#include "fractions.h"

typedef struct {
	int lt_borders_n;
	fraction_t border_min;
	int size;
	fraction_t height;
	fraction_t depth;
	fraction_t volume;
	fraction_t new_height;
}
basin_t;

typedef struct {
	fraction_t height;
	int row;
	int column;
	int visited;
	int basins_idx;
}
cell_t;

typedef struct {
	int rows_n;
	int columns_n;
	int cells_n;
	cell_t *source_cell;
	int *used;
	cell_t **le_queue;
	cell_t **eq_queue;
	int le_queue_size;
	int eq_queue_offset;
}
square_well_t;

int read_cell(cell_t *, int, int, square_well_t *);
void add_le_neighbours(cell_t *, square_well_t *);
void check_le_link(cell_t *, cell_t *, square_well_t *);
void add_to_le_queue(cell_t *, square_well_t *);
void set_basin(basin_t *, square_well_t *);
void add_eq_neighbours(cell_t *, basin_t *, square_well_t *);
void check_eq_link(cell_t *, cell_t *, basin_t *, square_well_t *);
void add_to_eq_queue(cell_t *, basin_t *, square_well_t *);
int is_lt_link(cell_t *, cell_t *);
void set_border_min(cell_t *, cell_t *, basin_t *);
void set_basin_volume(basin_t *, fraction_t *);
void update_basin_volume(basin_t *, fraction_t *);
void set_basin_new_height(basin_t *, fraction_t *);

int main(void) {
	int row, column, height, cells_idx, basins_max;
	fraction_t target_height, target_border, total_time;
	basin_t *basins;
	cell_t *cells, *target_cell;
	square_well_t square_well;

	/* Parse input */
	if (scanf("%d%d", &square_well.rows_n, &square_well.columns_n) != 2 || square_well.rows_n < 1 || square_well.columns_n < 1) {
		fprintf(stderr, "Invalid square well size\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	square_well.cells_n = square_well.rows_n*square_well.columns_n;
	cells = malloc(sizeof(cell_t)*(size_t)square_well.cells_n);
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	square_well.used = calloc((size_t)square_well.cells_n, sizeof(int));
	if (!square_well.used) {
		fprintf(stderr, "Could not allocate memory for square_well.used\n");
		fflush(stderr);
		free(cells);
		return EXIT_FAILURE;
	}
	for (row = 0; row < square_well.rows_n; row++) {
		for (column = 0; column < square_well.columns_n; column++) {
			if (!read_cell(cells+row*square_well.columns_n+column, row, column, &square_well)) {
				free(square_well.used);
				free(cells);
				return EXIT_FAILURE;
			}
		}
	}
	if (scanf("%d", &height) != 1 || height < 1 || height > square_well.cells_n) {
		fprintf(stderr, "Invalid target height\n");
		fflush(stderr);
		free(square_well.used);
		free(cells);
		return EXIT_FAILURE;
	}
	set_fraction(&target_height, height, 1, 0);

	/* Set the target cell */
	for (cells_idx = 0; cells_idx < square_well.cells_n && compare_fractions(&cells[cells_idx].height, &target_height) != 0; cells_idx++);
	add_fraction_int(&target_height, 1, &target_border);
	target_cell = cells+cells_idx;

	/* Allocate memory for queues and basins */
	square_well.le_queue = malloc(sizeof(cell_t *)*(size_t)square_well.cells_n);
	if (!square_well.le_queue) {
		fprintf(stderr, "Could not allocate memory for square_well.le_queue\n");
		fflush(stderr);
		free(square_well.used);
		free(cells);
		return EXIT_FAILURE;
	}
	square_well.eq_queue = malloc(sizeof(cell_t *)*(size_t)square_well.cells_n);
	if (!square_well.eq_queue) {
		fprintf(stderr, "Could not allocate memory for square_well.eq_queue\n");
		fflush(stderr);
		free(square_well.le_queue);
		free(square_well.used);
		free(cells);
		return EXIT_FAILURE;
	}
	basins = malloc(sizeof(basin_t));
	if (!basins) {
		fprintf(stderr, "Could not allocate memory for basins\n");
		fflush(stderr);
		free(square_well.eq_queue);
		free(square_well.le_queue);
		free(square_well.used);
		free(cells);
		return EXIT_FAILURE;
	}
	basins_max = 1;

	/* Iterate until target border is reached by water */
	set_fraction(&total_time, 0, 1, 0);
	do {
		int le_queue_idx, basins_n, eq_queue_idx, basins_idx;
		fraction_t basin_volume_min;

		/* Search cells reachable by water from source */
		square_well.le_queue_size = 0;
		add_to_le_queue(square_well.source_cell, &square_well);
		for (le_queue_idx = 0; le_queue_idx < square_well.le_queue_size; le_queue_idx++) {
			add_le_neighbours(square_well.le_queue[le_queue_idx], &square_well);
		}

		/* Search for basins with local minimum height */
		square_well.eq_queue_offset = 0;
		basins_n = 0;
		for (le_queue_idx = 0; le_queue_idx < square_well.le_queue_size; le_queue_idx++) {
			if (basins_n == basins_max) {
				basin_t *basins_tmp = realloc(basins, sizeof(basin_t)*(size_t)(basins_max+1));
				if (!basins_tmp) {
					fprintf(stderr, "Could not reallocate memory for basins\n");
					fflush(stderr);
					free(basins);
					free(square_well.eq_queue);
					free(square_well.le_queue);
					free(square_well.used);
					free(cells);
					return EXIT_FAILURE;
				}
				basins = basins_tmp;
				basins_max++;
			}
			set_basin(basins+basins_n, &square_well);
			add_to_eq_queue(square_well.le_queue[le_queue_idx], basins+basins_n, &square_well);
			for (eq_queue_idx = square_well.eq_queue_offset; eq_queue_idx < square_well.eq_queue_offset+basins[basins_n].size; eq_queue_idx++) {
				add_eq_neighbours(square_well.eq_queue[eq_queue_idx], basins+basins_n, &square_well);
			}
			if (basins[basins_n].lt_borders_n == 0 && basins[basins_n].size > 0) {
				fraction_t basin_depth;
				set_basin_volume(basins+basins_n, &square_well.eq_queue[square_well.eq_queue_offset]->height);
				for (eq_queue_idx = square_well.eq_queue_offset; eq_queue_idx < square_well.eq_queue_offset+basins[basins_n].size; eq_queue_idx++) {
					square_well.eq_queue[eq_queue_idx]->basins_idx = basins_n;
				}
				square_well.eq_queue_offset += basins[basins_n].size;
				basins_n++;
			}
		}

		/* If target belongs to a basin adjust its wolume if necessary */
		if (target_cell->basins_idx < square_well.cells_n) {
			update_basin_volume(basins+target_cell->basins_idx, &target_border);
		}

		/* Search for the basin with the smallest volume (V) */
		basin_volume_min = basins[0].volume;
		for (basins_idx = 1; basins_idx < basins_n; basins_idx++) {
			if (compare_fractions(&basins[basins_idx].volume, &basin_volume_min) < 0) {
				basin_volume_min = basins[basins_idx].volume;
			}
		}


		/* Distribute an equal volume of water - V - in each basin */
		for (basins_idx = 0; basins_idx < basins_n; basins_idx++) {
			set_basin_new_height(basins+basins_idx, &basin_volume_min);
			add_fractions(&total_time, &basin_volume_min, &total_time);
		}

		/* Adjust basin cells height after distribution */
		for (eq_queue_idx = 0; eq_queue_idx < square_well.eq_queue_offset; eq_queue_idx++) {
			square_well.eq_queue[eq_queue_idx]->height = basins[square_well.eq_queue[eq_queue_idx]->basins_idx].new_height;
			square_well.eq_queue[eq_queue_idx]->basins_idx = square_well.cells_n;
		}

		/* Reset cells visited state */
		for (le_queue_idx = 0; le_queue_idx < square_well.le_queue_size; le_queue_idx++) {
			square_well.le_queue[le_queue_idx]->visited = 0;
		}
	}
	while (compare_fractions(&target_cell->height, &target_border) < 0);

	/* Output the resulting square well and total time spent */
	for (row = 0; row < square_well.rows_n; row++) {
		print_fraction(&cells[row*square_well.columns_n].height);
		for (column = 1; column < square_well.columns_n; column++) {
			putchar(' ');
			print_fraction(&cells[row*square_well.columns_n+column].height);
		}
		puts("");
	}
	printf("\nTime ");
	print_fraction(&total_time);
	puts("");
	fflush(stdout);

	/* Free allocated memory and exit */
	free(basins);
	free(square_well.eq_queue);
	free(square_well.le_queue);
	free(square_well.used);
	free(cells);
	return EXIT_SUCCESS;
}

int read_cell(cell_t *cell, int row, int column, square_well_t *square_well) {
	int height;
	if (scanf("%d", &height) != 1 || height < 1 || height > square_well->cells_n || square_well->used[height-1]) {
		fprintf(stderr, "Invalid cell height\n");
		fflush(stderr);
		return 0;
	}
	set_fraction(&cell->height, height, 1, 0);
	cell->row = row;
	cell->column = column;
	cell->visited = 0;
	cell->basins_idx = square_well->cells_n;
	if (height == 1) {
		square_well->source_cell = cell;
	}
	square_well->used[height-1] = 1;
	return 1;
}

void add_le_neighbours(cell_t *cell, square_well_t *square_well) {
	if (cell->column > 0) {
		check_le_link(cell, cell-1, square_well);
	}
	if (cell->row > 0) {
		check_le_link(cell, cell-square_well->columns_n, square_well);
	}
	if (cell->column < square_well->columns_n-1) {
		check_le_link(cell, cell+1, square_well);
	}
	if (cell->row < square_well->rows_n-1) {
		check_le_link(cell, cell+square_well->columns_n, square_well);
	}
}

void check_le_link(cell_t *from, cell_t *to, square_well_t *square_well) {
	if (compare_fractions(&from->height, &to->height) >= 0) {
		add_to_le_queue(to, square_well);
	}
}

void add_to_le_queue(cell_t *cell, square_well_t *square_well) {
	if (cell->visited == 0) {
		cell->visited = 1;
		square_well->le_queue[square_well->le_queue_size++] = cell;
	}
}

void set_basin(basin_t *basin, square_well_t *square_well) {
	basin->lt_borders_n = 0;
	set_fraction(&basin->border_min, square_well->cells_n+1, 1, 0);
	basin->size = 0;
}

void add_eq_neighbours(cell_t *cell, basin_t *basin, square_well_t *square_well) {
	if (cell->column > 0) {
		check_eq_link(cell, cell-1, basin, square_well);
	}
	if (cell->row > 0) {
		check_eq_link(cell, cell-square_well->columns_n, basin, square_well);
	}
	if (cell->column < square_well->columns_n-1) {
		check_eq_link(cell, cell+1, basin, square_well);
	}
	if (cell->row < square_well->rows_n-1) {
		check_eq_link(cell, cell+square_well->columns_n, basin, square_well);
	}
}

void check_eq_link(cell_t *from, cell_t *to, basin_t *basin, square_well_t *square_well) {
	if (compare_fractions(&from->height, &to->height) == 0) {
		add_to_eq_queue(to, basin, square_well);
	}
}

void add_to_eq_queue(cell_t *cell, basin_t *basin, square_well_t *square_well) {
	if (cell->visited == 1) {
		if ((cell->column > 0 && is_lt_link(cell, cell-1)) || (cell->row > 0 && is_lt_link(cell, cell-square_well->columns_n)) || (cell->column < square_well->columns_n-1 && is_lt_link(cell, cell+1)) || (cell->row < square_well->rows_n-1 && is_lt_link(cell, cell+square_well->columns_n))) {
			basin->lt_borders_n++;
		}
		if (cell->column > 0) {
			set_border_min(cell, cell-1, basin);
		}
		if (cell->row > 0) {
			set_border_min(cell, cell-square_well->columns_n, basin);
		}
		if (cell->column < square_well->columns_n-1) {
			set_border_min(cell, cell+1, basin);
		}
		if (cell->row < square_well->rows_n-1) {
			set_border_min(cell, cell+square_well->columns_n, basin);
		}
		cell->visited = 2;
		square_well->eq_queue[square_well->eq_queue_offset+basin->size++] = cell;
	}
}

int is_lt_link(cell_t *from, cell_t *to) {
	return compare_fractions(&from->height, &to->height) > 0;
}

void set_border_min(cell_t *from, cell_t *to, basin_t *basin) {
	if (compare_fractions(&from->height, &to->height) < 0 && compare_fractions(&to->height, &basin->border_min) < 0) {
		basin->border_min = to->height;
	}
}

void set_basin_volume(basin_t *basin, fraction_t *height) {
	basin->height = *height;
	subtract_fractions(&basin->border_min, &basin->height, &basin->depth);
	multiply_fraction_int(&basin->depth, basin->size, &basin->volume);
}

void update_basin_volume(basin_t *basin, fraction_t *new_border_min) {
	if (compare_fractions(new_border_min, &basin->border_min) < 0) {
		basin->border_min = *new_border_min;
		subtract_fractions(&basin->border_min, &basin->height, &basin->depth);
		multiply_fraction_int(&basin->depth, basin->size, &basin->volume);
	}
}

void set_basin_new_height(basin_t *basin, fraction_t *new_volume) {
	fraction_t new_depth;
	divide_fraction_int(new_volume, basin->size, &new_depth);
	add_fractions(&basin->height, &new_depth, &basin->new_height);
}
