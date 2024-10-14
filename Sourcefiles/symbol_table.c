#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symbol_table.h"
#include "pre_assembler.h"
#include "error_handling.h"

/**
 * Initializes the symbol table with default values and allocates initial memory.
 * @param table Pointer to the symbol table to initialize.
 */
void init_symbol_table(SymbolTable *table) {
    table->size = 0;
    table->capacity = 10;
    table->has_entries = false;
    table->has_externs = false;
    table->symbols = malloc(sizeof(Symbol) * table->capacity);
    init_external_table(&table->external_table);
}

/**
 * Adds a new symbol to the symbol table. 
 * It checks for duplicate symbols and conflicts with macro names.
 * If necessary, it resizes the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param name The name of the symbol to add.
 * @param address The address associated with the symbol.
 * @param type The type of the symbol (e.g., code, data, entry, external).
 * @param filename The name of the current file (for error logging).
 * @param line_number The current line number (for error logging).
 * @return true if the symbol was successfully added, false otherwise.
 */
bool add_symbol(SymbolTable *table, const char *name, int address, SymbolType type, const char *filename, int line_number) {
    
    /* Check if the symbol already exists in the symbol table */
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            log_error(ERR_SYMBOL, "Duplicate symbol definition", filename, line_number);
            return false;
        }
    }
    
    /* Check if the symbol name conflicts with a macro name */
    for (int i = 0; i < macro_table.count; i++) {
        if (strcmp(macro_table.macros[i].name, name) == 0) {
            log_error(ERR_SYMBOL, "Symbol name conflicts with macro name", filename, line_number);
            return false;
        }
    }
    
    /* Resize the symbol table if necessary */
    if (table->size == table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);
        if (table->symbols == NULL) {
            log_error(ERR_MEMORY, "Failed to resize symbol table", filename, line_number);
            return false;
        }
    }

    /* Add the new symbol */
    Symbol *symbol = &table->symbols[table->size++];
    strncpy(symbol->name, name, MAX_LABEL_LENGTH);
    symbol->name[MAX_LABEL_LENGTH] = '\0';  /* Ensure null-termination */
    symbol->address = address;
    symbol->type = type;

    /* Update flags for entries and externals */
    if (type == SYMBOL_TYPE_ENTRY) {
        table->has_entries = true;
    } else if (type == SYMBOL_TYPE_EXTERNAL) {
        table->has_externs = true;
    }

    return true;
}

/**
 * Searches for a symbol in the symbol table by its name.
 * @param table Pointer to the symbol table to search.
 * @param name The name of the symbol to find.
 * @return Pointer to the found Symbol, or NULL if not found.
 */
Symbol *find_symbol(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}

/**
 * Frees the memory allocated for the symbol table.
 * @param table Pointer to the symbol table to free.
 */
void free_symbol_table(SymbolTable *table) {
    free(table->symbols);
    table->size = 0;
    table->capacity = 0;
}

/**
 * Initializes the external symbol table with default values and allocates initial memory.
 * @param table Pointer to the external table to initialize.
 */
void init_external_table(ExternalTable *table) {
    table->externals = malloc(sizeof(ExternalSymbol) * 10);
    table->count = 0;
    table->capacity = 10;
}

/**
 * Adds a reference to an external symbol. If the symbol doesn't exist in the table,
 * it creates a new entry. If it exists, it adds the address to the existing entry's references.
 * @param table Pointer to the external table.
 * @param name The name of the external symbol.
 * @param address The address where the external symbol is referenced.
 */
void add_external_reference(ExternalTable *table, const char *name, int address) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->externals[i].name, name) == 0) {
            if (table->externals[i].reference_count < MAX_EXTERNAL_REFERENCES) {
                table->externals[i].references[table->externals[i].reference_count++] = address;
            }
            return;
        }
    }

    if (table->count == table->capacity) {
        table->capacity *= 2;
        table->externals = realloc(table->externals, sizeof(ExternalSymbol) * table->capacity);
    }

    strcpy(table->externals[table->count].name, name);
    table->externals[table->count].references[0] = address;
    table->externals[table->count].reference_count = 1;
    table->count++;
}

/**
 * Frees the memory allocated for the external symbol table.
 * @param table Pointer to the external table to free.
 */
void free_external_table(ExternalTable *table) {
    free(table->externals);
    table->count = 0;
    table->capacity = 0;
}
