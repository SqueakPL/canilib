#include "cani.h"

void CANI_BGRA_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRAQUAD* SrcNow = src;
	CANI_BGRA16QUAD* DestNow = dest;
	CANI_BGRA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = ((((CANI_WORD)SrcNow->Red) << 8) | ((CANI_WORD)SrcNow->Red));
		DestNow->Green = ((((CANI_WORD)SrcNow->Green) << 8) | ((CANI_WORD)SrcNow->Green));
		DestNow->Blue = ((((CANI_WORD)SrcNow->Blue) << 8) | ((CANI_WORD)SrcNow->Blue));
		DestNow->Alpha = ((((CANI_WORD)SrcNow->Alpha) << 8) | ((CANI_WORD)SrcNow->Alpha));
		SrcNow++;
		DestNow++;
	}
}

void CANI_BGRA16_2_BGRA(CANI_BGRAQUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRA16QUAD* SrcNow = src;
	CANI_BGRAQUAD* DestNow = dest;
	CANI_BGRAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = (CANI_BYTE)(SrcNow->Red >> 8);
		DestNow->Green = (CANI_BYTE)(SrcNow->Green >> 8);
		DestNow->Blue = (CANI_BYTE)(SrcNow->Blue >> 8);
		DestNow->Alpha = (CANI_BYTE)(SrcNow->Alpha >> 8);
		SrcNow++;
		DestNow++;
	}

}

void CANI_RGBA_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBAQUAD* SrcNow = src;
	CANI_RGBA16QUAD* DestNow = dest;
	CANI_RGBA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = ((((CANI_WORD)SrcNow->Red) << 8) | ((CANI_WORD)SrcNow->Red));
		DestNow->Green = ((((CANI_WORD)SrcNow->Green) << 8) | ((CANI_WORD)SrcNow->Green));
		DestNow->Blue = ((((CANI_WORD)SrcNow->Blue) << 8) | ((CANI_WORD)SrcNow->Blue));
		DestNow->Alpha = ((((CANI_WORD)SrcNow->Alpha) << 8) | ((CANI_WORD)SrcNow->Alpha));
		SrcNow++;
		DestNow++;
	}
}

void CANI_RGBA16_2_RGBA(CANI_RGBAQUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBA16QUAD* SrcNow = src;
	CANI_RGBAQUAD* DestNow = dest;
	CANI_RGBAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = (CANI_BYTE)(SrcNow->Red >> 8);
		DestNow->Green = (CANI_BYTE)(SrcNow->Green >> 8);
		DestNow->Blue = (CANI_BYTE)(SrcNow->Blue >> 8);
		DestNow->Alpha = (CANI_BYTE)(SrcNow->Alpha >> 8);
		SrcNow++;
		DestNow++;
	}

}

void CANI_ALPHA_2_ALPHA16(CANI_WORD* dest, CANI_BYTE* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BYTE* SrcNow = src;
	CANI_WORD* DestNow = dest;
	CANI_WORD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		*DestNow = ((((CANI_WORD)*SrcNow) << 8) | ((CANI_WORD)*SrcNow));
		SrcNow++;
		DestNow++;
	}
}


void CANI_ALPHA16_2_ALPHA(CANI_BYTE* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_WORD* SrcNow = src;
	CANI_BYTE* DestNow = dest;
	CANI_BYTE* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		*DestNow = (CANI_BYTE)(*SrcNow >> 8);
		SrcNow++;
		DestNow++;
	}

}

#define CLIP(X) ( (X) > 0xFF ? 0xFF : (X) < 0 ? 0 : X)
#define CLIP16(X) ( (X) > 0xFFFF ? 0xFFFF : (X) < 0 ? 0 : X)

#define RGB2G(R, G, B) CLIP( ( 17216 * R + 44433 * G + 3886 * B ) >> 16 )
#define RGB2G16(R, G, B) CLIP16( ( 17216 * R + 44433 * G + 3886 * B ) >> 16 )

void CANI_BGRA16_2_GRAY16(CANI_WORD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRA16QUAD* SrcNow = src;
	CANI_WORD* DestNow = dest;
	CANI_WORD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		// HDR standard conversion


		/*CANI_CONVERT_VARIABLE value = (((CANI_CONVERT_VARIABLE)2627) * ((CANI_CONVERT_VARIABLE)(SrcNow->Red)) / ((CANI_CONVERT_VARIABLE)10000));
		value += (((CANI_CONVERT_VARIABLE)678) * ((CANI_CONVERT_VARIABLE)(SrcNow->Green)) / ((CANI_CONVERT_VARIABLE)1000));
		value += (((CANI_CONVERT_VARIABLE)593) * ((CANI_CONVERT_VARIABLE)(SrcNow->Blue)) / ((CANI_CONVERT_VARIABLE)10000));
		if (value > 0xFFFF) value = 0xFFFF;*/

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		*DestNow = (CANI_WORD)RGB2G16( Red, Green, Blue );
		SrcNow++;
		DestNow++;
	}
}

