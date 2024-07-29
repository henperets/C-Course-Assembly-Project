/* The C Programming Language Course Final Project, Semester 2024B */
/* Menahem Hen Perets, ID: 318246014 */
#include "assembler.h"
#include "pre_processor.h"
#include "utility.h"

char *stringDuplicate(const char *original)
{
    size_t length = strlen(original) + 1;
    char *duplicate = mallocAllocateAndCheck(length); /* Allocate memory for the new string. */
    if (duplicate)
    {
        strcpy(duplicate, original); /* Copy the original string to the newly allocated memory. */
    }
    return duplicate;
}

void addToTheList(node **head, char *name, char *content, int line)
{
    node *new_node = (node *)malloc(sizeof(node)); /* Allocate memory for a new node. */
    if (!new_node)
    {
        fprintf(stdout, "ERROR: Failed to allocate memory for new node.\n");
        exit(EXIT_FAILURE);
    }
    new_node->name = stringDuplicate(name); /* Duplicate the name string. */
    new_node->content = stringDuplicate(content); /* Duplicate the content string. */
    new_node->line = line;
    new_node->next = *head;
    *head = new_node; /* Insert the new node at the beginning of the list. */
}

void printError(int lineNum, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("At line %d: ", lineNum); /* Print the error message with the line number. */
    vprintf(format, args); /* Print the formatted error message. */
    printf("\n");
    va_end(args);
}

void freeList(node *head)
{
    node *temp;
    while (head)
    {
        temp = head;
        head = head->next;
        free(temp->name); /* Free the memory allocated for the name. */
        free(temp->content); /* Free the memory allocated for the content. */
        free(temp); /* Free the node itself. */
    }
}

labelInfo *getLabel(char *labelName)
{
    int i = 0;

    if (labelName)
    {
        for (i = 0; i < g_labelNum; i++)
        {
            if (strcmp(labelName, g_labelArr[i].name) == 0)
            {
                return &g_labelArr[i]; /* Return a pointer to the label if found. */
            }
        }
    }
    return NULL; /* Return NULL if the label is not found. */
}

int getCmdId(char *cmdName)
{
    int i = 0;

    while (g_cmdArr[i].name)
    {
        if (strcmp(cmdName, g_cmdArr[i].name) == 0)
        {
            return i; /* Return the command ID if found. */
        }

        i++;
    }
    return -1; /* Return -1 if the command is not found. */
}

void unexpectedCrash(int args_count, ...)
{
    char *str;
    int i;
    FILE *source_pointer;
    va_list args;
    va_start(args, args_count); /* Initialize the va_list with the argument count. */

    for (i = 0; i < args_count; i++)
    {
        if (strcmp(va_arg(args, char*), "%s") == 0)
        {
            i++;
            str = va_arg(args, char*); /* Get the next argument as a string. */
            remove(str); /* Remove the file associated with the string. */
            free(str); /* Free the memory allocated for the string. */
        }
        else
        {
            source_pointer = va_arg(args, FILE*); /* Get the next argument as a FILE pointer. */
            fclose(source_pointer); /* Close the file. */
        }
    }
    va_end(args); /* Clean up the va_list. */
}

void trimLeftStr(char **ptStr)
{
    if (!ptStr)
    {
        return; /* Return if the pointer is NULL. */
    }

    while (isspace(**ptStr))
    {
        ++*ptStr; /* Increment the pointer to skip leading whitespace. */
    }
}

void trimStr(char **ptStr)
{
    char *endofstring;

    if (!ptStr || **ptStr == '\0')
    {
        return; /* Return if the pointer is NULL or points to an empty string. */
    }

    trimLeftStr(ptStr); /* Remove leading whitespace. */

    endofstring = *ptStr + strlen(*ptStr) - 1; /* Find the end of the string. */

    while (isspace(*endofstring) && endofstring != *ptStr)
    {
        *endofstring-- = '\0'; /* Remove trailing whitespace. */
    }
}

char *getFirstTok(char *str, char **endOfTok)
{
    char *tokStart = str;
    char *tokEnd = NULL;

    trimLeftStr(&tokStart); /* Remove leading whitespace. */

    tokEnd = tokStart;
    while (*tokEnd != '\0' && !isspace(*tokEnd))
    {
        tokEnd++; /* Find the end of the first token. */
    }

    if (*tokEnd != '\0')
    {
        *tokEnd = '\0'; /* Null-terminate the token. */
        tokEnd++;
    }

    if (endOfTok)
    {
        *endOfTok = tokEnd; /* Update the endOfTok pointer. */
    }
    return tokStart; /* Return the start of the first token. */
}

