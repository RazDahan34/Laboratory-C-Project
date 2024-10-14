#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>

/**
 * @brief Enumeration of error categories.
 *
 * This enum defines the various categories of errors that can occur
 * during the assembly process.
 */
typedef enum {
    ERR_MEMORY,     /**< Memory allocation or management errors */
    ERR_FILE_INPUT, /**< Errors related to input file operations */
    ERR_FILE_OUTPUT,/**< Errors related to output file operations */
    ERR_SYNTAX,     /**< Syntax errors in the assembly code */
    ERR_SEMANTIC,   /**< Semantic errors in the assembly code */
    ERR_MACRO,      /**< Errors related to macro definitions or expansions */
    ERR_OVERFLOW,   /**< Numeric overflow errors */
    ERR_SYMBOL      /**< Errors related to symbol definitions or references */
} ErrorCategory;

/**
 * @brief Structure to hold information about an error.
 */
typedef struct {
    ErrorCategory category; /**< The category of the error */
    const char *description; /**< A description of the error */
} ErrorInfo;

/**
 * @brief Log an error with the specified details.
 *
 * This function logs an error with the given category, message, filename,
 * and line number. It stores the error information and prints it to stderr.
 *
 * @param category The category of the error.
 * @param specific_message A specific message describing the error.
 * @param filename The name of the file where the error occurred.
 * @param line_number The line number where the error occurred.
 */
void log_error(ErrorCategory category, const char *specific_message, const char *filename, int line_number);

/**
 * @brief Print a summary of all logged errors.
 *
 * This function prints a summary of all errors that have been logged
 * during the assembly process.
 */
void print_error_summary(void);

#endif /* ERROR_HANDLING_H */
