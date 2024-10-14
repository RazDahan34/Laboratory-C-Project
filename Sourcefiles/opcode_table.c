#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcode_table.h"

static const OpcodeEntry opcode_table[] = {
    {"mov", 0, 2},
    {"cmp", 1, 2},
    {"add", 2, 2},
    {"sub", 3, 2},
    {"lea", 4, 2},
    {"clr", 5, 1},
    {"not", 6, 1},
    {"inc", 7, 1},
    {"dec", 8, 1},
    {"jmp", 9, 1},
    {"bne", 10, 1},
    {"red", 11, 1},
    {"prn", 12, 1},
    {"jsr", 13, 1},
    {"rts", 14, 0},
    {"stop", 15, 0}
};
/**
 * Searches the opcode table for a given mnemonic and returns its opcode.
 * @param mnemonic The instruction mnemonic to look up.
 * @return The opcode if the mnemonic is found, -1 otherwise.
 */
int get_opcode(const char *mnemonic) {
    for (size_t i = 0; i < sizeof(opcode_table) / sizeof(opcode_table[0]); i++) {
        if (strcmp(opcode_table[i].mnemonic, mnemonic) == 0) {
            return opcode_table[i].opcode;
        }
    }
    return -1;  
}
/**
 * Searches the opcode table for a given mnemonic and returns the number of operands it expects.
 * @param mnemonic The instruction mnemonic to look up.
 * @return The number of operands if the mnemonic is found, -1 otherwise.
 */
int get_operand_count(const char *mnemonic) {
    for (size_t i = 0; i < sizeof(opcode_table) / sizeof(opcode_table[0]); i++) {
        if (strcmp(opcode_table[i].mnemonic, mnemonic) == 0) {
            return opcode_table[i].operands;
        }
    }
    return -1;  
}
