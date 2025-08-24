# 1.Windows 下输出空行的正确方法echo.

### Project name (also used for output file name)
PROJECT	= ylad_gps_oled

### C Standard level (c89, gnu89, c99 or gnu99)
CSTD = c11

### Optimization level (0, 1, 2, 3, g or s)
OPTIMIZE = s

### Processor type and Thumb(-2) mode for CSRC/ASRC files (YES or NO)
CPU   = cortex-m3
THUMB = YES

### Linker script for the target MCU
LINKSCRIPT = STM32F103C6T6.ld

### Object output directory
OBJDIR = obj

# Default target.
all: version build size #在终端只执行make时，默认执行version build size


### Source files and search directories
#匹配$(csrc/*.c)文件
CSRC =	\
		$(wildcard *.c) \
    	$(wildcard start/*.c) \
    	$(wildcard SPL/STM32F10x_StdPeriph_Driver/src/*.c) \
    	$(wildcard SPL/CMSIS/CM3/*.c)
#匹配$(asrc/*.S)文件
ASRC	=$(wildcard start/*.S)
CSRCARM	=
ASRCARM	=


### Include dirs, library dirs and definitions
DEFS	=
ADEFS	=
MATHLIB	= -lm
LIBS	=
LIBDIRS	=
INCDIRS := \
			start/ \
			SPL/STM32F10x_StdPeriph_Driver/inc/ \
			SPL/CMSIS/CM3/ \


### Search directories (make uses VPATH to search source files)
# VPATH   = start/ SPL/STM32F10x_StdPeriph_Driver/src/ SPL/CMSIS/CM3/ #linux只能使用:当分隔符
vpath %.c $(sort $(dir $(CSRC)))#提取目录，sort去重


### Warning controls
WARNINGS = all extra error unused

### Output file type (hex, bin or both) and debugger type
OUTPUT	= hex
HEXFMT  = ihex
DEBUG	= dwarf-2#dwarf-3 #dwarf-4 


### Programs to build porject
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE    = arm-none-eabi-size
NM      = arm-none-eabi-nm


# Define all object files
COBJ      = $(CSRC:.c=.o) 
AOBJ      = $(ASRC:.S=.o)
COBJARM   = $(CSRCARM:.c=.o)
AOBJARM   = $(ASRCARM:.S=.o)
# Define all object files with directory
COBJ_dir  := $(addprefix $(OBJDIR)/,$(notdir $(COBJ)))
AOBJ_dir  := $(addprefix $(OBJDIR)/,$(notdir $(AOBJ)))
COBJARM_dir   := $(addprefix $(OBJDIR)/,$(notdir $(COBJARM)))
AOBJARM_dir   := $(addprefix $(OBJDIR)/,$(notdir $(AOBJARM)))
PROJECT_dir   := $(OBJDIR)/$(PROJECT)


### Compiler flags
ifeq ($(THUMB),YES)
THUMBFLAG = -mthumb
THUMBIW = -mthumb-interwork
else
THUMBFLAG =
THUMBIW =
endif


# Flags for C files
CFLAGS += -std=$(CSTD)
CFLAGS += -g -g$(DEBUG)
CFLAGS += -O$(OPTIMIZE)
CFLAGS += $(addprefix -W,$(WARNINGS))
CFLAGS += $(addprefix -I,$(INCDIRS))
CFLAGS += $(addprefix -D,$(DEFS))
CFLAGS += -Wp,-MM,-MP,-MT,$(OBJDIR)/$(*F).o,-MF,$(OBJDIR)/$(*F).d#-Wp:表示将后面的选项传递给预处理器 -MM:生成依赖关系 -MP:生成伪目标 -MT:指定目标文件 -MF:指定依赖文件的名称
# CFLAGS += -fno-diagnostics-color #禁止错误有颜色
CFLAGS += -fdiagnostics-color=always #始终使用错误颜色
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wa,-a,-ad,-alms=$(OBJDIR)/$(notdir $(<:.c=.lst))#查看单文件源码与汇编对应


# Assembler flags
ASFLAGS += $(addprefix -D,$(ADEFS)) -Wa,-g -g$(DEBUG)
ASFLAGS += -ffunction-sections -fdata-sections


# Linker flags
# LDFLAGS += -nostartfiles -Wl,-Map=$(PROJECT_dir).map,--cref,--gc-sections #-nostartfiles 是 GCC 前端选项，不在 binutils/ld 手册中
LDFLAGS += -Wl,-Map=$(PROJECT_dir).map,--cref,--gc-sections
LDFLAGS += -specs=nano.specs # 使用类似微库的newlib-nano库
# LDFLAGS += -lc_nano 你只需要选择 -specs=nano.specs 或 -lc_nano 其中之一即可指明使用的nano库;
LDFLAGS += $(patsubst %,-L%,$(LIBDIRS)) $(patsubst %,-l%,$(LIBS))
LDFLAGS += $(MATHLIB)
LDFLAGS += -T$(LINKSCRIPT)


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -mcpu=$(CPU) $(THUMBIW) -I. $(CFLAGS)
ALL_ASFLAGS = -mcpu=$(CPU) $(THUMBIW) -I. -x assembler-with-cpp $(ASFLAGS)


# Display compiler version information.
version :
	@$(CC) --version


# Display size of file.
size:$(PROJECT_dir).elf
	@echo "-----------------------------"
	$(SIZE) -A --radix=16 $<
	@echo "-----------------------------"
	$(SIZE) -A --radix=10 $<
	@echo "-----------------------------"
	$(SIZE) -B --radix=10 $<


ifeq ($(OUTPUT),hex)
build: elf hex lst sym
hex: $(PROJECT_dir).hex
else
ifeq ($(OUTPUT),bin)
build: elf bin lst sym
bin: $(PROJECT_dir).bin
else
ifeq ($(OUTPUT),both)
build: elf hex bin lst sym
hex: $(PROJECT_dir).hex
bin: $(PROJECT_dir).bin
else
$(error "Invalid format: $(OUTPUT)")
endif
endif
endif


elf: $(PROJECT_dir).elf
lst: $(PROJECT_dir).lst
sym: $(PROJECT_dir).sym


# Create final output file (.hex or .bin) from ELF output file.
%.hex: %.elf | $(OBJDIR)
	@echo.
	$(OBJCOPY) -O $(HEXFMT) $< $@

%.bin: %.elf | $(OBJDIR)
	@echo.
	$(OBJCOPY) -O binary $< $@

# Create extended listing file from ELF output file.
%.lst: %.elf | $(OBJDIR)
	@echo.
	$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf | $(OBJDIR)
	@echo.
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
%.elf:  $(AOBJARM_dir) $(AOBJ_dir) $(COBJARM_dir) $(COBJ_dir) | $(OBJDIR)
	@echo.
	@echo Linking...
	$(CC) $(THUMBFLAG) $(ALL_CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile: create object files from C source files. ARM or Thumb(-2)
$(COBJ_dir) : $(OBJDIR)/%.o : %.c | $(OBJDIR)
	@echo.
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_CFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARM_dir) : $(OBJDIR)/%.o : %.c | $(OBJDIR)
	@echo.
	@echo $< :
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Assemble: create object files from assembler source files. ARM or Thumb(-2)
$(AOBJ_dir) : $(OBJDIR)/%.o : %.S | $(OBJDIR)
	@echo.		
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_ASFLAGS) $< -o $@

# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM_dir) : $(OBJDIR)/%.o : %.S | $(OBJDIR)
	@echo.
	@echo $< :
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean:
	@echo.
	rmdir /S /Q $(OBJDIR) | exit 0
# 	rm -f -r $(OBJDIR) | exit 0 #等效于-rm -f -r $(OBJDIR)


# Include the dependency files.
# 确保目录存在的目标
$(OBJDIR):
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
# 包含依赖文件
-include $(wildcard $(OBJDIR)/*.d)


### help
help : 
	@echo ***********************************************
	@echo Available targets:
	@echo   all       - Build all targets
	@echo   clean     - Remove all generated files
	@echo   size      - Display size of the output file
	@echo   version   - Display compiler version
	@echo   elf       - Build ELF file
	@echo   hex       - Build HEX file
	@echo   bin       - Build BIN file
	@echo   lst       - Generate listing file
	@echo   sym       - Generate symbol file
	@echo ***********************************************
	@echo *************all .c file with dir**************
	@echo $(CSRC)
	@echo ****************all .c file********************
	@echo $(notdir $(CSRC))
	@echo *******************obj************************
	@echo $(COBJ_dir)
	@echo *******************vpath***********************
	@echo $(sort $(dir $(CSRC)))


.PHONY: all version build size elf hex bin lst sym clean help