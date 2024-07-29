/* The C Programming Language Course Final Project, Semester 2024B */
/* Menahem Hen Perets, ID: 318246014 */
#include "assembler.h"
#include "pre_processor.h"
#include "utility.h"
#include "sort_text.h"

void printErrorInternal(const char *msg)
{
    fprintf(stdout, "Internal Error: %s\n", msg); /* Print the internal error message and exit. */
    exit(EXIT_FAILURE);
}

char *mallocAllocateAndCheck(size_t size)
{
    char *ptr = (char *)malloc(size);
    if (!ptr)
    {
        printErrorInternal("ERROR: Failed to allocate memory");
    }
    return ptr;
}

char *saveMacroContent(FILE *fp, fpos_t *pos, int *line_count)
{
    char str[MAX_LINE_LENGTH];
    int macro_length = 0;
    char *macro;

    fsetpos(fp, pos);
    while (fgets(str, MAX_LINE_LENGTH, fp) && strcmp(str, "endmacr\n") != 0)
    {
        (*line_count)++; /* Increment line count for each line read. */
        macro_length += strlen(str); /* Calculate total length of the macro. */
    }
    fsetpos(fp, pos);

    if (feof(fp))
    {
        printErrorInternal("ERROR: No macro declaration ending found");
        return NULL;
    }

    macro = mallocAllocateAndCheck(macro_length + 1);
    macro[0] = '\0';
    while (fgets(str, MAX_LINE_LENGTH, fp) && strcmp(str, "endmacr\n") != 0)
    {
        strcat(macro, str); /* Concatenate each line to macro. */
    }
    return macro;
}

int validMacroDeclaration(char *str, char **name, int line_count, char *file_name)
{
    char *temp_name = strtok(NULL, " \n");
    if (!temp_name)
    {
        printErrorInternal("ERROR: Macro declaration with no word");
        return 0;
    }
    *name = mallocAllocateAndCheck(strlen(temp_name) + 1);
    strcpy(*name, temp_name); /* Copy the macro name. */
    return 1;
}

int addMacros(char *file_name, node **head)
{
    FILE *fp = fopen(file_name, "r");
    char str[MAX_LINE_LENGTH];
    char *name, *content;
    int line_count = 0;
    fpos_t pos;

    if (!fp)
    {
        printErrorInternal("ERROR: Failed to open file");
        return 0;
    }

    while (fgets(str, MAX_LINE_LENGTH, fp))
    {
        line_count++; /* Increment line count for each line read. */
        if (strcmp(strtok(str, " "), "macr") == 0)
        {
            if (!validMacroDeclaration(str, &name, line_count, file_name))
            {
                fclose(fp);
                return 0;
            }
            fgetpos(fp, &pos);
            content = saveMacroContent(fp, &pos, &line_count);
            if (!content)
            {
                fclose(fp);
                return 0;
            }
            addToTheList(head, name, content, line_count);
        }
    }
    fclose(fp);
    return 1;
}

char *replaceMacro(char *str, node *macr)
{
    char *pos = strstr(str, macr->name);
    size_t new_len;
    char *new_str;

    if (!pos)
    {
        return NULL;
    }
    new_len = strlen(str) + strlen(macr->content) - strlen(macr->name) + 1;
    new_str = mallocAllocateAndCheck(new_len);
    strncpy(new_str, str, pos - str);
    new_str[pos - str] = '\0';
    strcat(new_str, macr->content); /* Concatenate macro content. */
    strcat(new_str, pos + strlen(macr->name)); /* Concatenate remaining string. */
    return new_str;
}

void processMacroCalls(char *input_file, node *head)
{
    FILE *fp_in = fopen(input_file, "r"); /* Open the input file for reading. */
    FILE *fp_out = fopen("temp_output_file", "w"); /* Open a temporary file for writing. */
    char str[MAX_LINE_LENGTH];
    char *modified_str;
    char *token;
    node *current;

    if (!fp_in || !fp_out)
    {
        printErrorInternal("ERROR: Failed to open file");
        if (fp_in) fclose(fp_in);
        if (fp_out) fclose(fp_out);
        return;
    }

    while (fgets(str, MAX_LINE_LENGTH, fp_in))
    {
        char *original_str = stringDuplicate(str); /* Duplicate the original string. */

        token = strtok(str, " \n");

        if (token && strcmp(token, "macr") == 0) /* Check for macro declaration. */
        {
            while (fgets(str, MAX_LINE_LENGTH, fp_in))
            {
                token = strtok(str, " \n");
                if (token && strcmp(token, "endmacr") == 0)
                {
                    break; /* Skip lines until the end of macro declaration. */
                }
            }
            continue;
        }

        current = head;
        while (current != NULL)
        {
            modified_str = replaceMacro(original_str, current); /* Replace macros in the line. */
            if (modified_str)
            {
                strcpy(original_str, modified_str);
                free(modified_str);
            }
            current = current->next;
        }
        fprintf(fp_out, "%s", original_str); /* Write the modified line to the temporary file.*/
        free(original_str);
    }

    fclose(fp_in); /* Close the input file. */
    fclose(fp_out); /* Close the temporary file. */

    remove(input_file); /* Remove the original input file. */
    rename("temp_output_file", input_file); /* Rename the temporary file to the original input file. */
}

int macroExecute(char *file_name)
{
    node *head = NULL;
    char *new_file_name = addNewFile(file_name, ".am"); /* Create the new .am file name. */

    if (!addMacros(file_name, &head))
    {
        free(new_file_name);
        return 0;
    }

    processMacroCalls(file_name, head); /* Process macro calls in the file. */

    if (!copyFile(new_file_name, file_name))
    {
        printErrorInternal("Failed to copy processed file to new file");
        freeList(head);
        free(new_file_name);
        return 0;
    }

    freeList(head); /* Free the macro list. */
    printf("Macro execution completed, output file: %s\n", new_file_name);
    free(new_file_name); /* Free the new file name. */
    return 1;
}