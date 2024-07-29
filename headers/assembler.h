/* The C Programming Language Course Final Project, Semester 2024B, Open University Israel */
/* Name: Menahem Hen Perets, ID: 318246014 */
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

/* Constants */
#define MAX_DATA_NUM 4096
#define FIRST_ADDRESS 100 
#define MAX_LINE_LENGTH	80
#define MAX_LABEL_LENGTH 31
#define MEMORY_WORD_LENGTH 15
#define MAX_REGISTER_DIGIT 7
#define MAX_LINES_NUM 300
#define MAX_LABELS_NUM MAX_LINES_NUM
#define MAX_FILE_NAME_LENGTH 256
#define OCTAL 8
#define DECIMAL 10
#define ONE_DIGIT 1
#define TWO_DIGITS 2
#define THREE_DIGITS 3
#define FOUR_DIGITS 4
#define FOREVER	for(;;)
#define BYTE_SIZE 8
#define FALSE 0
#define TRUE 1

/*The numbers are assigned as the bits which will be turned on regarding the matching opType. */
typedef enum { NUMBER = 1, LABEL = 2, INDREGISTER = 4, REGISTER = 8, INVALID = -1 } opType; /* Operands */

/*The numbers are assigned as the bits which will be turned on regarding the matching ARE type. */
typedef enum { EXTERNAL = 1, RELOCATABLE = 2, ABSOLUTE = 4 } areType;

typedef unsigned int bool; /* Only get TRUE or FALSE values */

typedef struct node /* Node Structure */
{
    char *name; /* The name of the macro. */
    char *content; /* The content of the macro. */
    int line; /* The line number where the macro is defined. */
    struct node *next; /* Pointer to the next node in the list. */
} node;

typedef struct /* Command Structure */
{
    char *name; /* The name of the command. */
    unsigned int opcode : 4; /* The opcode of the command, using 4 bits. */
    int numOfParams; /* The number of parameters the command takes. */
} command;

typedef struct /* Directive Structure */
{
    char *name; /* The name of the directive. */
    void (*parseFunc)(); /* Function pointer to the function that parses this directive. */
} directive;

typedef struct /* Labels Structure */
{
	int address; /* The address it contains. */
	char name[MAX_LABEL_LENGTH]; /* The name of the label. */					
	bool isExtern; /* Extern flag. */
	bool isData; /* Data flag (.data or .string). */
} labelInfo;

typedef struct /* Operand Structure */
{
	int value; /* Value. */
	char *str; /* String. */
	opType type; /* Type. */
	int address; /* The address of the operand in the memory. */
} operandInfo;

typedef struct /* Line Structure */
{
	int lineNum; /* The number of the line in the file. */
	int address; /* The address of the first word in the line. */
	char *originalString; /* The original pointer, allocated by malloc. */
	char *lineStr; /* The text it contains (changed while using parseLine). */
	bool isError; /* Represent whether there is an error or not. */
	labelInfo *label; /* A poniter to the lines label in labelArr. */
	char *commandStr; /* The string of the command or directive. */
	const command *cmd;	/* A pointer to the command in g_cmdArr. */
	operandInfo op1; /* The 1st operand. */
	operandInfo op2; /* The 2nd operand. */
} lineInfo;

typedef struct /* Memory Word Structure - 15 bits */
{
	unsigned int are : 3;
	union /* 12 bits */
	{
		struct /* Commands (only 12 bits) */
		{
			unsigned int dest : 4; /* Destination op addressing method ID. */
			unsigned int src : 4; /* Source op addressing method ID. */
			unsigned int opcode : 4; /* Command ID. */
		} cmdBits;

		struct /* Registers (only 6 bits) */
		{
			unsigned int destBits : 3; /* DEST Register. */
			unsigned int srcBits : 3; /* SRC Register. */
		} regBits;
		/* Other operands */
		int value : 12; /* (12 bits) */

	} valueBits; /* End of 12 bits union. */

} memoryWord;

/**
 * @brief Declares an external array of `command` structures.
 * This array holds information about the available commands in the assembler.
 * Each `command` structure typically includes the command's name, opcode, and the number of parameters it accepts.
 */
extern const command g_cmdArr[];

/**
 * @brief Declares an external array of `labelInfo` structures.
 * This array stores information about labels encountered during the assembly process.
 * Each `labelInfo` structure contains details like the label's address, name, and flags
 * indicating if it is external or associated with data.
 */
extern labelInfo g_labelArr[MAX_LABELS_NUM];

/**
 * @brief Declares an external array of pointers to `lineInfo` structures.
 * This array holds pointers to lines that define entry labels.
 */
extern lineInfo *g_entryLines[MAX_LABELS_NUM];

/**
 * @brief Declares an external integer variable to track the number of labels.
 * This variable keeps track of the number of labels currently stored in `g_labelArr`.
 */
extern int g_labelNum;

/**
 * @brief Declares an external integer variable to track the number of entry labels.
 * This variable keeps track of the number of entry labels currently stored in `g_entryLines`.
 */
