/* The C Programming Language Course Final Project, Semester 2024B, Open University Israel */
/* Name: Menahem Hen Perets, ID: 318246014 */
#ifndef UTILITY_H
#define UTILITY_H

#include "assembler.h"

/**
 * Duplicates a string by allocating memory and copying the original string.
 * @param original The original string to be duplicated.
 * @return A pointer to the newly allocated and duplicated string.
 */
char *stringDuplicate(const char *original);

/**
 * Adds a new node to the beginning of a linked list.
 * @param head A pointer to the head of the linked list.
 * @param name The name to be stored in the new node.
 * @param content The content to be stored in the new node.
 * @param line The line number associated with the node.
 */
void addToTheList(node **head, char *name, char *content, int line);

/**
 * Prints an error message with the line number.
 * @param lineNum The line number where the error occurred.
 * @param format The format string for the error message.
 * @param ... Additional arguments for the format string.
 */
void printError(int lineNum, const char *format, ...);

/**
 * Frees all nodes in a linked list.
 * @param head A pointer to the head of the linked list to be freed.
 */
void freeList(node *head);

/**
 * Searches for a label in the global label array and returns a pointer to it if found.
 * @param labelName The name of the label to search for.
 * @return A pointer to the label if found, NULL otherwise.
 */
labelInfo *getLabel(char *labelName);

/**
 * Searches for a command in the global command array and returns its ID if found.
 * @param cmdName The name of the command to search for.
 * @return The ID of the command if found, -1 otherwise.
 */
int getCmdId(char *cmdName);

/**
 * Handles unexpected crashes by cleaning up resources.
 * @param args_count The number of arguments provided.
 * @param ... Variable arguments: strings ("%s") followed by their pointers, and FILE pointers.
 */
void unexpectedCrash(int args_count, ...);

/**
 * Removes leading spaces from a string.
 * @param ptStr A pointer to the string to be trimmed.
 */
void trimLeftStr(char **ptStr);

/**
 * Removes leading and trailing spaces from a string.
 * @param ptStr A pointer to the string to be trimmed.
 */
void trimStr(char **ptStr);

/**
 * Returns the first token in a string and updates the end of the token.
 * @param str The input string.
 * @param endOfTok A pointer to update with the end of the token.
 * @return A pointer to the start of the first token.
 */
char *getFirstTok(char *str, char **endOfTok);

/**
 * Checks if a string contains only one word.
 * @param str The input string.
 * @return TRUE if the string contains only one word, FALSE otherwise.
 */
bool isOneWord(char *str);

/**
 * Checks if a string contains only whitespace characters.
 * @param str The input string.
 * @return TRUE if the string contains only whitespace characters, FALSE otherwise.
 */
bool isWhiteSpaces(char *str);

/**
 * Checks if a label name is legal.
 * @param labelStr The label name to check.
 * @param lineNum The line number for error reporting.
 * @param printErrors Whether to print errors if the label is illegal.
 * @return TRUE if the label name is legal, FALSE otherwise.
 */
bool isLegalLabel(char *labelStr, int lineNum, bool printErrors);

/**
 * Checks if a label exists in the global label array.
 * @param label The label name to check.
 * @return TRUE if the label exists, FALSE otherwise.
 */
bool isExistingLabel(char *label);

/**
 * Checks if a label is already defined as an entry label.
 * @param labelName The label name to check.
 * @return TRUE if the label is already defined as an entry label, FALSE otherwise.
 */
bool isExistingEntryLabel(char *labelName);

/**
 * Checks if a string is a register name and updates the value if it is.
 * @param str The string to check.
 * @param value A pointer to update with the register value.
 * @return TRUE if the string is a register name, FALSE otherwise.
 */
bool isRegister(char *str, int *value);

/**
 * Checks if a string is an indirect register and updates the value if it is.
 * @param str The string to check.
 * @param value A pointer to update with the register value.
 * @return TRUE if the string is an indirect register, FALSE otherwise.
 */
bool isIndirectRegister(char *str, int *value);

/**
 * Checks if a line is a comment or empty.
 * @param line A pointer to the line information structure.
 * @return TRUE if the line is a comment or empty, FALSE otherwise.
 */
bool isCommentOrEmpty(lineInfo *line);

/**
 * Returns the first operand in a line and updates the end of the operand.
 * @param line The input line.
 * @param endOfOp A pointer to update with the end of the operand.
 * @param foundComma A pointer to update if a comma is found.
 * @return A pointer to the start of the first operand.
 */
char *getFirstOperand(char *line, char **endOfOp, bool *foundComma);

