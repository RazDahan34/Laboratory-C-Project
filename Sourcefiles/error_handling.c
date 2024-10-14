#include "error_handling.h"
#include <stdlib.h>
#include <string.h>

#define MAX_ERRORS 100 

/** 
 * @brief Array to store logged errors
 * This static array stores information about all logged errors up to MAX_ERRORS.
 */
static ErrorInfo error_log[MAX_ERRORS];

/**
 * @brief Counter for the number of logged errors
 * This static variable keeps track of how many errors have been logged.
 */
static int error_count = 0;

void log_error(ErrorCategory category, const char *specific_message, const char *filename, int line_number) {
    /* Check if we haven't exceeded the maximum number of errors we can log */
    if (error_count < MAX_ERRORS) {
        /* Store the error category */
        error_log[error_count].category = category;

        /* Allocate memory for the error description
         * The +50 is to account for the additional formatting characters
         * and potential line number length */
        char *description = malloc(strlen(specific_message) + strlen(filename) + 50);
        if (description == NULL) {
            /* If malloc fails, print an error message and return */
            fprintf(stderr, "Failed to allocate memory for error description\n");
            return;
        }

        /* Format the error description */
        if (line_number != - 1) {
        sprintf(description, "%s (File: %s, Line: %d)", specific_message, filename, line_number);
        }
        else {
        sprintf(description, "%s (File: %s)\n", specific_message, filename);
        }
        error_log[error_count].description = description;

        /* Increment the error count */
        error_count++;
    }

    
}

void print_error_summary() {
    /* Only print the summary if there are errors */
    if(error_count != 0) {
        printf("\nError Summary:\n");
        for (int i = 0; i < error_count; i++) {
            /* Print each error with its number, category, and description */
            printf("%d. [%s] %s\n", i + 1, 
                   error_log[i].category == ERR_MEMORY ? "Memory" :
                   error_log[i].category == ERR_FILE_INPUT ? "File Input" :
                   error_log[i].category == ERR_FILE_OUTPUT ? "File Output" :
                   error_log[i].category == ERR_SYNTAX ? "Syntax" :
                   error_log[i].category == ERR_SEMANTIC ? "Semantic" :
                   error_log[i].category == ERR_MACRO ? "Macro" :
                   error_log[i].category == ERR_OVERFLOW ? "Overflow" :
                   error_log[i].category == ERR_SYMBOL ? "Symbol" : "Unknown",
                   error_log[i].description);
        }
    }

    /* Free the allocated memory for error descriptions */
    for (int i = 0; i < error_count; i++) {
        free((void*)error_log[i].description);
    }
}