void CANI_BGRA16_2_GRAY(CANI_BYTE* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRA16QUAD* SrcNow = src;
	CANI_BYTE* DestNow = dest;
	CANI_BYTE* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		*DestNow = (CANI_BYTE)((RGB2G16(Red, Green, Blue))>>8);
		SrcNow++;
		DestNow++;
	}
}

void CANI_BGRA_2_GRAY(CANI_BYTE* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRAQUAD* SrcNow = src;
	CANI_BYTE* DestNow = dest;
	CANI_BYTE* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		*DestNow = (CANI_BYTE)RGB2G(Red, Green, Blue);
		SrcNow++;
		DestNow++;
	}
}

void CANI_GRAY16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_WORD* SrcNow = src;
	CANI_BGRA16QUAD* DestNow = dest;
	CANI_BGRA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = *SrcNow;
		DestNow->Green = *SrcNow;
		DestNow->Blue = *SrcNow;
		SrcNow++;
		DestNow++;
	}
}

void CANI_GRAY_2_BGRA(CANI_BGRAQUAD* dest, CANI_BYTE* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BYTE* SrcNow = src;
	CANI_BGRAQUAD* DestNow = dest;
	CANI_BGRAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = *SrcNow;
		DestNow->Green = *SrcNow;
		DestNow->Blue = *SrcNow;
		SrcNow++;
		DestNow++;
	}
}


void CANI_RGBA16_2_GRAY16(CANI_WORD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBA16QUAD* SrcNow = src;
	CANI_WORD* DestNow = dest;
	CANI_WORD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		*DestNow = (CANI_WORD)RGB2G16(Red, Green, Blue);
		SrcNow++;
		DestNow++;
	}
}

void CANI_RGBA_2_GRAY(CANI_BYTE* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBAQUAD* SrcNow = src;
	CANI_BYTE* DestNow = dest;
	CANI_BYTE* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		*DestNow = (CANI_BYTE)RGB2G(Red, Green, Blue);
		SrcNow++;
		DestNow++;
	}
}

void CANI_GRAY16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_WORD* SrcNow = src;
	CANI_RGBA16QUAD* DestNow = dest;
	CANI_RGBA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = *SrcNow;
		DestNow->Green = *SrcNow;
		DestNow->Blue = *SrcNow;
		SrcNow++;
		DestNow++;
	}
}

void CANI_GRAY_2_RGBA(CANI_RGBAQUAD* dest, CANI_BYTE* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BYTE* SrcNow = src;
	CANI_RGBAQUAD* DestNow = dest;
	CANI_RGBAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		DestNow->Red = *SrcNow;
		DestNow->Green = *SrcNow;
		DestNow->Blue = *SrcNow;
		SrcNow++;
		DestNow++;
	}
}

void CANI_BGRA16_2_RGB16(CANI_WORD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRA16QUAD* SrcNow = src;
	CANI_WORD* DestNow = dest;
	CANI_WORD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		
		CANI_WORD ColorNow = ((((CANI_WORD)SrcNow->Red) >> (3+8) ) & 0x1F);
		ColorNow |= (((((CANI_WORD)SrcNow->Green) >> (2 + 8)) & 0x3F) << 5);
		ColorNow |= (((((CANI_WORD)SrcNow->Blue) >> (3 + 8)) & 0x1F) << 11);

		*DestNow = (CANI_WORD)ColorNow;
		SrcNow++;
		DestNow++;
	}
}

void CANI_RGBA16_2_RGB16(CANI_WORD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBA16QUAD* SrcNow = src;
	CANI_WORD* DestNow = dest;
	CANI_WORD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_WORD ColorNow = ((((CANI_WORD)SrcNow->Red) >> (3+8) ) & 0x1F);
		ColorNow |= (((((CANI_WORD)SrcNow->Green) >> (2 + 8)) & 0x3F) << 5);
		ColorNow |= (((((CANI_WORD)SrcNow->Blue) >> (3 + 8)) & 0x1F) << 11);

		*DestNow = (CANI_WORD)ColorNow;
		SrcNow++;
		DestNow++;
	}
}

