#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

#define MAX_LABEL_LENGTH 31
#define MAX_EXTERNAL_REFERENCES 100

/**
 * @brief Enumeration of symbol types.
 */
typedef enum {
    SYMBOL_TYPE_CODE,
    SYMBOL_TYPE_DATA,
    SYMBOL_TYPE_ENTRY,
    SYMBOL_TYPE_EXTERNAL
} SymbolType;

/**
 * @brief Represents a symbol in the symbol table.
 */
typedef struct {
    char name[MAX_LABEL_LENGTH + 1];
    int address;
    SymbolType type;
} Symbol;

/**
 * @brief Represents an external symbol and its references.
 */
typedef struct {
    char name[MAX_LABEL_LENGTH + 1];
    int references[MAX_EXTERNAL_REFERENCES];
    int reference_count;
} ExternalSymbol;

/**
 * @brief Represents the table of external symbols.
 */
typedef struct {
    ExternalSymbol *externals;
    int count;
    int capacity;
} ExternalTable;

/**
 * @brief Represents the symbol table.
 */
typedef struct {
    Symbol *symbols;
    int size;
    int capacity;
    bool has_entries;
    bool has_externs;
    ExternalTable external_table;
} SymbolTable;

/**
 * @brief Initializes the symbol table.
 * @param table Pointer to the symbol table to initialize.
 */
void init_symbol_table(SymbolTable *table);

/**
 * @brief Adds a symbol to the symbol table.
 * @param table Pointer to the symbol table.
 * @param name The name of the symbol.
 * @param address The address of the symbol.
 * @param type The type of the symbol.
 * @return true if the symbol was added successfully, false if it already exists.
 */
bool add_symbol(SymbolTable *table, const char *name, int address, SymbolType type, const char *filename, int line_number);

/**
 * @brief Finds a symbol in the symbol table.
 * @param table Pointer to the symbol table.
 * @param name The name of the symbol to find.
 * @return Pointer to the found symbol, or NULL if not found.
 */
Symbol *find_symbol(SymbolTable *table, const char *name);

/**
 * @brief Frees the memory allocated for the symbol table.
 * @param table Pointer to the symbol table to free.
 */
void free_symbol_table(SymbolTable *table);

/**
 * @brief Initializes the external symbol table.
 * @param table Pointer to the external table to initialize.
 */
void init_external_table(ExternalTable *table);

/**
 * @brief Adds an external symbol reference to the external table.
 * @param table Pointer to the external table.
 * @param name The name of the external symbol.
 * @param address The address where the symbol is referenced.
 */
void add_external_reference(ExternalTable *table, const char *name, int address);

/**
 * @brief Frees the memory allocated for the external table.
 * @param table Pointer to the external table to free.
 */
void free_external_table(ExternalTable *table);

#endif 
