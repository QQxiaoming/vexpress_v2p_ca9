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
OPT_LEVEL = 1
# tools dir
OS_NAME = $(shell uname -o)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')
ifeq ($(LC_OS_NAME), cygwin)
GCC_DIR     = /cygdrive/c/Users/pc/Desktop/v2p_ca9/gcc-arm-10.2-2020.11-mingw-w64-i686-arm-none-linux-gnueabihf/bin
QEMU_DIR    = /cygdrive/c/Program\ Files/qemu
INSIGHT_DIR = /cygdrive/c/Users/pc/Desktop/v2p_ca9/arm-none-linux-gnueabihf-insight/bin
RUST_DIR    = /cygdrive/c/Users/pc/.cargo/bin
else
GCC_DIR     = /opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin
QEMU_DIR    = /usr/local/bin
INSIGHT_DIR = /opt/arm-none-linux-gnueabihf-insight/bin
RUST_DIR    = ~/.cargo/bin
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
INSIGHT   = $(INSIGHT_DIR)/arm-none-linux-gnueabihf-insight
QEMU      = $(QEMU_DIR)/qemu-system-arm
RUSTC     = $(RUST_DIR)/rustc

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

# RUST sources
RUST_SOURCES =  \
        ${wildcard $(TOP_DIR)/rust_src/rust_test.rs}

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
ASFLAGS = -marm -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon $(AS_DEFS) $(AS_INCLUDES) -O$(OPT_LEVEL) -Wall -fdata-sections -ffunction-sections -fno-pie 

CFLAGS = -marm -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon $(C_DEFS) $(C_INCLUDES) -O$(OPT_LEVEL) -Wall -fdata-sections -ffunction-sections -fno-pie 

RUSTFLAGS = -C panic=abort -C opt-level=$(OPT_LEVEL) --crate-type=lib --emit obj --emit asm --emit dep-info --target=armv7-unknown-linux-gnueabihf

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
RUSTFLAGS += -g
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
LIBS = -lm -lrust_core
LIBDIR = -L$(TOP_DIR)/rust_core_lib
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
# list of RUST program objects
OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(RUST_SOURCES:.rs=.o)))
vpath %.rs $(sort $(dir $(RUST_SOURCES)))
# list of lib objects
OBJECTS += $(THIRDLIB)

$(OBJ_DIR)/%.o: %.c Makefile | $(OBJ_DIR)
	@echo CC $(notdir $<)
	@$(CC) -c $(subst $(TOP_DIR), ., $(CFLAGS)) -Wa,-a,-ad,-alms=$(OBJ_DIR)/$(notdir $(<:.c=.lst)) $(subst $(TOP_DIR), ., $<) -o $@

$(OBJ_DIR)/%.o: %.s Makefile | $(OBJ_DIR)
	@echo AS $(notdir $<)
	@$(AS) -c $(subst $(TOP_DIR), ., $(CFLAGS)) $(subst $(TOP_DIR), ., $<) -o $@

$(OBJ_DIR)/%.o: %.rs Makefile | $(OBJ_DIR)
	@echo RUSTC $(notdir $<)
	@$(RUSTC) $(RUSTFLAGS) $(subst $(TOP_DIR), ., $<) --out-dir $(OBJ_DIR)

$(TOP_DIR)/rust_core_lib/librust_core.a: $(TOP_DIR)/rust_core_lib/rust_core.rs Makefile
	@echo BUILD $(notdir $@)
	@$(RUSTC) -C panic=abort -C opt-level=$(OPT_LEVEL) --crate-type=staticlib --target=armv7-unknown-linux-gnueabihf $(subst $(TOP_DIR), ., $<) -o $(subst $(TOP_DIR), ., $@)

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) $(LD_FILE) $(TOP_DIR)/rust_core_lib/librust_core.a Makefile
	@echo LD $(notdir $@)
	@$(CC) $(OBJECTS) $(subst $(TOP_DIR), ., $(LDFLAGS)) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf Makefile | $(BUILD_DIR)
	@echo HEX $(notdir $@)
	@$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf Makefile | $(BUILD_DIR)
	@echo BIN $(notdir $@)
	@$(BIN) $< $@

$(BUILD_DIR)/%.lst: $(BUILD_DIR)/%.elf Makefile | $(BUILD_DIR)
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
debug: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/../gdb.script Makefile
	@echo start GDB
	@$(GDB) -x $(BUILD_DIR)/../gdb.script $(BUILD_DIR)/$(TARGET).elf

debug_gui: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/../gdb.script Makefile
	@echo start GDB
	@$(INSIGHT) -x $(BUILD_DIR)/../gdb.script $(BUILD_DIR)/$(TARGET).elf

qemu: $(BUILD_DIR)/$(TARGET).bin Makefile
	@echo start qemu
	@$(QEMU) \
	-M vexpress-a9 \
	-m 1024m \
	-smp 4 \
	-device loader,file=$(BUILD_DIR)/$(TARGET).bin,addr=0x60000000 \
	-device loader,addr=0x60000000,cpu-num=0 \
	-device loader,addr=0x60000000,cpu-num=1 \
	-device loader,addr=0x60000000,cpu-num=2 \
	-device loader,addr=0x60000000,cpu-num=3 \
	-nographic

qemu_gdb: $(BUILD_DIR)/$(TARGET).bin Makefile
	@echo start qemu
	@$(QEMU) \
	-M vexpress-a9 \
	-m 1024m \
	-smp 4 \
	-device loader,file=$(BUILD_DIR)/$(TARGET).bin,addr=0x60000000 \
	-nographic \
	-gdb tcp::1234 -S

#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
