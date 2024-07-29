/* The C Programming Language Course Final Project, Semester 2024B */
/* Menahem Hen Perets, ID: 318246014 */
#include "assembler.h"
#include "utility.h"
#include "pre_processor.h"

int tabOrSpaceCheck(char c)
{
    return (isspace(c) && c != '\n'); /* Check if the character is a tab or a space. */
}

void removeSpacesNearComma(char *str)
{
    char *ptr = str;
    if (*ptr == ',')
    {
        return;
    }

    while ((ptr = strchr(ptr, ',')) != NULL)
    {
        if (*(ptr - 1) == ' ') /* Check for spaces before comma. */
        {
            memmove(ptr - 1, ptr, strlen(ptr) + 1); /* Removes space before the comma. */
            if (*(ptr) == ' ')
            {
                memmove(ptr, ptr + 1, strlen(ptr + 1) + 1); /* Removes space after the comma. */
            }
        }
        else if (*(ptr + 1) == ' ')
        {
            memmove(ptr + 1, ptr + 2, strlen(ptr + 2) + 1); /* Moves the string starting from ptr + 2 one position to the left to ptr + 1. */
            ptr++;
        }
        else
        {
            ptr++;
        }
    }
}

void removeExtraSpacesString(char str[])
{
    char str_temp[MAX_LINE_LENGTH];
    int source_i, dest_i;
    source_i = dest_i = 0;

    while (tabOrSpaceCheck(*(str + source_i))) /* Remove space where the line starts. */
    {
        source_i++;
    }
    while (*(str + source_i) != '\0') /* As long as we are not at the end. */
    {
        while (!tabOrSpaceCheck(*(str + source_i)) && *(str + source_i) != '\0')
        {
            *(str_temp + dest_i) = *(str + source_i);
            source_i++;
            dest_i++;
        }
        if (*(str + source_i) == '\0') /* If we reached the end line. */
        {
            break;
        }
        while (tabOrSpaceCheck(*(str + source_i))) /* Skipping white spaces until we encounter another char. */
        {
            source_i++;
        }
        if (!(*(str + source_i) == '\n' || *(str + source_i) == '\0')) /* Reached end of line, copy a single space for those who were skipped. */
        {
            *(str_temp + dest_i) = ' ';
            dest_i++;
        }
    }
    *(str_temp + dest_i) = *(str + source_i);
    *(str_temp + dest_i + 1) = '\0';
    removeSpacesNearComma(str_temp);
    strcpy(str, str_temp);
}

/* Function to remove extra spaces from a file */
char *removeExtraSpacesFile(char file_name[])
{
    char str[MAX_LINE_LENGTH + 2]; /* +2 for \n and \0 */
    char *new_file_name;
    int line_number;
    FILE *source_pointer, *source_pointer_temp;

    source_pointer = fopen(file_name, "r"); /* Open file for reading. */
    if (source_pointer == NULL)
    {
        printf("ERROR: Failed to open the source file \"%s\" for reading.\n", file_name);
        return NULL;
    }

    new_file_name = addNewFile(file_name, ".temp1"); /* Creating new file name for the temporary file. */
    if (new_file_name == NULL)
    {
        fclose(source_pointer);
        printf("ERROR: Failed to allocate memory for the new file name.\n");
        return NULL;
    }

    source_pointer_temp = fopen(new_file_name, "w"); /* Open the temporary file for writing. */
    if (source_pointer_temp == NULL)
    {
        fclose(source_pointer);
        free(new_file_name);
        printf("ERROR: Failed to open the temporary file \"%s\" for writing.\n", new_file_name);
        return NULL;
    }

    line_number = 0;
    while (fgets(str, MAX_LINE_LENGTH + 2, source_pointer) != NULL)
    {
        line_number++;
        if (strlen(str) > MAX_LINE_LENGTH)
        {
            printf("ERROR: Line %d in file \"%s\" is too long.\n", line_number, file_name);
            fclose(source_pointer);
            fclose(source_pointer_temp);
            free(new_file_name);
            return NULL;
        }
        else if (*str == ';') /* Handle comment lines */
        {
            *str = '\n'; /* Replace comment line with a new line. */
            *(str + 1) = '\0';
        }
        else
        {
            removeExtraSpacesString(str); /* Remove extra white spaces from the line. */
        }
        fprintf(source_pointer_temp, "%s", str); /* Save changes to the temporary file. */
    }

    fclose(source_pointer); /* Close the source file. */
    fclose(source_pointer_temp); /* Close the temporary file. */
    return new_file_name;
}