bool isOneWord(char *str)
{
    trimLeftStr(&str); /* Remove leading whitespace. */
    while (!isspace(*str) && *str)
    {
        str++; /* Skip the text in the middle. */
    }
    return isWhiteSpaces(str); /* Check if the remaining text is all whitespace. */
}

bool isWhiteSpaces(char *str)
{
    while (*str)
    {
        if (!isspace(*str++))
        {
            return FALSE; /* Return false if a non-whitespace character is found. */
        }
    }
    return TRUE; /* Return true if only whitespace characters are found. */
}

bool isLegalLabel(char *labelStr, int lineNum, bool printErrors)
{
    int labelLength = strlen(labelStr), i;

    if (strlen(labelStr) > MAX_LABEL_LENGTH)
    {
        if (printErrors)
        {
            printError(lineNum, "ERROR: Label is too long. Max label name length is %d.", MAX_LABEL_LENGTH);
        }
        return FALSE; /* Return false if the label is too long. */
    }

    if (*labelStr == '\0')
    {
        if (printErrors)
        {
            printError(lineNum, "ERROR: Label name is empty.");
        }
        return FALSE; /* Return false if the label is empty. */
    }

    if (isspace(*labelStr))
    {
        if (printErrors)
        {
            printError(lineNum, "ERROR: Label must start at the beginning of the line.");
        }
        return FALSE; /* Return false if the label starts with whitespace. */
    }

    for (i = 1; i < labelLength; i++)
    {
        if (!isalnum(labelStr[i]))
        {
            if (printErrors)
            {
                printError(lineNum, "ERROR: \"%s\" is illegal label - use letters and numbers only.", labelStr);
            }
            return FALSE; /* Return false if the label contains non-alphanumeric characters. */
        }
    }

    if (!isalpha(*labelStr))
    {
        if (printErrors)
        {
            printError(lineNum, "ERROR: \"%s\" is illegal label - first char must be a letter.", labelStr);
        }
        return FALSE; /* Return false if the label does not start with a letter. */
    }

    if (isRegister(labelStr, NULL))
    {
        if (printErrors)
        {
            printError(lineNum, "ERROR: \"%s\" is illegal label - don't use a name of a register.", labelStr);
        }
        return FALSE; /* Return false if the label is a register name. */
    }

    if (isIndirectRegister(labelStr, NULL))
    {
        if (printErrors)
        {
            printError(lineNum, "ERROR: \"%s\" is illegal label - don't use a name of indirect register.", labelStr);
        }
        return FALSE; /* Return false if the label is an indirect register name. */
    }

    if (getCmdId(labelStr) != -1)
    {
        if (printErrors)
        {
            printError(lineNum, "ERROR: \"%s\" is illegal label - don't use a name of command.", labelStr);
        }
        return FALSE; /* Return false if the label is a command name. */
    }

    return TRUE;
}

bool isExistingLabel(char *label)
{
    if (getLabel(label))
    {
        printf("ERROR: Existing label was found: %s\n", label);
        return TRUE; /* Return true if the label exists. */
    }

    return FALSE;
}

bool isExistingEntryLabel(char *labelName)
{
    int i = 0;

    if (labelName)
    {
        for (i = 0; i < g_entryLabelsNum; i++)
        {
            if (strcmp(labelName, g_entryLines[i]->lineStr) == 0)
            {
                return TRUE; /* Return true if the label is an existing entry label. */
            }
        }
    }
    return FALSE;
}

bool isRegister(char *str, int *value)
{
    if (str[0] == 'r' && str[1] >= '0' && str[1] - '0' <= MAX_REGISTER_DIGIT && str[2] == '\0')
    {
        if (value)
        {
            *value = str[1] - '0'; /* Set the register value. */
        }
        return TRUE;
    }

    return FALSE;
}

bool isIndirectRegister(char *str, int *value)
{
    if (str[0] == '*' && str[1] == 'r' && str[2] >= '0' && str[2] - '0' <= MAX_REGISTER_DIGIT && str[3] == '\0')
    {
        if (value)
        {
            *value = str[2] - '0'; /* Set the indirect register value. */
        }
        return TRUE;
    }

    return FALSE;
}

