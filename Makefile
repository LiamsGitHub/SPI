######################################
# For STM32F100C8 on Mars Eclipse using the ARM GNU toolchain
# Supports both the assembler and C startup code stubs
# Liam Goudge August 2016
#
# This Makefile imports a generic Makefile at the top of the directory
#
################### Project name and sources #####################

ROOTDIR = ~/Eclipse
FOUNDATION = $(ROOTDIR)/WorkspaceJan16/Foundation
TOOLS = $(ROOTDIR)/gcc-arm-none-eabi-5_2-2015q4/bin/

NAME=SPI

C_SOURCES = main.c	# Enter list of all the C source files here
S_SOURCES = $(FOUNDATION)/STM32F100C8startup.S 	# Enter list of all the assembler source files here
#S_SOURCES = StartUp_simple.s 	# Enter list of all the assembler source files here


OBJECTS = $(C_SOURCES:.c=.o) $(S_SOURCES:.S=.o)
OBJDIR = Objects2
INCLUDES = -I./

################### Tool Location #####################
# Compiler/Assembler/Linker Paths

CC=		$(TOOLS)arm-none-eabi-gcc
OD =	$(TOOLS)arm-none-eabi-objdump
NM =	$(TOOLS)arm-none-eabi-nm
AS =	$(TOOLS)arm-none-eabi-as
SZ =	$(TOOLS)arm-none-eabi-size

################### Libraries #####################
# Library settings
USE_NANO=--specs=nano.specs --specs=rdimon.specs
USE_SEMIHOST=--specs=rdimon.specs -lc -lc -lrdimon
NO_SEMIHOST = -lgcc -lc -lm -nostartfiles
SIMPLE = -nostartfiles


################### GNU Flags #####################
# Compiler Flags
CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -g -c -v

# Assembler Flags
ASFLAGS = -mcpu=cortex-m3 -mthumb

# Linker Flags 
LINKER_SCRIPT = $(FOUNDATION)/STM32F100C8.ld
LDFLAGS=-mthumb -mcpu=cortex-m3 $(NO_SEMIHOST) -T $(LINKER_SCRIPT) # Use std libraries

# Other Stuff
ODFLAGS = -h --syms -S
REMOVE = rm -f

# This is the generic Make file for all my projects. MUST go at the end here and not before
#include $(FOUNDATION)/GenericMakefilev1


################### Build Steps #####################

all: $(NAME).elf

$(NAME).elf: $(OBJECTS)
	@ echo "Link:"
	$(CC) $(LDFLAGS)  $^ -o $@
	/bin/rm -f -v *.o $(FOUNDATION)/*.o
	$(OD) $(ODFLAGS) $@ > $(NAME).lst
	$(SZ) --format=berkeley $@
	
.S.o:
	@ echo "asm:"
	$(CC) $(ASFLAGS) -o $@ -c $<

.c.o:
	@ echo "c:"
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	@ echo " "
	@ echo "Clean up"
	/bin/rm -f *.o *.elf *.lst