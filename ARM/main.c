
/*
; ********************************************************************
;  Zaxxon HD Demo
;
;    Main source file ARM
;
;    $Date: Sun, 27 Sep 2015 00:35:47 +0200 $
;    $Author: dietrich $
;    $Revision: 397 $
;
;  Copyright (C) 2015 Andreas Dietrich
; ********************************************************************
*/

//
// Based on Frantic ARM routines by Darrell Spice, Jr.
//

// To compile, this you'll need to install an ARM compiler.
//
// for OS X use:
// http://www.gnuarm.com/files.html
// http://www.gnuarm.com/binutils-2.14_gcc-3.3.2-c-c++_newlib-1.12.0_gdb-6.0.pkg.tgz
//
// for Linux/Windows use:
// http://www.codesourcery.com/sgpp/lite/arm/portal/release1033
//
// You will need to update the "Makefile" if you're using Linux/Windows.
// Find the toolchain lines and move the # from the aebi toolchain to the elf version.
// #TOOLCHAIN=arm-elf
// TOOLCHAIN=arm-none-eabi

// src/custom.h defines the following:
//
// queue[]     - points to the 4K Display Data Bank
//             - treat as RAM
//             - Any data passed to/from the ARM and 6507 must be done via queue[]
//
// flashdata[] - points to the 24K that comprises the six 4K banks.
//             - treat as ROM

#include "src/custom.h"

// types of variables, storage used and range
// const              = constant, which is compiled into the ROM section and cannot be changed during run time.
// signed char        =  8 bit, 1 byte  per value.  Range is           -128 to           127
// unsigned char      =  8 bit, 1 byte  per value.  Range is              0 to           255
// short int          = 16 bit, 2 bytes per value.  Range is        -32,768 to        32,767
// unsigned short int = 16 bit, 2 bytes per value.  Range is              0 to        65,535
// int                = 32 bit, 4 bytes per value.  Range is -2,147,483,648 to 2,147,483,647
// unsigned int       = 32 bit, 4 bytes per value.  Range is              0 to 4,294,967,295
// long int           = same as int
// float              = 32 bit, 4 bytes.  Not Supported#
// double             = 64 bit, 8 bytes.  Not Supported#
// *                  = 32 bit, 4 bytes, pointer (ie: char*, int*)
//
// # - at least not supported with the compiler under OS X.  It might be supported by the Linux/Windows ARM
//     compiler - but support would be implemented via software as the ARM in the Harmony Cartridge does not
//     have an FPU (floating point unit).  As such, performance will most likely be as good as using
//     integer math based routines.
//
// NOTE : Only 448 bytes are allocated for use by variables (the rest of the 512 byte section is used
//        as the stack).  If you use too many variables, you'll get a compile time error:  "region ram is full"
//        You can use RAM in the Display Data to store values, see defines.h for queue[xxx] defines
//
// NOTE : Compiled code can be significantly smaller if you use INTs for your variables instead of
//        SHORTs or CHARs.

// -------------------------------------------------------------------
//      Configuration
// -------------------------------------------------------------------

// TV system
#define TV_SYSTEM 1 // NTSC = 1, PAL60 = 2

// -------------------------------------------------------------------
//      Includes
// -------------------------------------------------------------------

// libc includes
#include "limits.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

// zhd includes
#include "colors.h"
#include "displaydata.h"
#include "mathf32.h"

// -------------------------------------------------------------------
//      Definitions
// -------------------------------------------------------------------

//
// Defines -----------------------------------------------------------
//

// ARM
#define FUNC_INIT           0
#define FUNC_VERTICAL_BLANK 1
#define FUNC_OVERSCAN       2

// ship
#define SHIP_X_MIN          0
#define SHIP_X_MAX          31
#define SHIP_Y_MIN          48
#define SHIP_Y_MAX          96

// background
#define BGND_WIDTH          48
#define BGND_HEIGHT         128
#define BGND_COLUMNS        38
#define BGND_Y_START        8

// scoling
#define SCROLL_REPEAT       298
#define SCROLL_CYCLE        128

// gauge
#define GAUGE_HEIGHT        104
#define GAUGE_Y_START       50

