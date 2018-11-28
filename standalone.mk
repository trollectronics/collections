# Tools
export TARGET		= m68k-elf-
export CC		= $(TARGET)gcc
export AS		= $(TARGET)as
export LD		= $(TARGET)ld
export AR		= $(TARGET)ar
SREC_CAT	= srec_cat

INCLUDEDIRS	= $(shell realpath $(TOPDIR)/include)

# Compiler and linker flags
INCLUDES	= $(addprefix -I,$(INCLUDEDIRS))

export CFLAGS		:= -Wall -g -O3 -std=c99 -ffreestanding -fno-builtin -nostdlib -static -m68040 $(INCLUDES)
export ASFLAGS		:= -msoft-float
export LDFLAGS		:= -T $(LINKSCRIPT) -lgcc

# Makefile configurations
MAKEFLAGS	+=	--no-print-directory
