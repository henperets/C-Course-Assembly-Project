# The C Programming Language Course Final Project (Course number 20465, Semester 2024B).
 
 Date: 29.07.2024

# Student Details:

 Name: Menahem Hen Perets
 ID  : 318246014

# Project details:

 This project is my final project of the course "The C Programming Language" in the Open university in Israel.
 The project involves processing assembly language files, handling macros, and generating output files.
 This is a comprehensive assembler program for a specific assembly language.
 The program is written in C language code and made from 3 main stages:

### Stage 1### Pre Processing: 
In the pre-assembler stage, the assembler handles a source input file with the extension ".as", it reads the source file line by line, stores the macro declarations and calls, organizes the file and prints the macro calls into a new file with the ".am" extension.
NOTE: The ".am" file will not create any output files if the file has errors.

### Stage 2### First Transition:
In the first pass stage labels (symbols) are recognized, copied to a symbols table and assigned numerical values representing their corresponding memory addresses.

### Stage 3### Second Transition:
In the second pass stage the assembler reads the source file line by line again and updates the machine codes of the operands that use symbols, the second pass then parses the ".am" file and creates 3 additional finals with the extensions: ".ent", ".ext" and ".ob".
The ".ent" file: this file contains all the entry label names found in the source file and their IC addresses (starting from 100).
The ".ext" file: this file contains all the extern label names found in the source file and their IC addresses (starting from 100).
NOTE: The ".ent" and ".ext" files are not created if there are no entry/extern lines in the input file.
The ".ob" file: this file contains all the command lines data in octal representation(converted from binary), and their corresponding addresses.
NOTE: The program can handle multiple files at once, so after the third stage if another file was called it will be processed next.
 
# GitHub Link:

 https://github.com/henperets/C-Course-Assembly-Project.git

# Project directory tree Structure:

├── headers # Header files
│ 	 ├── assembler.h 
│ 	 ├── pre_processor.h
│ 	 ├── sort_text.h
│ 	 ├── utility.h
├── source_files # C Source files
│ 	 ├── assembler.c # The main assembler file
│	 ├── pre_processor.c # Handles macros and creates the ".am" file
│ 	 ├── first_pass.c # Handles the first pass of the assembler
│	 ├── second_pass.c # Handles the second pass of the assembler
│ 	 ├── sort_text.c # Handles text sorting
│ 	 ├── utility.c # Utility functions
├── bin # Program binaries
│ 	 ├── assembler.o
│ 	 ├── first_pass.o
│ 	 ├── pre_processor.o
│ 	 ├── second_pass.o
│ 	 ├── sort_text.o
│ 	 ├── utility.o
├── tests # Example tests with output
│    ├── course_example
│          ├── course_example.as
│    ├── valid_tests
│	  		├── valid_01.as
│	  		├── valid_02.as
│	  		├── double_macro.as
│    ├── invalid_tests
│	  		├── invalid_01.as
│	  		├── invalid_02.as
│	  		├── invalid_03.as
│    ├── screenshots
│	  		├── success_01.png
│	  		├── success_02.png
│	  		├── errors_01.png
│	  		├── errors_02.png
│	  		├── errors_success_mixed.png
├── README.md # This readme file
└── assembler # The compiled assembler 

# How to use:
 Run the make file by using the command "make".
 To run the assembler on the input ".as" files use the command: "./assembler 'name'"
 NOTE: No need to add the extension ".as" to run the assembler, and multiple files can be called at once: "./assembler 'name 'name' name'".
 If you want to run an input file that is inside a folder, use the command: "./assembler 'folder_name'/'name'"
 For example in the main directory the command will look like this: "./assembler tests/courseexample doublemacro errors".
 *IMPORTANT*: If you want to run the assembler on multiple input files, have the input files in the same directory as the assembler file.
              If you are running the assembler on one file, it can be inside any folder just use the right command.	

# Documentation:
 Every function in this code has clear and informative documentation, the functions documentations for each code can be found as followed:
 first_pass.c: assembler.h
 second_pass.c: assembler.h
 pre_processor.c: pre_processor.h
 sort_text.c: sort_text.h
 utility.c: utility.h

# Tests:
 All test files can be found in the "tests" folder, screenshots of running the assembler are attached.

In the main directory you can find 4 input files ready to be processed by the program(The output files can be found in "tests" folder):
#course_example.c #double_macro.c #invalid_01 #invalid_02

--- 8 directories , 41 files ---