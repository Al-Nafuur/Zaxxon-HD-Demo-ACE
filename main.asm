
; ********************************************************************
;  Zaxxon HD Demo
;
;    Main source file 6507
;
;    $Date: Sun, 27 Sep 2015 00:35:47 +0200 $
;    $Author: dietrich $
;    $Revision: 397 $
;
;  Copyright (C) 2015 Andreas Dietrich
; ********************************************************************

; --------------------------------------------------------------------        
;       Constants
; --------------------------------------------------------------------

; DPC+ data fetcher assigment

DF_GRP0         = <DF0DATA
DF_GRP1         = <DF1DATA
DF_GRP2         = <DF2DATA
DF_GRP3         = <DF3DATA
DF_GRP4         = <DF4DATA
DF_GRP5         = <DF5DATA

DF_COLUP        = <DF0FRACDATA
DF_COLUBK       = <DF1FRACDATA
DF_HMBL         = <DF2FRACDATA
DF_CTRLPF       = <DF3FRACDATA

; DPC+ ARM function identifiers

DPC_FUNC_INIT           = 0
DPC_FUNC_VERTICAL_BLANK = 1
DPC_FUNC_OVERSCAN       = 2

; --------------------------------------------------------------------        
;       Macros
; --------------------------------------------------------------------

        MAC DPC_SET_FETCHER

                lda     #<({1})
                sta     DF{2}LOW
                lda     #>({1})
                sta     DF{2}HI

        ENDM

; --------------------------------------------------------------------

        MAC DPC_READ_FETCHER

                lda     DF{2}DATA
                sta     {1}

        ENDM

; --------------------------------------------------------------------

        MAC DPC_WRITE_FETCHER

                lda     {1}
                sta     DF{2}WRITE

        ENDM

; --------------------------------------------------------------------

        MAC DPC_CALL_FUNCTION

                DPC_SET_FETCHER {1}, {2}

                lda     #{3}
                sta     DF{2}WRITE
                lda     #{4}
                sta     CALLFUNCTION

        ENDM

; --------------------------------------------------------------------

        MAC DPC_CALL_INIT

                DPC_CALL_FUNCTION $0FFF, 0, DPC_FUNC_INIT, $FF

        ENDM

        MAC DPC_CALL_VERTICAL_BLANK

                DPC_CALL_FUNCTION $0FFF, 0, DPC_FUNC_VERTICAL_BLANK, $FF

        ENDM

        MAC DPC_CALL_OVERSCAN

                DPC_CALL_FUNCTION $0FFF, 0, DPC_FUNC_OVERSCAN, $FF

        ENDM


; ********************************************************************
;
;       Code Section
;
; ********************************************************************

; ====================================================================
;       Zaxxon HD Demo
; ====================================================================

ZaxxonHDDemo:   CLEAN_START

; --------------------------------------------------------------------        
;       ZHD Init
; --------------------------------------------------------------------

ZHD_Init:       SUBROUTINE

                ; call ARM init function

ZHD_ARMInit:    DPC_CALL_INIT

                ; position bitmap (P0, P1)

ZHD_PosScore:   ldx     #0
                lda     #56
                jsr     ZHD_HPosBZNoHMOVE
                inx
                lda     #56+8
                jsr     ZHD_HPosBZNoHMOVE

                ; init non-changing TIA registers

ZHD_SetBgnd:    ldx     #%11111110
                stx     PF2
                ldx     #%11111111
                stx     PF1
                stx     PF0
                stx     ENABL

; --------------------------------------------------------------------        
;       ZHD VSync
; --------------------------------------------------------------------

ZHD_VSync:      SUBROUTINE

                ; VSYNC on

                lda     #%00000010
                sta     WSYNC
                sta     VSYNC
                sta     WSYNC
                sta     WSYNC

                ; set timer to skip vertical blank

                lda     #44
                sta     TIM64T

                ; VSYNC off

                sta     WSYNC
                sta     VSYNC

; --------------------------------------------------------------------        
;       ZHD VBlank
; --------------------------------------------------------------------
 
ZHD_VBlank:     SUBROUTINE

                ; call ARM vertical blank function

ZHD_ARMVBlank:  DPC_CALL_VERTICAL_BLANK

                ; position gauge (BL)

ZHD_PosGauge:   ldx     #4
                lda     #50
                jsr     ZHD_HPosBZNoHMOVE

                ; copy data from ARM side

ZHD_CopyFromARM:DPC_SET_FETCHER $0FF0, 0

                DPC_READ_FETCHER AUDC0, 0
                DPC_READ_FETCHER AUDC1, 0
                DPC_READ_FETCHER AUDF0, 0
                DPC_READ_FETCHER AUDF1, 0
                DPC_READ_FETCHER AUDV0, 0
                DPC_READ_FETCHER AUDV1, 0

                ; trigger positioning

ZHD_Postioning: sta     WSYNC
                sta     HMOVE

                ; setup data fetchers for display       

