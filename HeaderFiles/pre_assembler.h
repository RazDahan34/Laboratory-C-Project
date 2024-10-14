#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include <stdio.h>
#include <stdbool.h>

#define MEMORY_SIZE 4096
#define MAX_LINE_LENGTH 80
#define MAX_MACRO_NAME 31
#define MAX_MACRO_LINES 100
#define MAX_FILENAME 100

/**
 * @brief Represents a macro with its name and content.
 */
typedef struct {
    char *name;
    char **lines;
    int line_count;
    int line_capacity;
} Macro;

/**
 * @brief Represents a table of macros.
 */
typedef struct {
    Macro *macros;
    int count;
    int capacity;
} MacroTable;


MacroTable macro_table;

/**
 * @brief Initializes the macro table.
 */
void init_macro_table(void);

/**
 * @brief Adds a macro to the macro table.
 * @param name The name of the macro.
 * @param file The input file.
 * @param line_number Pointer to the current line number.
 * @param filename The name of the input file.
 */
void add_macro(const char *name, FILE *file, int *line_number, const char *filename);

/**
 * @brief Frees the memory allocated for the macro table.
 */
void free_macro_table(void);

/**
 * @brief Checks if a word is a reserved word.
 * @param word The word to check.
 * @return true if the word is reserved, false otherwise.
 */
bool is_reserved_word(const char *word);

/**
 * @brief Checks if a macro name is valid.
 * @param name The macro name to check.
 * @return true if the name is valid, false otherwise.
 */
bool is_valid_macro_name(const char *name);

/**
 * @brief Expands a macro in the output file.
 * @param name The name of the macro to expand.
 * @param output The output file.
 */
void expand_macro(const char *name, FILE *output);

/**
 * @brief Performs the pre-assembly process.
 * @param input_filename The name of the input file.
 * @return The name of the expanded file, or NULL on error.
 */
char *pre_assembler(const char *input_filename);

#endif 
