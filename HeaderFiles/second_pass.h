#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdbool.h>
#include "symbol_table.h"  

#define MAX_LINE_LENGTH 80
#define FIRST_ADDRESS 100

/**
 * @brief Performs the second pass of the assembler.
 * @param filename The name of the input file.
 * @param symbol_table Pointer to the symbol table.
 * @param IC Instruction Counter.
 * @param DC Data Counter.
 * @return true if the second pass was successful, false otherwise.
 */
bool second_pass(const char *filename, SymbolTable *symbol_table, int IC, int DC);

#endif 
