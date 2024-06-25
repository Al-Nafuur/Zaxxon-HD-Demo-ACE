
/*
; ********************************************************************
;  colors
;
;    NTSC PAL color translation table
;
;    $Date: Sun, 27 Sep 2015 00:35:47 +0200 $
;    $Author: dietrich $
;    $Revision: 397 $
;
;  Copyright (C) 2014 Andreas Dietrich
; ********************************************************************
*/

#pragma once

//
// External settings
//

#ifdef TV_EXTERN
#  if (TV_EXTERN == 1) || (TV_EXTERN == 2)
#    undef TV_SYSTEM
#    define TV_SYSTEM TV_EXTERN
#  endif
#endif

//
// Switch colors
//

#if TV_SYSTEM == 1

// NTSC colors

#define COL_00 0x00
#define COL_02 0x02
#define COL_04 0x04
#define COL_06 0x06
#define COL_08 0x08
#define COL_0A 0x0A
#define COL_0C 0x0C
#define COL_0E 0x0E

#define COL_10 0x10
#define COL_12 0x12
#define COL_14 0x14
#define COL_16 0x16
#define COL_18 0x18
#define COL_1A 0x1A
#define COL_1C 0x1C
#define COL_1E 0x1E

#define COL_20 0x20
#define COL_22 0x22
#define COL_24 0x24
#define COL_26 0x26
#define COL_28 0x28
#define COL_2A 0x2A
#define COL_2C 0x2C
#define COL_2E 0x2E

#define COL_30 0x30
#define COL_32 0x32
#define COL_34 0x34
#define COL_36 0x36
#define COL_38 0x38
#define COL_3A 0x3A
#define COL_3C 0x3C
#define COL_3E 0x3E

#define COL_40 0x40
#define COL_42 0x42
#define COL_44 0x44
#define COL_46 0x46
#define COL_48 0x48
#define COL_4A 0x4A
#define COL_4C 0x4C
#define COL_4E 0x4E

#define COL_50 0x50
#define COL_52 0x52
#define COL_54 0x54
#define COL_56 0x56
#define COL_58 0x58
#define COL_5A 0x5A
#define COL_5C 0x5C
#define COL_5E 0x5E

#define COL_60 0x60
#define COL_62 0x62
#define COL_64 0x64
#define COL_66 0x66
#define COL_68 0x68
#define COL_6A 0x6A
#define COL_6C 0x6C
#define COL_6E 0x6E

#define COL_70 0x70
#define COL_72 0x72
#define COL_74 0x74
#define COL_76 0x76
#define COL_78 0x78
#define COL_7A 0x7A
#define COL_7C 0x7C
#define COL_7E 0x7E

#define COL_80 0x80
#define COL_82 0x82
#define COL_84 0x84
#define COL_86 0x86
#define COL_88 0x88
#define COL_8A 0x8A
#define COL_8C 0x8C
#define COL_8E 0x8E

#define COL_90 0x90
#define COL_92 0x92
#define COL_94 0x94
#define COL_96 0x96
#define COL_98 0x98
#define COL_9A 0x9A
#define COL_9C 0x9C
#define COL_9E 0x9E

#define COL_A0 0xA0
#define COL_A2 0xA2
#define COL_A4 0xA4
#define COL_A6 0xA6
#define COL_A8 0xA8
#define COL_AA 0xAA
#define COL_AC 0xAC
#define COL_AE 0xAE

#define COL_B0 0xB0
#define COL_B2 0xB2
#define COL_B4 0xB4
#define COL_B6 0xB6
#define COL_B8 0xB8
#define COL_BA 0xBA
#define COL_BC 0xBC
#define COL_BE 0xBE

#define COL_C0 0xC0
#define COL_C2 0xC2
#define COL_C4 0xC4
#define COL_C6 0xC6
#define COL_C8 0xC8
#define COL_CA 0xCA
#define COL_CC 0xCC
#define COL_CE 0xCE

#define COL_D0 0xD0
#define COL_D2 0xD2
#define COL_D4 0xD4
#define COL_D6 0xD6
#define COL_D8 0xD8
#define COL_DA 0xDA
#define COL_DC 0xDC
#define COL_DE 0xDE

#define COL_E0 0xE0
#define COL_E2 0xE2
#define COL_E4 0xE4
#define COL_E6 0xE6
#define COL_E8 0xE8
#define COL_EA 0xEA
#define COL_EC 0xEC
#define COL_EE 0xEE

