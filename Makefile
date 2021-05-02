##########################################################################################################################
# v2p_ca9 GCC compiler Makefile
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
# ------------------------------------------------

######################################
# target
######################################
TARGET = v2p_ca9
######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og
# tools dir
OS_NAME = $(shell uname -o)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')
ifeq ($(LC_OS_NAME), cygwin)
GCC_DIR  = /cygdrive/c/Users/pc/Desktop/v2p_ca9/gcc-arm-10.2-2020.11-mingw-w64-i686-arm-none-linux-gnueabihf/bin
QEMU_DIR = /cygdrive/c/Program\ Files/qemu
else
GCC_DIR  = /media/xiaoming/xiaoming_data/data/qiaoqm/linux_kernel/ci_linux/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin
QEMU_DIR = /usr/local/bin
endif

#######################################
# binaries
#######################################
PREFIX    = $(GCC_DIR)/arm-none-linux-gnueabihf-
CC        = $(PREFIX)gcc
AS        = $(PREFIX)gcc -x assembler-with-cpp
OBJCOPY   = $(PREFIX)objcopy
OBJDUMP   = $(PREFIX)objdump
AR        = $(PREFIX)ar
SZ        = $(PREFIX)size
LD        = $(PREFIX)ld
HEX       = $(OBJCOPY) -O ihex
BIN       = $(OBJCOPY) -O binary -S
GDB       = $(PREFIX)gdb
QEMU      = $(QEMU_DIR)/qemu-system-arm


PROJECTBASE = $(PWD)
override PROJECTBASE    := $(abspath $(PROJECTBASE))
TOP_DIR = $(PROJECTBASE)


#######################################
# Build path
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

######################################
# source
######################################
# C sources
C_SOURCES =  \
        ${wildcard $(TOP_DIR)/cmsis/source/*.c} \
        ${wildcard $(TOP_DIR)/debug_log/*.c} \
        ${wildcard $(TOP_DIR)/driver/*.c} \
        ${wildcard $(TOP_DIR)/freertos/*.c} \
        ${wildcard $(TOP_DIR)/freertos/portable/GCC/ARM_CA9/*.c} \
        ${wildcard $(TOP_DIR)/freertos/portable/MemMang/heap_4.c} \
        ${wildcard $(TOP_DIR)/*.c}

# ASM sources
ASM_SOURCES =  \
        ${wildcard $(TOP_DIR)/freertos/portable/GCC/ARM_CA9/*.s} \
        ${wildcard $(TOP_DIR)/cstartup.s}


######################################
# third party ibrary
######################################
THIRDLIB = 

#######################################
# CFLAGS
#######################################
# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS = 

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES = \
        -I $(TOP_DIR)/debug_log  \
        -I $(TOP_DIR)/driver  \
        -I $(TOP_DIR)/cmsis/include \
        -I $(TOP_DIR)/freertos/include \
        -I $(TOP_DIR)/freertos/portable/GCC/ARM_CA9 \
        -I $(TOP_DIR)


# compile gcc flags
ASFLAGS = -marm -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fno-pie 

CFLAGS = -marm -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fno-pie 

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LD_FILE = $(TARGET).lds
LDSCRIPT = $(PROJECTBASE)/$(LD_FILE)

# libraries
LIBS = -lm 
LIBDIR =
LDFLAGS = -marm -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -nostartfiles -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -static -Wl,--gc-sections -Wl,--build-id=none

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).lst


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
# list of lib objects
OBJECTS += $(THIRDLIB)

$(OBJ_DIR)/%.o: %.c Makefile | $(OBJ_DIR)
	@echo CC $(notdir $<)
	@$(CC) -c $(subst $(TOP_DIR), ., $(CFLAGS)) -Wa,-a,-ad,-alms=$(OBJ_DIR)/$(notdir $(<:.c=.lst)) $(subst $(TOP_DIR), ., $<) -o $@

$(OBJ_DIR)/%.o: %.s Makefile | $(OBJ_DIR)
	@echo AS $(notdir $<)
	@$(AS) -c $(subst $(TOP_DIR), ., $(CFLAGS)) $(subst $(TOP_DIR), ., $<) -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) $(LD_FILE) Makefile
	@echo LD $(notdir $@)
	@$(CC) $(OBJECTS) $(subst $(TOP_DIR), ., $(LDFLAGS)) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo HEX $(notdir $@)
	@$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo BIN $(notdir $@)
	@$(BIN) $< $@

$(BUILD_DIR)/%.lst: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo OBJDUMP $(notdir $@)
	@$(OBJDUMP) --source --demangle --disassemble --reloc --wide $< > $@
	@$(SZ) --format=berkeley $<

$(BUILD_DIR):
	@echo MKDIR $@
	@mkdir $@

ifeq ($(OBJ_DIR), $(wildcard $(OBJ_DIR)))
else
$(OBJ_DIR):$(BUILD_DIR)
	@echo MKDIR $@
	@mkdir $@
endif

#######################################
# clean up
#######################################
clean:
	@echo RM $(BUILD_DIR)
	@rm -fR $(BUILD_DIR)

#######################################
# use gdb debug
#######################################
debug:
	@echo start GDB
	@$(GDB) -x $(BUILD_DIR)/../gdb.script $(BUILD_DIR)/$(TARGET).elf

qemu:
	@echo start qemu
	@$(QEMU) -M vexpress-a9 -m 1024m -smp 4 -kernel $(BUILD_DIR)/$(TARGET).elf -nographic

#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
