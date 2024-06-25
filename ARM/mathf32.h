
/*
; ********************************************************************
;  mathf32
;
;    Fixed point math routines
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
// f32 fixed point type (32 bit)
//
// Based on code from the SGDK Genesis development kit
// https://code.google.com/p/sgdk
//

typedef int f32;

#define F32_INT_BITS              22
#define F32_FRAC_BITS             (32 - F32_INT_BITS)
#define F32_INT_MASK              (((1 << F32_INT_BITS) - 1) << F32_FRAC_BITS)
#define F32_FRAC_MASK             ((1 << F32_FRAC_BITS) - 1)

#define F32(value)                ((f32) ((value) * (1 << F32_FRAC_BITS)))

#define IntToF32(value)           ((value) << F32_FRAC_BITS)
#define F32ToInt(value)           ((value) >> F32_FRAC_BITS)

#define F32Frac(value)            ((value) & F32_FRAC_MASK)
#define F32Int(value)             ((value) & F32_INT_MASK)

#define F32Add(val1, val2)        ((val1) + (val2))
#define F32Sub(val1, val2)        ((val1) - (val2))
#define F32Mul(val1, val2)        (((val1) * (val2)) >> F32_FRAC_BITS)
#define F32Div(val1, val2)        (((val1) << F32_FRAC_BITS) / (val2))
#define F32Neg(value)             (0 - (value))

//
// f32 fixed point 2D vector type
//

typedef struct
{
  f32 x, y;

} Vec2f32;

inline Vec2f32 VecSet(f32 x, f32 y)
{
  Vec2f32 result = {x, y};

  return result;
};

inline Vec2f32 VecAdd(Vec2f32 v0, Vec2f32 v1)
{
  Vec2f32 result;

  result.x = F32Add(v0.x, v1.x);
  result.y = F32Add(v0.y, v1.y);

  return result;
};

inline Vec2f32 VecSub(Vec2f32 v0, Vec2f32 v1)
{
  Vec2f32 result;

  result.x = F32Sub(v0.x, v1.x);
  result.y = F32Sub(v0.y, v1.y);

  return result;
};

inline Vec2f32 VecMul(f32 s, Vec2f32 v)
{
  Vec2f32 result;

  result.x = F32Mul(s, v.x);
  result.y = F32Mul(s, v.y);

  return result;
};

inline Vec2f32 VecDiv(Vec2f32 v, f32 s)
{
  Vec2f32 result;

  result.x = F32Div(v.x, s);
  result.y = F32Div(v.y, s);

  return result;
};