void CANI_RGB16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_WORD* SrcNow = src;
	CANI_BGRA16QUAD* DestNow = dest;
	CANI_BGRA16QUAD* DestMax = &dest[PixelsSize];
	//MessageBox(0,L"CANI_RGB16_2_BGRA16",L"",0);

	while (DestNow < DestMax) {

		/*CANI_WORD ColorNow = ((((CANI_WORD)SrcNow->Red) >> 3) & 0x1F);
		ColorNow |= (((((CANI_WORD)SrcNow->Green) >> 2) & 0x3F) << 5);
		ColorNow |= (((((CANI_WORD)SrcNow->Blue) >> 3) & 0x1F) << 11);*/

		CANI_WORD ColorNow = *SrcNow;

		/*CANI_WORD Red = ((ColorNow & 0x001F) << 3);
		CANI_WORD Green = (((ColorNow >> 5) & 0x003F) << 2);
		CANI_WORD Blue = (((ColorNow >> 11) & 0x001F) << 3);*/

		/*CANI_WORD Red = CAF_UnMask16(((ColorNow & 0x001F) << 3), 5);
		CANI_WORD Green = CAF_UnMask16((((ColorNow >> 5) & 0x003F) << 2), 6);
		CANI_WORD Blue = CAF_UnMask16((((ColorNow >> 11) & 0x001F) << 3), 5);*/

		/*CANI_WORD Red = CAF_UnMask16(((ColorNow & 0x001F) << ( 3 + 8 ) ), 5);
		CANI_WORD Green = CAF_UnMask16((((ColorNow >> 5) & 0x003F) << ( 2 + 8 ) ), 6);
		CANI_WORD Blue = CAF_UnMask16((((ColorNow >> 11) & 0x001F) << ( 3 + 8 )), 5);*/

		CANI_WORD Red = CAF_UnMask(((ColorNow & 0x001F) << 3), 5);
		CANI_WORD Green = CAF_UnMask((((ColorNow >> 5) & 0x003F) << 2), 6);
		CANI_WORD Blue = CAF_UnMask((((ColorNow >> 11) & 0x001F) << 3), 5);

		DestNow->Red = ( ( Red << 8) | Red );
		DestNow->Green = ((Green << 8) | Green);
		DestNow->Blue = ((Blue << 8) | Blue);

		//*DestNow = (CANI_WORD)ColorNow;
		SrcNow++;
		DestNow++;
	}
}

void CANI_RGB16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_WORD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_WORD* SrcNow = src;
	CANI_RGBA16QUAD* DestNow = dest;
	CANI_RGBA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		/*CANI_WORD ColorNow = ((((CANI_WORD)SrcNow->Red) >> 3) & 0x1F);
		ColorNow |= (((((CANI_WORD)SrcNow->Green) >> 2) & 0x3F) << 5);
		ColorNow |= (((((CANI_WORD)SrcNow->Blue) >> 3) & 0x1F) << 11);*/

		/*Image[m].Red = CAF_UnMask(((ColorNow & 0x001F) << 3), 5);
		Image[m].Green = CAF_UnMask((((ColorNow >> 5) & 0x003F) << 2), 6);
		Image[m].Blue = CAF_UnMask((((ColorNow >> 11) & 0x001F) << 3), 5);*/

		CANI_WORD ColorNow = *SrcNow;

		CANI_WORD Red = CAF_UnMask(((ColorNow & 0x001F) << 3), 5);
		CANI_WORD Green = CAF_UnMask((((ColorNow >> 5) & 0x003F) << 2), 6);
		CANI_WORD Blue = CAF_UnMask((((ColorNow >> 11) & 0x001F) << 3), 5);

		DestNow->Red = ((Red << 8) | Red);
		DestNow->Green = ((Green << 8) | Green);
		DestNow->Blue = ((Blue << 8) | Blue);

		//*DestNow = (CANI_WORD)ColorNow;
		SrcNow++;
		DestNow++;
	}
}



void CANI_RGBA16_2_CMYKA16(CANI_CMYKA16QUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBA16QUAD* SrcNow = src;
	CANI_CMYKA16QUAD* DestNow = dest;
	CANI_CMYKA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {
		
		CANI_CONVERT_VARIABLE Cyan, Magenta, Yellow, KeyColour,MaxColour;

		MaxColour = SrcNow->Red;
		if (SrcNow->Green > MaxColour) MaxColour = SrcNow->Green;
		if (SrcNow->Blue > MaxColour) MaxColour = SrcNow->Blue;

		KeyColour = ((CANI_CONVERT_VARIABLE)0xFFFF) - MaxColour;

		if (KeyColour>= ((CANI_CONVERT_VARIABLE)0xFFFF)) {

			Cyan = ((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red);
			Magenta = ((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green);
			Yellow = ((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue);
		}
		else {

			Cyan = (((CANI_CONVERT_VARIABLE)0xFFFF) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red) - KeyColour)) / ( ((CANI_CONVERT_VARIABLE)0xFFFF) - KeyColour );
			Magenta = (((CANI_CONVERT_VARIABLE)0xFFFF) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFFFF) - KeyColour );
			Yellow = (((CANI_CONVERT_VARIABLE)0xFFFF) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFFFF) - KeyColour );
		}

		/*if (Cyan < 0) Cyan = 0;
		else if (Cyan > ((CANI_CONVERT_VARIABLE)0xFFFF)) Cyan = ((CANI_CONVERT_VARIABLE)0xFFFF);

		if (Magenta < 0) Magenta = 0;
		else if (Magenta > ((CANI_CONVERT_VARIABLE)0xFFFF)) Magenta = ((CANI_CONVERT_VARIABLE)0xFFFF);

		if (Yellow < 0) Yellow = 0;
		else if (Yellow > ((CANI_CONVERT_VARIABLE)0xFFFF)) Yellow = ((CANI_CONVERT_VARIABLE)0xFFFF);*/

		DestNow->C = (CANI_WORD)Cyan;
		DestNow->M = (CANI_WORD)Magenta;
		DestNow->Y = (CANI_WORD)Yellow;
		DestNow->K = (CANI_WORD)KeyColour;
		//DestNow->Alpha = SrcNow->Alpha;

		SrcNow++;
		DestNow++;
	}
}