extern int g_entryLabelsNum;

/**
 * @brief Declares an external array of integers to store numerical data.
 * This array stores the numerical data values encountered during the assembly process.
 */
extern int g_dataArr[MAX_DATA_NUM];

/************** First Pass Functions **************/
/**
 * @brief Adds a label to the label array if it is legal and doesn't already exist.
 *
 * @param label The label information to be added.
 * @param line The line information containing the label.
 * @return A pointer to the added label in the label array, or NULL if the label is illegal or already exists.
 */
labelInfo *addLabelToArr(labelInfo label, lineInfo *line);

/**
 * @brief Adds a number to the data array if there is enough space.
 *
 * @param num The number to be added.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 * @param lineNum The line number (used for error reporting).
 * @return TRUE if the number was added successfully, FALSE if there is not enough space.
 */
bool addNumberToData(int num, int *IC, int *DC, int lineNum);

/**
 * @brief Adds a string to the data array by converting each character to its ASCII value.
 *
 * @param str The string to be added.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 * @param lineNum The line number (used for error reporting).
 * @return TRUE if the string was added successfully, FALSE if there is not enough space.
 */
bool addStringToData(char *str, int *IC, int *DC, int lineNum);

/**
 * @brief Finds and processes a label in a line of assembly code.
 *
 * @param line The line information containing the potential label.
 * @param IC The instruction counter.
 * @return A pointer to the next character after the label in the line, or NULL if no label is found.
 */
char *findLabel(lineInfo *line, int IC);

/**
 * @brief Removes the last added label from the label array and prints a warning.
 *
 * @param lineNum The line number where the label was found (used for warning message).
 */
void removeLastLabel(int lineNum);

/**
 * @brief Parses a .data directive and adds its values to the data array.
 *
 * @param line The line information containing the .data directive.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 */
void parseDataDirc(lineInfo *line, int *IC, int *DC);

/**
 * @brief Parses a .string directive and adds its values to the data array.
 *
 * @param line The line information containing the .string directive.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 */
void parseStringDirc(lineInfo *line, int *IC, int *DC);

/**
 * @brief Parses an .extern directive and adds the label as an external label.
 * @param line The line information containing the .extern directive.
 */
void parseExternDirc(lineInfo *line);

/**
 * @brief Parses an .entry directive and adds the label to the entry labels list.
 * @param line The line information containing the .entry directive.
 */
void parseEntryDirc(lineInfo *line);

/**
 * @brief Parses a directive and calls the appropriate parsing function.
 *
 * @param line The line information containing the directive.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 */
void parseDirective(lineInfo *line, int *IC, int *DC);

/**
 * @brief Parses and validates operand information.
 *
 * @param operand The operand information to be parsed and validated.
 * @param lineNum The line number (used for error reporting).
 */
void parseOpInfo(operandInfo *operand, int lineNum);

/**
 * @brief Parses and validates the operands for a command.
 *
 * This function parses and validates the operands for a given command. It checks if the operands
 * are legal and updates the instruction counter (IC) accordingly.
 * @param line The line information containing the command and operands.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 */
void parseCmdOperands(lineInfo *line, int *IC, int *DC);

/**
 * @brief Parses a command and its operands.
 *
 * This function identifies and parses a command in a line, extracts and validates its operands,
 * and updates the instruction counter (IC) accordingly.
 * @param line The line information containing the command and operands.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 */
void parseCommand(lineInfo *line, int *IC, int *DC);

/**
 * @brief Allocates memory for a string and copies its content.
 *
 * This function allocates memory for a new string and copies the content of the input string
 * to the newly allocated memory.
 * @param str The input string to be copied.
 * @return A pointer to the newly allocated and copied string.
 */
char *allocString(const char *str);

/**
 * @brief Parses a line of assembly code.
 *
 * This function parses a line of assembly code, identifies labels, directives, and commands,
 * and updates the instruction counter (IC) and data counter (DC) accordingly.
 * @param line The line information structure to be filled.
 * @param lineStr The input line string to be parsed.
 * @param lineNum The line number (used for error reporting).
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 */
void parseLine(lineInfo *line, char *lineStr, int lineNum, int *IC, int *DC);

/**
 * @brief Reads a line from a file.
 *
 * This function reads a line from a file, ensuring the line does not exceed the maximum length.
 * If the line is too long, it reads until the end of the line or file.
 * @param file The file pointer to read from.
 * @param line_data The buffer to store the read line.
 * @param maxLength The maximum length of the line to read.
 * @return Returns TRUE if a line is successfully read, otherwise FALSE.
 */
bool readLine(FILE *file, char *line_data, size_t maxLength);

/**
 * @brief Performs the first pass of the assembler.
 *
 * This function performs the first pass of the assembler, reading and parsing each line of the source file.
 * It updates the instruction counter (IC), data counter (DC), and line information array (linesArr).
 * @param file The file pointer to the source file.
 * @param linesArr The array to store parsed line information.
 * @param linesFound A pointer to the number of lines found.
 * @param IC A pointer to the instruction counter.
 * @param DC A pointer to the data counter.
 * @return Returns the number of errors found during the first pass.
 */
