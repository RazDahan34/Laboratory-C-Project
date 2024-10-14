#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utilities.h"
#include "symbol_table.h"
#include "opcode_table.h"
#include "error_handling.h"

/* Function implementations */

/**
 * @brief Check if a token is a label.
 * 
 * This function checks if the given token is a label
 * 
 * @param token The token to check.
 * @return true if the token is a label, false otherwise.
 */
bool is_label(const char *token) {

    if (token == NULL || *token == '\0') {
        return false;
    }

    if (strlen(token) > MAX_LABEL_LENGTH) {
        return false;
    }

    if (!isalpha(*token)) {
        return false;
    }

    if (is_register(token) || get_opcode(token) != -1) {
        return false;
    }

    for (const char *p = token + 1; *p != '\0'; p++) {
        if (!isalnum(*p)) {
            return false;
        }
    }
    

    return true;
}


/**
 * @brief Check if a token is a directive.
 * 
 * This function checks if the given token is a valid assembler directive
 * (.data, .string, .entry, or .extern).
 * 
 * @param token The token to check.
 * @return true if the token is a directive, false otherwise.
 */
bool is_directive(const char *token) {
    return (token[0] == '.' && (strcmp(token, ".data") == 0 ||
                                strcmp(token, ".string") == 0 ||
                                strcmp(token, ".entry") == 0 ||
                                strcmp(token, ".extern") == 0));
}

/**
 * @brief Get the addressing mode of an operand.
 * 
 * This function determines the addressing mode of the given operand:
 * 0 - Immediate, 1 - Direct, 2 - Register indirect, 3 - Register direct
 * 
 * @param operand The operand to check.
 * @return The addressing mode (0-3) or -1 if invalid.
 */
int get_addressing_mode(const char *operand) {
    if (operand == NULL || operand[0] == '\0' || (operand[0] == ' ' && operand[1] == '\0'))
       return 4; /* No operand, empty string, or just a space */
    if (operand[0] == '#' && is_number(operand)) return 0;  /* Immediate */
    if (operand[0] == 'r' && is_register(operand)) return 3;  /* Register direct */
    if (operand[0] == '*' && operand[1] == 'r' && is_register(operand + 1)) return 2;  /* Register indirect */
    return 1;  /* Direct (label) */
}

/**
 * @brief Get the length of an instruction.
 * 
 * This function calculates the length of an instruction in words based on
 * the operation and its operands.
 * 
 * @param operation The operation part of the instruction.
 * @param operands The operands part of the instruction.
 * @return The length of the instruction in words.
 */
int get_instruction_length(const char *operation, const char *operands) {
    int length = 1;  
    int operand_count = 0;
    if (operands != NULL) {
        char source[MAX_LABEL_LENGTH + 1] = {0};
        char target[MAX_LABEL_LENGTH + 1] = {0};
        /*
         * Parse the operands string to separate source and target operands.
         * The %[^,] format specifier reads until a comma is encountered.
         */
        sscanf(operands, "%[^,], %s", source, target);
        trim(source);
        trim(target);           
        int source_mode = get_addressing_mode(source);
        int target_mode = get_addressing_mode(target);
        /* Adds extra words for operands that need them */
        if (source[0] != '\0') {
                length++;
                operand_count++;
        }

        if (target[0] != '\0') {
                length++;
                operand_count++;
        }
        /* Check if operand count matches the opcode's expected operand count */
        if (operand_count != get_operand_count(operation)) {
            return -1;
        }
        /* Special case: if both operands are registers (direct or indirect),
         * they can be encoded in a single additional word.
         */
        if ((source_mode == 3 || source_mode == 2) &&
        (target_mode == 3 || target_mode == 2)) {
                length = 2;  
        }
    }

    return length;
}

/**
 * @brief Count the number of data values in a data directive.
 * 
 * This function counts the number of comma-separated values in the operands
 * of a data directive.
 * 
 * @param operands The operands of the data directive.
 * @return The number of data values.
 */