void CANI_RGBA_2_CMYKA(CANI_CMYKAQUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBAQUAD* SrcNow = src;
	CANI_CMYKAQUAD* DestNow = dest;
	CANI_CMYKAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Cyan, Magenta, Yellow, KeyColour, MaxColour;

		MaxColour = SrcNow->Red;
		if (SrcNow->Green > MaxColour) MaxColour = SrcNow->Green;
		if (SrcNow->Blue > MaxColour) MaxColour = SrcNow->Blue;

		KeyColour = ((CANI_CONVERT_VARIABLE)0xFF) - MaxColour;

		if (KeyColour >= ((CANI_CONVERT_VARIABLE)0xFF)) {

			Cyan = ((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red);
			Magenta = ((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green);
			Yellow = ((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue);
		}
		else {

			Cyan = (((CANI_CONVERT_VARIABLE)0xFF) * (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFF) - KeyColour);
			Magenta = (((CANI_CONVERT_VARIABLE)0xFF) * (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFF) - KeyColour);
			Yellow = (((CANI_CONVERT_VARIABLE)0xFF) * (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFF) - KeyColour);
		}

		/*if (Cyan < 0) Cyan = 0;
		else if (Cyan > ((CANI_CONVERT_VARIABLE)0xFF)) Cyan = ((CANI_CONVERT_VARIABLE)0xFF);

		if (Magenta < 0) Magenta = 0;
		else if (Magenta > ((CANI_CONVERT_VARIABLE)0xFF)) Magenta = ((CANI_CONVERT_VARIABLE)0xFF);

		if (Yellow < 0) Yellow = 0;
		else if (Yellow > ((CANI_CONVERT_VARIABLE)0xFF)) Yellow = ((CANI_CONVERT_VARIABLE)0xFF);*/

		DestNow->C = (CANI_BYTE)Cyan;
		DestNow->M = (CANI_BYTE)Magenta;
		DestNow->Y = (CANI_BYTE)Yellow;
		DestNow->K = (CANI_BYTE)KeyColour;
		//DestNow->Alpha = SrcNow->Alpha;

		SrcNow++;
		DestNow++;
	}
}




void CANI_BGRA16_2_CMYKA16(CANI_CMYKA16QUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRA16QUAD* SrcNow = src;
	CANI_CMYKA16QUAD* DestNow = dest;
	CANI_CMYKA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Cyan, Magenta, Yellow, KeyColour, MaxColour;

		MaxColour = SrcNow->Red;
		if (SrcNow->Green > MaxColour) MaxColour = SrcNow->Green;
		if (SrcNow->Blue > MaxColour) MaxColour = SrcNow->Blue;

		KeyColour = ((CANI_CONVERT_VARIABLE)0xFFFF) - MaxColour;

		if (KeyColour >= ((CANI_CONVERT_VARIABLE)0xFFFF)) {

			Cyan = ((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red);
			Magenta = ((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green);
			Yellow = ((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue);
		}
		else {

			Cyan = (((CANI_CONVERT_VARIABLE)0xFFFF) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFFFF) - KeyColour);
			Magenta = (((CANI_CONVERT_VARIABLE)0xFFFF) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFFFF) - KeyColour);
			Yellow = (((CANI_CONVERT_VARIABLE)0xFFFF) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFFFF) - KeyColour);
		}

		/*if (Cyan < 0) Cyan = 0;
		else if (Cyan > ((CANI_CONVERT_VARIABLE)0xFFFF)) Cyan = ((CANI_WORD)0xFFFF);

		if (Magenta < 0) Magenta = 0;
		else if (Magenta > ((CANI_CONVERT_VARIABLE)0xFFFF)) Magenta = ((CANI_WORD)0xFFFF);

		if (Yellow < 0) Yellow = 0;
		else if (Yellow > ((CANI_CONVERT_VARIABLE)0xFFFF)) Yellow = ((CANI_WORD)0xFFFF);*/

		DestNow->C = (CANI_WORD)Cyan;
		DestNow->M = (CANI_WORD)Magenta;
		DestNow->Y = (CANI_WORD)Yellow;
		DestNow->K = (CANI_WORD)KeyColour;
		//DestNow->Alpha = SrcNow->Alpha;

		SrcNow++;
		DestNow++;
	}
}

void CANI_BGRA_2_CMYKA(CANI_CMYKAQUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRAQUAD* SrcNow = src;
	CANI_CMYKAQUAD* DestNow = dest;
	CANI_CMYKAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Cyan, Magenta, Yellow, KeyColour, MaxColour;

		MaxColour = SrcNow->Red;
		if (SrcNow->Green > MaxColour) MaxColour = SrcNow->Green;
		if (SrcNow->Blue > MaxColour) MaxColour = SrcNow->Blue;

		KeyColour = ((CANI_CONVERT_VARIABLE)0xFF) - MaxColour;

		if (KeyColour >= ((CANI_CONVERT_VARIABLE)0xFF)) {

			Cyan = ((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red);
			Magenta = ((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green);
			Yellow = ((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue);
		}
		else {

			Cyan = (((CANI_CONVERT_VARIABLE)0xFF) * (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Red) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFF) - KeyColour);
			Magenta = (((CANI_CONVERT_VARIABLE)0xFF) * (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Green) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFF) - KeyColour);
			Yellow = (((CANI_CONVERT_VARIABLE)0xFF) * (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Blue) - KeyColour)) / (((CANI_CONVERT_VARIABLE)0xFF) - KeyColour);
		}

		/*if (Cyan < 0) Cyan = 0;
		else if (Cyan > ((CANI_CONVERT_VARIABLE)0xFF)) Cyan = ((CANI_WORD)0xFF);

		if (Magenta < 0) Magenta = 0;
		else if (Magenta > ((CANI_CONVERT_VARIABLE)0xFF)) Magenta = ((CANI_WORD)0xFF);

		if (Yellow < 0) Yellow = 0;
		else if (Yellow > ((CANI_CONVERT_VARIABLE)0xFF)) Yellow = ((CANI_WORD)0xFF);*/

		DestNow->C = (CANI_BYTE)Cyan;
		DestNow->M = (CANI_BYTE)Magenta;
		DestNow->Y = (CANI_BYTE)Yellow;
		DestNow->K = (CANI_BYTE)KeyColour;
		//DestNow->Alpha = SrcNow->Alpha;

		SrcNow++;
		DestNow++;
	}
}


