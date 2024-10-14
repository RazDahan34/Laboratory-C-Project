#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdbool.h>
#include "symbol_table.h"

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 31
#define FIRST_ADDRESS 100


/**
 * @brief Performs the first pass of the assembler.
 * @param filename The name of the input file.
 * @param symbol_table Pointer to the symbol table.
 * @return true if the first pass was successful, false otherwise.
 */
bool first_pass(const char *filename, SymbolTable *symbol_table);

#endif 
