/* The C Programming Language Course Final Project, Semester 2024B */
/* Menahem Hen Perets, ID: 318246014 */
#include "assembler.h"
#include "utility.h"

/* List of Directives */
void parseDataDirc(lineInfo *line, int *IC, int *DC);
void parseStringDirc(lineInfo *line, int *IC, int *DC);
void parseExternDirc(lineInfo *line);
void parseEntryDirc(lineInfo *line);

const directive g_dircArr[] = 
{	/* Name | Parsing Function */
	{ "data", parseDataDirc } ,
	{ "string", parseStringDirc } ,
	{ "extern", parseExternDirc },
	{ "entry", parseEntryDirc },
	{ NULL } /* This value will represent the end of the array. */
};	

/* List of Commands */
const command g_cmdArr[] =	
{	/* Name | Opcode | NumOfParams */
	{ "mov", 0, 2 } , 
	{ "cmp", 1, 2 } ,
	{ "add", 2, 2 } ,
	{ "sub", 3, 2 } ,
	{ "lea", 4, 2 } ,
	{ "clr", 5, 1 } ,
	{ "not", 6, 1 } ,
	{ "inc", 7, 1 } ,
	{ "dec", 8, 1 } ,
	{ "jmp", 9, 1 } ,
	{ "bne", 10, 1 } ,
	{ "red", 11, 1 } ,
	{ "prn", 12, 1 } ,
	{ "jsr", 13, 1 } ,
	{ "rts", 14, 0 } ,
	{ "stop", 15, 0 } ,
	{ NULL } /* This value will represent the end of the array. */
}; 

labelInfo *addLabelToArr(labelInfo label, lineInfo *line) /* Documentation in "assembler.h". */
{
	if (!isLegalLabel(line->lineStr, line->lineNum, TRUE)) /* Check if the label is legal. */
	{
		line->isError = TRUE; /* Illegal label name. */
		return NULL;
	}
	/* Checks if the label already exists. */
	if (isExistingLabel(line->lineStr))
	{
		printError(line->lineNum, "ERROR: Label already exists.");
		line->isError = TRUE;
		return NULL;
	}
	strcpy(label.name, line->lineStr); /* Add the name to the label. */
	if (g_labelNum < MAX_LABELS_NUM) /* Add the label to g_labelArr and to the lineInfo. */
	{
		g_labelArr[g_labelNum] = label;
		return &g_labelArr[g_labelNum++];
	}
	
	printError(line->lineNum, "ERROR: Too many labels - max is %d.", MAX_LABELS_NUM, TRUE); /* Too many labels. */
	line->isError = TRUE;
	return NULL;
}

