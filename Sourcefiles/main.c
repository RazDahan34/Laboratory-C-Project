/**
 * Assembler Project
 * 
 * Purpose:
 * This project implements a two-pass assembler for a custom assembly language.
 * The assembler converts assembly language source code into machine code,
 * generating object files that can be used by a linker or loader.
 * 
 * Key Features:
 * - Macro expansion: Supports definition and expansion of macros in the source code.
 * - Symbol resolution: Handles labels, entry points, and external references.
 * - Instruction encoding: Converts assembly instructions into binary machine code.
 * - Error detection and reporting: Identifies and reports syntax and semantic errors.
 * - Output generation: Produces object (.ob), entry (.ent), and external (.ext) files.
 * 
 * The assembler performs the following steps for each input file:
 * 1. Pre-assembly (macro expansion):
 *    - Expands macros defined in the source code.
 *    - Generates an intermediate file with expanded macros.
 * 
 * 2. First pass (symbol table creation and initial encoding):
 *    - Builds a symbol table with labels and their addresses.
 *    - Performs initial encoding of instructions and data.
 *    - Identifies entry points and external references.
 * 
 * 3. Second pass (final encoding and output generation):
 *    - Resolves symbol references using the symbol table.
 *    - Completes the encoding of instructions.
 *    - Generates the final object file and auxiliary files (.ent and .ext).
 * 
 * Usage: ./assembler <input_file1> [input_file2] ...
 * 
 * The program expects one or more input files as command-line arguments.
 * Each file is processed independently, and any errors encountered during
 * the assembly process are logged and reported at the end of execution.
 * 
 * Functions:
 * - main: The main entry point of the program. It iterates through the input files,
 *   calling the necessary functions for each stage of the assembly process.
 * 
 * Dependencies:
 * - pre_assembler.h: For macro expansion (pre_assembler function)
 * - first_pass.h: For the first pass of assembly (first_pass function)
 * - symbol_table.h: For symbol table management
 * - error_handling.h: For error logging and reporting
 * 
 * Output:
 * For each successfully assembled input file, the program generates:
 * - An object file (.ob) containing the encoded instructions and data.
 * - An entry file (.ent) listing all entry symbols, if any are defined.
 * - An external file (.ext) listing all external symbol references, if any are used.
 * 
 * Error handling:
 * The assembler detects and reports various types of errors, including:
 * - Syntax errors in the assembly code
 * - Undefined symbols
 * - Multiply defined symbols
 * - Invalid addressing modes or operands
 * Errors are logged during the assembly process and a summary is printed at the end.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assembler.h"
#include "symbol_table.h"
#include "first_pass.h"
#include "error_handling.h"


/**
 * The main function of the assembler program.
 * 
 * This function performs the following tasks:
 * 1. Checks for valid command-line arguments (input files).
 * 2. Iterates through each input file provided.
 * 3. For each file, it:
 *    a. Runs the pre-assembler to expand macros.
 *    b. Initializes a symbol table.
 *    c. Performs the first pass of assembly, that performs the second pass etc.
 *    d. Cleans up resources after processing each file.
 * 4. Prints a summary of any errors encountered during assembly.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 *             argv[0] is the program name, and subsequent elements are input filenames.
 * 
 * @return 0 if the program executes successfully, 1 if no input files were provided.
 * 
 * Note: The function continues processing subsequent files even if errors occur in one file.
 *       This allows for batch processing of multiple files, with errors reported at the end.
 */

int main(int argc, char *argv[]) {
    /* Check if at least one input file was provided */
    if (argc < 2) {
        log_error(ERR_FILE_INPUT, "No input files provided", "main", -1);
        return 1;
    }
    int valid_files = 0;
    int i = 1;
    /* Process each input file */
    while (i < argc) {
        char input_filename[MAX_FILENAME];
        char full_filename[MAX_FILENAME];

        /* Copy the input filename (without .as) */
        strncpy(input_filename, argv[i], sizeof(input_filename) - 1);
        input_filename[sizeof(input_filename) - 1] = '\0';  

        /* Create the full filename with .as extension for checking */
        snprintf(full_filename, sizeof(full_filename), "%s.as", input_filename);
        /* Check if the file with .as extension exists */
        FILE *file = fopen(full_filename, "r");
        if (!file) {
            log_error(ERR_FILE_INPUT, "Cannot open file (make sure it ends with .as)", full_filename, -1);
            i++;
            continue;
        }
        fclose(file);
        valid_files++;
        /* Step 1: Pre-assembly (macro expansion) */
        char *expanded_filename = pre_assembler(full_filename);
        if (!expanded_filename) {
            log_error(ERR_FILE_INPUT, "Pre-assembler failed", input_filename, -1);
            i++;
            continue;  /* Skip to the next file if pre-assembly fails */
        }
        printf("Pre-assembler done for file: %s\n", full_filename);
        /* Step 2: First pass */
        SymbolTable symbol_table;
        init_symbol_table(&symbol_table);
        if (!first_pass(expanded_filename, &symbol_table)) {
            log_error(ERR_SEMANTIC, "First pass failed", expanded_filename, -1);
            free(expanded_filename);
            free_symbol_table(&symbol_table);
            i++;
            continue;  /* Skip to the next file if first pass fails */
        }
        printf("First and second pass are done for file : %s\n", full_filename);
        /* Clean up resources */
        free(expanded_filename);
        free_symbol_table(&symbol_table);
        free_external_table(&symbol_table.external_table);
        i++;
       }

    if (valid_files == 0) {
        log_error(ERR_FILE_INPUT, "No valid input files to process", "main", -1);
        return 1;
    }
    /* Print a summary of all errors encountered during assembly */
    print_error_summary();
    
    return 0;
}
