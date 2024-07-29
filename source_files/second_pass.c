/* The C Programming Language Course Final Project, Semester 2024B */
/* Menahem Hen Perets, ID: 318246014 */
#include "assembler.h"
#include "utility.h"

void updateDataLabelsAddress(int IC)
{
	int i;

	for (i = 0; i < g_labelNum; i++)
	{
		if (g_labelArr[i].isData)
		{
			g_labelArr[i].address += IC; /* Update the address for data labels by adding IC. */
		}
	}
}

int countIllegalEntries()
{
	int i, ret = 0;
	labelInfo *label;

	for (i = 0; i < g_entryLabelsNum; i++)
	{
		label = getLabel(g_entryLines[i]->lineStr);
		if (label)
		{
			if (label->isExtern)
			{
				printError(g_entryLines[i]->lineNum, "The parameter for .entry can't be an external label.");
				ret++; /* Increment the error count for illegal entry labels. */
			}
		}
		else
		{
			printError(g_entryLines[i]->lineNum, "No such label as \"%s\".", g_entryLines[i]->lineStr);
			ret++; /* Increment the error count for non-existing labels. */
		}
	}

	return ret;
}

bool updateLabelOpAddress(operandInfo *op, int lineNum)
{
	if (op->type == LABEL)
	{
		labelInfo *label = getLabel(op->str);
		if (label == NULL)
		{
			if (isLegalLabel(op->str, lineNum, TRUE))
			{
				printError(lineNum, "No such label as \"%s\"", op->str);
			}
			return FALSE; /* Return false if the label does not exist. */
		}
		op->value = label->address; /* Update the operand value with the label address. */
	}

	return TRUE;
}

int getNumFromMemoryWord(memoryWord memory)
{
	unsigned int mask = ~0;
	mask >>= (sizeof(int) * BYTE_SIZE - MEMORY_WORD_LENGTH);

	return mask & ((memory.valueBits.value << 3) + memory.are); /* Return the memory word value with the mask applied. */
}

int getOpTypeId(operandInfo op)
{
	if (op.type != INVALID)
	{
		return (int)op.type; /* Return the operand type ID if it's valid. */
	}

	return 0; /* Return 0 for invalid operand type. */
}

memoryWord getCmdMemoryWord(lineInfo line)
{
	memoryWord memory = { 0 };

	memory.are = (areType)ABSOLUTE;
	memory.valueBits.cmdBits.dest = getOpTypeId(line.op2); /* Set the destination operand type. */
	memory.valueBits.cmdBits.src = getOpTypeId(line.op1); /* Set the source operand type. */
	memory.valueBits.cmdBits.opcode = line.cmd->opcode; /* Set the opcode. */

	return memory;
}

memoryWord getOpMemoryWord(operandInfo op, bool isDest)
{
	memoryWord memory = { 0 };

	if (op.type == REGISTER)
	{
		memory.are = (areType)ABSOLUTE;

		if (isDest)
		{
			memory.valueBits.regBits.destBits = op.value; /* Set the destination register value. */
		}
		else
		{
			memory.valueBits.regBits.srcBits = op.value; /* Set the source register value. */
		}
	}
	else if (op.type == INDREGISTER)
	{
		memory.are = (areType)ABSOLUTE;

		if (isDest)
		{
			memory.valueBits.regBits.destBits = op.value; /* Set the destination indirect register value. */
		}
		else
		{
			memory.valueBits.regBits.srcBits = op.value; /* Set the source indirect register value. */
		}
	}
	else
	{
		labelInfo *label = getLabel(op.str);

		if (op.type == LABEL && label && label->isExtern)
		{
			memory.are = EXTERNAL; /* Set the ARE type to external if the label is external. */
		}
		else
		{
			memory.are = (op.type == NUMBER) ? (areType)ABSOLUTE : (areType)RELOCATABLE; /* Set ARE type based on operand type. */
		}

		memory.valueBits.value = op.value; /* Set the operand value. */
	}

	return memory;
}

void addWordToMemory(int *memoryArr, int *memoryCounter, memoryWord memory)
{
	if (*memoryCounter < MAX_DATA_NUM)
	{
		memoryArr[(*memoryCounter)++] = getNumFromMemoryWord(memory); /* Add the memory word to the memory array. */
	}
}