void CANI_CMYKA16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_CMYKA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_CMYKA16QUAD* SrcNow = src;
	CANI_RGBA16QUAD* DestNow = dest;
	CANI_RGBA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		if (SrcNow->K == 0xFFFF) {

			CANI_CONVERT_VARIABLE Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->C));
			DestNow->Red = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->M));
			DestNow->Green = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Y));
			DestNow->Blue = (CANI_WORD)Value;
		}
		else {

			CANI_CONVERT_VARIABLE Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->C)) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / ((CANI_CONVERT_VARIABLE)0xFFFF);
			DestNow->Red = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->M)) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / ((CANI_CONVERT_VARIABLE)0xFFFF);
			DestNow->Green = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Y)) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / ((CANI_CONVERT_VARIABLE)0xFFFF);
			DestNow->Blue = (CANI_WORD)Value;

		}

		//DestNow->Alpha = SrcNow->Alpha;

		// R = 255 ×(1 - C) ×(1 - K)
		// G = 255 ×(1 - M) ×(1 - K)
		// B = 255 ×(1 - Y) ×(1 - K)

		SrcNow++;
		DestNow++;
	}
}


void CANI_CMYKA_2_RGBA(CANI_RGBAQUAD* dest, CANI_CMYKAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_CMYKAQUAD* SrcNow = src;
	CANI_RGBAQUAD* DestNow = dest;
	CANI_RGBAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		if (SrcNow->K == 0xFF) {

			CANI_CONVERT_VARIABLE Value = (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->C));
			DestNow->Red = (CANI_BYTE)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->M));
			DestNow->Green = (CANI_BYTE)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Y));
			DestNow->Blue = (CANI_BYTE)Value;
		}
		else {

			CANI_CONVERT_VARIABLE Value = (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->C)) * (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / 0xFF;
			DestNow->Red = (CANI_BYTE)Value;

			Value = (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->M)) * (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / 0xFF;
			DestNow->Green = (CANI_BYTE)Value;

			Value = (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->Y)) * (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / 0xFF;
			DestNow->Blue = (CANI_BYTE)Value;
		}

		//DestNow->Alpha = SrcNow->Alpha;

		// R = 255 ×(1 - C) ×(1 - K)
		// G = 255 ×(1 - M) ×(1 - K)
		// B = 255 ×(1 - Y) ×(1 - K)

		SrcNow++;
		DestNow++;
	}
}






void CANI_CMYKA16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_CMYKA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_CMYKA16QUAD* SrcNow = src;
	CANI_BGRA16QUAD* DestNow = dest;
	CANI_BGRA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		if (SrcNow->K == 0xFFFF) {

			CANI_CONVERT_VARIABLE Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->C));
			DestNow->Red = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->M));
			DestNow->Green = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Y));
			DestNow->Blue = (CANI_WORD)Value;
		}
		else {

			CANI_CONVERT_VARIABLE Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->C)) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / ((CANI_CONVERT_VARIABLE)0xFFFF);
			DestNow->Red = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->M)) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / ((CANI_CONVERT_VARIABLE)0xFFFF);
			DestNow->Green = (CANI_WORD)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Y)) * (((CANI_CONVERT_VARIABLE)0xFFFF) - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / ((CANI_CONVERT_VARIABLE)0xFFFF);
			DestNow->Blue = (CANI_WORD)Value;

		}

		//DestNow->Alpha = SrcNow->Alpha;

		// R = 255 ×(1 - C) ×(1 - K)
		// G = 255 ×(1 - M) ×(1 - K)
		// B = 255 ×(1 - Y) ×(1 - K)

		SrcNow++;
		DestNow++;
	}
}


