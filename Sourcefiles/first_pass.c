#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "first_pass.h"
#include "utilities.h"
#include "second_pass.h"
#include "opcode_table.h"
#include "symbol_table.h"
#include "pre_assembler.h"
#include "error_handling.h"

/**
 * Performs the first pass of the assembler. It reads the input file line by line, processes labels,
 * directives, and instructions, updates the symbol table, and keeps track of the instruction and
 * data counters (IC and DC).
 * @param filename The name of the input file to process.
 * @param symbol_table Pointer to the symbol table to be populated during the first pass.
 * @return true if the first pass was successful, false if errors were encountered.
 */
bool first_pass(const char *filename, SymbolTable *symbol_table) {
    /* Initialize instruction counter (IC) and data counter (DC) */
    int IC = FIRST_ADDRESS;
    int DC = 0;

    FILE *file = fopen(filename, "r");
    if (!file) {
        log_error(ERR_FILE_INPUT, "Failed to open file for reading", filename, 0);
        return false;
    }

    char line[MAX_LINE_LENGTH + 1];
    char label[MAX_LABEL_LENGTH + 1];
    char operation[MAX_LABEL_LENGTH + 1];
    char operands[MAX_LINE_LENGTH + 1];
    int line_number = 0;
    bool error_found = false;

    /* Process each line of the input file */
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        handle_comment(line);
        handle_extra_spaces(line);
        trim(line);      
      
        if (line[0] == '\0') {
            continue;  /* Skip empty lines */
        }
        
        /* Check for line length exceeding maximum */
        if (strlen(line) > MAX_LINE_LENGTH) {
            log_error(ERR_SYNTAX, "Line exceeds maximum length", filename, line_number);
            error_found = true;
            continue;
        }

        /* Reset variables for each line */
        label[0] = '\0';
        operation[0] = '\0';
        operands[0] = '\0';

        /* Parse the line into label, operation, and operands */
        char *token = strtok(line, " \t\n");
        if (token != NULL && token[strlen(token) - 1] == ':') {
            token[strlen(token) - 1] = '\0';  /* Remove colon from label */
            strncpy(label, token, MAX_LABEL_LENGTH);
           /* Check if the label is legal */
            if (!is_label(label)) {
                log_error(ERR_SYNTAX, "Illegal label", filename, line_number);
                error_found = true;
                continue;
            }
            token = strtok(NULL, " \t");  /* Get next token for operation */
        }

        if (token != NULL) {
            strncpy(operation, token, MAX_LABEL_LENGTH);
            token = strtok(NULL, "\n");
            if (token != NULL) {
                strncpy(operands, token, MAX_LINE_LENGTH);
                trim(operands);
            }
        }

        /* Process directives */
        if (is_directive(operation)) {
            if (strcmp(operation, ".data") == 0) {
                /* Handle .data directive */
                if (label[0] != '\0') {
                    if (!add_symbol(symbol_table, label, DC, SYMBOL_TYPE_DATA, filename, line_number)) {
                        error_found = true;
                    }
                }
                int data_count = count_data_values(operands);
                if (data_count == -1) {
                    log_error(ERR_SYNTAX, "Invalid .data directive", filename, line_number);
                    error_found = true;
                } else {
                    DC += data_count;
                }
            } else if (strcmp(operation, ".string") == 0) {
                /* Handle .string directive */
                if (label[0] != '\0') {
                    if (!add_symbol(symbol_table, label, DC, SYMBOL_TYPE_DATA, filename, line_number)) {
                        error_found = true;
                    }
                }
                if (!validate_string(operands)) {
                    log_error(ERR_SYNTAX, "Invalid .string directive", filename, line_number);
                    error_found = true;
                } else {
                    DC += strlen(operands) - 2 + 1;  /* Length of string + null terminator - quotes */
                }
            } else if (strcmp(operation, ".entry") == 0) {
                /* Mark that the file has entry points */
                symbol_table->has_entries = true;
            } else if (strcmp(operation, ".extern") == 0) {
                /* Handle .extern directive */
                trim(operands);
                if (operands[0] == '\0') {
                    log_error(ERR_SYNTAX, "Missing operand for .extern directive", filename, line_number);
                    error_found = true;
                } else {
                    /* Add each external symbol to the symbol table */
                    char *token = strtok(operands, ", \t");
                    while (token != NULL) {
                        if (!add_symbol(symbol_table, token, 0, SYMBOL_TYPE_EXTERNAL, filename, line_number)) {
                            log_error(ERR_SYMBOL, "Duplicate external symbol definition", filename, line_number);
                            error_found = true;
                        }
                        token = strtok(NULL, ", \t");
                    }
                    symbol_table->has_externs = true;
                }
            }
        } else if (get_opcode(operation) != -1) {
            /* Process instructions */
            int inst_length = get_instruction_length(operation, operands);
            if (inst_length == -1) {
                log_error(ERR_SYNTAX, "Invalid instruction format", filename, line_number);
                error_found = true;
            } else {
                if (label[0] != '\0') {
                    if (!add_symbol(symbol_table, label, IC, SYMBOL_TYPE_CODE, filename, line_number)) {
                        error_found = true;
                    }
                }
                IC += inst_length; /* Updating IC */
            }
        } else {
            log_error(ERR_SYNTAX, "Unknown operation", filename, line_number);
            error_found = true;
        }
    }

    free_macro_table(); /* Free macro table after using it to compare it to the symbol table */
    fclose(file);

    /* Update addresses of data symbols */
    for (int i = 0; i < symbol_table->size; i++) {
        if (symbol_table->symbols[i].type == SYMBOL_TYPE_DATA) {
            symbol_table->symbols[i].address += IC;
        }
    }

    /* Perform second pass if no errors were found in the first pass */
    if (!error_found) {
        second_pass(filename, symbol_table, IC, DC);
    }

    return !error_found;
}