ZHD_SetupDPC:   DPC_SET_FETCHER $0000,   0
                DPC_SET_FETCHER $0100,   1
                DPC_SET_FETCHER $0200,   2
                DPC_SET_FETCHER $0300,   3
                DPC_SET_FETCHER $0400,   4
                DPC_SET_FETCHER $0500,   5

                DPC_SET_FETCHER $0700,   0FRAC
                DPC_SET_FETCHER $0800,   1FRAC
                DPC_SET_FETCHER $0900,   2FRAC
                DPC_SET_FETCHER $0A00,   3FRAC

                lda     #255
                sta     DF0FRACINC
                sta     DF1FRACINC
                sta     DF2FRACINC
                sta     DF3FRACINC

                ; finish vertical blank

ZHD_VBlankEnd:  lda     INTIM
                bne     ZHD_VBlankEnd

; --------------------------------------------------------------------        
;       ZHD Kernel
; --------------------------------------------------------------------

ZHD_Kernel:     SUBROUTINE

                ; VBLANK off

                sta     WSYNC
                sta     VBLANK

                ; first line, setup graphics

ZHD_FirstLine:  sta     COLUPF
                sta     COLUP0
                sta     COLUP1
                sta     HMCLR

                ; prime fetchers

ZHD_PrimeFetch: sta     FASTFETCH       ; fast fetch on

                lda     #<DF0FRACDATA 
                lda     #<DF1FRACDATA 
                lda     #<DF2FRACDATA 
                lda     #<DF3FRACDATA 

                ; start main kernel

ZHD_DrawScreen: ldx     #%00000011
                stx     NUSIZ0
                stx     NUSIZ1
                stx     VDELP0
                stx     VDELP1

                jsr     ZHD_MainKernel

; --------------------------------------------------------------------        
;       ZHD Overscan
; --------------------------------------------------------------------

ZHD_Overscan:   SUBROUTINE

                ; VBLANK on

                ldx     #%00000010
                stx     WSYNC
                stx     VBLANK

                ; clear sprite channels

                ldx     #0
                stx     GRP0
                stx     GRP1
                stx     GRP0

                ; fast fetch off

                ldx     #$FF
                stx     FASTFETCH

                ; set timer to skip overscan

                lda     #34 
                sta     TIM64T

                ;copy TIA registers to ARM side

ZHD_CopyToARM:  DPC_SET_FETCHER $0FF0, 0

                DPC_WRITE_FETCHER SWCHA, 0
                DPC_WRITE_FETCHER SWCHB, 0
                DPC_WRITE_FETCHER INPT4, 0

                ; call ARM overscan function

ZHD_ARMOverscan:DPC_CALL_OVERSCAN

                ; finish overscan

ZHD_OverscanEnd:lda     INTIM
                bne     ZHD_OverscanEnd
                jmp     ZHD_VSync

; --------------------------------------------------------------------        
;       ZHD Subroutines
; --------------------------------------------------------------------

                ALIGN   $0100

ZHD_MainKernel: SUBROUTINE

                nop
                nop
                nop
                nop
                nop
                nop
                nop

                ldy     #193
                sty     LineCtr

.loop           lda     #DF_CTRLPF              ; 2 [68]        
                sta     CTRLPF                  ; 3 [71]
                 ;
                lda     #DF_HMBL                ; 2 [73]
                sta     HMBL                    ; 3 [ 0]
                sta     HMOVE                   ; 3 [ 3]
                 ;
                lda     #DF_COLUBK              ; 2 [ 5]
                sta     COLUBK                  ; 3 [ 8]
                lda     #DF_COLUP               ; 2 [10]
                sta     COLUP0                  ; 3 [13]
                sta     COLUP1                  ; 3 [16]
                 ;
                lda     #DF_GRP0                ; 2 [18]
                sta     GRP0                    ; 3 [21]
                lda     #DF_GRP1                ; 2 [23]
                sta     GRP1                    ; 3 [26]
                lda     #DF_GRP2                ; 2 [28]
                sta     GRP0                    ; 3 [31]
                lda     #DF_GRP3                ; 2 [33]
                tax                             ; 2 [35]
                lda     #DF_GRP4                ; 2 [37]
                tay                             ; 2 [39]
                lda     #DF_GRP5                ; 2 [41]
                stx     GRP1                    ; 3 [44]
                sty     GRP0                    ; 3 [47]
                sta     GRP1                    ; 3 [50]
                sta     GRP0                    ; 3 [53]
                 ;
                ldx     #$00                    ; 2 [55]
                stx     COLUBK                  ; 3 [58]
                 ;
                dec     LineCtr                 ; 5 [63]                        
                bne     .loop                   ; 3 [66]

                rts

; --------------------------------------------------------------------

; Horizontal sprite positioning
;   from Battlezone by Michael Feinstein
;
; Input
;   A : horizontal position (0 - 159)
;   X : object ID (0 - 4) for P0,P1,M0,M1,BL
;
;   For X = M0,M1,BL, A must be increased by 1 due to different RES timing.

ZHD_HPosBZNoHMOVE: SUBROUTINE

                sec
                sta     WSYNC
.loop           sbc     #15
                bcs     .loop
                eor     #7
                asl
                asl
                asl
                asl
                sta.wx  HMP0,x
                sta     RESP0,x
                rts

; ********************************************************************