void CANI_CMYKA_2_BGRA(CANI_BGRAQUAD* dest, CANI_CMYKAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_CMYKAQUAD* SrcNow = src;
	CANI_BGRAQUAD* DestNow = dest;
	CANI_BGRAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		if (SrcNow->K == 0xFF) {

			CANI_CONVERT_VARIABLE Value = (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->C));
			DestNow->Red = (CANI_BYTE)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->M));
			DestNow->Green = (CANI_BYTE)Value;

			Value = (((CANI_CONVERT_VARIABLE)0xFF) - ((CANI_CONVERT_VARIABLE)SrcNow->Y));
			DestNow->Blue = (CANI_BYTE)Value;
		}
		else {

			CANI_CONVERT_VARIABLE Value = (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->C)) * (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / 0xFF;
			DestNow->Red = (CANI_BYTE)Value;

			Value = (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->M)) * (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / 0xFF;
			DestNow->Green = (CANI_BYTE)Value;

			Value = (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->Y)) * (0xFF - ((CANI_CONVERT_VARIABLE)SrcNow->K)) / 0xFF;
			DestNow->Blue = (CANI_BYTE)Value;
		}

		//DestNow->Alpha = SrcNow->Alpha;

		// R = 255 ×(1 - C) ×(1 - K)
		// G = 255 ×(1 - M) ×(1 - K)
		// B = 255 ×(1 - Y) ×(1 - K)

		SrcNow++;
		DestNow++;
	}
}

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

#define RGB2Y16(R, G, B) CLIP16(( (  66 * (R) + 129 * (G) +  25 * (B) + (128<<8)) >> 8) +  (16<<8))
#define RGB2U16(R, G, B) CLIP16(( ( -38 * (R) -  74 * (G) + 112 * (B) + (128<<8)) >> 8) + (128<<8))
#define RGB2V16(R, G, B) CLIP16(( ( 112 * (R) -  94 * (G) -  18 * (B) + (128<<8)) >> 8) + (128<<8))

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define C16(Y) ( (Y) - (16<<8)  )
#define D16(U) ( (U) - (128<<8) )
#define E16(V) ( (V) - (128<<8) )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

#define YUV2R16(Y, U, V) CLIP16(( 298 * C16(Y)                + 409 * E16(V) + (128<<8)) >> 8)
#define YUV2G16(Y, U, V) CLIP16(( 298 * C16(Y) - 100 * D16(U) - 208 * E16(V) + (128<<8)) >> 8)
#define YUV2B16(Y, U, V) CLIP16(( 298 * C16(Y) + 516 * D16(U)                + (128<<8)) >> 8)


void CANI_RGBA16_2_YUVA16(CANI_YUVA16QUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBA16QUAD* SrcNow = src;
	CANI_YUVA16QUAD* DestNow = dest;
	CANI_YUVA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red,Green,Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_WORD)RGB2Y16(Red, Green, Blue);
		DestNow->U = (CANI_WORD)RGB2U16(Red, Green, Blue);
		DestNow->V = (CANI_WORD)RGB2V16(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}

void CANI_RGBA_2_YUVA(CANI_YUVAQUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBAQUAD* SrcNow = src;
	CANI_YUVAQUAD* DestNow = dest;
	CANI_YUVAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_BYTE)RGB2Y(Red, Green, Blue);
		DestNow->U = (CANI_BYTE)RGB2U(Red, Green, Blue);
		DestNow->V = (CANI_BYTE)RGB2V(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}




void CANI_BGRA16_2_YUVA16(CANI_YUVA16QUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRA16QUAD* SrcNow = src;
	CANI_YUVA16QUAD* DestNow = dest;
	CANI_YUVA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_WORD)RGB2Y16(Red, Green, Blue);
		DestNow->U = (CANI_WORD)RGB2U16(Red, Green, Blue);
		DestNow->V = (CANI_WORD)RGB2V16(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}

void CANI_BGRA_2_YUVA(CANI_YUVAQUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRAQUAD* SrcNow = src;
	CANI_YUVAQUAD* DestNow = dest;
	CANI_YUVAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_BYTE)RGB2Y(Red, Green, Blue);
		DestNow->U = (CANI_BYTE)RGB2U(Red, Green, Blue);
		DestNow->V = (CANI_BYTE)RGB2V(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}




