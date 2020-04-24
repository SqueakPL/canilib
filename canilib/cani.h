#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "config.h"

typedef struct tagCANI_SIZE {
	CANI_LONG        Width;
	CANI_LONG        Height;
} CANI_SIZE;

typedef struct tagCANI_BGRAQUAD {
        CANI_BYTE    Blue;
        CANI_BYTE    Green;
        CANI_BYTE    Red;
        CANI_BYTE    Alpha;
} CANI_BGRAQUAD;

typedef struct tagCANI_BGRA16QUAD {
	CANI_WORD    Blue;
	CANI_WORD    Green;
	CANI_WORD    Red;
	CANI_WORD    Alpha;
} CANI_BGRA16QUAD;

typedef struct tagCANI_RGBAQUAD {
        CANI_BYTE    Red;
        CANI_BYTE    Green;
        CANI_BYTE    Blue;
        CANI_BYTE    Alpha;
} CANI_RGBAQUAD;

typedef struct tagCANI_RGBA16QUAD {
	CANI_WORD    Red;
	CANI_WORD    Green;
	CANI_WORD    Blue;
	CANI_WORD    Alpha;
} CANI_RGBA16QUAD;


typedef struct tagCAF_RGB {
        CANI_BYTE    Red;
        CANI_BYTE    Green;
        CANI_BYTE    Blue;
} CAF_RGB;

typedef struct tagCAF_YUV {
        CANI_BYTE    Y;
        CANI_BYTE    U;
        CANI_BYTE    V;
} CAF_YUV;

typedef struct tagCAF_YCbCr {
        CANI_BYTE    Y;
        CANI_BYTE    Cb;
        CANI_BYTE    Cr;
} CAF_YCbCr;

typedef struct tagCAF_GREY {
        CANI_BYTE    Gray;
} CAF_GREY;

typedef struct tagCAF_FORMAT {
	CANI_BYTE format;
	union {
		CAF_RGB rgb;
		CAF_YUV yuv;
		CAF_YCbCr ycbcr;
		CAF_GREY gray;
	};
	CANI_DWORD options;
} CANI_FORMAT;

typedef struct tagCAF_RGB_V2 {
	CANI_BYTE    Red;
	CANI_BYTE    Green;
	CANI_BYTE    Blue;
	CANI_BYTE    Reserved1;
	CANI_BYTE    Alpha;
} CAF_RGB_V2;

typedef struct tagCAF_YUV_V2 {
	CANI_BYTE    Y;
	CANI_BYTE    U;
	CANI_BYTE    V;
	CANI_BYTE    Reserved1;
	CANI_BYTE    Alpha;
} CAF_YUV_V2;

typedef struct tagCAF_YCbCr_V2 {
	CANI_BYTE    Y;
	CANI_BYTE    Cb;
	CANI_BYTE    Cr;
	CANI_BYTE    Reserved1;
	CANI_BYTE    Alpha;
} CAF_YCbCr_V2;

typedef struct tagCAF_CMYK_V2 {
	CANI_BYTE    C;
	CANI_BYTE    M;
	CANI_BYTE    Y;
	CANI_BYTE    K;
	CANI_BYTE    Alpha;
} CAF_CMYK_V2;

typedef struct tagCAF_YCbCrK_V2 {
	CANI_BYTE    Y;
	CANI_BYTE    Cb;
	CANI_BYTE    Cr;
	CANI_BYTE    K;
	CANI_BYTE    Alpha;
} CAF_YCbCrK_V2;

typedef struct tagCAF_GREY_V2 {
	CANI_BYTE    Gray;
	CANI_BYTE    Reserved1;
	CANI_BYTE    Reserved2;
	CANI_BYTE    Reserved3;
	CANI_BYTE    Alpha;
} CAF_GREY_V2;

typedef struct tagCAF_RGB16_V2 {
	CANI_BYTE    Reserved1;
	CANI_BYTE    Reserved2;
	CANI_BYTE    Reserved3;
	CANI_BYTE    Reserved4;
	CANI_BYTE    Alpha;
} CAF_RGB16_V2;

typedef struct tagCAF_PALETTE_V2 {
	CANI_BYTE    Palette;
	CANI_BYTE    Reserved2;
	CANI_BYTE    Reserved3;
	CANI_BYTE    Reserved4;
	CANI_BYTE    Alpha;
} CAF_PALETTE_V2;