bool isCommentOrEmpty(lineInfo *line)
{
    char *startOfText = line->lineStr;

    if (*line->lineStr == ';')
    {
        return TRUE; /* Return true if the line is a comment. */
    }

    trimLeftStr(&startOfText);
    if (*startOfText == '\0')
    {
        return TRUE; /* Return true if the line is empty. */
    }
    if (*startOfText == ';')
    {
        printError(line->lineNum, "ERROR: Comments must start with ';' at the start of the line.");
        line->isError = TRUE; /* Mark the line as an error. */
        return TRUE;
    }

    return FALSE;
}

char *getFirstOperand(char *line, char **endOfOp, bool *foundComma)
{
    if (!isWhiteSpaces(line))
    {
        char *end = strchr(line, ','); /* Find the first comma. */
        if (end)
        {
            *foundComma = TRUE;
            *end = '\0'; /* Null-terminate the operand. */
            end++;
        }
        else
        {
            *foundComma = FALSE;
        }

        if (endOfOp)
        {
            if (end)
            {
                *endOfOp = end;
            }
            else
            {
                *endOfOp = strchr(line, '\0'); /* Set endOfOp to the end of the line. */
            }
        }
    }

    trimStr(&line); /* Remove leading and trailing whitespace from the operand. */
    return line;
}

bool isDirective(char *cmd)
{
    return (*cmd == '.') ? TRUE : FALSE;
}

bool isLegalStringParam(char **strParam, int lineNum)
{
    if ((*strParam)[0] == '"' && (*strParam)[strlen(*strParam) - 1] == '"')
    {
        (*strParam)[strlen(*strParam) - 1] = '\0'; /* Remove the ending quote. */
        ++*strParam; /* Remove the starting quote. */
        return TRUE;
    }

    if (**strParam == '\0')
    {
        printError(lineNum, "ERROR: No parameter.");
    }
    else
    {
        printError(lineNum, "ERROR: The parameter for .string must be enclosed in quotes.");
    }
    return FALSE;
}

bool isLegalNum(char *numStr, int numOfBits, int lineNum, int *value)
{
    char *endOfNum;
    int maxNum = (1 << numOfBits) - 1; /* Calculate the maximum number that can be represented. */

    if (isWhiteSpaces(numStr))
    {
        printError(lineNum, "ERROR: Empty parameter.");
        return FALSE;
    }

    *value = strtol(numStr, &endOfNum, 0); /* Convert the string to a number. */

    if (*endOfNum)
    {
        printError(lineNum, "ERROR: \"%s\" isn't a valid number.", numStr);
        return FALSE;
    }

    if (*value > maxNum || *value < -maxNum)
    {
        printError(lineNum, "ERROR: \"%s\" is too %s, must be between %d and %d.", numStr, (*value > 0) ? "big" : "small", -maxNum, maxNum);
        return FALSE;
    }

    return TRUE;
}

int getNumOctalLength(int num)
{
    int l = !num;
    while (num)
    {
        l++;
        num /= OCTAL; /* Calculate the length of the number in octal. */
    }
    return l;
}

int getNumDecimalLength(int num)
{
    int l = !num;
    while (num)
    {
        l++;
        num /= DECIMAL; /* Calculate the length of the number in decimal. */
    }
    return l;
}

int convertDecimalToOctal(int decimalNumber)
{
    int octalNumber = 0, i = 1;
    while (decimalNumber != 0)
    {
        octalNumber += (decimalNumber % 8) * i;
        decimalNumber /= 8; /* Convert the decimal number to octal. */
        i *= 10;
    }
    return octalNumber;
}

void fprintfDest(FILE *file, int num)
{
    int length;
    length = getNumDecimalLength(num); /* Get the length of the number in decimal. */
    if (length == ONE_DIGIT)
    {
        fprintf(file, "000");
    }
    else if (length == TWO_DIGITS)
    {
        fprintf(file, "00");
    }
    else if (length == THREE_DIGITS)
    {
        fprintf(file, "0");
    }
    fprintf(file, "%d", num); /* Print the number with leading zeros. */
}

void fprintfICDC(FILE *file, int num)
{
    fprintf(file, "\t%d", num); /* Print the IC or DC value. */
}

void fprintfEnt(FILE *file, int num)
{
    fprintf(file, "%d", num); /* Print the entry value. */
}

