
#include "cani.h"

CANI_INLINE CANI_BYTE CAF_Mask(CANI_LONG value, CANI_BYTE NrOfBits) {

	static CANI_BYTE MaskList[8] = {
		0x00,
		0x80,
		0xC0,
		0xE0,
		0xF0,
		0xF8,
		0xFC,
		0xFE
	};

	if (NrOfBits>=8) return (CANI_BYTE)value;

	return (CANI_BYTE)(value & MaskList[NrOfBits]);

	/*switch (NrOfBits) {
	case 1:
		return (CANI_BYTE)(value & 0x80);
	case 2:
		return (CANI_BYTE)(value & 0xC0);
	case 3:
		return (CANI_BYTE)(value & 0xE0);
	case 4:
		return (CANI_BYTE)(value & 0xF0);
	case 5:
		return (CANI_BYTE)(value & 0xF8);
	case 6:
		return (CANI_BYTE)(value & 0xFC);
	case 7:
		return (CANI_BYTE)(value & 0xFE);
	}
	return (CANI_BYTE)value;*/
}

CANI_INLINE CANI_BYTE CAF_UnMask(CANI_LONG value, CANI_BYTE NrOfBits) {
	if (NrOfBits >= 8) return (CANI_BYTE)value;
	//return value;
	switch (NrOfBits) {
	case 1:
		return (CANI_BYTE)((value & 0x80) ? 0xFF : 0x00);
		//return ( value & 0x80 );
	case 2:
		return (CANI_BYTE)((value & 0xC0) | ((value & 0xC0) >> 2) | ((value & 0xC0) >> 4) | ((value & 0xC0) >> 6));
		//return ( value & 0xC0 );
	case 3:
		return (CANI_BYTE)((value & 0xE0) | ((value & 0xE0) >> 3) | ((value & 0xE0) >> 6));
		//return ( value & 0xE0 );
	case 4:
		return (CANI_BYTE)((value & 0xF0) | ((value & 0xF0) >> 4));
	case 5:
		return (CANI_BYTE)((value & 0xF8) | ((value & 0xF8) >> 5));
		//return ( value & 0xF8 );
	case 6:
		return (CANI_BYTE)((value & 0xFC) | ((value & 0xFC) >> 6));
		//return ( value & 0xFC );
	case 7:
		//return (CANI_BYTE)( value & 0xFE );
		return (CANI_BYTE)((value & 0xFE) | ((value & 0xFE) >> 7));
	}
	return (CANI_BYTE)value;
}

CANI_INLINE CANI_WORD CAF_Mask16(CANI_LONG value, CANI_BYTE NrOfBits) {

	static CANI_WORD MaskList[16] = {
		0x0000,
		0x8000,
		0xC000,
		0xE000,
		0xF000,
		0xF800,
		0xFC00,
		0xFE00,
		0xFF00,
		0xFF80,
		0xFFC0,
		0xFFE0,
		0xFFF0,
		0xFFF8
	};

	if (NrOfBits >= 16) return (CANI_WORD)value;

	return (CANI_WORD)(value & MaskList[NrOfBits]);

	/*switch (NrOfBits) {
	case 1:
		return (CANI_WORD)(value & 0x8000);
	case 2:
		return (CANI_WORD)(value & 0xC000);
	case 3:
		return (CANI_WORD)(value & 0xE000);
	case 4:
		return (CANI_WORD)(value & 0xF000);
	case 5:
		return (CANI_WORD)(value & 0xF800);
	case 6:
		return (CANI_WORD)(value & 0xFC00);
	case 7:
		return (CANI_WORD)(value & 0xFE00);
	case 8:
		return (CANI_WORD)(value & 0xFF00);
	case 9:
		return (CANI_WORD)(value & 0xFF80);
	case 10:
		return (CANI_WORD)(value & 0xFFC0);
	case 11:
		return (CANI_WORD)(value & 0xFFE0);
	case 12:
		return (CANI_WORD)(value & 0xFFF0);
	case 13:
		return (CANI_WORD)(value & 0xFFF8);
	case 14:
		return (CANI_WORD)(value & 0xFFFC);
	case 15:
		return (CANI_WORD)(value & 0xFFFE);
	}
	return (CANI_WORD)value;*/
}

CANI_INLINE CANI_WORD CAF_UnMask16(CANI_LONG value, CANI_BYTE NrOfBits) {
	//return value;
	if (NrOfBits >= 16) return (CANI_WORD)value;
	switch (NrOfBits) {
	case 1:
		return (CANI_WORD)((value & 0x8000) ? 0xFFFF : 0x0000);
	case 2:
		return (CANI_WORD)((value & 0xC000) | ((value & 0xC000) >> 2) | ((value & 0xC000) >> 4) | ((value & 0xC000) >> 6) | ((value & 0xC000) >> 8) | ((value & 0xC000) >> 10) | ((value & 0xC000) >> 12) | ((value & 0xC000) >> 14));
	case 3:
		return (CANI_WORD)((value & 0xE000) | ((value & 0xE000) >> 3) | ((value & 0xE000) >> 6) | ((value & 0xE000) >> 9) | ((value & 0xE000) >> 12) | ((value & 0xE000) >> 15));
	case 4:
		return (CANI_WORD)((value & 0xF000) | ((value & 0xF000) >> 4) | ((value & 0xF000) >> 8) | ((value & 0xF000) >> 12));
	case 5:
		return (CANI_WORD)((value & 0xF800) | ((value & 0xF800) >> 5) | ((value & 0xF800) >> 10) | ((value & 0xF800) >> 15));
	case 6:
		return (CANI_WORD)((value & 0xFC00) | ((value & 0xFC00) >> 6) | ((value & 0xFC00) >> 12));
	case 7:
		return (CANI_WORD)((value & 0xFE00) | ((value & 0xFE00) >> 7) | ((value & 0xFE00) >> 14));
	case 8:
		return (CANI_WORD)((value & 0xFF00) | ((value & 0xFE00) >> 8));
	case 9:
		return (CANI_WORD)((value & 0xFF80) | ((value & 0xFE00) >> 9));
	case 10:
		return (CANI_WORD)((value & 0xFFC0) | ((value & 0xFE00) >> 10));
	case 11:
		return (CANI_WORD)((value & 0xFFE0) | ((value & 0xFE00) >> 11));
	case 12:
		return (CANI_WORD)((value & 0xFFF0) | ((value & 0xFE00) >> 12));
	case 13:
		return (CANI_WORD)((value & 0xFFF8) | ((value & 0xFE00) >> 13));
	case 14:
		return (CANI_WORD)((value & 0xFFFC) | ((value & 0xFE00) >> 14));
	case 15:
		return (CANI_WORD)((value & 0xFFFE) | ((value & 0xFE00) >> 15));
	}
	return (CANI_WORD)value;
}