//
// Datatypes ---------------------------------------------------------
//

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;

//
// RAM/ROM layout ----------------------------------------------------
//

#define QUEUE_ADDR     0x20010C00 // RAM 4K
#define FLASHDATA_ADDR 0x20000C00 // ROM 24K
#define DDROM_ADDR     0x20006C00 // ROM copy of Display Data contents

u8*  const queue     = (u8*)QUEUE_ADDR;
u8*  const flashdata = (u8*)FLASHDATA_ADDR;
u8*  const ddrom     = (u8*)DDROM_ADDR;

int* const queue_int = (int*)QUEUE_ADDR;

//
// ARM registers -----------------------------------------------------
//

//#define MAMCR (*(u8*)0xE01FC000)

//
// 6507 register copies ----------------------------------------------
//

// SWCHA bits
#define P0_RIGHT                 0x80
#define P0_LEFT                  0x40
#define P0_DOWN                  0x20
#define P0_UP                    0x10

// SWCHB bits
#define GAME_SELECT              0x02
#define GAME_RESET               0x01

// INPT4 bits
#define P0_BUTTON                0x80

// data from 6507 to ARM
#define SWCHA                    (*(u8*)(QUEUE_ADDR + 0x0FF0))
#define SWCHB                    (*(u8*)(QUEUE_ADDR + 0x0FF1))
#define INPT4                    (*(u8*)(QUEUE_ADDR + 0x0FF2))
#define ARM_FUNCTION             (*(u8*)(QUEUE_ADDR + 0x0FFF))

// data from ARM to 6507
#define AUDC0                    (*(u8*)(QUEUE_ADDR + 0x0FF0))
#define AUDC1                    (*(u8*)(QUEUE_ADDR + 0x0FF1))
#define AUDF0                    (*(u8*)(QUEUE_ADDR + 0x0FF2))
#define AUDF1                    (*(u8*)(QUEUE_ADDR + 0x0FF3))
#define AUDV0                    (*(u8*)(QUEUE_ADDR + 0x0FF4))
#define AUDV1                    (*(u8*)(QUEUE_ADDR + 0x0FF5))

//
// Frame buffers -----------------------------------------------------
//

#define BUFFER_SIZE 0x0100

#define BUFFER_GRP0_PTR          ((u8*)(QUEUE_ADDR + 0x0000))
#define BUFFER_GRP1_PTR          ((u8*)(QUEUE_ADDR + 0x0100))
#define BUFFER_GRP2_PTR          ((u8*)(QUEUE_ADDR + 0x0200))
#define BUFFER_GRP3_PTR          ((u8*)(QUEUE_ADDR + 0x0300))
#define BUFFER_GRP4_PTR          ((u8*)(QUEUE_ADDR + 0x0400))
#define BUFFER_GRP5_PTR          ((u8*)(QUEUE_ADDR + 0x0500))
#define BUFFER_CLIP_PTR          ((u8*)(QUEUE_ADDR + 0x0600))
#define BUFFER_COL_PTR           ((u8*)(QUEUE_ADDR + 0x0700))
#define BUFFER_COLUBK_PTR        ((u8*)(QUEUE_ADDR + 0x0800))
#define BUFFER_HMBL_PTR          ((u8*)(QUEUE_ADDR + 0x0900))
#define BUFFER_CTRLPF_PTR        ((u8*)(QUEUE_ADDR + 0x0A00))

u8* const buffer_GRP0            = BUFFER_GRP0_PTR;
u8* const buffer_GRP1            = BUFFER_GRP1_PTR;
u8* const buffer_GRP2            = BUFFER_GRP2_PTR;
u8* const buffer_GRP3            = BUFFER_GRP3_PTR;
u8* const buffer_GRP4            = BUFFER_GRP4_PTR;
u8* const buffer_GRP5            = BUFFER_GRP5_PTR;
u8* const buffer_CLIP            = BUFFER_CLIP_PTR;
u8* const buffer_COL             = BUFFER_COL_PTR;
u8* const buffer_COLUBK          = BUFFER_COLUBK_PTR;
u8* const buffer_HMBL            = BUFFER_HMBL_PTR;
u8* const buffer_CTRLPF          = BUFFER_CTRLPF_PTR;