/**
 * Checks if a command is a directive.
 * @param cmd The command string to check.
 * @return TRUE if the command is a directive, FALSE otherwise.
 */
bool isDirective(char *cmd);

/**
 * Checks if a string parameter is a legal string and removes the quotes.
 * @param strParam A pointer to the string parameter.
 * @param lineNum The line number for error reporting.
 * @return TRUE if the string parameter is legal, FALSE otherwise.
 */
bool isLegalStringParam(char **strParam, int lineNum);

/**
 * Checks if a number string is a legal number and updates its value.
 * @param numStr The number string to check.
 * @param numOfBits The number of bits for the number.
 * @param lineNum The line number for error reporting.
 * @param value A pointer to update with the number value.
 * @return TRUE if the number string is legal, FALSE otherwise.
 */
bool isLegalNum(char *numStr, int numOfBits, int lineNum, int *value);

/**
 * Returns the length of a number in octal representation.
 * @param num The input number.
 * @return The length of the number in octal representation.
 */
int getNumOctalLength(int num);

/**
 * Returns the length of a number in decimal representation.
 * @param num The input number.
 * @return The length of the number in decimal representation.
 */
int getNumDecimalLength(int num);

/**
 * Converts a decimal number to octal.
 * @param decimalNumber The decimal number to convert.
 * @return The octal representation of the number.
 */
int convertDecimalToOctal(int decimalNumber);

/**
 * Prints the destination value as a decimal with at least 4 digits.
 * @param file The file to print to.
 * @param num The number to print.
 */
void fprintfDest(FILE *file, int num);

/**
 * Prints the instruction or data counter value as a decimal.
 * @param file The file to print to.
 * @param num The number to print.
 */
void fprintfICDC(FILE *file, int num);

/**
 * Prints the entry value as a decimal.
 * @param file The file to print to.
 * @param num The number to print.
 */
void fprintfEnt(FILE *file, int num);

/**
 * Prints the data as an octal representation with 5 digits.
 * @param file The file to print to.
 * @param num The number to print.
 */
void fprintfData(FILE *file, int num);

/**
 * Prints the external value as a decimal.
 * @param file The file to print to.
 * @param num The number to print.
 */
void fprintfExt(FILE *file, int num);

/**
 * Creates a file with a given name and ending, and returns a pointer to it.
 * @param name The base name of the file.
 * @param ending The ending to append to the file name.
 * @param mode The mode to open the file with.
 * @return A pointer to the created file.
 */
FILE *openFile(char *name, char *ending, const char *mode);

/**
 * Removes the specified extension from a filename.
 * @param filename The original filename (null-terminated string).
 * @param extension The extension to remove (null-terminated string).
 * @return A new string with the extension removed if found, otherwise the original filename.
 *         The caller is responsible for freeing the returned string.
 */
char *stripExtension(char *filename, const char *extension);

/**
 * Creates the object file (.ob) with the given name, instruction count, data count, and memory array.
 * @param name The base name of the file.
 * @param IC The instruction count.
 * @param DC The data count.
 * @param memoryArr The memory array containing the data to write.
 */
void createObjectFile(char *name, int IC, int DC, int *memoryArr);

/**
 * Creates the entries file (.ent) with the given name, containing addresses for entry labels.
 * @param name The base name of the file.
 */
void createEntriesFile(char *name);

/**
 * Creates the extern file (.ext) with the given name, containing addresses for extern label operands.
 * @param name The base name of the file.
 * @param linesArr The array of line information structures.
 * @param linesFound The number of lines found.
 */
void createExternFile(char *name, lineInfo *linesArr, int linesFound);

/**
 * Resets all global variables and frees all allocated memory blocks.
 * @param linesArr The array of line information structures.
 * @param linesFound The number of lines found.
 * @param dataCount The number of data items.
 */
void clearData(lineInfo *linesArr, int linesFound, int dataCount);

/**
 * Parses a file after macro expansion and processes it to generate the necessary output files.
 * @param fileName The name of the file to parse.
 */
void parseFile(char *fileName);

/**
 * Creates a new file name by replacing the extension of the original file name with a new extension.
 * @param file_name The original file name.
 * @param new_extension The new extension to append to the file name.
 * @return A pointer to the newly allocated string containing the new file name.
 */
char *addNewFile(char *file_name, char *new_extension);

/**
 * Copies the contents of one file to another.
 * @param file_name_dest The name of the destination file.
 * @param file_name_orig The name of the source file.
 * @return Returns 1 if the file copy is successful, otherwise returns 0.
 */
int copyFile(char *file_name_dest, char *file_name_orig);

#endif