void CANI_ConvertImage(CANI_CHAR Colors16, CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16,
	CANI_FRAME_DATA* Frame, CANI_DWORD BitMapSize ) {

	if (Colors16) {
		if (Frame->Style & CANI_STYLE_16BITS_IMAGE) {
			if (Image8) CANI_BGRA16_2_BGRA(Image8, (CANI_BGRA16QUAD*)Frame->Image, BitMapSize);
			if (Image16) CANI_MEMCPY(Image16, (CANI_BGRA16QUAD*)Frame->Image, BitMapSize * sizeof(CANI_BGRA16QUAD));
		}
		else {
			// background color

			//MessageBoxA(0, "Colors16", "Colors16", 0);
			CANI_RGBA_2_RGBA16(&Frame->bgColorRGB16, &Frame->bgColorRGB, 1);

			if (Image8) CANI_MEMCPY(Image8, (CANI_BGRAQUAD*)Frame->Image, BitMapSize * sizeof(CANI_BGRAQUAD));
			if (Image16) CANI_BGRA_2_BGRA16(Image16, (CANI_BGRAQUAD*)Frame->Image, BitMapSize);
		}
	}
	else {
		if (Frame->Style & CANI_STYLE_16BITS_IMAGE) {
			// background color
			CANI_RGBA16_2_RGBA(&Frame->bgColorRGB, &Frame->bgColorRGB16, 1);

			if (Image8) CANI_BGRA16_2_BGRA(Image8, (CANI_BGRA16QUAD*)Frame->Image, BitMapSize);
			if (Image16) CANI_MEMCPY(Image16, (CANI_BGRA16QUAD*)Frame->Image, BitMapSize * sizeof(CANI_BGRA16QUAD));
		}
		else {
			if (Image8) CANI_MEMCPY(Image8, (CANI_BGRAQUAD*)Frame->Image, BitMapSize * sizeof(CANI_BGRAQUAD));
			if (Image16) CANI_BGRA_2_BGRA16(Image16, (CANI_BGRAQUAD*)Frame->Image, BitMapSize);
		}
	}
}

CANI_BYTE* CANI_WriteBackgroundColor(CANI_CHAR Red16, CANI_CHAR Green16, CANI_CHAR Blue16, CANI_BYTE* SrcNow,
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2 *settings ) {

	if (Red16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(Frame->bgColorRGB16.Red, settings->rgb.Red);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask( Frame->bgColorRGB.Red, settings->rgb.Red);
		SrcNow += 1;
	}

	if (Green16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(Frame->bgColorRGB16.Green, settings->rgb.Green);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(Frame->bgColorRGB.Green, settings->rgb.Green);
		SrcNow += 1;
	}

	if (Blue16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(Frame->bgColorRGB16.Blue, settings->rgb.Blue);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(Frame->bgColorRGB.Blue, settings->rgb.Blue);
		SrcNow += 1;
	}

	return SrcNow;
}

// CANI_WriteBackgroundColorCMYK(Cyan16, Magenta16, Yellow16, KeyColour16, SrcNow, &animation->Images[n], &animation->Format);

CANI_BYTE* CANI_WriteBackgroundColorCMYK(CANI_CHAR Cyan16, CANI_CHAR Magenta16, CANI_CHAR Yellow16, CANI_CHAR KeyColour16, CANI_BYTE* SrcNow,
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings) {


	CANI_CMYKAQUAD bg8;
	CANI_CMYKA16QUAD bg16;

	CANI_RGBA_2_CMYKA(&bg8, &Frame->bgColorRGB, 1);

	CANI_RGBA16_2_CMYKA16(&bg16, &Frame->bgColorRGB16, 1);

	if (Cyan16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.C, settings->cmyk.C);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.C, settings->cmyk.C);
		SrcNow += 1;
	}

	if (Magenta16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.M, settings->cmyk.M);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.M, settings->cmyk.M);
		SrcNow += 1;
	}

	if (Yellow16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.Y, settings->cmyk.Y);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.Y, settings->cmyk.Y);
		SrcNow += 1;
	}

	if (KeyColour16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.K, settings->cmyk.K);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.K, settings->cmyk.K);
		SrcNow += 1;
	}

	return SrcNow;
}

CANI_BYTE* CANI_WriteBackgroundColorYUV(CANI_CHAR Y16, CANI_CHAR U16, CANI_CHAR V16, CANI_BYTE* SrcNow,
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings) {


	CANI_YUVAQUAD bg8;
	CANI_YUVA16QUAD bg16;

	CANI_RGBA_2_YUVA(&bg8, &Frame->bgColorRGB, 1);

	CANI_RGBA16_2_YUVA16(&bg16, &Frame->bgColorRGB16, 1);

	if (Y16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.Y, settings->yuv.Y);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.Y, settings->yuv.Y);
		SrcNow += 1;
	}

	if (U16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.U, settings->yuv.U);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.U, settings->yuv.U);
		SrcNow += 1;
	}

	if (V16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.V, settings->yuv.V);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.V, settings->yuv.V);
		SrcNow += 1;
	}

	return SrcNow;
}

CANI_BYTE* CANI_WriteBackgroundColorYCbCr(CANI_CHAR Y16, CANI_CHAR Cb16, CANI_CHAR Cr16, CANI_BYTE* SrcNow,
	CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings) {


	CANI_YCbCrAQUAD bg8;
	CANI_YCbCrA16QUAD bg16;

	CANI_RGBA_2_YCbCrA(&bg8, &Frame->bgColorRGB, 1);

	CANI_RGBA16_2_YCbCrA16(&bg16, &Frame->bgColorRGB16, 1);

	if (Y16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.Y, settings->ycbcr.Y);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.Y, settings->ycbcr.Y);
		SrcNow += 1;
	}

	if (Cb16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.Cb, settings->ycbcr.Cb);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.Cb, settings->ycbcr.Cb);
		SrcNow += 1;
	}

	if (Cr16) {
		*((CANI_WORD*)SrcNow) = CAF_Mask16(bg16.Cr, settings->ycbcr.Cr);
		SrcNow += 2;
	}
	else {
		*((CANI_BYTE*)SrcNow) = CAF_Mask(bg8.Cr, settings->ycbcr.Cr);
		SrcNow += 1;
	}

	return SrcNow;
}