//
// Macros ------------------------------------------------------------
//

#define MIN(a, b)                ((a) < (b) ? (a) : (b))
#define MAX(a, b)                ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b)           (MIN(MAX(x, a), b))
#define SWAP(type, a, b)         {type t=a; a=b; b=t;}

#define JOY0_LEFT_PRESSED        (!(SWCHA & P0_LEFT)  )
#define JOY0_LEFT_RELEASED       (  SWCHA & P0_LEFT   )
#define JOY0_RIGHT_PRESSED       (!(SWCHA & P0_RIGHT) )
#define JOY0_RIGHT_RELEASED      (  SWCHA & P0_RIGHT  )
#define JOY0_UP_PRESSED          (!(SWCHA & P0_UP)    )
#define JOY0_UP_RELEASED         (  SWCHA & P0_UP     )
#define JOY0_DOWN_PRESSED        (!(SWCHA & P0_DOWN)  )
#define JOY0_DOWN_RELEASED       (  SWCHA & P0_DOWN   )
#define JOY0_BUTTON_PRESSED      (!(INPT4 & P0_BUTTON))
#define JOY0_BUTTON_RELEASED     (  INPT4 & P0_BUTTON )

#define GAME_SELECT_PRESSED      (!(SWCHB & GAME_SELECT))
#define GAME_SELECT_RELEASED     (  SWCHB & GAME_SELECT )
#define GAME_RESET_PRESSED       (!(SWCHB & GAME_RESET) )
#define GAME_RESET_RELEASED      (  SWCHB & GAME_RESET  )

//
// Variables ---------------------------------------------------------
//

typedef struct
{
  // counters
  u32 rgbCtr;
  u32 frameCtr;
  s32 scrollCtr;

  // scrolling
  u8  scrollEnable;
  u8  scrollSelect;

  // laser
  s32 laserX[2];
  s32 laserY[2];
  s32 laserZ[2];
  u8  laserAudio;

  // ship
  s32 shipX;
  s32 shipY;
  u32 shipGfx;

} Variables;

Variables* const var = (Variables*)(QUEUE_ADDR + 0x0F00);

// -------------------------------------------------------------------
//      Data
// -------------------------------------------------------------------

//
// Bitmap column buffer pointers (including clip buffer)
//

static u8* const bufferGRP[7] =
  {
    BUFFER_GRP0_PTR,
    BUFFER_GRP1_PTR,
    BUFFER_GRP2_PTR,
    BUFFER_GRP3_PTR,
    BUFFER_GRP4_PTR,
    BUFFER_GRP5_PTR,
    BUFFER_CLIP_PTR
  };

//
// Primary RGB colors for ChronoColour
//

static const u8 colorsRGB[3] =
  {
    COL_74,
    COL_C6,
    COL_34
  };

//
// Altitude bar tables
//

static const u8 gaugeTopCOLUBK[8] =
  {
    COL_42,
    COL_42,
    COL_42,
    COL_42,
    COL_42,
    COL_42,
    COL_42,
    COL_00
  };

static const u8 gaugeTopHMBL[8] =
  {
     0 << 4,
     0 << 4,
     0 << 4,
    -3 << 4,
    +3 << 4,
     0 << 4,
     0 << 4,
    -3 << 4
  };

static const u8 gaugeTopCTRLPF[8] =
  {
    0x10,
    0x10,
    0x10,
    0x10,
    0x10,
    0x10,
    0x10,
    0x10
  };

static const u8 gaugeBottomCOLUBK[8] =
  {
    COL_C6,
    COL_C6,
    COL_C6,
    COL_C6,
    COL_C6,
    COL_C6,
    COL_C6,
    COL_00
  };

static const u8 gaugeBottomHMBL[8] =
  {
    +3 << 4,
     0 << 4,
     0 << 4,
     0 << 4,
     0 << 4,
     0 << 4,
    -3 << 4,
     0 << 4
  };

static const u8 gaugeBottomCTRLPF[8] =
  {
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20
  };

