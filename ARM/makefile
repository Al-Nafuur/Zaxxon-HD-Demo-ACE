
###############################################################################
# ARM code makefile
#
# Harmony custom C makefile - (C) Copyright - Fred Quimby 2010  - AtariAge
# Based on code by Chris Walton
###############################################################################

# Tool names
TOOLCHAIN = ../../../ARM/bin/arm-elf
CC        = $(TOOLCHAIN)-gcc
AS        = $(TOOLCHAIN)-as
LD        = $(TOOLCHAIN)-ld
OBJCOPY   = $(TOOLCHAIN)-objcopy
SIZE      = $(TOOLCHAIN)-size
MKDIR     = mkdir

# Compiler flags
OPTIMIZATION = -Os
CFLAGS = -mcpu=arm7tdmi -march=armv4t -mthumb # -mthumb-interwork
CFLAGS += -Wall -ffunction-sections -save-temps -g -Wa,-a,-ad,-alhms=$(<:%.c=%.lst) # -mlong-calls -save-temps
CFLAGS += $(OPTIMIZATION)

# Preprocessor flags
#CPPFLAGS = -D${VIDEO}

# Source paths
SRC = src

# Output directory
BIN = bin

############################# HARMONY CUSTOM C ##############################

CUSTOMNAME = zhd
CUSTOMELF  = $(BIN)/$(CUSTOMNAME).elf
CUSTOMARM  = $(BIN)/$(CUSTOMNAME).arm
CUSTOMMAP  = $(BIN)/$(CUSTOMNAME).map
CUSTOMLST  = $(BIN)/$(CUSTOMNAME).lst
CUSTOMLINK = $(SRC)/custom.boot.lds
CUSTOMOBJS = custom.o main.o


$(CUSTOMNAME): mkbin $(CUSTOMARM)

mkbin:
	@$(MKDIR) -p $(BIN)

custom.o:
	@$(AS) -o $@ $(SRC)/custom.s

main.o: main.c colors.h displaydata.h
	@$(CC) $(CFLAGS) -DTV_EXTERN=$(TV_EXTERN) -c -o $@ $<

$(CUSTOMELF): $(CUSTOMOBJS)
	@$(CC) $(CFLAGS) -o $(CUSTOMELF) $(CUSTOMOBJS) -T $(CUSTOMLINK) -nostartfiles -Wl,-Map=$(CUSTOMMAP),--gc-sections

$(CUSTOMARM): $(CUSTOMELF)
	@$(OBJCOPY) -O binary -S $(CUSTOMELF) $(CUSTOMARM)
	@$(SIZE) $(CUSTOMOBJS) $(CUSTOMELF)

############################# CLEAN PROJECT ###################################

clean:
	@$(RM) custom.o main.o main.i main.s main.lst
	@$(RM) -r $(BIN)