void CANI_YUVA16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_YUVA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YUVA16QUAD* SrcNow = src;
	CANI_RGBA16QUAD* DestNow = dest;
	CANI_RGBA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y,U,V;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		U = ((CANI_CONVERT_VARIABLE)SrcNow->U);
		V = ((CANI_CONVERT_VARIABLE)SrcNow->V);

		DestNow->Red = (CANI_WORD)YUV2R16( Y, U, V );
		DestNow->Green = (CANI_WORD)YUV2G16(Y, U, V);
		DestNow->Blue = (CANI_WORD)YUV2B16(Y, U, V);

		SrcNow++;
		DestNow++;
	}
}

void CANI_YUVA_2_RGBA(CANI_RGBAQUAD* dest, CANI_YUVAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YUVAQUAD* SrcNow = src;
	CANI_RGBAQUAD* DestNow = dest;
	CANI_RGBAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y, U, V;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		U = ((CANI_CONVERT_VARIABLE)SrcNow->U);
		V = ((CANI_CONVERT_VARIABLE)SrcNow->V);

		DestNow->Red = (CANI_BYTE)YUV2R(Y, U, V);
		DestNow->Green = (CANI_BYTE)YUV2G(Y, U, V);
		DestNow->Blue = (CANI_BYTE)YUV2B(Y, U, V);

		SrcNow++;
		DestNow++;
	}
}

void CANI_YUVA16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_YUVA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YUVA16QUAD* SrcNow = src;
	CANI_BGRA16QUAD* DestNow = dest;
	CANI_BGRA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y, U, V;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		U = ((CANI_CONVERT_VARIABLE)SrcNow->U);
		V = ((CANI_CONVERT_VARIABLE)SrcNow->V);

		DestNow->Red = (CANI_WORD)YUV2R16(Y, U, V);
		DestNow->Green = (CANI_WORD)YUV2G16(Y, U, V);
		DestNow->Blue = (CANI_WORD)YUV2B16(Y, U, V);

		SrcNow++;
		DestNow++;
	}
}

void CANI_YUVA_2_BGRA(CANI_BGRAQUAD* dest, CANI_YUVAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YUVAQUAD* SrcNow = src;
	CANI_BGRAQUAD* DestNow = dest;
	CANI_BGRAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y, U, V;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		U = ((CANI_CONVERT_VARIABLE)SrcNow->U);
		V = ((CANI_CONVERT_VARIABLE)SrcNow->V);

		DestNow->Red = (CANI_BYTE)YUV2R(Y, U, V);
		DestNow->Green = (CANI_BYTE)YUV2G(Y, U, V);
		DestNow->Blue = (CANI_BYTE)YUV2B(Y, U, V);

		SrcNow++;
		DestNow++;
	}
}

// RGB -> YCbCr
#define CRGB2Y(R, G, B) CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16)
#define CRGB2Cb(R, G, B) CLIP((36962 * (B - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)
#define CRGB2Cr(R, G, B) CLIP((46727 * (R - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)

#define CRGB2Y16(R, G, B) CLIP16((19595 * R + 38470 * G + 7471 * B ) >> 16)
#define CRGB2Cb16(R, G, B) CLIP16((36962 * (B - CLIP16((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 32768)
#define CRGB2Cr16(R, G, B) CLIP16((46727 * (R - CLIP16((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 32768)





void CANI_RGBA16_2_YCbCrA16(CANI_YCbCrA16QUAD* dest, CANI_RGBA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBA16QUAD* SrcNow = src;
	CANI_YCbCrA16QUAD* DestNow = dest;
	CANI_YCbCrA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_WORD)CRGB2Y16(Red, Green, Blue);
		DestNow->Cb = (CANI_WORD)CRGB2Cb16(Red, Green, Blue);
		DestNow->Cr = (CANI_WORD)CRGB2Cr16(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}

void CANI_RGBA_2_YCbCrA(CANI_YCbCrAQUAD* dest, CANI_RGBAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_RGBAQUAD* SrcNow = src;
	CANI_YCbCrAQUAD* DestNow = dest;
	CANI_YCbCrAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_BYTE)CRGB2Y(Red, Green, Blue);
		DestNow->Cb = (CANI_BYTE)CRGB2Cb(Red, Green, Blue);
		DestNow->Cr = (CANI_BYTE)CRGB2Cr(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}




void CANI_BGRA16_2_YCbCrA16(CANI_YCbCrA16QUAD* dest, CANI_BGRA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRA16QUAD* SrcNow = src;
	CANI_YCbCrA16QUAD* DestNow = dest;
	CANI_YCbCrA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_WORD)CRGB2Y16(Red, Green, Blue);
		DestNow->Cb = (CANI_WORD)CRGB2Cb16(Red, Green, Blue);
		DestNow->Cr = (CANI_WORD)CRGB2Cr16(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}