//
// Zaxxon background bitmap
//

#include "zaxxon.h"

const u8* const zaxxonGfx = ZaxxonGfx;

// -------------------------------------------------------------------
//      Subroutines
// -------------------------------------------------------------------

static inline void CycleRGB()
{
  // rotate color index
  var->rgbCtr = (var->rgbCtr + 1) % 3;
}

// -------------------------------------------------------------------

static inline void Scrolling()
{
  if (var->scrollEnable)
    {
      int x = (int)(var->scrollCtr);

      // scroll only after one RGB cycle
      if ((var->frameCtr % 3) == 0)
          x += 1;

      // repeat background
      if (x >= (SCROLL_REPEAT + SCROLL_CYCLE - BGND_WIDTH))
        x -= SCROLL_CYCLE;

      var->scrollCtr = (s32)x;
    }
}

// -------------------------------------------------------------------

static inline void ClearBuffers()
{
  int i;

  // clear players and background color (all 0)
  {
    unsigned *buffer0 = (unsigned*)BUFFER_GRP0_PTR;
    unsigned *buffer1 = (unsigned*)BUFFER_GRP1_PTR;
    unsigned *buffer2 = (unsigned*)BUFFER_GRP2_PTR;
    unsigned *buffer3 = (unsigned*)BUFFER_GRP3_PTR;
    unsigned *buffer4 = (unsigned*)BUFFER_GRP4_PTR;
    unsigned *buffer5 = (unsigned*)BUFFER_GRP5_PTR;
    unsigned *buffer6 = (unsigned*)BUFFER_COLUBK_PTR;

    for (i=0; i<192/(4*sizeof(unsigned)); i++)
      {
        *buffer0++ =
        *buffer1++ =
        *buffer2++ =
        *buffer3++ =
        *buffer4++ =
        *buffer5++ =
        *buffer6++ = 0;

        *buffer0++ =
        *buffer1++ =
        *buffer2++ =
        *buffer3++ =
        *buffer4++ =
        *buffer5++ =
        *buffer6++ = 0;

        *buffer0++ =
        *buffer1++ =
        *buffer2++ =
        *buffer3++ =
        *buffer4++ =
        *buffer5++ =
        *buffer6++ = 0;

        *buffer0++ =
        *buffer1++ =
        *buffer2++ =
        *buffer3++ =
        *buffer4++ =
        *buffer5++ =
        *buffer6++ = 0;
      }
  }

  // fill player color with repeating RGB color pattern
  {
    const unsigned channel = var->rgbCtr;

    const u8 r = colorsRGB[(0+channel+2)%3];
    const u8 g = colorsRGB[(1+channel+2)%3];
    const u8 b = colorsRGB[(2+channel+2)%3];

    u8 *buffer = (u8*)BUFFER_COL_PTR;

    for (i=0; i<195/3; i++)
      {
        *buffer++ = r;
        *buffer++ = g;
        *buffer++ = b;
      }
  }
}

// -------------------------------------------------------------------

static inline void DrawGauge()
{
  int i;

  // interpolate gauge position (fixed point)
  f32 y = F32(var->shipY);
  y = F32Sub( y, F32(SHIP_Y_MIN) );
  y = F32Div( y, F32(SHIP_Y_MAX-SHIP_Y_MIN) );
  y = F32Mul( y, F32(72-8) );

  // limit line of altitude bar
  const int limit = GAUGE_Y_START + 16 + F32ToInt(y);

  // fill bar below limit line
  for (i=GAUGE_Y_START+16; i<GAUGE_Y_START+GAUGE_HEIGHT-16; i++)
    {
      if (i>limit)
        buffer_COLUBK[i] = COL_AE;
    }

  // invert marks
  for (i=0; i<5; i++)
    {
      const int t = (GAUGE_Y_START+GAUGE_HEIGHT-16) - (8 + i*16);

      buffer_COLUBK[t] ^= COL_AE;
    }

  // setup H(igh) and L(ow) letters
  for (i=0; i<8; i++)
    {
      const int t0 = GAUGE_Y_START + i;
      const int t1 = GAUGE_Y_START+GAUGE_HEIGHT-8 + i;

      buffer_COLUBK[t0] = gaugeTopCOLUBK[i];
      buffer_HMBL[t0]   = gaugeTopHMBL[i];
      buffer_CTRLPF[t0] = gaugeTopCTRLPF[i];

      buffer_COLUBK[t1] = gaugeBottomCOLUBK[i];
      buffer_HMBL[t1]   = gaugeBottomHMBL[i];
      buffer_CTRLPF[t1] = gaugeBottomCTRLPF[i];
    }
}