#pragma pack(1)

typedef struct tagCANI_CANI_CMYKAQUAD {
	CANI_BYTE    C;
	CANI_BYTE    M;
	CANI_BYTE    Y;
	CANI_BYTE    K;
	CANI_BYTE    Alpha;
} CANI_CMYKAQUAD;

typedef struct tagCANI_CMYKA16QUAD {
	CANI_WORD    C;
	CANI_WORD    M;
	CANI_WORD    Y;
	CANI_WORD    K;
	CANI_WORD    Alpha;
} CANI_CMYKA16QUAD;

typedef struct tagCANI_YUVAQUAD {
	CANI_BYTE    Y;
	CANI_BYTE    U;
	CANI_BYTE    V;
	CANI_BYTE    Alpha;
} CANI_YUVAQUAD;

typedef struct tagCANI_YUVA16QUAD {
	CANI_WORD    Y;
	CANI_WORD    U;
	CANI_WORD    V;
	CANI_WORD    Alpha;
} CANI_YUVA16QUAD;

typedef struct tagCANI_YCbCrAQUAD {
	CANI_BYTE    Y;
	CANI_BYTE    Cb;
	CANI_BYTE    Cr;
	CANI_BYTE    Alpha;
} CANI_YCbCrAQUAD;

typedef struct tagCANI_YCbCrA16QUAD {
	CANI_WORD    Y;
	CANI_WORD    Cb;
	CANI_WORD    Cr;
	CANI_WORD    Alpha;
} CANI_YCbCrA16QUAD;

typedef struct tagCAF_FORMAT_V2 {
	union {
		CAF_RGB_V2 rgb;
		CAF_RGB16_V2 rgb16;
		CAF_YUV_V2 yuv;
		CAF_YCbCr_V2 ycbcr;
		CAF_CMYK_V2 cmyk;
		CAF_YCbCrK_V2 ycbcrk;
		CAF_GREY_V2 gray;
		CAF_PALETTE_V2 palette;
	};
	CANI_DWORD options;
	CANI_BYTE format;
} CANI_FORMAT_V2;

#pragma pack()

#define CANI_OPTION_ALPHA                  0x00000001 // one alpha channel for all frames
#define CANI_OPTION_ALPHA_DIFFERENTATION   0x00000002 // alpha differentiation for all iframes
#define CANI_OPTION_DOUBLE_COMPRESSION     0x00000004
#define CANI_OPTION_CHANNEL_SEPARATION     0x00000008
#define CANI_OPTION_JPEG_DIFFERENTATION    0x00000010
#define CANI_OPTION_NO_ALPHA               0x00000020 // don't write alpha
#define CANI_OPTION_JPEG_GRAYSCALE         0x00000040
#define CANI_OPTION_48BITS_IMAGES          0x00000080
#define CANI_OPTION_16BITS_ALPHA           0x00000100 // 16 bits global alpha
#define CANI_OPTION_COMPRESS_SEPARATELY    0x00000200 // compress each image separately 
#define CANI_OPTION_CALCULATE_CHANGE_AREA  0x00000400

enum CAF_FORMAT_VALUE {
	CAF_FORMAT_RGB,
	CAF_FORMAT_YUV444,
	CAF_FORMAT_YUV422,
	CAF_FORMAT_YUV411,
	CAF_FORMAT_YCbCr,
	CAF_FORMAT_YCbCr422,
	CAF_FORMAT_YCbCr411,
	CAF_FORMAT_RGB16,
	CAF_FORMAT_RGB16_ND,
	CAF_FORMAT_RGB_ND,
	CAF_FORMAT_YUV444_ND,
	CAF_FORMAT_YUV422_ND,
	CAF_FORMAT_YUV411_ND,
	CAF_FORMAT_YCbCr_ND,
	CAF_FORMAT_YCbCr422_ND,
	CAF_FORMAT_YCbCr411_ND,
	CAF_FORMAT_GRAY,
	CAF_FORMAT_GRAY_ND,
	CAF_FORMAT_CMYK,
	CAF_FORMAT_CMYK_ND,
	CAF_FORMAT_YCbCrK,
	CAF_FORMAT_YCbCrK_ND,
	CAF_FORMAT_PALETTE,
	CAF_FORMAT_PALETTE_ND,
};

