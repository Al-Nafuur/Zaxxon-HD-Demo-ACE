
CC     = g++
DASM   = ../../DASM/bin/Mac/OSX/dasm
STELLA = ../../Stella/Stella3.9.3.app/Contents/MacOS/Stella

PROJECT = zhd
RELEASE = ZaxxonHDDemo

build = @$(DASM)   $(PROJECT).asm \
                 -o$(PROJECT).bin \
                 -l$(PROJECT).lst \
                 -s$(PROJECT).sym \
                 -DTV_EXTERN=$(1) \
                 -f3

headers = grep DD_ $(PROJECT).sym                            \
          | awk '{printf "\#define %-24s 0x%s\n", $$1, $$2}' \
          | sort                                             \
          > ./ARM/displaydata.h

$(PROJECT):
	@$(call build) > /dev/null
	@$(call headers)
	@$(MAKE) -C ./ARM TV_EXTERN=0
	@$(call build,0)

release: $(PROJECT)
	@$(MAKE) -C ./ARM clean
	@$(MAKE) -C ./ARM TV_EXTERN=1
	@$(call build,1)
	@cp $(PROJECT).bin $(RELEASE)_`date +%y%m%d`_NTSC.bin
	@$(MAKE) -C ./ARM clean
	@$(MAKE) -C ./ARM TV_EXTERN=2
	@$(call build,2)
	@cp $(PROJECT).bin $(RELEASE)_`date +%y%m%d`_PAL60.bin

clean:
	@$(MAKE) -C ./ARM clean
	@$(RM) ./ARM/displaydata.h
	@$(RM) $(PROJECT).bin $(PROJECT).lst $(PROJECT).sym
	@$(RM) $(RELEASE)*

run: $(PROJECT)
	@$(STELLA) -pp Yes $(PROJECT).bin 2>/dev/null