bool addLineToMemory(int *memoryArr, int *memoryCounter, lineInfo *line)
{
	bool foundError = FALSE;

	if (!line->isError && line->cmd != NULL)
	{
		if (!updateLabelOpAddress(&line->op1, line->lineNum) || !updateLabelOpAddress(&line->op2, line->lineNum))
		{
			line->isError = TRUE;
			foundError = TRUE; /* Mark the line as an error if label update fails. */
		}

		addWordToMemory(memoryArr, memoryCounter, getCmdMemoryWord(*line)); /* Add the command memory word to memory. */

		if (line->op1.type == REGISTER && line->op2.type == REGISTER)
		{
			memoryWord memory = { 0 };
			memory.are = (areType)ABSOLUTE;
			memory.valueBits.regBits.destBits = line->op2.value;
			memory.valueBits.regBits.srcBits = line->op1.value;

			addWordToMemory(memoryArr, memoryCounter, memory); /* Add register operand memory word. */
		}
		else if (line->op1.type == REGISTER && line->op2.type == INDREGISTER)
		{
			memoryWord memory = { 0 };
			memory.are = (areType)ABSOLUTE;
			memory.valueBits.regBits.destBits = line->op2.value;
			memory.valueBits.regBits.srcBits = line->op1.value;

			addWordToMemory(memoryArr, memoryCounter, memory); /* Add indirect register operand memory word. */
		}
		else if (line->op1.type == INDREGISTER && line->op2.type == REGISTER)
		{
			memoryWord memory = { 0 };
			memory.are = (areType)ABSOLUTE;
			memory.valueBits.regBits.destBits = line->op2.value;
			memory.valueBits.regBits.srcBits = line->op1.value;

			addWordToMemory(memoryArr, memoryCounter, memory); /* Add indirect register operand memory word. */
		}
		else if (line->op1.type == INDREGISTER && line->op2.type == INDREGISTER)
		{
			memoryWord memory = { 0 };
			memory.are = (areType)ABSOLUTE;
			memory.valueBits.regBits.destBits = line->op2.value;
			memory.valueBits.regBits.srcBits = line->op1.value;

			addWordToMemory(memoryArr, memoryCounter, memory); /* Add indirect register operand memory word. */
		}
		else
		{
			if (line->op1.type != INVALID)
			{
				line->op1.address = FIRST_ADDRESS + *memoryCounter;
				addWordToMemory(memoryArr, memoryCounter, getOpMemoryWord(line->op1, FALSE)); /* Add operand 1 memory word to memory. */
			}

			if (line->op2.type != INVALID)
			{
				line->op2.address = FIRST_ADDRESS + *memoryCounter;
				addWordToMemory(memoryArr, memoryCounter, getOpMemoryWord(line->op2, TRUE)); /* Add operand 2 memory word to memory. */
			}
		}
	}

	return !foundError; /* Return true if no error was found. */
}

void addDataToMemory(int *memoryArr, int *memoryCounter, int DC)
{
	int i;
	unsigned int mask = ~0;
	mask >>= (sizeof(int) * BYTE_SIZE - MEMORY_WORD_LENGTH);

	for (i = 0; i < DC; i++)
	{
		if (*memoryCounter < MAX_DATA_NUM)
		{
			memoryArr[(*memoryCounter)++] = mask & g_dataArr[i]; /* Add data to memory array with mask applied. */
		}
		else
		{
			return;
		}
	}
}

int secondPass(int *memoryArr, lineInfo *linesArr, int lineNum, int IC, int DC)
{
	int errorsFound = 0, memoryCounter = 0, i;

	updateDataLabelsAddress(IC); /* Update the address of data labels based on IC. */

	errorsFound += countIllegalEntries(); /* Count illegal entries and update errorsFound. */

	for (i = 0; i < lineNum; i++)
	{
		if (!addLineToMemory(memoryArr, &memoryCounter, &linesArr[i]))
		{
			errorsFound++; /* Increment errorsFound if adding a line to memory fails. */
		}
	}

	addDataToMemory(memoryArr, &memoryCounter, DC); /* Add data to memory after processing lines. */

	return errorsFound; /* Return the total number of errors found. */
}