// -------------------------------------------------------------------

static inline void DrawBackground(unsigned start, unsigned end)
{
  int xs, xt, ys, yt;

  // draw vertical lines
  for (xt=start; xt<end; xt++)
    {
      // source x-position
      xs = xt + (int)(var->scrollCtr);

      // repeat background
      if (xs >= SCROLL_REPEAT)
        xs -= SCROLL_CYCLE;

      // blank area
      if (xs < 0)
        continue;

      // source bit
      const unsigned srcColumn = xs >> 3;
      const unsigned srcBit    = xs & 7;
      const unsigned srcShift  = 7 - srcBit;

      // target bit
      const unsigned tgtColumn = xt >> 3;
      const unsigned tgtBit    = xt & 7;
      const unsigned tgtShift  = 7 - tgtBit;

      // perspective shearing and RGB pattern
      const unsigned shear   = BGND_WIDTH - xt;
      const unsigned channel = ((unsigned)(var->rgbCtr) + shear) % 3;

      // source and target pointers
      const u8* gfx    = zaxxonGfx + (srcColumn * BGND_HEIGHT) + (channel * BGND_COLUMNS * BGND_HEIGHT);
            u8* buffer = buffer_GRP0 + (BUFFER_SIZE * tgtColumn);

      // init source and target line counter
      ys = 0;
      yt = BGND_Y_START + BGND_HEIGHT + shear;

      // copy line
      while (ys < BGND_HEIGHT)
      {
        {
          const u8 bit  = ( *gfx++ >> srcShift ) & 0x01;
          const u8 mask = bit << tgtShift;
          buffer[yt] |= mask;
        }
        ys++, yt--;
        {
          const u8 bit  = ( *gfx++ >> srcShift ) & 0x01 ;
          const u8 mask = bit << tgtShift;
          buffer[yt] |= mask;
        }
        ys++, yt--;
      }
    }
}

// -------------------------------------------------------------------