bool addNumberToData(int num, int *IC, int *DC, int lineNum) /* Documentation in "assembler.h". */
{
	if (*DC + *IC < MAX_DATA_NUM) /* Checks if there is enough space in g_dataArr for the data. */
	{
		g_dataArr[(*DC)++] = num;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

bool addStringToData(char *str, int *IC, int *DC, int lineNum) /* Documentation in "assembler.h". */
{
	do
	{
		if (!addNumberToData((int)*str, IC, DC, lineNum))
		{
			return FALSE;
		}
	} while (*str++);

	return TRUE;
}

char *findLabel(lineInfo *line, int IC) /* Documentation in "assembler.h". */
{
	char *labelEnd = strchr(line->lineStr, ':');
	labelInfo label = { 0 };
	label.address = FIRST_ADDRESS + IC;

	if (!labelEnd) /* Find the label (or return NULL if there isn't) */
	{
		return NULL;
	}
	*labelEnd = '\0';

	if (!isOneWord(line->lineStr)) /* Check if the ':' came after the first word */
	{
		*labelEnd = ':'; /* Fix the change in line->lineStr. */
		return NULL;
	}

	line->label = addLabelToArr(label, line); /* Check of the label is legal and add it to the labelList. */
	return labelEnd + 1; /* +1 to make it point at the next char after the \0. */
}

void removeLastLabel(int lineNum) /* Documentation in "assembler.h". */
{
	g_labelNum--;
	printf("WARNING: At line %d: The assembler ignored the label before the directive.\n", lineNum);
}

void parseDataDirc(lineInfo *line, int *IC, int *DC) /* Documentation in "assembler.h". */
{
	char *operandTok = line->lineStr, *endOfOp = line->lineStr;
	int operandValue;
	bool foundComma;

	if (line->label) /* Make the label a data label (if there is one). */
	{
		line->label->isData = TRUE;
		line->label->address = FIRST_ADDRESS + *DC;
	}

	if (isWhiteSpaces(line->lineStr)) /* Checks if there are params. */
	{
		printError(line->lineNum, "ERROR: No parameter.");
		line->isError = TRUE;
		return;
	}

	FOREVER /* Find all the params and add them to g_dataArr */
	{
		if (isWhiteSpaces(line->lineStr)) /* Get next param or break if there is none. */
		{
			break;
		}
		operandTok = getFirstOperand(line->lineStr, &endOfOp, &foundComma);
		
		if (isLegalNum(operandTok, MEMORY_WORD_LENGTH - 3, line->lineNum, &operandValue)) /* Add the param to g_dataArr. */
		{
			if (!addNumberToData(operandValue, IC, DC, line->lineNum))
			{
				line->isError = TRUE; /* Not enough memory. */
				return;
			}
		}
		else
		{
			line->isError = TRUE; /* Illegal number. */
			return;
		}
		line->lineStr = endOfOp; /* Change the line to start after the parameter. */
	}

	if (foundComma)
	{
		printError(line->lineNum, "ERROR: Commas found after the last parameter."); /* Comma after the last param. */
		line->isError = TRUE;
		return;
	}
}

void parseStringDirc(lineInfo *line, int *IC, int *DC) /* Documentation in "assembler.h". */
{
	if (line->label) /* Make the label a data label (if there is one). */
	{
		line->label->isData = TRUE;
		line->label->address = FIRST_ADDRESS + *DC;
	}
	trimStr(&line->lineStr);

	if (isLegalStringParam(&line->lineStr, line->lineNum))
	{
		if (!addStringToData(line->lineStr, IC, DC, line->lineNum))
		{
			line->isError = TRUE; /* Not enough memory. */
			return;
		}
	}
	else
	{
		line->isError = TRUE; /* Illegal string. */
		return;
	}
}

void parseExternDirc(lineInfo *line) /* Documentation in "assembler.h". */
{
	labelInfo label = { 0 }, *labelPointer;

	if (line->label) /* If there is a label in the line, remove the it from labelArr. */
	{
		removeLastLabel(line->lineNum);
	}

	trimStr(&line->lineStr);
	labelPointer = addLabelToArr(label, line);

	if (!line->isError) /* Make the label an extern label. */
	{
		labelPointer->address = 0;
		labelPointer->isExtern = TRUE;
	}
}

void parseEntryDirc(lineInfo *line) /* Documentation in "assembler.h". */
{
	if (line->label) /* If there is a label in the line, remove the it from labelArr. */
	{
		removeLastLabel(line->lineNum);
	}

	trimStr(&line->lineStr); /* Add the label to the entry labels list. */

	if (isLegalLabel(line->lineStr, line->lineNum, TRUE))
	{
		if (isExistingEntryLabel(line->lineStr))
		{
			printError(line->lineNum, "ERROR: Label already defined as an entry label.");
			line->isError = TRUE;
		}
		if (g_entryLabelsNum < MAX_LABELS_NUM)
		{
			g_entryLines[g_entryLabelsNum++] = line;
		}
	}
}

void parseDirective(lineInfo *line, int *IC, int *DC) /* Documentation in "assembler.h". */
{
	int i = 0;
	while (g_dircArr[i].name)
	{
		if (!strcmp(line->commandStr, g_dircArr[i].name))
		{	
			g_dircArr[i].parseFunc(line, IC, DC); /* Call the parse function for this type of directive. */
			return;
		}
		i++;
	}	
	
	printError(line->lineNum, "ERROR: No such directive as \"%s\".", line->commandStr); /* line->commandStr isn't a real directive. */
	line->isError = TRUE;
}

bool areLegalOpTypes(const command *cmd, operandInfo op1, operandInfo op2, int lineNum) /* Documentation in "assembler.h". */
{
	/* Checks First Operand. */
	if (cmd->opcode == 4 && op1.type != LABEL) /* "lea" command (opcode is 4) can only get a label as the 1st op. */
	{
		printError(lineNum, "ERROR: Source operand for \"%s\" command must be a label.", cmd->name);
		return FALSE;
	}

	/* Checks Second Operand.*/
	if (op2.type == NUMBER && cmd->opcode != 1 && cmd->opcode != 12) /* 2nd operand can be a number only if the command is "cmp" (opcode is 1) or "prn" (opcode is 12). */
	{
		printError(lineNum, "ERROR: Destination operand for \"%s\" command can't be a number.", cmd->name);
		return FALSE;
	}
	return TRUE;
}

void parseOpInfo(operandInfo *operand, int lineNum) /* Documentation in "assembler.h". */
{
	int value = 0;

	if (isWhiteSpaces(operand->str))
	{
		printError(lineNum, "ERROR: Empty parameter.");
		operand->type = INVALID;
		return;
	}

	if (*operand->str == '#') /* Checks if the type is a NUMBER. */
	{
		operand->str++; /* Remove the '#'. */
		if (isspace(*operand->str)) /* Checks if the number is legal. */
		{
			printError(lineNum, "ERROR: There is a white space after the '#'.");
			operand->type = INVALID;
		}
		else
		{
			operand->type = isLegalNum(operand->str, MEMORY_WORD_LENGTH - 3, lineNum, &value) ? NUMBER : INVALID;
		}
	 }
	
	else if (isIndirectRegister(operand->str, &value)) /* Checks if the type is INDREGISTER. */
	{
		operand->type = INDREGISTER;
	}
	else if (isRegister(operand->str, &value)) /* Checks if the type is REGISTER. */
	{
		operand->type = REGISTER;
	}
	else if (isLegalLabel(operand->str, lineNum, FALSE)) /* Checks if the type is LABEL. */
	{
		operand->type = LABEL;
	}
	else /* The type is INVALID. */
	{
		printError(lineNum, "ERROR: \"%s\" is an invalid parameter.", operand->str);
		operand->type = INVALID;
		value = -1;
	}

	operand->value = value;
}
	
void parseCmdOperands(lineInfo *line, int *IC, int *DC) /* Documentation in "assembler.h". */
{
	char *startOfNextPart = line->lineStr;
	bool foundComma = FALSE;
	int numOfOpsFound = 0;

	/* Reset the op types. */
	line->op1.type = INVALID;
	line->op2.type = INVALID;
	
	FOREVER /* Get the parameters. */
	{
		/* If both of the operands are registers, or indirect registers they will only take 1 memory word (instead of 2). */
		if (!(line->op1.type == REGISTER && line->op2.type == REGISTER) && !(line->op1.type == REGISTER && line->op2.type == INDREGISTER) && !(line->op1.type == INDREGISTER && line->op2.type == REGISTER) && !(line->op1.type == INDREGISTER && line->op2.type == INDREGISTER))
		{	
			if (*IC + *DC < MAX_DATA_NUM) /* Checks if there is enough memory. */
			{
				++*IC; /* Count the last command word or operand. */
			}
			else
			{
				line->isError = TRUE;
				return;
			}
		}
		
		if (isWhiteSpaces(line->lineStr) || numOfOpsFound > 2) /* Checks if there are still more operands to read. */
		{
			break; /* If there are more than 2 operands it's illegal. */
		}

		if (numOfOpsFound == 1) /* If there are 2 ops, make the destination become the source op. */
		{
			line->op1 = line->op2;
			line->op2.type = INVALID; /* Reset op2. */
		}
		
		line->op2.str = getFirstOperand(line->lineStr, &startOfNextPart, &foundComma); /* Parse the opernad. */
		parseOpInfo(&line->op2, line->lineNum);

		if (line->op2.type == INVALID)
		{
			line->isError = TRUE;
			return;
		}

		numOfOpsFound++;
		line->lineStr = startOfNextPart;
	} /* While loop end. */

	if (numOfOpsFound != line->cmd->numOfParams) /* Checks if there are enough operands. */
	{
		
		if (numOfOpsFound < line->cmd->numOfParams) /* Checks if there are more or less operands than needed. */
		{
			printError(line->lineNum, "ERROR: Not enough operands.", line->commandStr);
		}
		else
		{
			printError(line->lineNum, "ERROR: Too many operands.", line->commandStr);
		}
		line->isError = TRUE;
		return;
	}

	if (foundComma) /* Check if there is a comma after the last param. */
	{
		printError(line->lineNum, "Don't write a comma after the last parameter.");
		line->isError = TRUE;
		return;
	}
	
	if (!areLegalOpTypes(line->cmd, line->op1, line->op2, line->lineNum)) /* Checsk if the operands types are legal. */
	{
		line->isError = TRUE;
		return;
	}
}

void parseCommand(lineInfo *line, int *IC, int *DC) /* Documentation in "assembler.h". */
{
	int cmdId = getCmdId(line->commandStr);

	if (cmdId == -1)
	{
		line->cmd = NULL;
		if (*line->commandStr == '\0')
		{	
			printError(line->lineNum, "ERROR: Can't write a label to an empty line.", line->commandStr); /* The command is empty, but the line isn't empty so it's only a label. */
		}
		else
		{
			printError(line->lineNum, "ERROR: No such command as \"%s\".", line->commandStr); /* Illegal command. */
		}
		line->isError = TRUE;
		return;
	}

	line->cmd = &g_cmdArr[cmdId];
	parseCmdOperands(line, IC, DC);
}

char *allocString(const char *str) 
{
	char *newString = (char *)malloc(strlen(str) + 1);
	if (newString) 
	{
		strcpy(newString, str); 
	}

	return newString;
}

void parseLine(lineInfo *line, char *lineStr, int lineNum, int *IC, int *DC) /* Documentation in "assembler.h". */
{
	char *startOfNextPart = lineStr;

	line->lineNum = lineNum;
	line->address = FIRST_ADDRESS + *IC;
	line->originalString = allocString(lineStr);
	line->lineStr = line->originalString;
	line->isError = FALSE;
	line->label = NULL;
	line->commandStr = NULL;
	line->cmd = NULL;

	if (!line->originalString)
	{
		printf("ERROR: Malloc failed, not enough memory.");
		return;
	}
	if (isCommentOrEmpty(line)) /* Check if the line is a comment. */
	{	
		return;
	}
	startOfNextPart = findLabel(line, *IC); /* Find a label and add it to the label list. */

	if (line->isError)
	{
		return;
	}
	if (startOfNextPart) /* Update the line if startOfNextPart isn't NULL. */
	{
		line->lineStr = startOfNextPart;
	}	
	line->commandStr = getFirstTok(line->lineStr, &startOfNextPart); /* Find the command token. */
	line->lineStr = startOfNextPart;
	
	if (isDirective(line->commandStr)) /* Parse the command / directive. */
	{
		line->commandStr++; /* Remove the '.' from the command. */
		parseDirective(line, IC, DC);
	}
	else
	{
		parseCommand(line, IC, DC);
	}
	if (line->isError)
	{
		return;
	}
}

bool readLine(FILE *file, char *line_data, size_t maxLength) /* Documentation in "assembler.h". */
{
	char *endOfLine;

	if (!fgets(line_data, maxLength, file))
	{
		return FALSE;
	}
	endOfLine = strchr(line_data, '\n'); /* Check if the line is too long (no '\n' was present). */

	if (endOfLine)
	{
		*endOfLine = '\0';
	}
	else
	{
		char c;
		bool ret = (feof(file)) ? TRUE : FALSE; /* Return FALSE, unless it's the end of the file. */

		do /* Keep reading chars until you reach the end of the line ('\n') or EOF. */
		{
			c = fgetc(file);
		} while (c != '\n' && c != EOF);

		return ret;
	}

	return TRUE;
}

int firstPass(FILE *file, lineInfo *linesArr, int *linesFound, int *IC, int *DC) /* Documentation in "assembler.h". */
{
	char lineStr[MAX_LINE_LENGTH + 2]; /* +2 for the \n and \0 at the end */
	int errorsFound = 0;
	*linesFound = 0;

	
	while (!feof(file)) /* Read lines and parse them. */
	{
		if (readLine(file, lineStr, MAX_LINE_LENGTH + 2)) 
		{
			if (*linesFound >= MAX_LINES_NUM) /* Checks if the file is too long. */
			{
				printf("ERROR: The file is too long. Max number of lines in a file is %d.\n", MAX_LINES_NUM);
				return ++errorsFound;
			}

			parseLine(&linesArr[*linesFound], lineStr, *linesFound + 1, IC, DC); /* Parse a line. */

			if (linesArr[*linesFound].isError) /* Update errorsFound. */
			{
				errorsFound++;
			}

			if (*IC + *DC >= MAX_DATA_NUM) /* Check if the number of memory words needed is small enough. */
			{
				
				printError(*linesFound + 1, "ERROR: The max memory words is %d, too much data and code.", MAX_DATA_NUM); /* dataArr is full. Stop reading the file. */
				printf("Memory is full, file reading terminated.\n");
				return ++errorsFound;
			}
			++*linesFound;
		}
		else if (!feof(file))
		{
			
			printError(*linesFound + 1, "ERROR: The max line length is %d, line is too long.", MAX_LINE_LENGTH); /* Line is too long. */
			errorsFound++;
			 ++*linesFound;
		}
	}

	return errorsFound;
}