#define COL_F0 0xF0
#define COL_F2 0xF2
#define COL_F4 0xF4
#define COL_F6 0xF6
#define COL_F8 0xF8
#define COL_FA 0xFA
#define COL_FC 0xFC
#define COL_FE 0xFE

#else

// PAL color translation

#define COL_00 0x00
#define COL_02 0x02
#define COL_04 0x04
#define COL_06 0x06
#define COL_08 0x08
#define COL_0A 0x0A
#define COL_0C 0x0C
#define COL_0E 0x0E

#define COL_10 0x20
#define COL_12 0x22
#define COL_14 0x24
#define COL_16 0x26
#define COL_18 0x28
#define COL_1A 0x2A
#define COL_1C 0x2C
#define COL_1E 0x2E

#define COL_20 0x20
#define COL_22 0x22
#define COL_24 0x24
#define COL_26 0x26
#define COL_28 0x28
#define COL_2A 0x2A
#define COL_2C 0x2C
#define COL_2E 0x2E

#define COL_30 0x40
#define COL_32 0x42
#define COL_34 0x44
#define COL_36 0x46
#define COL_38 0x48
#define COL_3A 0x4A
#define COL_3C 0x4C
#define COL_3E 0x4E

#define COL_40 0x60
#define COL_42 0x62
#define COL_44 0x64
#define COL_46 0x66
#define COL_48 0x68
#define COL_4A 0x6A
#define COL_4C 0x6C
#define COL_4E 0x6E

#define COL_50 0x80
#define COL_52 0x82
#define COL_54 0x84
#define COL_56 0x86
#define COL_58 0x88
#define COL_5A 0x8A
#define COL_5C 0x8C
#define COL_5E 0x8E

#define COL_60 0xA0
#define COL_62 0xA2
#define COL_64 0xA4
#define COL_66 0xA6
#define COL_68 0xA8
#define COL_6A 0xAA
#define COL_6C 0xAC
#define COL_6E 0xAE

#define COL_70 0xC0
#define COL_72 0xC2
#define COL_74 0xC4
#define COL_76 0xC6
#define COL_78 0xC8
#define COL_7A 0xCA
#define COL_7C 0xCC
#define COL_7E 0xCE

#define COL_80 0xD0
#define COL_82 0xD2
#define COL_84 0xD4
#define COL_86 0xD6
#define COL_88 0xD8
#define COL_8A 0xDA
#define COL_8C 0xDC
#define COL_8E 0xDE

#define COL_90 0xB0
#define COL_92 0xB2
#define COL_94 0xB4
#define COL_96 0xB6
#define COL_98 0xB8
#define COL_9A 0xBA
#define COL_9C 0xBC
#define COL_9E 0xBE

#define COL_A0 0x90
#define COL_A2 0x92
#define COL_A4 0x94
#define COL_A6 0x96
#define COL_A8 0x98
#define COL_AA 0x9A
#define COL_AC 0x9C
#define COL_AE 0x9E

#define COL_B0 0x70
#define COL_B2 0x72
#define COL_B4 0x74
#define COL_B6 0x76
#define COL_B8 0x78
#define COL_BA 0x7A
#define COL_BC 0x7C
#define COL_BE 0x7E

#define COL_C0 0x50
#define COL_C2 0x52
#define COL_C4 0x54
#define COL_C6 0x56
#define COL_C8 0x58
#define COL_CA 0x5A
#define COL_CC 0x5C
#define COL_CE 0x5E

#define COL_D0 0x30
#define COL_D2 0x32
#define COL_D4 0x34
#define COL_D6 0x36
#define COL_D8 0x38
#define COL_DA 0x3A
#define COL_DC 0x3C
#define COL_DE 0x3E

#define COL_E0 0x30
#define COL_E2 0x32
#define COL_E4 0x34
#define COL_E6 0x36
#define COL_E8 0x38
#define COL_EA 0x3A
#define COL_EC 0x3C
#define COL_EE 0x3E

#define COL_F0 0x20
#define COL_F2 0x22
#define COL_F4 0x24
#define COL_F6 0x26
#define COL_F8 0x28
#define COL_FA 0x2A
#define COL_FC 0x2C
#define COL_FE 0x2E

#endif