typedef struct tagYUVQUAD {
        CANI_BYTE    yuvY;
        CANI_BYTE    yuvU;
        CANI_BYTE    yuvV;
        CANI_BYTE    yuvReserved;
} YUVQUAD;

typedef struct tagYCbCrQUAD {
        CANI_BYTE    Y;
        CANI_BYTE    Cb;
        CANI_BYTE    Cr;
        CANI_BYTE    Reserved;
} YCbCrQUAD;

typedef struct tagCANI_FRAME_DATA {
	CANI_BYTE	*Image;
	CANI_DWORD Time; // frame display time in milliseconds
	union {
		CANI_COLORREF bgColor;
		CANI_RGBAQUAD bgColorRGB;
	};
	union {
		CANI_QWORD bgColor16;
		CANI_RGBA16QUAD bgColorRGB16;
	};
	CANI_DWORD Style; // 0x01 - transparent for bgColor, 0x02 - alpha channel added for this frame
} CANI_FRAME_DATA;

#define CANI_STYLE_TRANSPARENT  0x00000001
#define CANI_STYLE_ALPHA        0x00000002 // image have own alpha channel
#define CANI_STYLE_16BITS_IMAGE 0x00000004
#define CANI_STYLE_16BITS_ALPHA 0x00000008

#define LCAF_STYLE_ALPHA        0x01
#define LCAF_STYLE_16BITS_IMAGE 0x02

typedef struct tagCANI_ANIMATION {
	CANI_DWORD	Width;
	CANI_DWORD	Height;
	CANI_FRAME_DATA *Images;
	CANI_DWORD ImagesNR;
	CANI_DWORD ImagesNow;
	CANI_LONG Time;
	CANI_FORMAT_V2 Format;
	CANI_BYTE* gAlpha;
} CANI_ANIMATION;


typedef struct tagCANI_PALETTE {
	CANI_BYTE Red,Green,Blue;
	CANI_DWORD Index;
	CANI_QWORD Count;
} CANI_PALETTE;


typedef struct tagCANI_PALETTE_2 {
	CANI_BYTE Red, Green, Blue;
} CANI_PALETTE_2;

#define CANI_BYTE2WORD(x) ((((CANI_WORD)x)<<8)|((CANI_WORD)x))

#ifdef CANI_OPENGL

typedef struct tagCANI_RGBGLQUAD {
	CANI_BYTE    Red;
	CANI_BYTE    Green;
	CANI_BYTE    Blue;
	CANI_BYTE    Alpha;
} CANI_RGBGLQUAD;

#endif

typedef void(*CANI_PROGRESS_FUNC)( CANI_BYTE Progress );

#include "resize.h"

#if defined(CANI_WINDOWS_GDI) || defined(CANI_WINDOWS)

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