void CANI_BGRA_2_YCbCrA(CANI_YCbCrAQUAD* dest, CANI_BGRAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_BGRAQUAD* SrcNow = src;
	CANI_YCbCrAQUAD* DestNow = dest;
	CANI_YCbCrAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Red, Green, Blue;

		Red = ((CANI_CONVERT_VARIABLE)SrcNow->Red);
		Green = ((CANI_CONVERT_VARIABLE)SrcNow->Green);
		Blue = ((CANI_CONVERT_VARIABLE)SrcNow->Blue);

		DestNow->Y = (CANI_BYTE)CRGB2Y(Red, Green, Blue);
		DestNow->Cb = (CANI_BYTE)CRGB2Cb(Red, Green, Blue);
		DestNow->Cr = (CANI_BYTE)CRGB2Cr(Red, Green, Blue);

		SrcNow++;
		DestNow++;
	}
}


// YCbCr -> RGB
#define CYCbCr2R(Y, Cb, Cr) CLIP( Y + ( 91881 * Cr >> 16 ) - 179 )
#define CYCbCr2G(Y, Cb, Cr) CLIP( Y - (( 22544 * Cb + 46793 * Cr ) >> 16) + 135)
#define CYCbCr2B(Y, Cb, Cr) CLIP( Y + (116129 * Cb >> 16 ) - 226 )

#define CYCbCr2R16(Y, Cb, Cr) CLIP16( Y + ( 91881 * Cr >> 16 ) - ( 179 << 8 ) )
#define CYCbCr2G16(Y, Cb, Cr) CLIP16( Y - (( 22544 * Cb + 46793 * Cr ) >> 16) + ( 135 << 8 ) )
#define CYCbCr2B16(Y, Cb, Cr) CLIP16( Y + (116129 * Cb >> 16 ) - ( 226 << 8 ) )



void CANI_YCbCrA16_2_RGBA16(CANI_RGBA16QUAD* dest, CANI_YCbCrA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YCbCrA16QUAD* SrcNow = src;
	CANI_RGBA16QUAD* DestNow = dest;
	CANI_RGBA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y, Cb, Cr;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		Cb = ((CANI_CONVERT_VARIABLE)SrcNow->Cb);
		Cr = ((CANI_CONVERT_VARIABLE)SrcNow->Cr);

		DestNow->Red = (CANI_WORD)CYCbCr2R16(Y, Cb, Cr);
		DestNow->Green = (CANI_WORD)CYCbCr2G16(Y, Cb, Cr);
		DestNow->Blue = (CANI_WORD)CYCbCr2B16(Y, Cb, Cr);

		SrcNow++;
		DestNow++;
	}
}

void CANI_YCbCrA_2_RGBA(CANI_RGBAQUAD* dest, CANI_YCbCrAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YCbCrAQUAD* SrcNow = src;
	CANI_RGBAQUAD* DestNow = dest;
	CANI_RGBAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y, Cb, Cr;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		Cb = ((CANI_CONVERT_VARIABLE)SrcNow->Cb);
		Cr = ((CANI_CONVERT_VARIABLE)SrcNow->Cr);

		DestNow->Red = (CANI_BYTE)CYCbCr2R(Y, Cb, Cr);
		DestNow->Green = (CANI_BYTE)CYCbCr2G(Y, Cb, Cr);
		DestNow->Blue = (CANI_BYTE)CYCbCr2B(Y, Cb, Cr);

		SrcNow++;
		DestNow++;
	}
}

void CANI_YCbCrA16_2_BGRA16(CANI_BGRA16QUAD* dest, CANI_YCbCrA16QUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YCbCrA16QUAD* SrcNow = src;
	CANI_BGRA16QUAD* DestNow = dest;
	CANI_BGRA16QUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y, Cb, Cr;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		Cb = ((CANI_CONVERT_VARIABLE)SrcNow->Cb);
		Cr = ((CANI_CONVERT_VARIABLE)SrcNow->Cr);

		DestNow->Red = (CANI_WORD)CYCbCr2R16(Y, Cb, Cr);
		DestNow->Green = (CANI_WORD)CYCbCr2G16(Y, Cb, Cr);
		DestNow->Blue = (CANI_WORD)CYCbCr2B16(Y, Cb, Cr);

		SrcNow++;
		DestNow++;
	}
}

void CANI_YCbCrA_2_BGRA(CANI_BGRAQUAD* dest, CANI_YCbCrAQUAD* src, CANI_MEMORY_SIZE PixelsSize) {
	CANI_YCbCrAQUAD* SrcNow = src;
	CANI_BGRAQUAD* DestNow = dest;
	CANI_BGRAQUAD* DestMax = &dest[PixelsSize];
	while (DestNow < DestMax) {

		CANI_CONVERT_VARIABLE Y, Cb, Cr;

		Y = ((CANI_CONVERT_VARIABLE)SrcNow->Y);
		Cb = ((CANI_CONVERT_VARIABLE)SrcNow->Cb);
		Cr = ((CANI_CONVERT_VARIABLE)SrcNow->Cr);

		DestNow->Red = (CANI_BYTE)CYCbCr2R(Y, Cb, Cr);
		DestNow->Green = (CANI_BYTE)CYCbCr2G(Y, Cb, Cr);
		DestNow->Blue = (CANI_BYTE)CYCbCr2B(Y, Cb, Cr);

		SrcNow++;
		DestNow++;
	}
}