#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H

#include "symbol_table.h" 

#define MEMORY_SIZE 4096
#define FIRST_ADDRESS 100

/**
 * @brief Generates all output files for the assembler.
 * @param input_filename The name of the input file.
 * @param symbol_table Pointer to the symbol table.
 * @param IC Instruction Counter.
 * @param DC Data Counter.
 */
void generate_output(const char *input_filename, SymbolTable *symbol_table, int IC, int DC);

/**
 * @brief Generates the object (.ob) file.
 * @param base_name The base name for the output file.
 * @param symbol_table Pointer to the symbol table.
 * @param IC Instruction Counter.
 * @param DC Data Counter.
 */
void generate_ob_file(const char *base_name, SymbolTable *symbol_table, int IC, int DC);

/**
 * @brief Generates the entry (.ent) file.
 * @param base_name The base name for the output file.
 * @param symbol_table Pointer to the symbol table.
 */
void generate_ent_file(const char *base_name, SymbolTable *symbol_table);

/**
 * @brief Generates the external (.ext) file.
 * @param base_name The base name for the output file.
 * @param symbol_table Pointer to the symbol table.
 */
void generate_ext_file(const char *base_name, SymbolTable *symbol_table);

#endif 