/*CANI_BYTE* CANI_WriteBackgroundColor(CANI_CHAR Background16, CANI_BYTE* SrcNow, CANI_FRAME_DATA* Frame) {

	if (Background16) {

		*((CANI_WORD*)SrcNow) = ((CANI_WORD*)&Frame->bgColor16)[0];// CAF_Mask16(((CANI_WORD*)&Frame->bgColor16)[0], animation->Format.rgb.Red);
		SrcNow += 2;

		*((CANI_WORD*)SrcNow) = ((CANI_WORD*)&Frame->bgColor16)[1];//CAF_Mask16(((CANI_WORD*)&Frame->bgColor16)[1], animation->Format.rgb.Green);
		SrcNow += 2;

		*((CANI_WORD*)SrcNow) = ((CANI_WORD*)&Frame->bgColor16)[2];//CAF_Mask16(((CANI_WORD*)&Frame->bgColor16)[2], animation->Format.rgb.Blue);
		SrcNow += 2;
	}
	else {

		*SrcNow = ((CANI_BYTE*)&Frame->bgColor)[0];// CAF_Mask(((CANI_BYTE*)&Frame->bgColor)[0], animation->Format.rgb.Red);
		SrcNow++;

		*SrcNow = ((CANI_BYTE*)&Frame->bgColor)[1];//CAF_Mask(((CANI_BYTE*)&Frame->bgColor)[1], animation->Format.rgb.Green);
		SrcNow++;

		*SrcNow = ((CANI_BYTE*)&Frame->bgColor)[2];//CAF_Mask(((CANI_BYTE*)&Frame->bgColor)[2], animation->Format.rgb.Blue);
		SrcNow++;

	}

	return SrcNow;
}*/

CANI_BYTE* CANI_WriteAlpha(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Alpha16, CANI_BYTE* SrcNow,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_BYTE* AlphaBefore8, CANI_WORD* AlphaBefore16 ) {

	static CANI_BYTE BitMask[8] = {
		0x01,
		0x02,
		0x04,
		0x08,
		0x10,
		0x20,
		0x40,
		0x80
	};

	CANI_DWORD m,b;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {
		if (Alpha16) {
			for (m = 0; m < BitMapSize; m++) {
				*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Alpha, animation->Format.rgb.Alpha) - AlphaBefore16[m];
				SrcNow += 2;
			}
			for (m = 0; m < BitMapSize; m++) {
				AlphaBefore16[m] = CAF_Mask16(Image16[m].Alpha, animation->Format.rgb.Alpha);
			}
		}
		else if (animation->Format.rgb.Alpha == 1) {

			CANI_DWORD Size = BitMapSize / 8;
			if (BitMapSize % 8) Size++;
			for (b = 0; b < BitMapSize;) {
				CANI_BYTE byte = 0;
				for (m = 0; b < BitMapSize && m < 8; m++, b++) {
					//if ( Image8[n/8].Alpha & BitMask[] ) byte
					if (Image8[b].Alpha & 0xF0) byte |= BitMask[m];
				}
				*SrcNow = byte - AlphaBefore16[b/8];
				AlphaBefore16[b / 8] = byte;
				SrcNow++;
			}
		}
		else {
			for (m = 0; m < BitMapSize; m++) {
				*SrcNow = CAF_Mask(Image8[m].Alpha, animation->Format.rgb.Alpha) - AlphaBefore8[m];
				SrcNow++;
			}
			for (m = 0; m < BitMapSize; m++) {
				AlphaBefore8[m] = CAF_Mask(Image8[m].Alpha, animation->Format.rgb.Alpha);
			}

		}
	}
	else if (animation->Images[n].Style & CANI_STYLE_ALPHA) {
		if (Alpha16) {
			for (m = 0; m < BitMapSize; m++) {
				*((CANI_WORD*)SrcNow) = Image16[m].Alpha;
				SrcNow += 2;
			}
		}
		else if (animation->Format.rgb.Alpha == 1 ) {

			CANI_DWORD Size = BitMapSize / 8;
			if (BitMapSize % 8) Size++;
			for (b = 0; b < BitMapSize;) {
				CANI_BYTE byte = 0;
				for (m = 0; b < BitMapSize && m < 8;m++,b++) {
					//if ( Image8[n/8].Alpha & BitMask[] ) byte
					if (Image8[b].Alpha & 0xF0) byte |= BitMask[m];
				}
				*SrcNow = byte;
				SrcNow++;
			}
		}
		else {
			for (m = 0; m < BitMapSize; m++) {
				*SrcNow = Image8[m].Alpha;
				SrcNow++;
			}
		}
	}

	return SrcNow;
}

/*CANI_BYTE* CANI_ReadBackgroundColor(CANI_CHAR Background16, CANI_BYTE* DestNow, CANI_BYTE * DestMax, CANI_FRAME_DATA* Frame) {

	if (Background16) {

		((CANI_WORD*)&Frame->bgColor16)[0] = *((CANI_WORD*)DestNow);// CAF_UnMask16(*((CANI_DWORD*)DestNow), rv->Format.rgb.Red);
		DestNow += 2;
		if (DestNow > DestMax) return DestNow;

		((CANI_WORD*)&Frame->bgColor16)[1] = *((CANI_WORD*)DestNow);// = CAF_UnMask16(*((CANI_DWORD*)DestNow), rv->Format.rgb.Green);
		DestNow += 2;
		if (DestNow > DestMax) return DestNow;

		((CANI_WORD*)&Frame->bgColor16)[2] = *((CANI_WORD*)DestNow);// = CAF_UnMask16(*((CANI_DWORD*)DestNow), rv->Format.rgb.Blue);
		DestNow += 2;
		if (DestNow > DestMax) return DestNow;

		CANI_RGBA16_2_RGBA(&Frame->bgColorRGB, &Frame->bgColorRGB16, 1);
	}
	else {

		((CANI_BYTE*)&Frame->bgColor)[0] = *((CANI_BYTE*)DestNow);// CAF_UnMask(*((CANI_BYTE*)DestNow), rv->Format.rgb.Red);
		DestNow += 1;
		if (DestNow > DestMax) return DestNow;

		((CANI_BYTE*)&Frame->bgColor)[1] = *((CANI_BYTE*)DestNow);//CAF_UnMask(*((CANI_BYTE*)DestNow), rv->Format.rgb.Green);
		DestNow += 1;
		if (DestNow > DestMax) return DestNow;

		((CANI_BYTE*)&Frame->bgColor)[2] = *((CANI_BYTE*)DestNow);//CAF_UnMask(*((CANI_BYTE*)DestNow), rv->Format.rgb.Blue);
		DestNow += 1;
		if (DestNow > DestMax) return DestNow;

		CANI_RGBA_2_RGBA16(&Frame->bgColorRGB16, &Frame->bgColorRGB, 1);

	}

	return DestNow;
}*/

// Red16,Green16,Blue16

