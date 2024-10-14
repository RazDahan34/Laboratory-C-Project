#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_assembler.h"
#include "opcode_table.h"
#include "utilities.h"
#include "error_handling.h"


/**
 * Initializes the macro table by setting its count and capacity to 0 and its macros pointer to NULL.
 */
void init_macro_table() {
    macro_table.macros = NULL;
    macro_table.count = 0;
    macro_table.capacity = 0;
}

/**
 * Adds a new macro to the macro table. It expands the table if necessary, allocates memory for the new macro,
 * and stores its name and content.
 * @param name The name of the macro to add.
 * @param file The input file from which to read the macro content.
 * @param line_number Pointer to the current line number, updated as the macro is read.
 * @param filename The name of the input file, used for error reporting.
 */
void add_macro(const char *name, FILE *file, int *line_number, const char *filename) {
    /* Expand macro table if necessary */
    if (macro_table.count == macro_table.capacity) {
        int new_capacity = macro_table.capacity == 0 ? 1 : macro_table.capacity * 2;
        Macro *new_macros = realloc(macro_table.macros, new_capacity * sizeof(Macro));
        if (!new_macros) {
            log_error(ERR_MEMORY, "Failed to allocate memory for macro table", filename, *line_number);
            return;
        }
        macro_table.macros = new_macros;
        macro_table.capacity = new_capacity;
    }

    /* Initialize new macro */
    Macro *macro = &macro_table.macros[macro_table.count++];
    macro->name = strdup(name);
    macro->lines = NULL;
    macro->line_count = 0;
    macro->line_capacity = 0;

    char line[MAX_LINE_LENGTH + 1];
    /* Read macro content until 'endmacr' is encountered */
    while (fgets(line, sizeof(line), file)) {
        (*line_number)++;
        char trimmed_line[MAX_LINE_LENGTH + 1];
        strcpy(trimmed_line, line);
        trim(trimmed_line);
        
        if (strcmp(trimmed_line, "endmacr") == 0) {
            /* Rewind file pointer to before 'endmacr' line */
            fseek(file, -strlen(line), SEEK_CUR);
            (*line_number)--;
            return;
        }
        
        /* Expand macro lines array if necessary */
        if (macro->line_count == macro->line_capacity) {
            int new_capacity = macro->line_capacity == 0 ? 1 : macro->line_capacity * 2;
            char **new_lines = realloc(macro->lines, new_capacity * sizeof(char*));
            if (!new_lines) {
                log_error(ERR_MEMORY, "Failed to allocate memory for macro lines", filename, *line_number);
                return;
            }
            macro->lines = new_lines;
            macro->line_capacity = new_capacity;
        }
        macro->lines[macro->line_count++] = strdup(line);
    }
}

/**
 * Frees all memory allocated for the macro table, including individual macros and their content.
 */
void free_macro_table() {
    for (int i = 0; i < macro_table.count; i++) {
        free(macro_table.macros[i].name);
        for (int j = 0; j < macro_table.macros[i].line_count; j++) {
            free(macro_table.macros[i].lines[j]);
        }
        free(macro_table.macros[i].lines);
    }
    free(macro_table.macros);
}

/**
 * Checks if a given word is a reserved word in the assembly language, including opcodes, registers, and directives.
 * @param word The word to check.
 * @return true if the word is reserved, false otherwise.
 */
bool is_reserved_word(const char *word) {
    /* Check if the word is an opcode or register */
    if (get_opcode(word) != -1) return true;
    if (is_register(word)) return true;
    
    /* Check against list of reserved words */
    const char *reserved[] = {"macr", "endmacr", "data", "string", "entry", "extern"};
    for (int i = 0; i < sizeof(reserved) / sizeof(reserved[0]); i++) {
        if (strcmp(word, reserved[i]) == 0) return true;
    }
    return false;
}