int firstPass(FILE *file, lineInfo *linesArr, int *linesFound, int *IC, int *DC);

/************** Second Pass Functions **************/
/**
 * @brief Updates the addresses of data labels by adding the instruction counter (IC) value.
 *
 * This function iterates through the global label array and updates the address of each data label
 * by adding the value of the instruction counter (IC). This is necessary for adjusting data labels
 * after the instruction section has been processed.
 * @param IC The instruction counter value to be added to the data labels' addresses.
 */
void updateDataLabelsAddress(int IC);

/**
 * @brief Counts the number of illegal entry labels and reports errors.
 *
 * This function iterates through the global entry lines array and checks each entry label.
 * It reports errors if an entry label is found to be an external label or if the label does not exist.
 * The function returns the total number of errors found.
 * @return The total number of errors found in the entry labels.
 */
int countIllegalEntries();

/**
 * @brief Updates the address of a label operand.
 *
 * This function checks if the operand is of type LABEL. If so, it retrieves the label information
 * and updates the operand's value with the label's address. If the label does not exist, an error is reported.
 * @param op A pointer to the operand information to be updated.
 * @param lineNum The line number where the operand is located (used for error reporting).
 * @return Returns TRUE if the label exists and the address was updated, otherwise FALSE.
 */
bool updateLabelOpAddress(operandInfo *op, int lineNum);

/**
 * @brief Extracts the numerical value from a memory word.
 *
 * This function applies a bitmask to the value in a memory word to extract the relevant bits.
 * It combines the value bits and the ARE (Addressing, Relocatable, External) bits into a single integer.
 * @param memory The memory word to extract the value from.
 * @return The extracted numerical value from the memory word.
 */
int getNumFromMemoryWord(memoryWord memory);

/**
 * @brief Retrieves the type ID of an operand.
 *
 * This function checks if the operand type is valid and returns its type ID.
 * If the operand type is invalid, it returns 0.
 * @param op The operand information.
 * @return The type ID of the operand if valid, otherwise 0.
 */
int getOpTypeId(operandInfo op);

/**
 * @brief Creates a memory word for a command line.
 *
 * This function creates and initializes a memory word for a given command line.
 * It sets the ARE type to ABSOLUTE and encodes the destination and source operand types and the opcode.
 *
 * @param line The line information containing the command and operands.
 * @return The created memory word for the command.
 */
memoryWord getCmdMemoryWord(lineInfo line);

/**
 * @brief Creates a memory word for an operand.
 *
 * This function creates and initializes a memory word for a given operand.
 * It sets the ARE type and encodes the operand value based on its type (register, indirect register, label, or number).
 *
 * @param op The operand information.
 * @param isDest A flag indicating if the operand is a destination operand.
 * @return The created memory word for the operand.
 */
memoryWord getOpMemoryWord(operandInfo op, bool isDest);

/**
 * @brief Adds a memory word to the memory array.
 *
 * This function adds a memory word to the memory array at the current memory counter position.
 * It increments the memory counter after adding the word.
 *
 * @param memoryArr The memory array to add the word to.
 * @param memoryCounter A pointer to the current memory counter.
 * @param memory The memory word to add.
 */
void addWordToMemory(int *memoryArr, int *memoryCounter, memoryWord memory);

/**
 * @brief Adds a line to the memory array, updating operand addresses as needed.
 *
 * This function adds the memory words for a line to the memory array, updating the addresses of label operands.
 * It handles different operand types (register, indirect register, label, number) and adds the appropriate memory words.
 * @param memoryArr The memory array to add the line to.
 * @param memoryCounter A pointer to the current memory counter.
 * @param line A pointer to the line information.
 * @return Returns TRUE if no error was found, otherwise FALSE.
 */
bool addLineToMemory(int *memoryArr, int *memoryCounter, lineInfo *line);

/**
 * @brief Adds data to the memory array.
 *
 * This function adds data words to the memory array, applying a bitmask to each data word.
 * It increments the memory counter for each data word added.
 * @param memoryArr The memory array to add the data to.
 * @param memoryCounter A pointer to the current memory counter.
 * @param DC The data counter indicating the number of data words.
 */
void addDataToMemory(int *memoryArr, int *memoryCounter, int DC);

/**
 * @brief Performs the second pass of reading and processing an assembly language file.
 * 
 * This function updates the addresses of data labels, checks for illegal entries,
 * and adds the parsed lines and data to the memory array. It returns the total
 * number of errors encountered during this process.
 * @param memoryArr An array of integers representing the memory of the assembly program.
 * @param linesArr An array of lineInfo structures holding information about each parsed line.
 * @param lineNum The number of lines in the linesArr array.
 * @param IC The instruction counter value at the end of the first pass.
 * @param DC The data counter value at the end of the first pass.
 * @return The total number of errors encountered during the second pass.
 */
int secondPass(int *memoryArr, lineInfo *linesArr, int lineNum, int IC, int DC);

#endif