CANI_BYTE* CANI_ReadBackgroundColor(CANI_CHAR Red16, CANI_CHAR Green16, CANI_CHAR Blue16,
	CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings ) {

	if (Red16) {
		Frame->bgColorRGB16.Red = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->rgb.Red);
		Frame->bgColorRGB.Red = (CANI_BYTE)(Frame->bgColorRGB16.Red >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		Frame->bgColorRGB.Red = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->rgb.Red);
		Frame->bgColorRGB16.Red = ((((CANI_WORD)Frame->bgColorRGB.Red) << 8) | ((CANI_WORD)Frame->bgColorRGB.Red));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (Green16) {
		Frame->bgColorRGB16.Green = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->rgb.Green);
		Frame->bgColorRGB.Green = (CANI_BYTE)(Frame->bgColorRGB16.Green >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		Frame->bgColorRGB.Green = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->rgb.Green);
		Frame->bgColorRGB16.Green = ((((CANI_WORD)Frame->bgColorRGB.Green) << 8) | ((CANI_WORD)Frame->bgColorRGB.Green));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (Blue16) {
		Frame->bgColorRGB16.Blue = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->rgb.Blue);
		Frame->bgColorRGB.Blue = (CANI_BYTE)(Frame->bgColorRGB16.Blue >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		Frame->bgColorRGB.Blue = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->rgb.Blue);
		Frame->bgColorRGB16.Blue = ((((CANI_WORD)Frame->bgColorRGB.Blue) << 8) | ((CANI_WORD)Frame->bgColorRGB.Blue));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	return DestNow;
}

CANI_BYTE* CANI_ReadBackgroundColorCMYK(CANI_CHAR Cyan16, CANI_CHAR Magenta16, CANI_CHAR Yellow16, CANI_CHAR KeyColour16,
	CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings) {

	CANI_CMYKAQUAD bg8;
	CANI_CMYKA16QUAD bg16;

	if (Cyan16) {
		bg16.C = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->cmyk.C );
		bg8.C = (CANI_BYTE)(bg16.C >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.C = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->cmyk.C );
		bg16.C = ((((CANI_WORD)bg8.C) << 8) | ((CANI_WORD)bg8.C));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (Magenta16) {
		bg16.M = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->cmyk.M);
		bg8.M = (CANI_BYTE)(bg16.M >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.M = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->cmyk.M);
		bg16.M = ((((CANI_WORD)bg8.M) << 8) | ((CANI_WORD)bg8.M));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (Yellow16) {
		bg16.Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->cmyk.Y);
		bg8.Y = (CANI_BYTE)(bg16.Y >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.Y = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->cmyk.Y);
		bg16.Y = ((((CANI_WORD)bg8.Y) << 8) | ((CANI_WORD)bg8.Y));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (KeyColour16) {
		bg16.K = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->cmyk.K);
		bg8.K = (CANI_BYTE)(bg16.K >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.K = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->cmyk.K);
		bg16.K = ((((CANI_WORD)bg8.K) << 8) | ((CANI_WORD)bg8.K));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	CANI_CMYKA_2_RGBA(&Frame->bgColorRGB, &bg8, 1);

	CANI_CMYKA16_2_RGBA16(&Frame->bgColorRGB16, &bg16, 1);

	//char szCaniBuffer[1024];

	//sprintf(szCaniBuffer, "0x%04x 0x%04x 0x%04x 0x%04x", bg16.C, bg16.M, bg16.Y, bg16.K );
	//MessageBoxA(0, szCaniBuffer, "CMYK", 0);

	//sprintf(szCaniBuffer, "0x%04x 0x%04x 0x%04x", Frame->bgColorRGB16.Red, Frame->bgColorRGB16.Green, Frame->bgColorRGB16.Blue);
	//MessageBoxA(0,szCaniBuffer, "", 0);

	return DestNow;
}



CANI_BYTE* CANI_ReadBackgroundColorYUV(CANI_CHAR Y16, CANI_CHAR U16, CANI_CHAR V16, CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings) {

	CANI_YUVAQUAD bg8;
	CANI_YUVA16QUAD bg16;

	if (Y16) {
		bg16.Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->yuv.Y);
		bg8.Y = (CANI_BYTE)(bg16.Y >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.Y = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->yuv.Y);
		bg16.Y = ((((CANI_WORD)bg8.Y) << 8) | ((CANI_WORD)bg8.Y));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (U16) {
		bg16.U = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->yuv.U);
		bg8.U = (CANI_BYTE)(bg16.U >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.U = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->yuv.U);
		bg16.U = ((((CANI_WORD)bg8.U) << 8) | ((CANI_WORD)bg8.U));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (V16) {
		bg16.V = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->yuv.V);
		bg8.V = (CANI_BYTE)(bg16.V >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.V = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->yuv.V);
		bg16.V = ((((CANI_WORD)bg8.V) << 8) | ((CANI_WORD)bg8.V));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	CANI_YUVA_2_RGBA(&Frame->bgColorRGB, &bg8, 1);

	CANI_YUVA16_2_RGBA16(&Frame->bgColorRGB16, &bg16, 1);

	//char szCaniBuffer[1024];

	//sprintf(szCaniBuffer, "0x%04x 0x%04x 0x%04x 0x%04x", bg16.C, bg16.M, bg16.Y, bg16.K );
	//MessageBoxA(0, szCaniBuffer, "CMYK", 0);

	//sprintf(szCaniBuffer, "0x%04x 0x%04x 0x%04x", Frame->bgColorRGB16.Red, Frame->bgColorRGB16.Green, Frame->bgColorRGB16.Blue);
	//MessageBoxA(0,szCaniBuffer, "", 0);

	return DestNow;
}



CANI_BYTE* CANI_ReadBackgroundColorYCbCr(CANI_CHAR Y16, CANI_CHAR Cb16, CANI_CHAR Cr16, CANI_BYTE* DestNow, CANI_BYTE* DestMax, CANI_FRAME_DATA* Frame, CANI_FORMAT_V2* settings) {

	CANI_YCbCrAQUAD bg8;
	CANI_YCbCrA16QUAD bg16;

	if (Y16) {
		bg16.Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->ycbcr.Y);
		bg8.Y = (CANI_BYTE)(bg16.Y >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.Y = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->ycbcr.Y);
		bg16.Y = ((((CANI_WORD)bg8.Y) << 8) | ((CANI_WORD)bg8.Y));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (Cb16) {
		bg16.Cb = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->ycbcr.Cb);
		bg8.Cb = (CANI_BYTE)(bg16.Cb >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.Cb = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->ycbcr.Cb);
		bg16.Cb = ((((CANI_WORD)bg8.Cb) << 8) | ((CANI_WORD)bg8.Cb));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	if (Cr16) {
		bg16.Cr = CAF_UnMask16(*((CANI_WORD*)DestNow), settings->ycbcr.Cr);
		bg8.Cr = (CANI_BYTE)(bg16.Cr >> 8);

		DestNow += 2;
		if (DestNow > DestMax) return DestNow;
	}
	else {
		bg8.Cr = CAF_UnMask(*((CANI_BYTE*)DestNow), settings->ycbcr.Cr);
		bg16.Cr = ((((CANI_WORD)bg8.Cr) << 8) | ((CANI_WORD)bg8.Cr));

		DestNow += 1;
		if (DestNow > DestMax) return DestNow;
	}

	CANI_YCbCrA_2_RGBA(&Frame->bgColorRGB, &bg8, 1);

	CANI_YCbCrA16_2_RGBA16(&Frame->bgColorRGB16, &bg16, 1);

	return DestNow;
}

