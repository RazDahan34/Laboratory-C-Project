#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "second_pass.h"
#include "utilities.h"
#include "opcode_table.h"
#include "output_generator.h"
#include "error_handling.h"

/**
 * Performs the second pass of the assembler. It processes the input file again,
 * resolving symbols, encoding instructions, and generating the object file.
 * @param filename The name of the input file to process.
 * @param symbol_table Pointer to the populated symbol table from the first pass.
 * @param IC The final Instruction Counter value from the first pass.
 * @param DC The final Data Counter value from the first pass.
 * @return true if the second pass was successful, false if errors were encountered.
 */
bool second_pass(const char *filename, SymbolTable *symbol_table, int IC, int DC) {
    FILE *file = fopen(filename, "r");
    FILE *ob_file = fopen("temp.ob", "w");
    if (!file || !ob_file) {
        log_error(ERR_FILE_INPUT, "Failed to open input or output file", filename, 0);
        return false;
    }

    char line[MAX_LINE_LENGTH + 1];
    int line_number = 0;
    int address = FIRST_ADDRESS;
    bool error_found = false;

    /* Process each line of the input file */
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        handle_comment(line);
        handle_extra_spaces(line);
        trim(line);

        if (line[0] == '\0') continue;  /* Skip empty lines */

        char *token = strtok(line, " \t");
        
        /* Skip label if present */
        if (token != NULL && token[strlen(token) - 1] == ':') {
            token = strtok(NULL, " \t");
        }

        /* Handle data and string directives */
        if (strcmp(token, ".data") == 0 || strcmp(token, ".string") == 0) {
            char *operands = strtok(NULL, "\n");
            if (strcmp(token, ".data") == 0) {
                write_data(ob_file, operands, &address);
            } else {
                write_string(ob_file, operands, &address);
            }
            continue;
        }

        /* Handle entry directive */
        if (strcmp(token, ".entry") == 0) {
           char *symbol_name = strtok(NULL, " \t\n");
           if (symbol_name) {
              Symbol *symbol = find_symbol(symbol_table, symbol_name);
               if (symbol) {
                   if (symbol->type != SYMBOL_TYPE_EXTERNAL) {
                       symbol->type = SYMBOL_TYPE_ENTRY;
                       symbol_table->has_entries = true;
                   } else {
                       log_error(ERR_SYMBOL, "Symbol declared as both .extern and .entry", filename, line_number);
                       error_found = true;
                   }
               } else {
                   log_error(ERR_SYMBOL, "Entry symbol not found in symbol table", filename, line_number);
                   error_found = true;
               }
           } else {
               log_error(ERR_SYNTAX, "Missing operand for .entry directive", filename, line_number);
               error_found = true;
           }
           continue;
        }

        /* Handle instructions */
        if (get_opcode(token) != -1) {
            char *operands = strtok(NULL, "\n");
            /* Encode the instruction */
            Instruction inst = encode_instruction(token, operands, symbol_table, address);
            if (inst.opcode == -1) {
                log_error(ERR_SYNTAX, "Failed to encode instruction", filename, line_number);
                error_found = true;
            } else {
                /* Write the encoded instruction to the object file */
                write_instruction(ob_file, inst, address);
                address += get_instruction_length(token, operands);
            }
        }
    }

    fclose(file);
    fclose(ob_file);

    if (error_found) {
        printf("Errors found during second pass. Assembly process halted.\n");
        return false;
    }

    /* Generate final output files */
    generate_output(filename, symbol_table, IC, DC);
    return true;
}
