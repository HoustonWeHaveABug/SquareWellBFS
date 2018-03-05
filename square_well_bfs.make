SQUARE_WELL_BFS_C_FLAGS=-O2 -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings
SQUARE_WELL_BFS_OBJS=square_well_bfs.o fractions.o

square_well_bfs: ${SQUARE_WELL_BFS_OBJS}
	gcc -o square_well_bfs ${SQUARE_WELL_BFS_OBJS}

square_well_bfs.o: square_well_bfs.c fractions.h square_well_bfs.make
	gcc -c ${SQUARE_WELL_BFS_C_FLAGS} -o square_well_bfs.o square_well_bfs.c

fractions.o: fractions.c fractions.h square_well_bfs.make
	gcc -c ${SQUARE_WELL_BFS_C_FLAGS} -o fractions.o fractions.c

clean:
	rm -f square_well_bfs ${SQUARE_WELL_BFS_OBJS}