void fprintfData(FILE *file, int num)
{
    int length;
    length = getNumOctalLength(num); /* Get the length of the number in octal. */
    if (length == ONE_DIGIT)
    {
        fprintf(file, "0000");
    }
    else if (length == TWO_DIGITS)
    {
        fprintf(file, "000");
    }
    else if (length == THREE_DIGITS)
    {
        fprintf(file, "00");
    }
    else if (length == FOUR_DIGITS)
    {
        fprintf(file, "0");
    }
    num = convertDecimalToOctal(num); /* Convert the number to octal. */
    fprintf(file, "%d", num); /* Print the octal number with leading zeros. */
}

void fprintfExt(FILE *file, int num)
{
    int length;
    length = getNumDecimalLength(num); /* Get the length of the number in decimal. */
    if (length == THREE_DIGITS)
    {
        fprintf(file, "0");
    }
    fprintf(file, "%d", num); /* Print the number with leading zeros. */
}

FILE *openFile(char *name, char *ending, const char *mode)
{
    FILE *file;
    char *mallocStr = (char *)malloc(strlen(name) + strlen(ending) + 1), *fileName = mallocStr;
    sprintf(fileName, "%s%s", name, ending); /* Create the complete file name with the ending. */

    file = fopen(fileName, mode); /* Open the file with the specified mode. */
    free(mallocStr);

    return file;
}

char *stripExtension(char *filename, const char *extension)
{
    char *new_filename = stringDuplicate(filename);
    char *ext_pos = strstr(new_filename, extension);
    if (ext_pos != NULL)
    {
        *ext_pos = '\0'; /* Remove the extension. */
    }
    return new_filename;
}

void createObjectFile(char *name, int IC, int DC, int *memoryArr)
{
    int i;
    FILE *file;
    char *base_name;
    base_name = stripExtension(name, ".am"); /* Creates the new ".ob" file without the ".am" extension. */
    file = openFile(base_name, ".ob", "w");

    fprintfICDC(file, IC); /* Print the IC value. */
    fprintf(file, "\t\t");
    fprintfICDC(file, DC); /* Print the DC value. */

    for (i = 0; i < IC + DC; i++)
    {
        fprintf(file, "\n");
        fprintfDest(file, FIRST_ADDRESS + i); /* Print the memory address. */
        fprintf(file, "\t\t");
        fprintfData(file, memoryArr[i]); /* Print the data in octal format. */
    }

    fclose(file);
}

