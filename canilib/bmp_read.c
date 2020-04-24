#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "cani.h"

#pragma pack(1)

typedef struct tagCANI_BITMAPFILEHEADER {
	CANI_WORD    bfType;
	CANI_DWORD   bfSize;
	CANI_WORD    bfAlpha1;
	CANI_WORD    bfAlpha2;
	CANI_DWORD   bfOffBits;
} CANI_BITMAPFILEHEADER;

typedef struct tagCANI_BITMAPINFOHEADER {
	CANI_DWORD      biSize;
	CANI_LONG       biWidth;
	CANI_LONG       biHeight;
	CANI_WORD       biPlanes;
	CANI_WORD       biBitCount;
	CANI_DWORD      biCompression;
	CANI_DWORD      biSizeImage;
	CANI_LONG       biXPelsPerMeter;
	CANI_LONG       biYPelsPerMeter;
	CANI_DWORD      biClrUsed;
	CANI_DWORD      biClrImportant;
} CANI_BITMAPINFOHEADER;

#pragma pack()

CANI_ANIMATION* CANI_ReadBMP_FILE(FILE* file) {
	if (file) {

		CANI_BITMAPFILEHEADER bmfh;
		CANI_BITMAPINFOHEADER bmih;
		CANI_BGRAQUAD* bmiColors;
		CANI_ANIMATION* rv;
		CANI_MEMORY_SIZE Size;
		CANI_BGRAQUAD* Image;

		CANI_FILE_POS pos = CANI_FTELL(file);

		CANI_FREAD(&bmfh, 1, sizeof(CANI_BITMAPFILEHEADER), file);
		if (((CANI_BYTE*)&bmfh.bfType)[0] != 'B' || ((CANI_BYTE*)&bmfh.bfType)[1] != 'M') {

			CANI_FSEEK(file, pos, SEEK_SET);
			return 0;
		}

		CANI_FREAD(&bmih, 1, sizeof(CANI_BITMAPINFOHEADER), file);

		if (bmih.biCompression != 0) { // BI_RGB

			CANI_FSEEK(file, pos, SEEK_SET);
			return 0;
		}

		bmiColors = 0;// (bmih.biClrUsed ? CANI_MALLOC(bmih.biClrUsed * CANI_BGRAQUAD) : 0);
		if (bmih.biClrUsed) {
			bmiColors = CANI_MALLOC(((CANI_MEMORY_SIZE)bmih.biClrUsed) * sizeof(CANI_BGRAQUAD));
			if (bmiColors) {
				CANI_FREAD(bmiColors, sizeof(CANI_BGRAQUAD), bmih.biClrUsed, file);
			}
			else {

				CANI_FSEEK(file, pos, SEEK_SET);
				return 0;
			}
		}

		rv = CANI_MALLOC(sizeof(CANI_ANIMATION));

		if (!rv) {
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return 0;
		}

		CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));

		rv->Format.format = CAF_FORMAT_RGB;
		rv->Format.rgb.Red = 8;
		rv->Format.rgb.Green = 8;
		rv->Format.rgb.Blue = 8;
		rv->Format.rgb.Alpha = 8;

		rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));

		if (!rv->Images) {
			if (bmiColors) CANI_FREE(bmiColors);
			CANI_FREE(rv);
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return 0;
		}

		CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

		rv->ImagesNR = 1;
		rv->Width = bmih.biWidth;
		rv->Height = bmih.biHeight;
		Size = bmih.biWidth * bmih.biHeight;

		Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));

		if (!Image) {
			if (bmiColors) CANI_FREE(bmiColors);
			CANI_FREE(rv->Images);
			CANI_FREE(rv);
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return 0;
		}

		if (bmih.biBitCount == 1) {
			static one_bit_mask[8] = {
				0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
			};
			CANI_MEMORY_SIZE ImageSize;
			CANI_BYTE* obraz;
			CANI_DWORD x,y;

			CANI_DWORD linia = 1 + (rv->Width / 8);

			if ((rv->Width % 8) == 0) linia--;

			if (linia % 4) {
				linia += (4 - (linia % 4));
			}

			ImageSize = ((CANI_MEMORY_SIZE)linia) * ((CANI_MEMORY_SIZE)rv->Height);

			obraz = CANI_MALLOC(ImageSize);

			if (!obraz) {
				//CANI_FSEEK(file, pos, SEEK_SET);
				if (bmiColors) CANI_FREE(bmiColors);
				CANI_FREE(Image);
				CANI_FREE(rv->Images);
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			CANI_FREAD(obraz, 1, ImageSize, file);

			for ( y = 0; y < rv->Height; y++) {
				for ( x = 0; x < rv->Width; x++) {
					CANI_DWORD pos = ((y * linia) + x / 8);
					CANI_DWORD shift = x % 8;

					pos = obraz[pos] & one_bit_mask[shift] ? 1 : 0;

					/*if (shift == 7) {
						pos = obraz[pos] & 0x01 ? 1 : 0;
					}
					else if (shift == 6) {
						pos = obraz[pos] & 0x02 ? 1 : 0;
					}
					else if (shift == 5) {
						pos = obraz[pos] & 0x04 ? 1 : 0;
					}
					else if (shift == 4) {
						pos = obraz[pos] & 0x08 ? 1 : 0;
					}
					else if (shift == 3) {
						pos = obraz[pos] & 0x10 ? 1 : 0;
					}
					else if (shift == 2) {
						pos = obraz[pos] & 0x20 ? 1 : 0;
					}
					else if (shift == 1) {
						pos = obraz[pos] & 0x40 ? 1 : 0;
					}
					else {
						pos = obraz[pos] & 0x80 ? 1 : 0;
					}*/

					Image[(y * rv->Width) + x].Red = bmiColors[pos].Red;

					Image[(y * rv->Width) + x].Green = bmiColors[pos].Green;

					Image[(y * rv->Width) + x].Blue = bmiColors[pos].Blue;

					Image[(y * rv->Width) + x].Alpha = 0xFF;
				}
			}

			if (bmiColors) CANI_FREE(bmiColors);
			CANI_FREE(obraz);
			return rv;

		}
		else if (bmih.biBitCount == 4) {

			static four_bits_mask[2] = {
				0xF0, 0x0F
			};
			static four_bits_shift[2] = {
				4, 0
			};
			CANI_MEMORY_SIZE ImageSize;
			CANI_BYTE* obraz;
			CANI_DWORD x,y;

			//CANI_DWORD linia = rv->Width / 2;

			CANI_DWORD linia = 1 + (rv->Width / 2);

			if ((rv->Width % 2) == 0) linia--;

			if (linia % 4) {
				linia += (4 - (linia % 4));
			}

			ImageSize = ((CANI_MEMORY_SIZE)linia) * ((CANI_MEMORY_SIZE)rv->Height);

			obraz = CANI_MALLOC(ImageSize);

			if (!obraz) {
				//CANI_FSEEK(file, pos, SEEK_SET);
				if (bmiColors) CANI_FREE(bmiColors);
				CANI_FREE(Image);
				CANI_FREE(rv->Images);
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			CANI_FREAD(obraz, 1, ImageSize, file);

			for ( y = 0; y < rv->Height; y++) {
				for ( x = 0; x < rv->Width; x++) {
					CANI_DWORD pos = ((y * linia) + x / 2);
					CANI_DWORD shift = x % 2;

					pos = (obraz[pos] & four_bits_mask[shift]) >> (four_bits_shift[shift]);
					/*if (x % 2) {
						pos = obraz[pos] & 0x0F;
					}
					else {
						pos = (obraz[pos] & 0xF0) >> 4;
					}*/

					Image[(y * rv->Width) + x].Red = bmiColors[pos].Red;

					Image[(y * rv->Width) + x].Green = bmiColors[pos].Green;

					Image[(y * rv->Width) + x].Blue = bmiColors[pos].Blue;

					Image[(y * rv->Width) + x].Alpha = 0xFF;
				}
			}

			if (bmiColors) CANI_FREE(bmiColors);
			CANI_FREE(obraz);
			return rv;

		}
		else if (bmih.biBitCount == 8) {
			// obraz z 8-o bitow¹ palet¹
			CANI_MEMORY_SIZE ImageSize;
			CANI_BYTE* obraz;
			CANI_DWORD x,y;

			CANI_DWORD linia = rv->Width;
			if (linia % 4) {
				linia += (4 - (linia % 4));
			}

			ImageSize = ((CANI_MEMORY_SIZE)linia) * ((CANI_MEMORY_SIZE)rv->Height);

			obraz = CANI_MALLOC(ImageSize);

			if (!obraz) {
				//CANI_FSEEK(file, pos, SEEK_SET);
				if (bmiColors) CANI_FREE(bmiColors);
				CANI_FREE(Image);
				CANI_FREE(rv->Images);
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			CANI_FREAD(obraz, 1, ImageSize, file);

			for ( y = 0; y < rv->Height; y++) {
				for ( x = 0; x < rv->Width; x++) {
					CANI_DWORD pos = ((y * linia) + x);
					CANI_BYTE pos2 = obraz[pos];

					Image[(y * rv->Width) + x].Red = bmiColors[pos2].Red;

					Image[(y * rv->Width) + x].Green = bmiColors[pos2].Green;

					Image[(y * rv->Width) + x].Blue = bmiColors[pos2].Blue;

					Image[(y * rv->Width) + x].Alpha = 0xFF;
				}
			}

			if (bmiColors) CANI_FREE(bmiColors);
			CANI_FREE(obraz);
			return rv;

		}
		else if (bmih.biBitCount == 24
			|| bmih.biBitCount == 32) {
			// obraz 24-o lub 32-u bitowy
			CANI_MEMORY_SIZE ImageSize;
			CANI_BYTE* obraz;
			//CANI_DWORD x,y;

			CANI_DWORD linia;
			if (bmih.biBitCount == 32) {
				linia = rv->Width * 4;
			}
			else {
				linia = rv->Width * 3;
				if (linia % 4) {
					linia += (4 - (linia % 4));
				}
			}

			ImageSize = ((CANI_MEMORY_SIZE)linia) * ((CANI_MEMORY_SIZE)rv->Height);;

			obraz = CANI_MALLOC(ImageSize);

			if (!obraz) {
				//CANI_FSEEK(file, pos, SEEK_SET);
				if (bmiColors) CANI_FREE(bmiColors);
				CANI_FREE(Image);
				CANI_FREE(rv->Images);
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			CANI_FREAD(obraz, 1, ImageSize, file);

			//MessageBoxA( g_hWnd, "1", "wczytano", 0 );
			if (bmih.biBitCount == 32) {
				CANI_MEMCPY(Image, obraz,
					rv->Width * rv->Height * sizeof(CANI_BGRAQUAD));
			}
			else {
				CANI_DWORD rozmiar = rv->Width * rv->Height;
				CANI_BYTE* src = obraz;//3
				CANI_BYTE* dest = (CANI_BYTE*)Image; // 4

				CANI_BYTE* src_max = &obraz[rozmiar * 3];//3

				CANI_DWORD linia2 = rv->Width * 3;
				while (src < src_max) {
					//CANI_DWORD x_src_act = 0;
					CANI_BYTE* src_now = src;
					CANI_BYTE* src_max = src + linia2;
					//for ( CANI_DWORD x = 0; x < szerokosc; x++ ) {
					//	CANI_DWORD src_pos = src_pos_act + x_src_act;
					//	CANI_DWORD dest_pos = dest_pos_act + x;
					while (src_now < src_max) {

						*(dest + 3) = 0xFF;

						*(dest + 2) = *(src_now + 2);
						//obraz[ src_pos + 2 ];

						*(dest + 1) = *(src_now + 1);
						//obraz[ src_pos + 1 ];

						*(dest + 0) = *(src_now);
						//obraz[ src_pos ];

					//*((CANI_DWORD*)(dest)) = *((CANI_DWORD*)(src_now));

						src_now += 3;
						dest += 4;
					}
					//src_pos_act += linia;
					//dest_pos_act += szerokosc;
					src += linia;

				}
			}

			if (bmiColors) CANI_FREE(bmiColors);
			CANI_FREE(obraz);
			return rv;
		}

		//CANI_FSEEK(file, pos, SEEK_SET);
		if (bmiColors) CANI_FREE(bmiColors);
		CANI_FREE(Image);
		CANI_FREE(rv->Images);
		CANI_FREE(rv);
		return 0;

	}
	return 0;
}