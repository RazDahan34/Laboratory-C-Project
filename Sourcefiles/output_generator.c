#include <stdio.h>
#include <string.h>
#include "output_generator.h"
#include "error_handling.h"

/**
 * Generates all output files for the assembler, including .ob, .ent, and .ext files.
 * @param input_filename The name of the input file, used to derive output file names.
 * @param symbol_table Pointer to the symbol table containing all symbols and their information.
 * @param IC The final Instruction Counter value.
 * @param DC The final Data Counter value.
 */
void generate_output(const char *input_filename, SymbolTable *symbol_table, int IC, int DC) {
    char base_name[MEMORY_SIZE];
    strncpy(base_name, input_filename, sizeof(base_name));
    
    /* Remove the file extension from the base name */
    char *dot = strrchr(base_name, '.');
    if (dot) *dot = '\0';

    /* Generate the object file */
    generate_ob_file(base_name, symbol_table, IC, DC);

    /* Generate the entry file if there are entry symbols */
    if (symbol_table->has_entries) {
        generate_ent_file(base_name, symbol_table);
    }
    
    /* Generate the external file if there are external symbols */
    if (symbol_table->has_externs) {
        generate_ext_file(base_name, symbol_table);
    }
}

/**
 * Generates the object (.ob) file containing the encoded instructions and data.
 * @param base_name The base name for the output file (without extension).
 * @param symbol_table Pointer to the symbol table (unused in current implementation).
 * @param IC The final Instruction Counter value.
 * @param DC The final Data Counter value.
 */
void generate_ob_file(const char *base_name, SymbolTable *symbol_table, int IC, int DC) {
    char temp_filename[MEMORY_SIZE];
    char ob_filename[MEMORY_SIZE];
    snprintf(temp_filename, sizeof(temp_filename), "temp.ob");
    snprintf(ob_filename, sizeof(ob_filename), "%s.ob", base_name);
    
    FILE *temp_file = fopen(temp_filename, "r");
    FILE *ob_file = fopen(ob_filename, "w");
    if (!temp_file || !ob_file) {
        printf("Error: Unable to open temporary file or create output file %s\n", ob_filename);
        return;
    }

    /* Write the IC and DC values to the object file */
    fprintf(ob_file, "%d %d\n", IC - FIRST_ADDRESS, DC);
    char line[MEMORY_SIZE];
    
    /* Copy the contents of the temporary file to the object file */
    while (fgets(line, sizeof(line), temp_file)) {
        fputs(line, ob_file);
    }

    fclose(temp_file);
    fclose(ob_file);
    remove(temp_filename);  /* Remove the temporary file */
}

/**
 * Generates the entry (.ent) file listing all entry symbols and their addresses.
 * @param base_name The base name for the output file (without extension).
 * @param symbol_table Pointer to the symbol table containing entry symbols.
 */
void generate_ent_file(const char *base_name, SymbolTable *symbol_table) {
    if (!symbol_table->has_entries) return;

    char filename[MEMORY_SIZE];
    snprintf(filename, sizeof(filename), "%s.ent", base_name);
    FILE *file = fopen(filename, "w");
    if (!file) {
        log_error(ERR_FILE_OUTPUT, "Failed to create .ent file", filename, 0);
        return;
    }

    /* Write all entry symbols and their addresses to the file */
    for (int i = 0; i < symbol_table->size; i++) {
        if (symbol_table->symbols[i].type == SYMBOL_TYPE_ENTRY) {
            fprintf(file, "%s %04d\n", symbol_table->symbols[i].name, symbol_table->symbols[i].address);
        }
    }

    fclose(file);
}

/**
 * Generates the external (.ext) file listing all external symbol references and their addresses.
 * @param base_name The base name for the output file (without extension).
 * @param symbol_table Pointer to the symbol table containing external symbol references.
 */
void generate_ext_file(const char *base_name, SymbolTable *symbol_table) {
    if (!symbol_table->has_externs) return;  

    char filename[MEMORY_SIZE];
    snprintf(filename, sizeof(filename), "%s.ext", base_name);
    FILE *file = fopen(filename, "w");
    if (!file) {
        log_error(ERR_FILE_OUTPUT, "Failed to create .ext file", filename, 0);
        return;
    }

    /* Write all external symbol references and their addresses to the file */
    for (int i = 0; i < symbol_table->external_table.count; i++) {
        ExternalSymbol *ext = &symbol_table->external_table.externals[i];
        for (int j = 0; j < ext->reference_count; j++) {
            fprintf(file, "%s %04d\n", ext->name, ext->references[j]);
        }
    }

    fclose(file);
}
