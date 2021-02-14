OBJS	= rogue.o Dungeon/dungeon.o Dungeon/dijkstra.o Helpers/helpers.o Helpers/tree.o
SOURCE	= rogue.c Dungeon/dungeon.c Dungeon/dijkstra.c Helpers/helpers.c Helpers/tree.c
HEADER	= settings.h Dungeon/dungeon.h Dungeon/dijkstra.h Helpers/helpers.h Helpers/tree.h
OUT	= rogue
LFLAGS	 = -g3 -Wall -Wextra -pedantic -Werror

all: rogue

rogue: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) -c -o $@ $< $(LFLAGS)

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# run the program
run: $(OUT)
	./$(OUT)

# compile program with debugging information
debug: $(OUT)
	valgrind ./$(OUT)

# run program with valgrind for errors
valgrind: $(OUT)
	valgrind ./$(OUT)

# run program with valgrind for leak checks
valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full ./$(OUT)

# run program with valgrind for leak checks (extreme)
valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes ./$(OUT)