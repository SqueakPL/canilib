#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

CANI_EXPORT CANI_CHAR CANI_FastResize(CANI_BGRAQUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRAQUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_FastResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_FastResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_FastResizeAlpha16(CANI_WORD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_WORD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);

CANI_EXPORT CANI_CHAR CANI_InterpolationResize(CANI_BGRAQUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRAQUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_InterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_InterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_InterpolationResizeAlpha16(CANI_WORD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_WORD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);

CANI_EXPORT CANI_CHAR CANI_SquareInterpolationResize(CANI_BGRAQUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRAQUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_SquareInterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_SquareInterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_SquareInterpolationResizeAlpha16(CANI_WORD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_WORD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);

CANI_EXPORT CANI_CHAR CANI_CubicInterpolationResize(CANI_BGRAQUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRAQUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_CubicInterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_CubicInterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_CubicInterpolationResizeAlpha16(CANI_WORD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_WORD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);

CANI_EXPORT CANI_CHAR CANI_LagrangeInterpolationResize(CANI_BGRAQUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRAQUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_LagrangeInterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_LagrangeInterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);
CANI_EXPORT CANI_CHAR CANI_LagrangeInterpolationResizeAlpha16(CANI_WORD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_WORD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy);

#ifdef __cplusplus
}
#endif