void createEntriesFile(char *name)
{
    int i;
    FILE *file;
    char *base_name;

    if (!g_entryLabelsNum)
    {
        return; /* Return if there are no entry labels. */
    }

    base_name = stripExtension(name, ".am"); /* Creates the new ".ent" file without the ".am" extension. */
    file = openFile(base_name, ".ent", "w");

    for (i = 0; i < g_entryLabelsNum; i++)
    {
        fprintf(file, "%s\t\t", g_entryLines[i]->lineStr); /* Print the entry label name. */
        fprintfEnt(file, getLabel(g_entryLines[i]->lineStr)->address); /* Print the entry label address. */

        if (i != g_entryLabelsNum - 1)
        {
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

void createExternFile(char *name, lineInfo *linesArr, int linesFound)
{
    int i;
    labelInfo *label;
    bool firstPrint = TRUE; /* Flag to indicate if this is the first extern label. */
    FILE *file = NULL;
    char *base_name;

    for (i = 0; i < linesFound; i++)
    {
        if (linesArr[i].cmd && linesArr[i].cmd->numOfParams >= 2 && linesArr[i].op1.type == LABEL)
        {
            label = getLabel(linesArr[i].op1.str);
            if (label && label->isExtern)
            {
                if (firstPrint)
                {
                    base_name = stripExtension(name, ".am"); /* Creates the new ".ext" file without the ".am" extension. */
                    file = openFile(base_name, ".ext", "w"); /* Open the file for writing. */
                }
                else
                {
                    fprintf(file, "\n");
                }

                fprintf(file, "%s\t\t", label->name); /* Print the extern label name. */
                fprintfExt(file, linesArr[i].op1.address); /* Print the extern label address. */
                firstPrint = FALSE;
            }
        }

        if (linesArr[i].cmd && linesArr[i].cmd->numOfParams >= 1 && linesArr[i].op2.type == LABEL)
        {
            label = getLabel(linesArr[i].op2.str);
            if (label && label->isExtern)
            {
                if (firstPrint)
                {
                    base_name = stripExtension(name, ".am"); /* Creates the new ".ext" file without the ".am" extension. */
                    file = openFile(base_name, ".ext", "w"); /* Open the file for writing. */
                }
                else
                {
                    fprintf(file, "\n");
                }

                fprintf(file, "%s\t\t", label->name); /* Print the extern label name. */
                fprintfExt(file, linesArr[i].op2.address); /* Print the extern label address. */
                firstPrint = FALSE;
            }
        }
    }

    if (file)
    {
        fclose(file);
    }
}

void clearData(lineInfo *linesArr, int linesFound, int dataCount)
{
    int i;

    for (i = 0; i < g_labelNum; i++)
    {
        g_labelArr[i].address = 0;
        g_labelArr[i].isData = 0;
        g_labelArr[i].isExtern = 0;
    }
    g_labelNum = 0;

    for (i = 0; i < g_entryLabelsNum; i++)
    {
        g_entryLines[i] = NULL;
    }
    g_entryLabelsNum = 0;

    for (i = 0; i < dataCount; i++)
    {
        g_dataArr[i] = 0;
    }

    for (i = 0; i < linesFound; i++)
    {
        free(linesArr[i].originalString); /* Free the original string allocated for each line. */
    }
}

void parseFile(char *fileName)
{
    char amFileName[MAX_FILE_NAME_LENGTH];
    FILE *file;
    lineInfo linesArr[MAX_LINES_NUM];
    int memoryArr[MAX_DATA_NUM] = { 0 };
    int IC = 0, DC = 0, numOfErrors = 0, linesFound = 0;

    clearData(linesArr, linesFound, IC + DC); /* Clear the data and reset global variables. */
    sprintf(amFileName, "%s", fileName);

    file = fopen(amFileName, "r"); /* Open the file for reading. */
    if (file == NULL)
    {
        printf("ERROR: Can't open the file \"%s\".\n", amFileName);
        return;
    }

    numOfErrors += firstPass(file, linesArr, &linesFound, &IC, &DC); /* Perform the first transition read. */
    numOfErrors += secondPass(memoryArr, linesArr, linesFound, IC, DC); /* Perform the second transition read. */
	printf("Phase 3: Start the second pass\n");

    if (numOfErrors == 0)
    {
        createObjectFile(fileName, IC, DC, memoryArr); /* Create the object file. */
        createExternFile(fileName, linesArr, linesFound); /* Create the extern file. */
        createEntriesFile(fileName); /* Create the entries file. */
        printf("[Info] Created output files for the file \"%s\".\n\n", fileName);
    }
    else
    {
        printf("[Info] A total number of %d error%s found in \"%s\".\n", numOfErrors, (numOfErrors > 1) ? "s were" : " was", fileName);
		printf("[Info] Found errors in \".am\" file, no output files created.\n\n");
    }

    clearData(linesArr, linesFound, IC + DC); /* Clear the data and reset global variables. */
    fclose(file);
}

char *addNewFile(char *file_name, char *new_extension)
{
    char *dot_position, *new_file_name;
    new_file_name = mallocAllocateAndCheck(strlen(file_name) + strlen(new_extension) + 1);

    strcpy(new_file_name, file_name);

    dot_position = strrchr(new_file_name, '.'); /* Find the last dot in the file name. */
    if (dot_position != NULL)
    {
        *dot_position = '\0'; /* Remove the current extension. */
    }

    strcat(new_file_name, new_extension); /* Append the new extension. */
    return new_file_name;
}

int copyFile(char *file_name_dest, char *file_name_orig)
{
    char str[MAX_LINE_LENGTH];
    FILE *fp, *fp_dest;

    fp = fopen(file_name_orig, "r"); /* Open the source file for reading. */
    if (fp == NULL)
    {
        printErrorInternal("ERROR: Failed to open file for reading");
        return 0;
    }

    fp_dest = fopen(file_name_dest, "w"); /* Open the destination file for writing. */
    if (fp_dest == NULL)
    {
        printErrorInternal("ERROR: Failed to open new file for writing");
        fclose(fp);
        return 0;
    }

    while (fgets(str, MAX_LINE_LENGTH, fp) != NULL) /* Read each line from the source file. */
    {
        fprintf(fp_dest, "%s", str); /* Write each line to the destination file. */
    }

    fclose(fp); /* Close the source file. */
    fclose(fp_dest); /* Close the destination file. */

    return 1;
}