int count_data_values(const char *operands) {
    int count = 0;
    /*
     * Use strtok to split the operands string by commas.
     * Each token represents a separate data value.
     */
    char *token = strtok((char *)operands, ",");
    while (token) {
        count++;
        token = strtok(NULL, ",");
    }
    return count;
}
/**
 * @brief Encode an instruction into machine code.
 *
 * @param operation The operation part of the instruction.
 * @param operands The operands part of the instruction.
 * @param symbol_table Pointer to the symbol table.
 * @param address The current address of the instruction.
 * @return The encoded Instruction structure.
 */
Instruction encode_instruction(const char *operation, const char *operands, SymbolTable *symbol_table, int address) {
    Instruction inst = {0};
    inst.opcode = get_opcode(operation);
    
    char source[MAX_LABEL_LENGTH + 1] = {0};
    char target[MAX_LABEL_LENGTH + 1] = {0};
    if (operands != NULL) {
        sscanf(operands, "%[^,], %s", source, target);
        trim(source);
        trim(target);
    }
    /* If there's only one operand, treat it as the target */
    if (target[0] == '\0') {
        strcpy(target, source);
        source[0] = '\0';
    }
    inst.source_addressing = get_addressing_mode(source);
    inst.target_addressing = get_addressing_mode(target);

    /* Set A.R.E field for the main instruction word */
    inst.are = 4; 
    unsigned int source_are = 4, target_are = 4;
    if (inst.source_addressing != 4) /* There are only 4 methods from 0 to 3, if it 4 so it is not method */
    inst.source_operand = encode_operand(source, symbol_table, &source_are, address, true);
    if (inst.target_addressing != 4)
    inst.target_operand = encode_operand(target, symbol_table, &target_are, address, false);
    if (inst.target_operand == -1 || inst.source_operand == -1)
    inst.opcode = -1;

    inst.source_are = source_are;
    inst.target_are = target_are;
    return inst;
}

/**
 * @brief Encode an operand into machine code.
 *
 * @param operand The operand to encode.
 * @param symbol_table Pointer to the symbol table.
 * @param are Pointer to the A.R.E. value.
 * @param address The current address.
 * @param is_source True if this is the source operand, false if it's the target.
 * @return The encoded operand value.
 */
int encode_operand(const char *operand, SymbolTable *symbol_table, unsigned int *are, int address, bool is_source) {

    /* Get the addressing mode of the operand */
    int mode = get_addressing_mode(operand);
    switch (mode) {
        case 0: 
            /* Immediate addressing mode */
            *are = 4; /* Set A.R.E. to absolute */
            return atoi(operand + 1) & 0xFFF; /* Convert to integer and mask to 12 bits */

        case 1: 
            /* Direct addressing mode */
            {
                Symbol *symbol = find_symbol(symbol_table, operand);
                if (symbol) {
                    if (symbol->type == SYMBOL_TYPE_EXTERNAL) {
                        /* Handle external symbol */
                        *are = 1; /* Set A.R.E. to external */
                        add_external_reference(&symbol_table->external_table, operand, address + (is_source ? 1 : 2));
                        return 1; /* Return 1 for external symbols */
                    } else {
                        /* Handle internal symbol */
                        *are = 2; /* Set A.R.E. to relocatable */
                        return symbol->address;
                    }
                } else {
                    /* Symbol not found in the symbol table */
                    return -1;
                }
            }

        case 2: 
            /* Indirect register addressing mode */
            *are = 4; /* Set A.R.E. to absolute */
            return get_register_number(operand + 1) & 0x7; /* Get register number and mask to 3 bits */

        case 3: 
            /* Direct register addressing mode */
            *are = 4; /* Set A.R.E. to absolute */
            return get_register_number(operand) & 0x7; /* Get register number and mask to 3 bits */

        default:
            /* Invalid addressing mode */
            return -1;
    }
}

