#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdbool.h>
#include "symbol_table.h"
#define MAX_LABEL_LENGTH 31
#define MAX_LINE_LENGTH 80

typedef struct {
    unsigned int opcode;
    unsigned int source_addressing;
    unsigned int target_addressing;
    unsigned int are;
    unsigned int source_are;
    unsigned int target_are;
    unsigned int source_operand;
    unsigned int target_operand;
} Instruction;

/**
 * @brief Check if a token is a label.
 * @param token The token to check.
 * @return true if the token is a label, false otherwise.
 */
bool is_label(const char *token);

/**
 * @brief Check if a token is a directive.
 * @param token The token to check.
 * @return true if the token is a directive, false otherwise.
 */
bool is_directive(const char *token);

/**
 * @brief Get the addressing mode of an operand.
 * @param operand The operand to check.
 * @return The addressing mode (0-3) or -1 if invalid.
 */
int get_addressing_mode(const char *operand);

/**
 * @brief Get the length of an instruction.
 * @param operation The operation part of the instruction.
 * @param operands The operands part of the instruction.
 * @return The length of the instruction in words.
 */
int get_instruction_length(const char *operation, const char *operands);

/**
 * @brief Count the number of data values in a data directive.
 * @param operands The operands of the data directive.
 * @return The number of data values.
 */
int count_data_values(const char *operands);

/**
 * @brief Encode an instruction into machine code.
 * @param operation The operation part of the instruction.
 * @param operands The operands part of the instruction.
 * @param symbol_table Pointer to the symbol table.
 * @param address The current address of the instruction.
 * @return The encoded Instruction structure.
 */
Instruction encode_instruction(const char *operation, const char *operands, SymbolTable *symbol_table, int address);

/**
 * @brief Encode an operand into machine code.
 * @param operand The operand to encode.
 * @param symbol_table Pointer to the symbol table.
 * @param are Pointer to the A.R.E. value.
 * @param address The current address.
 * @param is_source True if this is the source operand, false if it's the target.
 * @return The encoded operand value.
 */
int encode_operand(const char *operand, SymbolTable *symbol_table, unsigned int *are, int address, bool is_source);

/**
 * @brief Write an encoded instruction to a file.
 * @param file The file to write to.
 * @param inst The encoded instruction.
 * @param address The address of the instruction.
 */
void write_instruction(FILE *file, Instruction inst, int address);

/**
 * @brief Write data values to a file.
 * @param file The file to write to.
 * @param data The data string to write.
 * @param address Pointer to the current address (will be updated).
 */
void write_data(FILE *file, const char *data, int *address);

/**
 * @brief Write a string to a file.
 * @param file The file to write to.
 * @param operands The string operand to write.
 * @param address Pointer to the current address (will be updated).
 */
void write_string(FILE *file, const char *operands, int *address);

/**
 * @brief Check if a string represents a number.
 * @param str The string to check.
 * @return true if the string is a number, false otherwise.
 */
bool is_number(const char* str);

/**
 * @brief Trim whitespace from the beginning and end of a string.
 * @param str The string to trim.
 */
void trim(char *str);

/**
 * @brief Handle extra spaces in a string.
 * @param str The string to process.
 */
void handle_extra_spaces(char *str);

/**
 * @brief Handle comments in a string.
 * @param str The string to process.
 */
void handle_comment(char *str);

/**
 * @brief Check if a string represents a register.
 * @param str The string to check.
 * @return true if the string is a register, false otherwise.
 */
bool is_register(const char *str);

/**
 * @brief Get the register number from a register string.
 * @param reg The register string.
 * @return The register number (0-7) or -1 if invalid.
 */
int get_register_number(const char *reg);

/**
 * @brief Validate a string constant.
 * @param str The string to validate.
 * @return true if the string is valid, false otherwise.
 */
bool validate_string(const char *str);

#endif 
