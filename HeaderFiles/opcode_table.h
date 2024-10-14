#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

/**
 * @brief Represents an entry in the opcode table.
 */
typedef struct {
    char mnemonic[5];
    int opcode;
    int operands;
} OpcodeEntry;

/**
 * @brief Gets the opcode for a given mnemonic.
 * @param mnemonic The mnemonic to look up.
 * @return The opcode if found, -1 otherwise.
 */
int get_opcode(const char *mnemonic);

/**
 * @brief Gets the number of operands for a given mnemonic.
 * @param mnemonic The mnemonic to look up.
 * @return The number of operands if found, -1 otherwise.
 */
int get_operand_count(const char *mnemonic);

#endif 