CANI_BYTE* CANI_ReadAlpha(CANI_ANIMATION* animation, CANI_FORMAT_V2 *settings, CANI_DWORD n, CANI_CHAR Alpha16,
	CANI_BYTE* DestNow, CANI_BYTE* DestMax,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_BYTE* AlphaBefore8, CANI_WORD* AlphaBefore16,
	CANI_BYTE * alph4all, CANI_BYTE Version ) {

	CANI_DWORD m,b;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	static CANI_BYTE BitMask[8] = {
		0x01,
		0x02,
		0x04,
		0x08,
		0x10,
		0x20,
		0x40,
		0x80
	};

	if (settings->options & CANI_OPTION_ALPHA_DIFFERENTATION) {
		//MessageBoxA(0, "CANI_OPTION_ALPHA_DIFFERENTATION", "", 0);
		if (Alpha16) {
			for (m = 0; m < BitMapSize; m++) {
				Image16[m].Alpha = CAF_UnMask16(*((CANI_WORD*)DestNow) + AlphaBefore16[m], settings->rgb.Alpha);
				DestNow += 2;
				if (DestNow > DestMax) return DestNow;
			}
			for (m = 0; m < BitMapSize; m++) {
				AlphaBefore16[m] = CAF_Mask16(Image16[m].Alpha, settings->rgb.Alpha);
			}
		}
		else if (Version >= '4' && settings->rgb.Alpha == 1) {

			CANI_DWORD Size = BitMapSize / 8;
			if (BitMapSize % 8) Size++;
			for (b = 0; b < BitMapSize;) {
				CANI_BYTE byte = *DestNow + AlphaBefore16[b/8];
				AlphaBefore16[b / 8] = byte;
				for (m = 0; b < BitMapSize && m < 8; m++, b++) {
					//if ( Image8[n/8].Alpha & BitMask[] ) byte
					//if (Image8[b].Alpha & 0xF0) byte |= BitMask[m];
					if (byte & BitMask[m]) Image8[b].Alpha = 0xFF;
					else Image8[b].Alpha = 0x00;
				}
				DestNow++;
			}
			/*for (b = 0; b < BitMapSize;) {
				CANI_BYTE byte = 0;
				for (m = 0; b < BitMapSize && m < 8; m++, b++) {
					//if ( Image8[n/8].Alpha & BitMask[] ) byte
					if (Image8[b].Alpha & 0xF0) byte |= BitMask[m];
				}
				*SrcNow = byte - AlphaBefore16[b / 8];
				AlphaBefore16[b / 8] = byte;
				SrcNow++;
			}*/
		}
		else {
			for (m = 0; m < BitMapSize; m++) {
				Image8[m].Alpha = CAF_UnMask(*((CANI_BYTE*)DestNow) + AlphaBefore8[m], settings->rgb.Alpha);
				DestNow++;
				if (DestNow > DestMax) return DestNow;
			}
			for (m = 0; m < BitMapSize; m++) {
				AlphaBefore8[m] = CAF_Mask(Image8[m].Alpha, settings->rgb.Alpha);
			}
		}
	}
	else if (animation->Images[n].Style & CANI_STYLE_ALPHA) {
		//MessageBoxA(0, "CANI_STYLE_ALPHA", "", 0);
		if (Alpha16) {
			for (m = 0; m < BitMapSize; m++) {
				Image16[m].Alpha = *((CANI_WORD*)DestNow);
				DestNow += 2;
				if (DestNow > DestMax) return DestNow;
			}
		}
		else if ( Version >= '4' && settings->rgb.Alpha == 1) {

			CANI_DWORD Size = BitMapSize / 8;
			if (BitMapSize % 8) Size++;
			for (b = 0; b < BitMapSize;) {
				CANI_BYTE byte = *DestNow;
				for (m = 0; b < BitMapSize && m < 8; m++, b++) {
					//if ( Image8[n/8].Alpha & BitMask[] ) byte
					//if (Image8[b].Alpha & 0xF0) byte |= BitMask[m];
					if (byte & BitMask[m]) Image8[b].Alpha = 0xFF;
					else Image8[b].Alpha = 0x00;
				}
				DestNow++;
			}
		}
		else {
			for (m = 0; m < BitMapSize; m++) {
				Image8[m].Alpha = *DestNow;
				DestNow++;
				if (DestNow > DestMax) return DestNow;
			}
		}
	}
	else if (alph4all) {
		if (Alpha16) {
			//MessageBoxA(0,"alph4all","",0);
			if (settings->options & CANI_OPTION_16BITS_ALPHA) {
				for (m = 0; m < BitMapSize; m++) {
					Image16[m].Alpha = ((CANI_WORD*)alph4all)[m];
				}
			}
			else {
				for (m = 0; m < BitMapSize; m++) {
					CANI_WORD Alpha = ((CANI_BYTE*)alph4all)[m];
					Image16[m].Alpha = ((Alpha << 8) | Alpha);
				}
			}
		}
		else {
			if (settings->options & CANI_OPTION_16BITS_ALPHA) {
				for (m = 0; m < BitMapSize; m++) {
					CANI_WORD Alpha = ((CANI_WORD*)alph4all)[m];
					Image8[m].Alpha = (CANI_BYTE)(Alpha >> 8);
				}
			}
			else {
				for (m = 0; m < BitMapSize; m++) {
					Image8[m].Alpha = ((CANI_BYTE*)alph4all)[m];
				}
			}
		}
		//for (m = 0; m < BitMapSize; m++) {
		//	Image[m].Alpha = alph4all[m];
		//}
	}
	else {
		if (Alpha16) {
			for (m = 0; m < BitMapSize; m++) Image16[m].Alpha = 0xFFFF;
		}
		else {
			for (m = 0; m < BitMapSize; m++) Image8[m].Alpha = 0xFF;
		}
	}

	return DestNow;
}