static inline void DrawScore()
{
  int i;

  // draw score
  {
    const u8 *src_GRP0 = ddrom + DD_Score + 0 * DD_SCORE_HEIGHT;
    const u8 *src_GRP1 = ddrom + DD_Score + 1 * DD_SCORE_HEIGHT;
    const u8 *src_GRP2 = ddrom + DD_Score + 2 * DD_SCORE_HEIGHT;
    const u8 *src_GRP3 = ddrom + DD_Score + 3 * DD_SCORE_HEIGHT;
    const u8 *src_GRP4 = ddrom + DD_Score + 4 * DD_SCORE_HEIGHT;
    const u8 *src_GRP5 = ddrom + DD_Score + 5 * DD_SCORE_HEIGHT;

    for (i=0; i<DD_SCORE_HEIGHT; i++)
      {
        const int s = i;
        const int t = 1 + (DD_SCORE_HEIGHT - i - 1);

        buffer_GRP0[t]  = src_GRP0[s];
        buffer_GRP1[t]  = src_GRP1[s];
        buffer_GRP2[t]  = src_GRP2[s];
        buffer_GRP3[t]  = src_GRP3[s];
        buffer_GRP4[t]  = src_GRP4[s];
        buffer_GRP5[t] |= src_GRP5[s];  // overlap with scroll area
      }
  }

  // draw status
  {
    const unsigned channel = var->rgbCtr;

    const u8 *src_GRP = ddrom + DD_Status_R + channel * (DD_STATUS_WIDTH * DD_STATUS_HEIGHT);

    const u8 *src_GRP0 = src_GRP + 0 * DD_STATUS_HEIGHT;
    const u8 *src_GRP1 = src_GRP + 1 * DD_STATUS_HEIGHT;
    const u8 *src_GRP2 = src_GRP + 2 * DD_STATUS_HEIGHT;
    const u8 *src_GRP3 = src_GRP + 3 * DD_STATUS_HEIGHT;
    const u8 *src_GRP4 = src_GRP + 4 * DD_STATUS_HEIGHT;
    const u8 *src_GRP5 = src_GRP + 5 * DD_STATUS_HEIGHT;

    // non-overlapping part
    for (i=0; i<DD_STATUS_HEIGHT-16; i++)
      {
        const int s = i;
        const int t = BGND_HEIGHT+41 + (DD_STATUS_HEIGHT - i - 1);

        buffer_GRP0[t] = src_GRP0[s];
        buffer_GRP1[t] = src_GRP1[s];
        buffer_GRP2[t] = src_GRP2[s];
        buffer_GRP3[t] = src_GRP3[s];
        buffer_GRP4[t] = src_GRP4[s];
        buffer_GRP5[t] = src_GRP5[s];
      }

    // overlapping part
    for (i=DD_STATUS_HEIGHT-16; i<DD_STATUS_HEIGHT; i++)
      {
        const int s = i;
        const int t = BGND_HEIGHT+41 + (DD_STATUS_HEIGHT - i - 1);

        buffer_GRP0[t] |= src_GRP0[s]; // overlap with scroll area
        buffer_GRP1[t] |= src_GRP1[s]; //
        buffer_GRP2[t] |= src_GRP2[s]; //
        buffer_GRP3[t]  = src_GRP3[s];
        buffer_GRP4[t]  = src_GRP4[s];
        buffer_GRP5[t]  = src_GRP5[s];
      }
  }
}

// -------------------------------------------------------------------

static inline void DrawLaser()
{
  int l, i;

  // draw two laser beams
  for (l=0; l<2; l++)
    {
      // axonometric projection
      const int x = (int)(var->laserX[l]) + (int)(var->laserZ[l]);
      const int y = (int)(var->laserY[l]) + (int)(var->laserX[l]) - (int)(var->laserZ[l]);

      // beam outside window ?
      if (x >= BGND_WIDTH)
        continue;

      // RGB pattern
      const unsigned channel = ((unsigned)(var->rgbCtr) + (unsigned)y) % 3;

      // beam source graphics and mask
      const u8* gfx_GRP0  = ddrom + DD_Laser_R + channel * (DD_LASER_WIDTH * DD_LASER_HEIGHT);
      const u8* mask_GRP0 = ddrom + DD_Laser_Mask;

      // bit shift and target start column
      const int shift  = x % 8;
      const int column = x / 8;

      // affected columns (two at most)
      u8* const buffer0 = bufferGRP[column+0];
      u8* const buffer1 = bufferGRP[column+1];

      // draw beam
      for (i=DD_LASER_HEIGHT-1; i>=0; i--)
        {
          const int j = DD_LASER_HEIGHT - i - 1;

          // axonometric projection
          const int s = i;
          const int t = j + y;

          // build graphics patterns
          const u32 gfx    =  (  gfx_GRP0[s]<<8 ) >> shift;
          const u32 mask   = ~( mask_GRP0[s]<<8 ) >> shift;

          // draw beam line
          buffer0[t] = ( buffer0[t] & (u8)(mask>>8) ) | (u8)(gfx>>8);
          buffer1[t] = ( buffer1[t] & (u8)(mask>>0) ) | (u8)(gfx>>0);
        }
    }
}

// -------------------------------------------------------------------

