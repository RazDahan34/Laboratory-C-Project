CC = gcc
CFLAGS = -Wall -ansi -pedantic -std=gnu99
OBJECTS = main.o pre_assembler.o opcode_table.o first_pass.o second_pass.o utilities.o symbol_table.o error_handling.o output_generator.o
EXEC = assembler

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)

