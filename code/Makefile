# -------------------------------- INFORMATION ------------------------------- #
$(info PROJET - INFO EMBARQUEE)
$(info Equipe : S4KOZ17)
$(info )
# ---------------------------------------------------------------------------- #

# -------------------------------- VARIABLES --------------------------------- #
# Archive name, path, and destination
ARCHNAME := projet-S4KOZ17.tar
CODEPATH := ../../projet-S4KOZ17/code/
DOCPATH := ../../projet-S4KOZ17/doc/
READMEPATH := ../../projet-S4KOZ17/README.md
ARCHDEST := ../../archives-projet-S4KOZ17/
ARCHPATH := $(CODEPATH) $(DOCPATH) $(READMEPATH)

# Directories
SRCDIR := src/
INCDIR := include/
LIBDIR := lib/micro-ecc/
ODIR := output/

# Others
OTHERS := ../.vscode/

# Source files and headers files
SOURCES := $(wildcard $(SRCDIR)*.c) $(wildcard $(LIBDIR)*.c)
HEADERS := $(wildcard $(INCDIR)*.h) $(wildcard $(LIBDIR)*.h)

# Microcontroller-specific settings
MCU_FLAGS := -mmcu=atmega328p -DF_CPU=16000000UL

# General compiler warnings
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
                   -Wwrite-strings -Winline -Wno-long-long -Wstrict-prototypes

# Additional warnings
ADDITIONAL_WARNINGS := -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls \
                       -Wunused-function -Wunused-variable -Wunused-parameter

# Relaxed warnings for library micro-ecc
RELAXED_WARNINGS := -Wno-missing-prototypes -Wno-missing-declarations -Wno-redundant-decls \
                    -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter

# IMPORTANT: micro-ecc requirement for AVR "When compiling for AVR, you must have optimizations enabled (compile with -O1 or higher)."
OPTIMIZATION_LEVEL := -Os # -Os optimizes for code size, enabling all -O2 optimizations that don't increase the binary size

# Compiler flags for optimization, target architecture, and include path
CFLAGS := $(OPTIMIZATION_LEVEL) $(MCU_FLAGS) -I$(INCDIR) -I$(LIBDIR)

# AVRDUDE settings
ARDUINO_PORT := /dev/ttyACM0

# Executable name
EXEC_NAME := main

# No print directory
MAKEFLAGS += --no-print-directory 

# Object files
LIB_OBJECTS := $(patsubst $(LIBDIR)%.c,$(ODIR)%.o,$(wildcard $(LIBDIR)*.c))
NONLIB_OBJECTS := $(patsubst $(SRCDIR)%.c,$(ODIR)%.o,$(wildcard $(SRCDIR)*.c))
# ---------------------------------------------------------------------------- #

# -------------------------------- TARGETS ----------------------------------- #
# Phony targets (avoid conflicts with files of the same name)
.PHONY: all clean upload check-tty reset_tty archive test

# Main build: ensures Arduino connection, compiles, and uploads program
all: clean | check-tty $(ODIR) reset_tty upload # | is a prerequisite separator (in order)

# Test mode
test:
	@$(MAKE) all CFLAGS="$(CFLAGS) -DTEST=1";  \
	printf "\033[1m> Test mode <\033[0m\n"

# Check if the Arduino is connected
check-tty:
	@if [ ! -e "$(ARDUINO_PORT)" ]; then \
		echo "Error: $(ARDUINO_PORT) does not exist." >&2; \
		exit 1; \
	fi

# Toggle DTR to reset the Arduino (to ensure a clean environment when uploading)
reset_tty:
	@stty -F $(ARDUINO_PORT) hupcl

# Upload the compiled program to the Arduino
upload: $(ODIR)$(EXEC_NAME).hex
	@echo "> Uploading program to Arduino<"
	@avrdude -v -patmega328p -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:$<:i

# Convert the compiled ELF file to HEX format for uploading
$(ODIR)$(EXEC_NAME).hex: $(ODIR)$(EXEC_NAME).elf
	@echo "> Converting ELF to HEX <"
	@avr-objcopy -O ihex $< $@

# Linking the object files to create the final ELF file
$(ODIR)$(EXEC_NAME).elf: $(LIB_OBJECTS) $(NONLIB_OBJECTS)
	@echo "> Linking <"
	@avr-gcc $(CFLAGS) -o $@ $^
	@-rm -f $(ODIR)*.o

# Compile library files
$(LIB_OBJECTS): $(ODIR)%.o: $(LIBDIR)%.c
	@echo "> Compiling library source $< <"
	@avr-gcc $(CFLAGS) $(WARNINGS) $(RELAXED_WARNINGS) -c $< -o $@

# Compile project files
$(NONLIB_OBJECTS): $(ODIR)%.o: $(SRCDIR)%.c
	@echo "> Compiling source $< <"
	@avr-gcc $(CFLAGS) $(WARNINGS) $(ADDITIONAL_WARNINGS) -c $< -o $@

# Create output directory if it doesn't exist
$(ODIR):
	@echo "> Creating output directory <"
	@mkdir -p $@

# Create an archive of the project
archive:
	mkdir -p $(ARCHDEST)
	tar -cvf $(ARCHDEST)$(ARCHNAME) $(ARCHPATH)

# Clean target to remove the "output" directory and its contents and the archive
clean:
	@echo "> Cleaning up <"
	@-rm -rf $(ODIR) $(ARCHDEST)$(ARCHNAME) $(OTHERS)
# ---------------------------------------------------------------------------- #


# ------------------------------- SYNTAX ------------------------------------- #
# cmd vs -cmd, make is not stopped if cmd fails (example : -rm)
# $@ name of the target
# $^ name of all the prerequisites with duplicates removed
# $< name of the first prerequisite
# $? name of all prerequisites that are newer than the target
# $* name of the target without suffix
# ---------------------------------------------------------------------------- #