static inline void DrawShip()
{
  int i;

  // RGB pattern
  const unsigned channel = (var->rgbCtr + var->shipY + var->shipX) % 3;

  // ship source graphics
  const u8 *gfx         = ddrom + (var->shipGfx) + channel * (DD_SHIP_WIDTH * DD_SHIP_HEIGHT);
  const u8 *gfx_GRP0    = gfx + 0 * DD_SHIP_HEIGHT;
  const u8 *gfx_GRP1    = gfx + 1 * DD_SHIP_HEIGHT;

  // ship mask
  const u8 *mask        = ddrom + (var->shipGfx) + 3 * (DD_SHIP_WIDTH * DD_SHIP_HEIGHT);
  const u8 *mask_GRP0   = mask + 0 * DD_SHIP_HEIGHT;
  const u8 *mask_GRP1   = mask + 1 * DD_SHIP_HEIGHT;

  // shadow graphics
  const u8 *shadow      = ddrom + DD_ShipShadow;
  const u8 *shadow_GRP0 = shadow + 0 * DD_SHIP_HEIGHT;
  const u8 *shadow_GRP1 = shadow + 1 * DD_SHIP_HEIGHT;

  // ship position
  const int x = (int)(var->shipX);
  const int y = (int)(var->shipY);

  // bit shift and target start column
  const int shift  = x % 8;
  const int column = x / 8;

  // affected buffers (three at most)
  u8* const buffer0 = bufferGRP[column+0];
  u8* const buffer1 = bufferGRP[column+1];
  u8* const buffer2 = bufferGRP[column+2];

  // disable shadow on wall
  const bool shadowOn = (var->scrollCtr < 10) || (var->scrollCtr > 52);

  // draw ship and shadow
  for (i=DD_SHIP_HEIGHT-1; i>=0; i--)
    {
      const int j = DD_SHIP_HEIGHT - i - 1;

      // axonometric projection
      const int s  = i;
      const int t0 = j + x + (SHIP_Y_MAX + 8);
      const int t1 = j + x + y;

      // build graphics patterns
      const u32 gfx    =  (    gfx_GRP0[s]<<16 |    gfx_GRP1[s]<<8 ) >> shift;
      const u32 mask   = ~(   mask_GRP0[s]<<16 |   mask_GRP1[s]<<8 ) >> shift;
      const u32 shadow = ~( shadow_GRP0[s]<<16 | shadow_GRP1[s]<<8 ) >> shift;

      // draw shadow line
      if (shadowOn)
      {
        buffer0[t0] = ( buffer0[t0] & (u8)(shadow>>16) );
        buffer1[t0] = ( buffer1[t0] & (u8)(shadow>> 8) );
        buffer2[t0] = ( buffer2[t0] & (u8)(shadow>> 0) );
      }

      // draw ship line
      buffer0[t1] = ( buffer0[t1] & (u8)(mask>>16) ) | (u8)(gfx>>16);
      buffer1[t1] = ( buffer1[t1] & (u8)(mask>> 8) ) | (u8)(gfx>> 8);
      buffer2[t1] = ( buffer2[t1] & (u8)(mask>> 0) ) | (u8)(gfx>> 0);
    }
}

// -------------------------------------------------------------------