/**
 * Validates a macro name by checking if it starts with a letter, contains only alphanumeric characters,
 * and is not a reserved word.
 * @param name The macro name to validate.
 * @return true if the name is valid, false otherwise.
 */
bool is_valid_macro_name(const char *name) {
    /* Check if the first character is a letter */
    if (!isalpha(name[0])) return false;
    /* Check if all characters are alphanumeric */
    for (int i = 1; name[i] != '\0'; i++) {
        if (!isalnum(name[i])) return false;
    }
    /* Check if it's not a reserved word */
    return !is_reserved_word(name);
}

/**
 * Expands a macro by writing its content to the output file. If the macro is not found, it prints a debug message.
 * @param name The name of the macro to expand.
 * @param output The output file to write the expanded macro content.
 */
void expand_macro(const char *name, FILE *output) {
    for (int i = 0; i < macro_table.count; i++) {
        if (strcmp(macro_table.macros[i].name, name) == 0) {
            for (int j = 0; j < macro_table.macros[i].line_count; j++) {
                fputs(macro_table.macros[i].lines[j], output);
            }
            return;
        }
    }
}

/**
 * Performs the pre-assembly process by reading the input file, handling macro definitions and expansions,
 * and writing the processed code to an output file.
 * @param input_filename The name of the input file to process.
 * @return The name of the expanded output file, or NULL on error.
 */
char *pre_assembler(const char *input_filename) {
    size_t len = strlen(input_filename);
    /* Allocate memory for the expanded filename */
    char *expanded_filename = malloc(strlen(input_filename) + 4);
    if (!expanded_filename) {
        log_error(ERR_MEMORY, "Failed to allocate memory for expanded filename", input_filename, 0);
        return NULL;
    }
    /* Copy the input filename (without .as) */
    strncpy(expanded_filename, input_filename, len - 3);
    expanded_filename[len - 3] = '\0';  

    strcat(expanded_filename, ".am");

    /* Open input and output files */
    FILE *input = fopen(input_filename, "r");
    FILE *output = fopen(expanded_filename, "w");

    if (!input || !output) {
        log_error(ERR_FILE_INPUT, "Failed to open input or output file", input_filename, 0);
        free(expanded_filename);
        return NULL;
    }

    init_macro_table();

    char line[MAX_LINE_LENGTH + 1];
    char macro_name[MAX_MACRO_NAME + 1];
    int line_number = 0;
    bool error = false;

    /* Process input file line by line */
    while (fgets(line, sizeof(line), input)) {
        line_number++;
        char trimmed_line[MAX_LINE_LENGTH + 1];
        strcpy(trimmed_line, line);
        trim(trimmed_line);

        /* Check for line length exceeding maximum */
        if (strlen(trimmed_line) > MAX_LINE_LENGTH) {
            log_error(ERR_SYNTAX, "Line exceeds maximum length", input_filename, line_number);
            error = true;
            continue;
        }

        /* Handle macro definition */
        if (strncmp(trimmed_line, "macr", 4) == 0) {
            sscanf(trimmed_line + 4, "%s", macro_name);
            if (is_valid_macro_name(macro_name)) {
                add_macro(macro_name, input, &line_number, input_filename);
            } else {
                log_error(ERR_MACRO, "Invalid macro name", input_filename, line_number);
                error = true;
            }
           continue;
        }

        /* Skip 'endmacr' lines */
        if (strcmp(trimmed_line, "endmacr") == 0) {
            continue;
        }

        /* Check for macro expansion */
        int is_macro = 0;
        for (int i = 0; i < macro_table.count; i++) {
            if (strcmp(trimmed_line, macro_table.macros[i].name) == 0) {
                expand_macro(macro_table.macros[i].name, output);
                is_macro = 1;
                break;
            }
        }

        /* Write non-macro lines to output */
        if (!is_macro) {
            fputs(line, output);
       }
    }

    fclose(input);
    fclose(output);
    if (error) return NULL;
    return expanded_filename;
}