void CANI_BGRA_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA16_2_BGRA(CANI_BGRAQUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA16_2_RGBA(CANI_RGBAQUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_ALPHA_2_ALPHA16(CANI_WORD* dest, CANI_BYTE* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_ALPHA16_2_ALPHA(CANI_BYTE* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA16_2_GRAY16(CANI_WORD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA16_2_GRAY(CANI_BYTE* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA_2_GRAY(CANI_BYTE* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA16_2_GRAY16(CANI_WORD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA_2_GRAY(CANI_BYTE* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_GRAY16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_GRAY_2_BGRA(CANI_BGRAQUAD* dest, CANI_BYTE* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_GRAY16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_GRAY_2_RGBA(CANI_RGBAQUAD* dest, CANI_BYTE* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA16_2_RGB16(CANI_WORD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA16_2_RGB16(CANI_WORD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGB16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGB16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA16_2_CMYKA16(CANI_CMYKA16QUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA_2_CMYKA(CANI_CMYKAQUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA16_2_CMYKA16(CANI_CMYKA16QUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA_2_CMYKA(CANI_CMYKAQUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_CMYKA16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_CMYKA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_CMYKA_2_RGBA(CANI_RGBAQUAD* dest, CANI_CMYKAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_CMYKA16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_CMYKA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_CMYKA_2_BGRA(CANI_BGRAQUAD* dest, CANI_CMYKAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA16_2_YUVA16(CANI_YUVA16QUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA_2_YUVA(CANI_YUVAQUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA16_2_YUVA16(CANI_YUVA16QUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA_2_YUVA(CANI_YUVAQUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YUVA16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_YUVA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YUVA_2_RGBA(CANI_RGBAQUAD* dest, CANI_YUVAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YUVA16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_YUVA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YUVA_2_BGRA(CANI_BGRAQUAD* dest, CANI_YUVAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA16_2_YCbCrA16(CANI_YCbCrA16QUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_RGBA_2_YCbCrA(CANI_YCbCrAQUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA16_2_YCbCrA16(CANI_YCbCrA16QUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_BGRA_2_YCbCrA(CANI_YCbCrAQUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YCbCrA16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_YCbCrA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YCbCrA_2_RGBA(CANI_RGBAQUAD* dest, CANI_YCbCrAQUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YCbCrA16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_YCbCrA16QUAD* src, CANI_MEMORY_SIZE PixelsSize);

void CANI_YCbCrA_2_BGRA(CANI_BGRAQUAD* dest, CANI_YCbCrAQUAD* src, CANI_MEMORY_SIZE PixelsSize);


void CANI_ConvertImage(CANI_CHAR Colors16, CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16,
	CANI_FRAME_DATA* Frame, CANI_DWORD BitMapSize);

//CANI_BYTE* CANI_WriteBackgroundColor(CANI_CHAR Background16, CANI_BYTE* SrcNow, CANI_FRAME_DATA* Frame);

CANI_BYTE* CANI_WriteBackgroundColor(CANI_CHAR Red16, CANI_CHAR Green16, CANI_CHAR Blue16, CANI_BYTE* SrcNow, 
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings );

CANI_BYTE* CANI_WriteBackgroundColorCMYK(CANI_CHAR Cyan16, CANI_CHAR Magenta16, CANI_CHAR Yellow16, CANI_CHAR KeyColour16, CANI_BYTE* SrcNow,
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings);

CANI_BYTE* CANI_WriteBackgroundColorYUV(CANI_CHAR Y16, CANI_CHAR U16, CANI_CHAR V16, CANI_BYTE* SrcNow,
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings);

CANI_BYTE* CANI_WriteBackgroundColorYCbCr(CANI_CHAR Y16, CANI_CHAR Cb16, CANI_CHAR Cr16, CANI_BYTE* SrcNow,
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings);

CANI_BYTE* CANI_WriteAlpha(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Alpha16, CANI_BYTE* SrcNow,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_BYTE* AlphaBefore8, CANI_WORD* AlphaBefore16);

//CANI_BYTE* CANI_ReadBackgroundColor(CANI_CHAR Background16, CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame);

CANI_BYTE* CANI_ReadBackgroundColor(CANI_CHAR Red16, CANI_CHAR Green16, CANI_CHAR Blue16,
	CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings );

CANI_BYTE* CANI_ReadBackgroundColorCMYK(CANI_CHAR Cyan16, CANI_CHAR Magenta16, CANI_CHAR Yellow16, CANI_CHAR KeyColour16,
	CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings);

CANI_BYTE* CANI_ReadBackgroundColorYUV(CANI_CHAR Y16, CANI_CHAR U16, CANI_CHAR V16, CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings);

CANI_BYTE* CANI_ReadBackgroundColorYCbCr(CANI_CHAR Y16, CANI_CHAR Cb16, CANI_CHAR Cr16, CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings);

CANI_BYTE* CANI_ReadAlpha(CANI_ANIMATION* animation, CANI_FORMAT_V2* settings, CANI_DWORD n, CANI_CHAR Alpha16,
	CANI_BYTE* DestNow, CANI_BYTE* DestMax,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_BYTE* AlphaBefore8, CANI_WORD* AlphaBefore16,
	CANI_BYTE* alph4all, CANI_BYTE Version );

#define CANI_IMAGE_RED16 0x0001
#define CANI_IMAGE_GREEN16 0x0002
#define CANI_IMAGE_BLUE16 0x0004
#define CANI_IMAGE_ALPHA16 0x1000

#define CANI_IMAGE_CYAN16 0x0001
#define CANI_IMAGE_MAGENTA16 0x0002
#define CANI_IMAGE_YELLOW16 0x0004
#define CANI_IMAGE_KEYCOLOUR16 0x0008

#define CANI_IMAGE_YUV_Y16 0x0001
#define CANI_IMAGE_YUV_U16 0x0002
#define CANI_IMAGE_YUV_V16 0x0004

#define CANI_IMAGE_YCbCr_Y16 0x0001
#define CANI_IMAGE_YCbCr_Cb16 0x0002
#define CANI_IMAGE_YCbCr_Cr16 0x0004

#define CANI_IMAGE_GRAY16 0x0001

#define CANI_IMAGE_RGB16 0x0001

CANI_CHAR CANI_DeConvertImage(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_WORD Which16);

CANI_CHAR CANI_DeConvertImageGray(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_WORD Which16);

CANI_CHAR CANI_DeConvertImageRGB16(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_WORD Which16);

CANI_CHAR CANI_DeConvertImageCMYK(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_CMYKAQUAD* ImageCMYK8, CANI_CMYKA16QUAD* ImageCMYK16, CANI_WORD Which16);

CANI_CHAR CANI_DeConvertImageYUV(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_YUVAQUAD* ImageYUV8, CANI_YUVA16QUAD* ImageYUV16, CANI_WORD Which16);

CANI_CHAR CANI_DeConvertImageYCbCr(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_YCbCrAQUAD* ImageYCbCr8, CANI_YCbCrA16QUAD* ImageYCbCr16, CANI_WORD Which16);


CANI_DWORD CANI_CalculateAlphaSize(CANI_ANIMATION* animation, CANI_CHAR Alpha16);

CANI_DWORD CANI_CalculateGlobalAlphaSize(CANI_ANIMATION* animation, CANI_CHAR Alpha16);

CANI_DWORD CANI_CalculateLocalAlphaSize(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Alpha16);

CANI_BYTE* CANI_WriteGlobalAlpha(CANI_ANIMATION* animation, CANI_CHAR Alpha16, CANI_BYTE* SrcNow);

CANI_CHAR CANI_CompressAndWrite(FILE* File, CANI_BYTE* Beg, CANI_BYTE* End);

CANI_CHAR CANI_WriteZeroData(FILE* File);

CANI_BYTE* CANI_ReadAndDeCompress(FILE* File, CANI_DWORD* size);

CANI_INLINE CANI_BYTE CAF_Mask(CANI_LONG value, CANI_BYTE NrOfBits);

CANI_INLINE CANI_BYTE CAF_UnMask(CANI_LONG value, CANI_BYTE NrOfBits);

CANI_INLINE CANI_WORD CAF_Mask16(CANI_LONG value, CANI_BYTE NrOfBits);

CANI_INLINE CANI_WORD CAF_UnMask16(CANI_LONG value, CANI_BYTE NrOfBits);

CANI_EXPORT void CANI_FreeAnimation(CANI_ANIMATION* animation);

CANI_EXPORT CANI_CHAR CANI_ResetAnimation(CANI_ANIMATION* animation);

CANI_EXPORT CANI_ANIMATION* CANI_CreateAnimation(CANI_LONG Width, CANI_LONG Height, CANI_FORMAT_V2* Format);

CANI_EXPORT CANI_CHAR CANI_SetGlobalAlpha16(CANI_ANIMATION* animation, CANI_WORD* Alpha);

CANI_EXPORT CANI_CHAR CANI_AddImage16(CANI_ANIMATION* animation, CANI_BGRA16QUAD* Image, CANI_DWORD Style,
	CANI_DWORD Time, CANI_RGBA16QUAD* bg_color);

CANI_EXPORT void CANI_ConvertBackgroundColorToAlpha(CANI_ANIMATION* animation);

CANI_EXPORT CANI_LONG CANI_NextFrame(CANI_ANIMATION* animation);

CANI_EXPORT CANI_LONG CANI_CalculateNextFrame(CANI_ANIMATION* animation, CANI_DWORD AddTime);

CANI_EXPORT CANI_DWORD CANI_GetImageSize(CANI_ANIMATION* animation, CANI_DWORD* Width, CANI_DWORD* Height);

CANI_EXPORT CANI_LONG CANI_GetNumberOfFrames(CANI_ANIMATION* animation);

CANI_EXPORT CANI_LONG CANI_GetCurrentFrame(CANI_ANIMATION* animation);

CANI_EXPORT CANI_CHAR CANI_GetImageOfCurrentFrame(CANI_ANIMATION* animation, CANI_BGRAQUAD* ImageData);

CANI_EXPORT CANI_LONG CANI_GetFrameDisplayTime(CANI_ANIMATION* animation, CANI_DWORD FrameNR);

CANI_EXPORT CANI_CHAR CANI_GetFrameBackgroundColor(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_RGBAQUAD* bgColorRGB);

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrame(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_BGRAQUAD* ImageData);

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrame16(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_BGRA16QUAD* ImageData);

CANI_EXPORT CANI_CHAR CANI_GetAlphaOfFrame(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_BYTE* ImageData);

CANI_EXPORT CANI_CHAR CANI_GetGlobalAlpha16(CANI_ANIMATION* animation, CANI_WORD* ImageData);

CANI_EXPORT CANI_CHAR CANI_GetGlobalAlpha(CANI_ANIMATION* animation, CANI_BYTE* ImageData);

CANI_EXPORT CANI_CHAR CANI_GetImageOfCurrentFrameRGBA(CANI_ANIMATION* animation, CANI_RGBAQUAD* ImageData);

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrameRGBA(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_RGBAQUAD* ImageData);

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrameRGBA16(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_RGBA16QUAD* ImageData);

CANI_EXPORT CANI_CHAR CANI_IsImageOfFrame16Bits(CANI_ANIMATION* animation, CANI_DWORD FrameNR);

typedef struct tagCANI_RECT {
	CANI_LONG    left;
	CANI_LONG    top;
	CANI_LONG    right;
	CANI_LONG    bottom;
} CANI_RECT;

enum CANI_RESIZE_METHOD
{
	CANI_FAST_RESIZE,
	CANI_INTERPOLATION,
	CANI_SQUARE_INTERPOLATION,
	CANI_CUBIC_INTERPOLATION,
	CANI_LAGRANGE_INTERPOLATION
};

#define CANI_PI 3.14159265359f

CANI_EXPORT CANI_ANIMATION* CANI_CreateResizedAnimation(CANI_ANIMATION* animation, CANI_LONG dest_cx, CANI_LONG dest_cy,
	CANI_BYTE method, CANI_PROGRESS_FUNC Progress);


#ifdef CANI_WINDOWS_GDI

CANI_EXPORT CANI_CHAR CANI_WinDrawImage(CANI_ANIMATION* animation, HDC dest, CANI_LONG x, CANI_LONG y);

CANI_EXPORT CANI_CHAR CANI_WinDrawMask(CANI_ANIMATION* animation, HDC dest, CANI_LONG x, CANI_LONG y);

CANI_EXPORT CANI_CHAR CANI_WinDrawImageResized(CANI_ANIMATION* animation, HDC dest,
	CANI_LONG xDest, CANI_LONG yDest, CANI_LONG DestWidth, CANI_LONG DestHeight, CANI_LONG xSrc, CANI_LONG ySrc, CANI_LONG SrcWidth, CANI_LONG SrcHeight);

#endif

CANI_CHAR CANI_ReadJPEG(FILE* file, CANI_BGRA16QUAD* ImageRGB, CANI_LONG image_width, CANI_LONG image_height);
CANI_CHAR CANI_ReadAlphaJPEG(FILE* file, CANI_BYTE* ImageA, CANI_LONG image_width, CANI_LONG image_height);
CANI_CHAR CANI_ReadAlphaJPEG_RGB(FILE* file, CANI_BGRA16QUAD* ImageRGB, CANI_LONG image_width, CANI_LONG image_height);

CANI_CHAR CANI_WriteJPEG(FILE* file, CANI_BGRA16QUAD* ImageRGB, int image_width, int image_height, int quality, CANI_BYTE typ);
CANI_CHAR CANI_WriteAlphaJPEG(FILE* file, CANI_BYTE* ImageA, int image_width, int image_height, int quality);
CANI_CHAR CANI_WriteAlphaJPEG_RGB(FILE* file, CANI_BGRA16QUAD* ImageRGB, int image_width, int image_height, int quality);

CANI_ANIMATION* CANI_ReadJPEG_FILE(FILE* file);
CANI_ANIMATION* CANI_ReadLCAF(FILE* File);

CANI_ANIMATION* CANI_ReadPXM_FILE(FILE* file);

#ifdef CANI_OPENGL

//#include <GL/gl.h>
//#include <GL/glu.h>

#endif

CANI_EXPORT CANI_ANIMATION* CANI_ReadAnimation(FILE* File, CANI_PROGRESS_FUNC Progress);
CANI_EXPORT CANI_CHAR CANI_WriteAnimation(FILE* File, CANI_ANIMATION* animation, CANI_PROGRESS_FUNC Progress);
CANI_EXPORT CANI_CHAR LCAF_WriteAnimation(FILE* File, CANI_ANIMATION* animation, CANI_WORD ImageQuality, CANI_WORD AlphaQuality, CANI_PROGRESS_FUNC Progress);


#ifdef CANI_UNICODE_FILE
CANI_EXPORT CANI_ANIMATION* CANI_OpenAnimationW(CANI_WORD* FileName, CANI_PROGRESS_FUNC Progress);
CANI_EXPORT CANI_CHAR CANI_SaveAnimationW(CANI_WORD* FileName, CANI_ANIMATION* animation, CANI_PROGRESS_FUNC Progress);
CANI_EXPORT CANI_CHAR LCAF_SaveAnimationW(CANI_WORD* FileName, CANI_ANIMATION* animation, CANI_WORD ImageQuality, CANI_WORD AlphaQuality, CANI_PROGRESS_FUNC Progress);
#endif

CANI_EXPORT CANI_ANIMATION* CANI_OpenAnimationA(CANI_CHAR* FileName, CANI_PROGRESS_FUNC Progress);
CANI_EXPORT CANI_CHAR CANI_SaveAnimationA(CANI_CHAR* FileName, CANI_ANIMATION* animation, CANI_PROGRESS_FUNC Progress);
CANI_EXPORT CANI_CHAR LCAF_SaveAnimationA(CANI_CHAR* FileName, CANI_ANIMATION* animation, CANI_WORD ImageQuality, CANI_WORD AlphaQuality, CANI_PROGRESS_FUNC Progress);

#if defined( CANI_UNICODE_FILE ) && ( defined( UNICODE ) || defined( _UNICODE )  ) 
#define CANI_OpenAnimation CANI_OpenAnimationW
#define CANI_SaveAnimation CANI_SaveAnimationW
#define LCAF_SaveAnimation LCAF_SaveAnimationW
#else
#define CANI_OpenAnimation CANI_OpenAnimationA
#define CANI_SaveAnimation CANI_SaveAnimationA
#define LCAF_SaveAnimation LCAF_SaveAnimationA
#endif

CANI_ANIMATION* CANI_ReadBMP_FILE(FILE* file);

#ifdef CANI_USE_LIBPNG
CANI_ANIMATION* CANI_ReadPNG_FILE(FILE* file);
#endif

#ifdef CANI_USE_LIBMNG
CANI_ANIMATION* CANI_ReadMNG_FILE(FILE* file, CANI_PROGRESS_FUNC Progress);
#endif

#ifdef CANI_USE_GIFLIB
CANI_ANIMATION* CANI_ReadGIF_FILE(FILE* file, CANI_PROGRESS_FUNC Progress);
#endif

#ifdef CANI_USE_LIBAPNG
CANI_ANIMATION* CANI_ReadAPNG_FILE(FILE* file, CANI_PROGRESS_FUNC Progress);
#endif

//char szCaniBuffer[1024];

#define CANI_ERROR_NO_ERROR                  0
#define CANI_ERROR_NO_FREE_MEMORY            1
#define CANI_ERROR_MEMORY_TOO_LARGE          2
#define CANI_ERROR_UNSUPPORTED_FILE          3
#define CANI_ERROR_UNSUPPORTED_VERSION       4
#define CANI_ERROR_END_OF_FILE               5
#define CANI_ERROR_PALLET_SIZE_TOO_LARGE     6
#define CANI_ERROR_TOO_MANY_COLORS           7
#define CANI_ERROR_ERROR_DURING_READING      8
#define CANI_ERROR_ERROR_DURING_WRITING      9
#define CANI_ERROR_NO_ANIMATION             10
#define CANI_ERROR_WRONG_FRAME_NUMBER       11
#define CANI_ERROR_TOO_BIG_IMAGE            12
#define CANI_ERROR_SMALL_IMAGE_SIZES        13

CANI_EXPORT void CANI_SetLastError(CANI_DWORD ErrorCode);

CANI_EXPORT CANI_DWORD CANI_GetLastError();

#ifdef __cplusplus
}
#endif