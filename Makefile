##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.18.0-B7] date: [Mon Dec 12 15:40:37 EST 2022] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = SpiLoopback


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
#OPT = -std=c++11
OPT =

#######################################
# paths
#######################################
# Build path
BUILD_DIR  := build.$(PLATFORM)
DEPS       := .deps.$(PLATFORM)
SRCDIR     :=VSrc Core/Src Drivers/STM32F3xx_HAL_Driver/Src Application
INCDIR     :=VInc Core/Inc Drivers/STM32F3xx_HAL_Driver/Inc Drivers/STM32F3xx_HAL_Driver/Inc/Legacy Drivers/CMSIS/Device/ST/STM32F3xx/Include Drivers/CMSIS/Include
INCS       := $(foreach dir,$(INCDIR),$(addprefix -I./,$(dir)))

ELF        := $(TARGET)
######################################
# source
######################################
# asm, C, C++ sources
SRCCPPS    := $(foreach dir,$(SRCDIR),$(shell find $(dir) -name '*.cpp'| egrep -v '/test|$(ELF)|template|DONTUSE'))
SRCCCS     := $(foreach dir,$(SRCDIR),$(shell find $(dir) -name '*.c'  | egrep -v '/test|$(ELF)|template|DONTUSE'))
SRCASMS    := $(foreach dir,$(SRCDIR),$(shell find $(dir) -name '*.s'  | egrep -v '/test|$(ELF)|template|DONTUSE'))

C_SOURCES   := $(SRCCCS)
C_INCLUDES  := $(INCS)
CPP_SOURCES := $(SRCCPPS)

# ASM sources
ASM_SOURCES := $(SRCASMS) 

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)g++
CPP= $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)g++
CPP= $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F303xE


# AS includes
AS_INCLUDES = 

## C includes


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -u _printf_float


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F303RETx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys -lstdc++
LIBDIR = 
LDFLAGS = $(MCU) $(LIBS) -specs=nosys.specs -T$(LDSCRIPT) $(LIBDIR) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS  = 
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

vpath %.c   $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
vpath %.s   $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	$(CPP) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) $(BUILD_DIR)/$(ELF).o Makefile
	$(CC) $(OBJECTS) $(BUILD_DIR)/$(ELF).o $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	

$(BUILD_DIR):
	mkdir $@	

debug:
	#@echo 'debug:'
	#@echo '       TESTOBJTARGETS: ' $(TESTOBJTARGETS)
	#@echo '       EXETARGETOBJ  : ' $(EXETARGETOBJ)
	#@echo '       TESTTARGETS   : ' $(TESTTARGETS)
	#@echo '       ELFTARGET     : ' $(ELFTARGET)
	#@echo '       LINKFLAGS     : ' $(LINKFLAGS)
	#@echo '       LIBTARGET     : ' $(LIBTARGET)
	#@echo '       EXETARGET     : ' $(EXETARGET)
	#@echo '       TARGET        : ' $(TARGET)
	#@echo '       LIBSRCXX      : ' $(LIBSRCXX)
	#@echo '       PLATFORM      : ' $(PLATFORM)
	#@echo '       LIBOBJS       : ' $(LIBOBJS)
	#@echo '       OBJECTS       : ' $(OBJECTS)
	#@echo '       TESTSRC       : ' $(TESTSRC)
	#@echo '       CXXFLAGS      : ' $(CXXFLAGS)
	#@echo '       EXCLUDET      : ' "$(EXCLUDET)"
	#@echo '       EXCLUDE       : ' "$(EXCLUDE)"
	#@echo '       DEPLIBS       : ' "$(DEPLIBS)"
	#@echo '       DEPINCS       : ' "$(DEPINCS)"
	#@echo '       SRCCPPS       : ' $(SRCCPPS)
	#@echo '       CPP_SOURCES   : ' $(CPP_SOURCES)
	#@echo '       C_SOURCES     : ' $(C_SOURCES)
	#@echo '       FLTKLIB       : ' $(FLTKLIB)
	#@echo '       SRCASMS       : ' $(SRCASMS)
	#@echo '       SRCCCS        : ' $(SRCCCS)
	#@echo '       INCDIR        : ' $(INCDIR)
	#@echo '       SRCDIR        : ' $(SRCDIR)
	#@echo '       SRCCCS        : ' $(SRCCCS)
	#@echo '       SRCASM        : ' $(SRCASM)
	#@echo '       WHICH         : ' $(WHICH)
	#@echo '       DEPLIBLIBS    : ' $(DEPLIBLIBS)
	#@echo '       LLIBS         : ' $(LLIBS)
	#@echo '       LIBS          : ' $(LIBS)
	#@echo '       LIB           : ' $(LIB)
	#@echo '       DEPS          : ' $(DEPS)
	#@echo '       EXES          : ' $(EXES)
	#@echo '       VPATHCPP      : ' $(VPATHCPP)
	#@echo '       VPATHC        : ' $(VPATHC)
	#@echo '       VPATHS        : ' $(VPATHS)
	#@echo '       VPATHO        : ' $(VPATHO)
	#@echo '       PREFIX        : ' $(PREFIX)
	#@echo '       INCS          : ' $(INCS)
	#@echo '       C_INCLUDES    : ' $(C_INCLUDES)
	#@echo '       ELF           : ' $(ELF)
	#@echo '       CC            : ' $(CC)
	#@echo '       CPP           : ' $(CPP)
	#@echo '       GPPSTD        : ' $(GPPSTD)
	#@echo '       DOPLATS       : ' $(DOPLATS)
	#@echo '       DOPLATS       : ' $(findstring $(PLATFORM),$(EXECSPLATS))
	#@echo '       EXESPLATS     : ' $(EXECSPLATS)
	#@echo 'DESKTOP APP DIR      : ' $(DESKTOP)
	#@echo 'sort dir SRCCPPS     : ' $(sort $(dir $(SRCCPPS)))
	#@echo 'sort dir SRCCCS      : ' $(sort $(dir $(SRCCCS)))
	#@echo 'sort dir SRCASMS     : ' $(sort $(dir $(SRCCASMS)))
	#@echo ' objects : ' $(addsuffix .o,$(EXETARGET) $(TESTTARGETS))
	#@echo '       TESTEXP       : ' $(TESTEXP)     
	#@echo '       TESTEXP2      : ' $(TESTEXP2)     
	#@echo '       TESTEXP3      : ' $(TESTEXP3)     

debugmsg:
TARGET = SpiLoopback


#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
