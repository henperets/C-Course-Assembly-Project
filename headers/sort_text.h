/* The C Programming Language Course Final Project, Semester 2024B, Open University Israel */
/* Name: Menahem Hen Perets, ID: 318246014 */
#include "assembler.h"

/**
 * This function checks if a character is either a tab or a space.
 * @param c The character that is being.
 * @return 1 if the character is a tab or space, otherwise return 0.
 */
int tabOrSpaceCheck(char c);

/**
 * This function removes white spaces that are before or after commas in a given string.
 * @param str The pointer to where white spaces are found.
 */
void removeSpacesNearComma(char *str);

/**
 * This function removes extra white spaces from a string.
 * @param str The string from which extra white spaces will be removed.
 */
void removeExtraSpacesString(char str[]);

/**
 * This function removes all extra unnecessary white spaces from a specified file.
 * @param file_name The name of the file being examined for white spaces.
 * @return The name of the new file after removing extra white spaces.
 */
char *removeExtraSpacesFile(char file_name[]);