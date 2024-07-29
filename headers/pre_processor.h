/* The C Programming Language Course Final Project, Semester 2024B, Open University Israel */
/* Name: Menahem Hen Perets, ID: 318246014 */
#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

#include "assembler.h"

/**
 * @brief Prints an internal error message and exits the program.
 * 
 * This function prints an internal error message to stdout and terminates the program
 * with a failure status.
 * @param msg The error message to be printed.
 */
void printErrorInternal(const char *msg);

/**
 * @brief Allocates memory and checks for allocation failure.
 * 
 * This function allocates memory of the specified size using malloc.
 * If the allocation fails, it prints an internal error message and exits the program.
 * @param size The size of the memory to allocate.
 * @return Pointer to the allocated memory.
 */
char *mallocAllocateAndCheck(size_t size);

/**
 * @brief Saves the content of a macro from a file.
 *
 * This function reads the content of a macro from the given file pointer starting from the specified position.
 * It increments the line count for each line read and calculates the total length of the macro.
 * @param fp Pointer to the file.
 * @param pos Position in the file to start reading from.
 * @param line_count Pointer to the line count.
 * @return Pointer to the saved macro content.
 */
char *saveMacroContent(FILE *fp, fpos_t *pos, int *line_count);

/**
 * @brief Validates a macro declaration.
 *
 * This function checks if the given string contains a valid macro declaration.
 * It extracts the macro name and allocates memory for it.
 * @param str The string containing the macro declaration.
 * @param name Pointer to store the macro name.
 * @param line_count The current line number.
 * @param file_name The name of the file.
 * @return 1 if the macro declaration is valid, 0 otherwise.
 */
int validMacroDeclaration(char *str, char **name, int line_count, char *file_name);

/**
 * @brief Adds macros from a file to a linked list.
 *
 * This function reads a file and adds any macro declarations to the provided linked list.
 * It validates each macro declaration and saves its content.
 * @param file_name The name of the file to read.
 * @param head Pointer to the head of the linked list.
 * @return 1 if macros were successfully added, 0 otherwise.
 */
int addMacros(char *file_name, node **head);

/**
 * @brief Replaces a macro with its content in a string.
 *
 * This function searches for a macro in the given string and replaces it with its content.
 * @param str The string containing the macro.
 * @param macr Pointer to the macro node.
 * @return A new string with the macro replaced, or NULL if the macro was not found.
 */
char *replaceMacro(char *str, node *macr);

/**
 * @brief Processes macro calls in the input file and replaces them with their corresponding content.
 *
 * This function reads the input file line by line, checks for macro declarations, and skips them.
 * For other lines, it replaces any macro calls with the corresponding macro content from the macro list.
 * The modified lines are written to a temporary file, which then replaces the original input file.
 * @param input_file The name of the input file to process.
 * @param head The head of the linked list containing macros and their content.
 */
void processMacroCalls(char *input_file, node *head);

/**
 * @brief Executes macro processing on the given file.
 *
 * This function processes the macros in the input file, replaces macro calls with their content,
 * and creates a new .am file with the processed content. It uses a linked list to store macros
 * and their content, and replaces the macro calls in the file with the corresponding content.
 * @param file_name The name of the file to process.
 * @return Returns 1 if the macro processing is successful, otherwise returns 0.
 */
int macroExecute(char *file_name);

#endif