static inline void Input()
{
  int i;

  //
  // Check SELECT and RESET switches ---------------------------------
  //

  // SELECT
  if (GAME_SELECT_RELEASED)
    {
      var->scrollSelect = 0;
    }
  else if (!var->scrollSelect)
    {
      var->scrollEnable = !(var->scrollEnable);
      var->scrollSelect = 1;
    }

  // RESET
  if (GAME_RESET_PRESSED)
    {
      var->scrollCtr    = -72;
      var->scrollEnable =   1;
    }

  //
  // Move ship -------------------------------------------------------
  //

  // any movement only every third frame
  if ((var->frameCtr % 3) != 0)
    return;

  // standard orientation
  var->shipGfx = DD_Ship_R;

  // ship up/down
  if (JOY0_UP_PRESSED)
    {
      var->shipY++;
      if (var->shipY < SHIP_Y_MAX-3) // pitch
        var->shipGfx = DD_ShipDown_R;
    }
  else if (JOY0_DOWN_PRESSED)
    {
      var->shipY--;
      if (var->shipY > SHIP_Y_MIN+3) // pitch
        var->shipGfx = DD_ShipUp_R;
    }
  // ship left/right
  if (JOY0_LEFT_PRESSED)
    {
      var->shipX--;
    }
  else if (JOY0_RIGHT_PRESSED)
    {
      var->shipX++;
    }

  // ensure valid ship positions
  var->shipX = CLAMP(var->shipX, SHIP_X_MIN, SHIP_X_MAX);
  var->shipY = CLAMP(var->shipY, SHIP_Y_MIN, SHIP_Y_MAX);

  //
  // Move lasers -----------------------------------------------------
  //

  // increase Z position
  for (i=0; i<2; i++)
    if (var->laserZ[i] < BGND_WIDTH)
      var->laserZ[i] += 6;

  // laser sound
  if (var->laserZ[0]>=BGND_WIDTH && var->laserZ[1]>=BGND_WIDTH)
    var->laserAudio = 0;
  else
    var->laserAudio++;

  //
  // Fire lasers -----------------------------------------------------
  //

  if (JOY0_BUTTON_PRESSED)
    {
      // find free slot
      for (i=0; i<2; i++)
        {
          // enough room ?
          if ((var->laserZ[i] >= BGND_WIDTH) && (var->laserZ[1-i] >= 25))
            {
              // init sound
              var->laserAudio = 4;

              // beam start position depends on ship orientation
              if (var->shipGfx == DD_Ship_R)
                {
                  var->laserX[i] = var->shipX + 0;
                  var->laserY[i] = var->shipY - 1;
                  var->laserZ[i] = 8;
                }
              else if (var->shipGfx == DD_ShipUp_R)
                {
                  var->laserX[i] = var->shipX - 0;
                  var->laserY[i] = var->shipY - 8;
                  var->laserZ[i] = 6;
                }
              else if (var->shipGfx == DD_ShipDown_R)
                {
                  var->laserX[i] = var->shipX + 0;
                  var->laserY[i] = var->shipY - 4;
                  var->laserZ[i] = 8;
                }
              break;
            }
        }
    }
}

// -------------------------------------------------------------------

static inline void Audio()
{
  // ship engine
  AUDV0 = 4;
  AUDC0 = 8;
  AUDF0 = 15 + ((unsigned)(var->shipY) - SHIP_Y_MIN) / 3;

  // laser
  if (var->laserAudio)
    {
      AUDV1 = 4;
      AUDC1 = 8;
      AUDF1 = var->laserAudio;
    }
  else
    {
      AUDV1 = 0;
      AUDC1 = 0;
      AUDF1 = 0;
    }
}

// -------------------------------------------------------------------
//      Init
// -------------------------------------------------------------------

static inline void Init()
{
  var->rgbCtr       =   0;
  var->frameCtr     =   0;
  var->scrollCtr    = -72;

  var->scrollEnable =   1;
  var->scrollSelect =   0;

  var->laserZ[0]    =  48;
  var->laserZ[1]    =  48;
  var->laserAudio   =   0;

  var->shipX        = (SHIP_X_MIN + SHIP_X_MAX) / 2;
  var->shipY        = SHIP_Y_MIN;
  var->shipGfx      = DD_Ship_R;
}

// -------------------------------------------------------------------
//      Main
// -------------------------------------------------------------------

// main() is what gets called when you store a non-zero value into DPC+ register CALLFUNCTION in your 6507 code.
// If the value is $FE then the ARM has an IRQ running to update audio.
// If the value is $FF then the ARM runs w/out the IRQ.

int main()
{
  // There's a bug in the ARM chip that causes it to crash based on how code is aligned in memory.
  // Partially disabling MAM (cache) stops the crash from occuring.
  //MAMCR = 1;

  switch (ARM_FUNCTION)
    {

    // init ----------------------------------------------------------

    case FUNC_INIT:
      {
        Init();
      }
      break;

    // vertical blank ------------------------------------------------

    case FUNC_VERTICAL_BLANK:
      {
        DrawBackground(20,48);
        DrawScore();
        DrawLaser();
        DrawShip();
      }
      break;

    // overscan ------------------------------------------------------

    case FUNC_OVERSCAN:
      {
        var->frameCtr++;

        Input();
        Audio();

        CycleRGB();
        Scrolling();
        ClearBuffers();
        DrawGauge();
        DrawBackground(0,20);
      }
      break;

    }

  // Turn MAM back on as the DPC+ driver doesn't trigger the bug and it needs the extra performance.
  //MAMCR = 2;

  return 0;
}
