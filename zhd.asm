
; ********************************************************************
;  Zaxxon HD Demo
;
;    Frame code
;
;    $Date: Sun, 27 Sep 2015 00:35:47 +0200 $
;    $Author: dietrich $
;    $Revision: 397 $
;
;  Copyright (C) 2015 Andreas Dietrich
; ********************************************************************
                
                PROCESSOR 6502

TV_SYSTEM       SET "NTSC"
PRINT_MESSAGES  SET 1

; --------------------------------------------------------------------        
;       Includes
; --------------------------------------------------------------------        
                
                INCLUDE vcs.h
                INCLUDE macro.h
                INCLUDE dpcplus.h
                INCLUDE colors.asm

; --------------------------------------------------------------------        
;       Global Constants
; --------------------------------------------------------------------

; address equates

ROMStart        = $F000
ROMSize         = $1000

RAMBase         = $80
RAMTop          = $FF

;---------------------------------------------------------------------
;       Global RAM Layout
;---------------------------------------------------------------------

; standard global variables

Bank            = $80
LineCtr         = $81
EventCtr        = $82
FrameCtr        = $83
Random          = $84
Temp            = $85
Ptr             = $86 ; [2]

; stack (3 levels)

StackBottom     = $FA
StackTop        = $FF


; ********************************************************************
;
;       ROM Code
;
; ********************************************************************

;
; DCP+ bank layout
;

DPCplusDriverStart = $0000
Bank0Start         = $0C00
Bank1Start         = $1C00
Bank2Start         = $2C00
Bank3Start         = $3C00
Bank4Start         = $4C00
Bank5Start         = $5C00
DisplayDataStart   = $6C00
FrequencyDataStart = $7C00

DisplayDataSize    = $1000
FrequencyDataSize  = $0400
DataSize           = DisplayDataSize + FrequencyDataSize

CodeDriverStart    = DPCplusDriverStart ; $0000 -  3K DPC+ driver
CodeARMStart       = Bank0Start         ; $0C00 - 20K ARM code
Code6507Start      = Bank5Start         ; $5C00 -  4K 6507 code
DataStart          = DisplayDataStart   ; $6C00 -  5K data

; --------------------------------------------------------------------        
;       ARM code
; --------------------------------------------------------------------

                ; DPC+ driver  ---------------------------------------

                ORG CodeDriverStart

                INCBIN "DPCplus/dpcplus.arm"

                ; custom ARM code ------------------------------------

                ORG CodeARMStart

                INCBIN "ARM/bin/zhd.arm"

BytesLeftARM = Code6507Start - *

; --------------------------------------------------------------------        
;       6507 code
; --------------------------------------------------------------------

                ; bank content ---------------------------------------

                ORG Code6507Start
                RORG ROMStart

                ; DPC+ registers
; $F000
DPCplus:        DS.B    128,0

                ; 6507 program
; $F080
Start:          INCLUDE "main.asm"

BytesLeft6507 = (ROMStart + $0FF6) - *

                ; bank end -------------------------------------------

                ORG Code6507Start + $0FF6
                RORG ROMStart + $0FF6

                ; bank switching hotspots
; $FFF6
SELECT_BANK_0   BYTE    $F6
SELECT_BANK_1   BYTE    $F7
SELECT_BANK_2   BYTE    $F8
SELECT_BANK_3   BYTE    $F9
SELECT_BANK_4   BYTE    $FA ; same address as NMI vector
SELECT_BANK_5   BYTE    $FB ;

                ; CPU vectors
; $FFFC
Reset:          WORD    Start ; $F080
IRQ:            WORD    Start ; $F080

; --------------------------------------------------------------------        
;       Data
; --------------------------------------------------------------------

                ORG DataStart
                RORG $0000

                ; Data

Data:           INCLUDE "data.asm"

BytesLeftData = DataSize - *

                DS.B    BytesLeftData, 0

#if PRINT_MESSAGES
                ECHO "---- Code ARM  :", BytesLeftARM, "bytes of ROM left"
                ECHO "---- Code 6507 :", BytesLeft6507, "bytes of ROM left"
                ECHO "---- Data      :", BytesLeftData, "bytes of ROM left"
                ECHO "===================================================="
#endif
