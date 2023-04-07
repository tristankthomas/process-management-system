
CC = gcc
CFLAGS = -Wall -g
EXE = allocate
OBJ = process_data.o process_scheduling.o memory_allocation.o linked_list.o

# linking object files to executable
$(EXE): src/main.c $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $<

# compiling all object files
%.o: src/%.c src/%.h
	$(CC) -c -o $@ $< $(CFLAGS)

# removing files
clean:
	rm -f *.o $(EXE)