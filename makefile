# The C Programming Language Course Final Project, Semester 2024B, Open University Israel
# Name: Menahem Hen Perets, ID: 318246014
# Course Instructor: Tal Rosenberger

# Directories
SRC_DIR = source_files
INC_DIR = headers
BIN_DIR = bin

# Files
EXEC_FILE = assembler
C_FILES = $(wildcard $(SRC_DIR)/*.c)
H_FILES = $(wildcard $(INC_DIR)/*.h)

# Object files
O_FILES = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(C_FILES))

# Targets
all: $(BIN_DIR) $(EXEC_FILE)

$(EXEC_FILE): $(O_FILES)
	gcc -Wall -ansi -pedantic $(O_FILES) -o $(EXEC_FILE)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c $(H_FILES)
	gcc -Wall -ansi -pedantic -I$(INC_DIR) -c -o $@ $<

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(BIN_DIR)/*.o $(EXEC_FILE)