/**
 * @brief Write an encoded instruction to a file.
 *
 * @param file The file to write to.
 * @param inst The encoded instruction.
 * @param address The address of the instruction.
 */
void write_instruction(FILE *file, Instruction inst, int address) {
    unsigned int first_word = 0;   
    first_word |= (inst.opcode & 0xF) << 11; /* Opcode encoding starting from bit 11 */
    if (inst.source_addressing != 4) {
        first_word |= (1 << (7 + inst.source_addressing)); /* Starting from the bit 7 that represent the source method */
    }
    if (inst.target_addressing != 4) {
       first_word |= (1 << (3 + inst.target_addressing)); /* Starting from the bit 3 that represent the target method */
    }   
    first_word |= 4; /* The A.R.E is always 4 in first word */
    fprintf(file, "%04d %05o\n", address, first_word & 0x7FFF);

    int add_words = 1;
    /* Handle additional words for operands */
    if ((inst.source_addressing == 2 || inst.source_addressing == 3) &&
        (inst.target_addressing == 2 || inst.target_addressing == 3)) {
        /* Both operands are registers (direct or indirect) */
        unsigned int reg_word = (inst.source_operand & 0x7) << 6 | 
                                (inst.target_operand & 0x7) << 3 | 
                                4; /* Creating one word that common to both source and target */
        fprintf(file, "%04d %05o\n", address + 1, reg_word);
    } else {
        /* Handle source operand if present */
        if (inst.source_addressing != 4) {
            unsigned int source_word = 0;
            if (inst.source_are == 1) { /* Handle external */
                source_word |= inst.source_are & 0x7; 
            }
            else if (inst.source_addressing == 1) { /* Direct */
               source_word = (inst.source_operand & 0xFFF) << 3;
               source_word |= inst.source_are & 0x7; /* Use provided A.R.E. */
            } else { /* Register (direct or indirect) */
               source_word = (inst.source_operand & 0x7) << 6;
               source_word |= 4; 
            }
            fprintf(file, "%04d %05o\n", address + 1, source_word);
            add_words ++;
        }

        /* Handle target operand if present */
        if (inst.target_addressing != 4) {
             unsigned int target_word = 0;
             if (inst.target_are == 1) { /* Handle external */
                target_word |= inst.target_are & 0x7; 
            } else if (inst.target_addressing == 0) { /* Immediate */
                target_word = (inst.target_operand & 0xFFF) << 3;
                target_word |= 4;
            } else if (inst.target_addressing == 1) { /* Direct */
               target_word = (inst.target_operand & 0xFFF) << 3;
               target_word |= inst.target_are & 0x7; /* Use provided A.R.E. */
            } else { 
            target_word = (inst.target_operand & 0x7) << 3;
            target_word |= 4; 
            }
            fprintf(file, "%04d %05o\n", address + add_words, target_word);
        }
    }
}


/**
 * @brief Check if a string represents a number.
 * 
 * This function checks if the given string represents a valid number,
 * including handling of the '#' prefix and optional sign.
 * 
 * @param str The string to check.
 * @return true if the string is a number, false otherwise.
 */
bool is_number(const char* str) {
    if (*str == '#') str++; 
    if (*str == '-' || *str == '+') str++; 
    /* Check if we've reached the end of the string */
    if (*str == '\0') return false;

    /* Check if the rest of the string contains only digits */
    while (*str != '\0') {
        if (*str < '0' || *str > '9') return false;
        str++;
    }

    return true;
}
/**
 * @brief Write data values to a file.
 *
 * @param file The file to write to.
 * @param data The data string to write.
 * @param address Pointer to the current address (will be updated).
 */
void write_data(FILE *file, const char *data, int *address) {
    char *token = strtok((char *)data, ",");
    while (token != NULL) {
        int value = atoi(token);
        fprintf(file, "%04d %05o\n", *address, (unsigned short)value & 0x7FFF);
        (*address)++;
        token = strtok(NULL, ",");
    }
}

