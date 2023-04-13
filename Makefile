
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm
EXE1 = allocate
EXE2 = process
OBJ = process_data.o process_scheduling.o memory_allocation.o linked_list.o min_heap.o

all: $(EXE1) $(EXE2)
# linking object files to executable
$(EXE1): src/main.c $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE1) $(OBJ) $< $(LDFLAGS)

$(EXE2): src/process.c
	$(CC) $(CFLAGS) $< -o $(EXE2)

# compiling all object files
%.o: src/%.c src/%.h
	$(CC) -c -o $@ $< $(CFLAGS)

# removing files
clean:
	rm -f *.o $(EXE1) $(EXE2)