CANI_CHAR CANI_DeConvertImage(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_WORD Which16 ) {

	CANI_DWORD m;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	if (Colors16) {
		CANI_BGRA16QUAD* Image = (CANI_BGRA16QUAD*)(animation->Images[n].Image) = (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			CANI_MEMCPY(Image, Image16, sizeof(CANI_BGRA16QUAD) * BitMapSize);

			if (!(Which16& CANI_IMAGE_RED16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Red = Image8[m].Red;
				Image[m].Red = ((Red << 8) | Red);
			}

			if (!(Which16 & CANI_IMAGE_GREEN16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Green = Image8[m].Green;
				Image[m].Green = ((Green << 8) | Green);
			}

			if (!(Which16 & CANI_IMAGE_BLUE16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Blue = Image8[m].Blue;
				Image[m].Blue = ((Blue << 8) | Blue);
			}

			if (!(Which16 & CANI_IMAGE_ALPHA16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Alpha = Image8[m].Alpha;
				Image[m].Alpha = ((Alpha << 8) | Alpha);
			}

			animation->Images[n].Style |= CANI_STYLE_16BITS_IMAGE;

		}
		else {
			return FALSE;
		}
	}
	else {
		CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			CANI_MEMCPY(Image, Image8, sizeof(CANI_BGRAQUAD) * BitMapSize);

			animation->Images[n].Style &= (~(CANI_STYLE_16BITS_IMAGE));
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}



CANI_CHAR CANI_DeConvertImageGray(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_WORD Which16) {

	CANI_DWORD m;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	if (Colors16) {
		CANI_BGRA16QUAD* Image = (CANI_BGRA16QUAD*)(animation->Images[n].Image) = (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			CANI_MEMCPY(Image, Image16, sizeof(CANI_BGRA16QUAD) * BitMapSize);

			if (!(Which16 & CANI_IMAGE_GRAY16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Red,Green,Blue;

				Red = Image8[m].Red;
				Image[m].Red = ((Red << 8) | Red);
				Green = Image8[m].Green;
				Image[m].Green = ((Green << 8) | Green);
				Blue = Image8[m].Blue;
				Image[m].Blue = ((Blue << 8) | Blue);
			}

			if (!(Which16 & CANI_IMAGE_ALPHA16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Alpha = Image8[m].Alpha;
				Image[m].Alpha = ((Alpha << 8) | Alpha);
			}

			animation->Images[n].Style |= CANI_STYLE_16BITS_IMAGE;

		}
		else {
			return FALSE;
		}
	}
	else {
		CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			CANI_MEMCPY(Image, Image8, sizeof(CANI_BGRAQUAD) * BitMapSize);

			animation->Images[n].Style &= (~(CANI_STYLE_16BITS_IMAGE));
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}

CANI_CHAR CANI_DeConvertImageRGB16(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_WORD Which16) {

	CANI_DWORD m;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	if (Colors16) {
		CANI_BGRA16QUAD* Image = (CANI_BGRA16QUAD*)(animation->Images[n].Image) = (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			CANI_MEMCPY(Image, Image16, sizeof(CANI_BGRA16QUAD) * BitMapSize);

			/*if (!(Which16 & CANI_IMAGE_GRAY16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Red = Image8[m].Red;
				Image[m].Red = ((Red << 8) | Red);
				CANI_WORD Green = Image8[m].Green;
				Image[m].Green = ((Green << 8) | Green);
				CANI_WORD Blue = Image8[m].Blue;
				Image[m].Blue = ((Blue << 8) | Blue);
			}*/

			if (!(Which16 & CANI_IMAGE_ALPHA16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Alpha = Image8[m].Alpha;
				Image[m].Alpha = ((Alpha << 8) | Alpha);
			}

			animation->Images[n].Style |= CANI_STYLE_16BITS_IMAGE;

		}
		else {
			return FALSE;
		}
	}
	else {
		CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			CANI_MEMCPY(Image, Image8, sizeof(CANI_BGRAQUAD) * BitMapSize);

			if ((Which16 & CANI_IMAGE_RGB16)) for (m = 0; m < BitMapSize; m++) {
				Image[m].Red = (Image16[m].Red >> 8);
				Image[m].Green = (Image16[m].Green >> 8);
				Image[m].Blue = (Image16[m].Blue >> 8);
			}

			animation->Images[n].Style &= (~(CANI_STYLE_16BITS_IMAGE));
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}



CANI_CHAR CANI_DeConvertImageCMYK(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_CMYKAQUAD* ImageCMYK8, CANI_CMYKA16QUAD* ImageCMYK16, CANI_WORD Which16) {

	CANI_DWORD m;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {

		//MessageBoxA(0,"CANI_STYLE_16BITS_IMAGE",0,0);

		CANI_BGRA16QUAD* Image = (CANI_BGRA16QUAD*)(animation->Images[n].Image) = (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {

			//CANI_MEMSET(Image, 0xFF, sizeof(CANI_BGRA16QUAD) * BitMapSize);

			if (!(Which16 & CANI_IMAGE_CYAN16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Cyan = ImageCMYK8[m].C;
				ImageCMYK16[m].C = ((Cyan << 8) | Cyan);
			}

			if (!(Which16 & CANI_IMAGE_MAGENTA16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Magenta = ImageCMYK8[m].M;
				ImageCMYK16[m].M = ((Magenta << 8) | Magenta);
			}

			if (!(Which16 & CANI_IMAGE_YELLOW16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageCMYK8[m].Y;
				ImageCMYK16[m].Y = ((Yellow << 8) | Yellow);
			}

			if (!(Which16 & CANI_IMAGE_KEYCOLOUR16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD KeyColour = ImageCMYK8[m].K;
				ImageCMYK16[m].K = ((KeyColour << 8) | KeyColour);
			}

			CANI_CMYKA16_2_BGRA16(Image, ImageCMYK16, BitMapSize);

			if ((Which16 & CANI_IMAGE_ALPHA16)) {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = Image16[m].Alpha;
			}
			else {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = ( ((CANI_WORD)Image8[m].Alpha) << 8 ) | ((CANI_WORD)Image8[m].Alpha);
			}

			return TRUE;
		}
		else return FALSE;
	}
	else {
		CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			//CANI_MEMSET(Image, 0xFF, sizeof(CANI_BGRAQUAD) * BitMapSize);

			if ((Which16 & CANI_IMAGE_CYAN16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Cyan = ImageCMYK16[m].C;
				ImageCMYK8[m].C = (CANI_BYTE)(Cyan >> 8);
			}

			if ((Which16 & CANI_IMAGE_MAGENTA16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Magenta = ImageCMYK16[m].M;
				ImageCMYK8[m].M = (CANI_BYTE)(Magenta >> 8);
			}

			if ((Which16 & CANI_IMAGE_YELLOW16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageCMYK16[m].Y;
				ImageCMYK8[m].Y = (CANI_BYTE)(Yellow >> 8);
			}

			if ((Which16 & CANI_IMAGE_KEYCOLOUR16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD KeyColour = ImageCMYK16[m].K;
				ImageCMYK8[m].K = (CANI_BYTE)(KeyColour>>8);
			}

			CANI_CMYKA_2_BGRA(Image, ImageCMYK8, BitMapSize);

			if ((Which16 & CANI_IMAGE_ALPHA16)) {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = ( Image16[m].Alpha >> 8 ) ;
			}
			else {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = Image8[m].Alpha;
			}

			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}



CANI_CHAR CANI_DeConvertImageYUV(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_YUVAQUAD* ImageYUV8, CANI_YUVA16QUAD* ImageYUV16, CANI_WORD Which16) {

	CANI_DWORD m;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {

		//MessageBoxA(0,"CANI_STYLE_16BITS_IMAGE",0,0);

		CANI_BGRA16QUAD* Image = (CANI_BGRA16QUAD*)(animation->Images[n].Image) = (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {

			if (!(Which16 & CANI_IMAGE_YUV_Y16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYUV8[m].Y;
				ImageYUV16[m].Y = ((Yellow << 8) | Yellow);
			}

			if (!(Which16 & CANI_IMAGE_YUV_U16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYUV8[m].U;
				ImageYUV16[m].U = ((Yellow << 8) | Yellow);
			}

			if (!(Which16 & CANI_IMAGE_YUV_V16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYUV8[m].V;
				ImageYUV16[m].V = ((Yellow << 8) | Yellow);
			}

			CANI_YUVA16_2_BGRA16(Image, ImageYUV16, BitMapSize);

			if ((Which16 & CANI_IMAGE_ALPHA16)) {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = Image16[m].Alpha;
			}
			else {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = (((CANI_WORD)Image8[m].Alpha) << 8) | ((CANI_WORD)Image8[m].Alpha);
			}

			return TRUE;
		}
		else return FALSE;
	}
	else {
		CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			//CANI_MEMSET(Image, 0xFF, sizeof(CANI_BGRAQUAD) * BitMapSize);

			if ((Which16 & CANI_IMAGE_YUV_Y16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYUV16[m].Y;
				ImageYUV8[m].Y = (CANI_BYTE)(Yellow >> 8);
			}

			if ((Which16 & CANI_IMAGE_YUV_U16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYUV16[m].U;
				ImageYUV8[m].U = (CANI_BYTE)(Yellow >> 8);
			}

			if ((Which16 & CANI_IMAGE_YUV_V16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYUV16[m].V;
				ImageYUV8[m].V = (CANI_BYTE)(Yellow >> 8);
			}

			CANI_YUVA_2_BGRA(Image, ImageYUV8, BitMapSize);

			if ((Which16 & CANI_IMAGE_ALPHA16)) {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = (Image16[m].Alpha >> 8);
			}
			else {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = Image8[m].Alpha;
			}

			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}

CANI_CHAR CANI_DeConvertImageYCbCr(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Colors16,
	CANI_BGRAQUAD* Image8, CANI_BGRA16QUAD* Image16, CANI_YCbCrAQUAD* ImageYCbCr8, CANI_YCbCrA16QUAD* ImageYCbCr16, CANI_WORD Which16) {

	CANI_DWORD m;

	CANI_DWORD BitMapSize = animation->Width * animation->Height;

	if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {

		//MessageBoxA(0,"CANI_STYLE_16BITS_IMAGE",0,0);

		CANI_BGRA16QUAD* Image = (CANI_BGRA16QUAD*)(animation->Images[n].Image) = (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {

			if (!(Which16 & CANI_IMAGE_YCbCr_Y16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYCbCr8[m].Y;
				ImageYCbCr16[m].Y = ((Yellow << 8) | Yellow);
			}

			if (!(Which16 & CANI_IMAGE_YCbCr_Cb16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYCbCr8[m].Cb;
				ImageYCbCr16[m].Cb = ((Yellow << 8) | Yellow);
			}

			if (!(Which16 & CANI_IMAGE_YCbCr_Cr16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYCbCr8[m].Cr;
				ImageYCbCr16[m].Cr = ((Yellow << 8) | Yellow);
			}

			CANI_YCbCrA16_2_BGRA16(Image, ImageYCbCr16, BitMapSize);

			if ((Which16 & CANI_IMAGE_ALPHA16)) {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = Image16[m].Alpha;
			}
			else {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = (((CANI_WORD)Image8[m].Alpha) << 8) | ((CANI_WORD)Image8[m].Alpha);
			}

			return TRUE;
		}
		else return FALSE;
	}
	else {
		CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
		if (Image) {
			//CANI_MEMSET(Image, 0xFF, sizeof(CANI_BGRAQUAD) * BitMapSize);

			if ((Which16 & CANI_IMAGE_YCbCr_Y16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYCbCr16[m].Y;
				ImageYCbCr8[m].Y = (CANI_BYTE)(Yellow >> 8);
			}

			if ((Which16 & CANI_IMAGE_YCbCr_Cb16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYCbCr16[m].Cb;
				ImageYCbCr8[m].Cb = (CANI_BYTE)(Yellow >> 8);
			}

			if ((Which16 & CANI_IMAGE_YCbCr_Cr16)) for (m = 0; m < BitMapSize; m++) {
				CANI_WORD Yellow = ImageYCbCr16[m].Cr;
				ImageYCbCr8[m].Cr = (CANI_BYTE)(Yellow >> 8);
			}

			CANI_YCbCrA_2_BGRA(Image, ImageYCbCr8, BitMapSize);

			if ((Which16 & CANI_IMAGE_ALPHA16)) {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = (Image16[m].Alpha >> 8);
			}
			else {
				for (m = 0; m < BitMapSize; m++) Image[m].Alpha = Image8[m].Alpha;
			}

			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	return TRUE;
}

CANI_DWORD CANI_CalculateAlphaSize(CANI_ANIMATION* animation, CANI_CHAR Alpha16 ) {

	CANI_DWORD BitMapSize = animation->Width * animation->Height;
	CANI_DWORD Size = 0;
	CANI_DWORD AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));
	CANI_DWORD n;

	if ((animation->Format.options & CANI_OPTION_ALPHA) && animation->gAlpha) {
		Size += ((BitMapSize * AlphaSize));
	}
	else animation->Format.options &= (~(CANI_OPTION_ALPHA));

	if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {
		Size += (animation->ImagesNR * (BitMapSize * AlphaSize));
	}
	else {

		for (n = 0; n < animation->ImagesNR; n++) {
			if (animation->Images[n].Style & CANI_STYLE_ALPHA) Size += (BitMapSize * AlphaSize);
		}
	}

	return Size;
}

CANI_DWORD CANI_CalculateGlobalAlphaSize(CANI_ANIMATION* animation, CANI_CHAR Alpha16) {

	CANI_DWORD BitMapSize = animation->Width * animation->Height;
	CANI_DWORD Size = 0;
	CANI_DWORD AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));
	//CANI_DWORD n;

	if ((animation->Format.options & CANI_OPTION_ALPHA) && animation->gAlpha) {
		Size += ((BitMapSize * AlphaSize));
	}
	else animation->Format.options &= (~(CANI_OPTION_ALPHA));

	return Size;
}


CANI_DWORD CANI_CalculateLocalAlphaSize(CANI_ANIMATION* animation, CANI_DWORD n, CANI_CHAR Alpha16) {

	CANI_DWORD BitMapSize = animation->Width * animation->Height;
	CANI_DWORD Size = 0;
	CANI_DWORD AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

	if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {
		Size += (/*animation->ImagesNR * */(BitMapSize * AlphaSize));
	}
	else {
		if (animation->Images[n].Style & CANI_STYLE_ALPHA) Size += (BitMapSize * AlphaSize);
	}

	return Size;
}


CANI_BYTE* CANI_WriteGlobalAlpha(CANI_ANIMATION* animation, CANI_CHAR Alpha16, CANI_BYTE* SrcNow ) {

	if ((animation->Format.options & CANI_OPTION_ALPHA) && animation->gAlpha) {

		CANI_DWORD BitMapSize = animation->Width * animation->Height;
		CANI_DWORD Size = 0;
		CANI_DWORD AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));
		CANI_DWORD m,b;

		static CANI_BYTE BitMask[8] = {
			0x01,
			0x02,
			0x04,
			0x08,
			0x10,
			0x20,
			0x40,
			0x80
		};

		if (Alpha16) {
			if (animation->Format.options & CANI_OPTION_16BITS_ALPHA)
				CANI_MEMCPY(SrcNow, animation->gAlpha, AlphaSize * BitMapSize);
			else CANI_ALPHA_2_ALPHA16((CANI_WORD*)SrcNow, (CANI_BYTE*)animation->gAlpha, BitMapSize);

			for (m = 0; m < BitMapSize; m++) {
				((CANI_WORD*)SrcNow)[m] = CAF_Mask16(((CANI_WORD*)SrcNow)[m], animation->Format.rgb.Alpha);
			}

			SrcNow += AlphaSize * BitMapSize;
		}
		else if (animation->Format.rgb.Alpha == 1) {

			CANI_DWORD Size;
			CANI_BYTE* alpha = malloc(BitMapSize);

			if (animation->Format.options & CANI_OPTION_16BITS_ALPHA)
				CANI_ALPHA16_2_ALPHA((CANI_BYTE*)alpha, (CANI_WORD*)animation->gAlpha, BitMapSize);
			else CANI_MEMCPY(alpha, animation->gAlpha, AlphaSize * BitMapSize);

			//CANI_BYTE* SrcNowStart = SrcNow;

			Size = BitMapSize / 8;
			if (BitMapSize % 8) Size++;


			for (b = 0; b < BitMapSize;) {
				CANI_BYTE byte = 0;
				for (m = 0; b < BitMapSize && m < 8; m++, b++) {
					//if ( Image8[n/8].Alpha & BitMask[] ) byte
					if (alpha[b] & 0xF0) byte |= BitMask[m];
				}
				*SrcNow = byte;
				SrcNow++;
			}

			free(alpha);
		}
		else {
			if (animation->Format.options & CANI_OPTION_16BITS_ALPHA)
				CANI_ALPHA16_2_ALPHA((CANI_BYTE*)SrcNow, (CANI_WORD*)animation->gAlpha, BitMapSize);
			else CANI_MEMCPY(SrcNow, animation->gAlpha, AlphaSize * BitMapSize);

			for (m = 0; m < BitMapSize; m++) {
				((CANI_BYTE*)SrcNow)[m] = CAF_Mask(((CANI_BYTE*)SrcNow)[m], animation->Format.rgb.Alpha);
			}

			SrcNow += AlphaSize * BitMapSize;
		}
	}

	return SrcNow;
}

CANI_CHAR CANI_CompressAndWrite( FILE * File, CANI_BYTE *Beg, CANI_BYTE *End ) {

	CANI_DWORD SrcLen = (CANI_DWORD)(End - Beg);

	CANI_DWORD DestLen = 2 * SrcLen + 100;
	CANI_BYTE* Dest = CANI_MALLOC(DestLen * sizeof(CANI_BYTE));//new CANI_BYTE[DestLen2];

	if (Dest == 0) {
		CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
		return FALSE;
	}

	compress2(Dest, &DestLen, Beg, SrcLen, 9);

	CANI_FWRITE(&DestLen, 1, sizeof(DestLen), File);

	CANI_FWRITE(&SrcLen, 1, sizeof(SrcLen), File);

	CANI_FWRITE(Dest, 1, DestLen, File);

	CANI_FREE(Dest);

	return TRUE;
}

CANI_CHAR CANI_WriteZeroData( FILE* File ) {

	CANI_DWORD Size = 0;

	CANI_FWRITE(&Size, 1, sizeof(Size), File);

	CANI_FWRITE(&Size, 1, sizeof(Size), File);

	return TRUE;
}

CANI_BYTE* CANI_ReadAndDeCompress(FILE* File, CANI_DWORD* size) {

	CANI_DWORD SrcLen, DestLen;

	*size = 0;

	CANI_FREAD(&SrcLen, 1, sizeof(CANI_DWORD), File);

	CANI_FREAD(&DestLen, 1, sizeof(CANI_DWORD), File);

	if (SrcLen && DestLen) {

		CANI_BYTE* Src;
		CANI_BYTE* Dest;

		Src = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * SrcLen);//new CANI_BYTE[ SrcLen ];

		if (!Src) {
			return 0;
		}

		if (CANI_FREAD(Src, 1, SrcLen, File) < SrcLen) {
			CANI_FREE(Src);//delete [] Src;
			return 0;
		}

		Dest = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * DestLen + 8);//new CANI_BYTE[ DestLen ];

		if (!Dest) {
			CANI_FREE(Src);//delete [] Src;
			return 0;
		}

		uncompress(Dest, &DestLen, Src, SrcLen);

		CANI_FREE(Src);//delete [] Src;

		if (DestLen > 0) {
			*size = DestLen;
			return Dest;
		}

		CANI_FREE(Dest);//delete [] Src;
	}

	return 0;

}