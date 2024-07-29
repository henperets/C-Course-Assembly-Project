/* The C Programming Language Course Final Project, Semester 2024B */
/* Menahem Hen Perets, ID: 318246014 */
#include "assembler.h"
#include "pre_processor.h"
#include "utility.h"
#include "sort_text.h"

labelInfo g_labelArr[MAX_LABELS_NUM]; /* Array of labelInfo structures to store labels encountered during assembly. */
lineInfo *g_entryLines[MAX_LABELS_NUM]; /* Array of pointers to lineInfo structures for entry lines. */
int g_dataArr[MAX_DATA_NUM]; /* Array to store data values encountered during assembly. */
int g_labelNum = 0; /* Counter to keep track of the number of labels. */
int g_entryLabelsNum = 0; /* Counter to keep track of the number of entry labels. */

/**
 * This is the main function that handles input file processing.
 * @param argc The count of command-line arguments.
 * @param argv An array of strings representing the command-line arguments.
 * @return Returns 0 if the program completes successfully.
 */
int main(int argc, char *argv[])
{
    char *as_file, *am_file, *temp_file; /* Pointers to the as, am, and temp files. */
    FILE *file; /* File pointer. */
    int i; /* Loop variable. */

    if (argc < 2)
    {
        printf("ERROR: No file names were given.\n");
        return 1;
    }

    for (i = 1; i < argc; i++)
    {
        printf("Phase 1: Start the pre-processor\n");
        as_file = addNewFile(argv[i], ".as"); /* Create a new file with the extension ".as". */
        temp_file = removeExtraSpacesFile(as_file); /* Remove extra spaces from the .as file. */

        if (!temp_file)
        {
            free(as_file); /* Free memory. */
            continue;
        }

        if (!macroExecute(temp_file)) /* Execute the macro pre-processor on the temp file. */
        {
            free(as_file); /* Free memory. */
            free(temp_file); /* Free memory. */
            continue; /* If the execution failed - move to the next file. */
        }

        printf("Phase 2: Start the first pass\n");
        am_file = addNewFile(argv[i], ".am"); /* Create a new file with the extension ".am". */
        remove(temp_file);
        file = fopen(am_file, "r"); /* Open the .am file for reading. */
        
        if (!file) 
        {
            printf("ERROR: Failed to open file \"%s\".\n", am_file);
            free(as_file); /* Free memory. */
            free(am_file); /* Free memory. */
            continue; /* Move to the next file if the file cannot be opened. */
        }

        parseFile(am_file); /* Parse the .am file after macro expansion and first pass. */

        free(as_file); /* Free memory. */
        free(am_file); /* Free memory. */
    }
    
    printf("Done\n");
    return 0;
}