/**
 * @brief Write a string to a file.
 *
 * @param file The file to write to.
 * @param operands The string operand to write.
 * @param address Pointer to the current address (will be updated).
 */
void write_string(FILE *file, const char *operands, int *address) {
    const char *str = operands + 1;  
    while (*str != '"' && *str != '\0') {
        fprintf(file, "%04d %05o\n", *address, (unsigned char)*str);
        str++;
        (*address)++;
    }
    fprintf(file, "%04d %05o\n", *address, 0);  
    (*address)++;
}

/**
 * @brief Trim whitespace from the beginning and end of a string.
 * 
 * This function removes leading and trailing whitespace from the given string.
 * 
 * @param str The string to trim.
 */
void trim(char *str) {
    if (str == NULL) return;
    /* Remove leading whitespace */  
    char *start = str;
    while(isspace((unsigned char)*start)) start++;
    /* If the string is all whitespace, make it an empty string */
    if(*start == 0) {
        *str = '\0';
        return;
    }
    /* Remove trailing whitespace */
    char *end = str + strlen(str) - 1;
    while(end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    /* If there was leading whitespace, shift the string to the left */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/**
 * @brief Handle extra spaces in a string.
 * 
 * This function removes extra spaces from the given string, ensuring that
 * there is only one space between words and no spaces around commas.
 * 
 * @param str The string to process.
 */
void handle_extra_spaces(char *str) {
    char *read = str;
    char *write = str;
    int in_word = 0;
    /*
     * This loop processes the string character by character:
     * - Collapses multiple spaces into a single space
     * - Removes spaces before and after commas
     * - Preserves other characters
     */
    while (*read) {
        if (isspace((unsigned char)*read)) {
            if (in_word) {
                *write++ = ' ';
                in_word = 0;
            }
        } else if (*read == ',') {
            if (write > str && *(write-1) == ' ') write--;
            *write++ = ',';
            in_word = 0;
        } else {
            *write++ = *read;
            in_word = 1;
        }
        read++;
    }
    
    if (write > str && *(write-1) == ' ') write--;
    *write = '\0';
}

/**
 * @brief Handle comments in a string.
 * 
 * This function removes comments from the given string by truncating
 * the string at the first occurrence of a semicolon.
 * 
 * @param str The string to process.
 */
void handle_comment(char *str) {
    char *comment_start = strchr(str, ';');
    if (comment_start) {
        *comment_start = '\0';
    }
}

/**
 * @brief Check if a string represents a register.
 * 
 * This function checks if the given string represents a valid register
 * (r0 through r7).
 * 
 * @param str The string to check.
 * @return true if the string is a register, false otherwise.
 */
bool is_register(const char *str) {
    return (str[0] == 'r' && str[1] >= '0' && str[1] <= '7' && str[2] == '\0');
}

/**
 * @brief Get the register number from a register string.
 * 
 * This function extracts the register number from a valid register string.
 * 
 * @param reg The register string.
 * @return The register number (0-7) or -1 if invalid.
 */
int get_register_number(const char *reg) {
    if (reg[0] == 'r' && reg[1] >= '0' && reg[1] <= '7' && reg[2] == '\0') {
        return reg[1] - '0';
    }
    return -1;  
}

/**
 * @brief Validate a string constant.
 * 
 * This function checks if the given string is a valid string constant,
 * enclosed in double quotes with no internal double quotes.
 * 
 * @param str The string to validate.
 * @return true if the string is valid, false otherwise.
 */
bool validate_string(const char *str) {
    int len = strlen(str);
    if (len < 2 || str[0] != '"' || str[len-1] != '"') {
        return false;
    }
    
    for (int i = 1; i < len - 1; i++) {
        if (str[i] == '"') {
            return false;  
        }
    }
    
    return true;
}
