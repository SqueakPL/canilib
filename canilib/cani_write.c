#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "cani.h"



CANI_EXPORT CANI_CHAR CANI_WriteAnimation(FILE* File, CANI_ANIMATION* animation, CANI_PROGRESS_FUNC Progress ) {

	char id[] = "CANI4";

	CANI_SIZE AniSize = { 0, 0 };
	AniSize.Width = animation->Width;
	AniSize.Height = animation->Height;

	CANI_FWRITE(id, 1, 5, File);

	//CANI_FWRITE( id, 5, &NumberOfBytesWritten, 0);

	CANI_FWRITE( &AniSize, 1, sizeof(AniSize), File);

	CANI_FWRITE( &animation->ImagesNR, 1, sizeof(animation->ImagesNR), File);

	CANI_FWRITE( &animation->Format, 1, sizeof(animation->Format), File);

	CANI_SetLastError(CANI_ERROR_NO_ERROR);

	if (animation->ImagesNR) {

		CANI_DWORD n, m;
		CANI_BYTE* Src = 0;
		CANI_BYTE* SrcNow = 0;
		CANI_DWORD SrcLen = 0;
		CANI_QWORD SrcLen64 = 0;

		CANI_DWORD BitMapSize = AniSize.Width * AniSize.Height;

		/*if (animation->Format.options & CANI_OPTION_ALPHA) {

			SrcLen += BitMapSize;
		}*/

		if (animation->Format.format == CAF_FORMAT_GRAY || animation->Format.format == CAF_FORMAT_GRAY_ND) {

			CANI_DWORD ColorSize = 1;
			CANI_CHAR Gray16;
			CANI_CHAR Colors16;
			CANI_CHAR Alpha16;
			CANI_DWORD BackgroundColorSize;
			CANI_DWORD AlphaSize;
			CANI_BYTE* AlphaBefore8;
			CANI_WORD* AlphaBefore16;
			CANI_DWORD PosAndSize = 4 * sizeof(CANI_DWORD);
			CANI_BYTE ProgressNow;
			CANI_LONG dest_x, dest_y, dest_width, dest_height, x, y;

			if (animation->Format.gray.Gray > 8) {
				Gray16 = TRUE;
				ColorSize++;
			}
			else Gray16 = FALSE;

			Colors16 = (ColorSize > 1 ? TRUE : FALSE);
			Alpha16 = (animation->Format.gray.Alpha > 8 ? TRUE : FALSE);

			BackgroundColorSize = ColorSize;// (Colors16 || Alpha16) ? (1 * sizeof(CANI_WORD)) : (1 * sizeof(CANI_BYTE));
			AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

			ProgressNow = 0;
			if (Progress) Progress(0);

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				SrcLen = CANI_CalculateGlobalAlphaSize(animation, Alpha16);

				if (SrcLen) {
					Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					if (Src == 0) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}
					SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, Src);

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						CANI_FREE(Src); return FALSE;
					}

					CANI_FREE(Src); Src = 0;
				}
				else {
					CANI_WriteZeroData(File);
				}
			}
			else {

				SrcLen64 = (CANI_QWORD)(animation->ImagesNR) * (CANI_QWORD)(PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

				SrcLen64 += CANI_CalculateAlphaSize(animation, Alpha16);

				if ( SrcLen64 > CANI_MAX_32BIT_SIZE) {
					CANI_SetLastError(CANI_ERROR_MEMORY_TOO_LARGE);
					return 0;
				}

				SrcLen = (CANI_DWORD)SrcLen64;

				Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
				if (Src == 0) {
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}
				SrcNow = Src;

				SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, SrcNow);
			}

			AlphaBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			AlphaBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));

			if (AlphaBefore8 == 0 || AlphaBefore16 == 0 ) {
				if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				if (AlphaBefore16) CANI_FREE(AlphaBefore16);
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

			if ( Colors16 ) {
				CANI_BGRAQUAD* Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
				CANI_BGRA16QUAD *Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
				//CANI_WORD* ImageGray16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));

				CANI_WORD* ImageBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD)); //CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
				CANI_WORD* ImageNow16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD)); //new CANI_BYTE[BitMapSize];

				if (Image8 == 0 || Image16 == 0 /*|| ImageGray16 == 0 */
					|| ImageBefore16 == 0 || ImageNow16 == 0) {

					if (AlphaBefore8) CANI_FREE(AlphaBefore8);
					if (AlphaBefore16) CANI_FREE(AlphaBefore16);

					if (Image8) CANI_FREE(Image8);
					if (Image16) CANI_FREE(Image16);
					//if (ImageGray16) CANI_FREE(ImageGray16);

					if (ImageBefore16) CANI_FREE(ImageBefore16);
					if (ImageNow16) CANI_FREE(ImageNow16);

					CANI_FREE(Src); Src = 0;
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}

				CANI_MEMSET(ImageBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));
				CANI_MEMSET(ImageNow16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

				for (n = 0; n < animation->ImagesNR; n++) {

					CANI_WORD BgColor;

					if (Progress && (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

						CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

						if (ProgressNow2 != ProgressNow) {
							ProgressNow = ProgressNow2;
							Progress(ProgressNow);
						}

					}

					CANI_ConvertImage(Colors16, Image8, Image16, &animation->Images[n], BitMapSize);

					//CANI_BGRA16_2_GRAY16(ImageGray16, Image16, BitMapSize );
					CANI_BGRA16_2_GRAY16(ImageNow16, Image16, BitMapSize);

					if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

						SrcLen = (PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

						if (SrcLen) {
							Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
							if (Src == 0) {
								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);

								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								//if (ImageGray16) CANI_FREE(ImageGray16);

								if (ImageBefore16) CANI_FREE(ImageBefore16);
								if (ImageNow16) CANI_FREE(ImageNow16);

								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return FALSE;
							}
							SrcNow = Src;
						}
					}
					dest_x = 0;
					dest_y = 0;
					dest_width = animation->Width;
					dest_height = animation->Height;

					//char szCaniBuffer[1024];

					if ( ( animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA ) && n > 0) {
						for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
							for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								m = y * animation->Width + x;
								if (ImageNow16[m] != ImageBefore16[m]) break;
							}

							if (y < ((CANI_LONG)animation->Height)) break;
						}

						dest_x = x;

						if (dest_x < ((CANI_LONG)animation->Width)) {
							for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
								for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
									m = y * ((CANI_LONG)animation->Width) + x;
									if (ImageNow16[m] != ImageBefore16[m]) break;
								}

								if (y < ((CANI_LONG)animation->Height)) break;
							}

							dest_width = x - dest_x + 1;
						}
						else {
							dest_x = 0;
							dest_width = 0;
						}

						for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							for (x = 0; x < dest_width; x++) {
								m = y * ((CANI_LONG)animation->Width) + dest_x + x;
								if (ImageNow16[m] != ImageBefore16[m]) break;
							}
							if (x < dest_width) break;
						}

						dest_y = y;

						if (dest_y < ((CANI_LONG)animation->Height)) {
							for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
								for (x = 0; x < dest_width; x++) {
									m = y * ((CANI_LONG)animation->Width) + dest_x + x;
									if (ImageNow16[m] != ImageBefore16[m]) break;
								}
								if (x < dest_width) break;
							}

							dest_height = y - dest_y + 1;
						}
						else {
							dest_y = 0;
							dest_height = 0;
						}

					}

					*((CANI_LONG*)SrcNow) = dest_x;
					SrcNow += sizeof(CANI_LONG);
					*((CANI_LONG*)SrcNow) = dest_y;
					SrcNow += sizeof(CANI_LONG);
					*((CANI_LONG*)SrcNow) = dest_width;
					SrcNow += sizeof(CANI_LONG);
					*((CANI_LONG*)SrcNow) = dest_height;
					SrcNow += sizeof(CANI_LONG);

					for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * ((CANI_LONG)animation->Width) + (dest_x + x);

							if (animation->Format.format == CAF_FORMAT_GRAY_ND) {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);
							}
							else {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray) - ImageBefore16[m];
							}

							SrcNow += sizeof(CANI_WORD);
						}
					}

					for (m = 0; m < BitMapSize; m++) {

						ImageBefore16[m] = CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);

					}

					/*for (m = 0; m < BitMapSize; m++) {

						//ImageNow16[m] = ImageGray16[m];

						if (animation->Format.format == CAF_FORMAT_GRAY_ND) {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);
						}
						else {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray) - ImageBefore16[m];
						}

						SrcNow += sizeof(CANI_WORD);

					}

					for (m = 0; m < BitMapSize; m++) {

						ImageBefore16[m] = CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);
						
					}*/

					*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
					SrcNow += sizeof(CANI_DWORD);

					CANI_RGBA16_2_GRAY16(&BgColor, &animation->Images[n].bgColorRGB16, 1);
					*((CANI_WORD*)SrcNow) = CAF_Mask16( BgColor, animation->Format.gray.Gray );
					SrcNow += sizeof(CANI_WORD);


					*((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
					SrcNow += sizeof(CANI_DWORD);

					if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

						if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);

							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							//if (ImageGray16) CANI_FREE(ImageGray16);

							if (ImageBefore16) CANI_FREE(ImageBefore16);
							if (ImageNow16) CANI_FREE(ImageNow16);
							CANI_FREE(Src);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}

						CANI_FREE(Src); Src = 0;

						SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

						if (SrcLen) {
							Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
							if (Src == 0) {
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return FALSE;
							}
							SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

							if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);

								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								//if (ImageGray16) CANI_FREE(ImageGray16);

								if (ImageBefore16) CANI_FREE(ImageBefore16);
								if (ImageNow16) CANI_FREE(ImageNow16);
								CANI_FREE(Src);
								return FALSE;
							}

							CANI_FREE(Src); Src = 0;
						}
						else {
							CANI_WriteZeroData(File);
						}

					}
					else {
						SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
					}
				}

				if (Image8) CANI_FREE(Image8);
				if (Image16) CANI_FREE(Image16);
				//if (ImageGray16) CANI_FREE(ImageGray16);

				if (ImageBefore16) CANI_FREE(ImageBefore16);
				if (ImageNow16) CANI_FREE(ImageNow16);
			}
			else {
				CANI_BGRAQUAD* Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
				CANI_BGRA16QUAD* Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
				//CANI_BYTE* ImageGray8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));

				CANI_BYTE* ImageBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE)); //CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
				CANI_BYTE* ImageNow8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE)); //new CANI_BYTE[BitMapSize];

				if (Image8 == 0 || Image16 == 0 /*|| ImageGray8 == 0*/
					|| ImageBefore8 == 0 || ImageNow8 == 0) {

					if (AlphaBefore8) CANI_FREE(AlphaBefore8);
					if (AlphaBefore16) CANI_FREE(AlphaBefore16);

					if (Image8) CANI_FREE(Image8);
					if (Image16) CANI_FREE(Image16);
					//if (ImageGray8) CANI_FREE(ImageGray8);

					if (ImageBefore8) CANI_FREE(ImageBefore8);
					if (ImageNow8) CANI_FREE(ImageNow8);

					CANI_FREE(Src);
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}

				CANI_MEMSET(ImageBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
				CANI_MEMSET(ImageNow8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));

				for (n = 0; n < animation->ImagesNR; n++) {

					CANI_BYTE BgColor;

					if (Progress && (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

						CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

						if (ProgressNow2 != ProgressNow) {
							ProgressNow = ProgressNow2;
							Progress(ProgressNow);
						}

					}

					CANI_ConvertImage(Colors16, Image8, Image16, &animation->Images[n], BitMapSize);

					//CANI_BGRA_2_GRAY(ImageGray8, Image8, BitMapSize);
					CANI_BGRA_2_GRAY(ImageNow8, Image8, BitMapSize);

					if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

						SrcLen = (PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

						if (SrcLen) {
							Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
							if (Src == 0) {
								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);

								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								//if (ImageGray8) CANI_FREE(ImageGray8);

								if (ImageBefore8) CANI_FREE(ImageBefore8);
								if (ImageNow8) CANI_FREE(ImageNow8);
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return FALSE;
							}
							SrcNow = Src;
						}
					}

					dest_x = 0;
					dest_y = 0;
					dest_width = animation->Width;
					dest_height = animation->Height;

					//char szCaniBuffer[1024];

					if ((animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA) && n > 0) {
						for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
							for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								m = y * animation->Width + x;
								if (ImageNow8[m] != ImageBefore8[m]) break;
							}

							if (y < ((CANI_LONG)animation->Height)) break;
						}

						dest_x = x;

						if (dest_x < ((CANI_LONG)animation->Width)) {
							for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
								for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
									m = y * ((CANI_LONG)animation->Width) + x;
									if (ImageNow8[m] != ImageBefore8[m]) break;
								}

								if (y < ((CANI_LONG)animation->Height)) break;
							}

							dest_width = x - dest_x + 1;
						}
						else {
							dest_x = 0;
							dest_width = 0;
						}

						for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							for (x = 0; x < dest_width; x++) {
								m = y * ((CANI_LONG)animation->Width) + dest_x + x;
								if (ImageNow8[m] != ImageBefore8[m]) break;
							}
							if (x < dest_width) break;
						}

						dest_y = y;

						if (dest_y < ((CANI_LONG)animation->Height)) {
							for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
								for (x = 0; x < dest_width; x++) {
									m = y * ((CANI_LONG)animation->Width) + dest_x + x;
									if (ImageNow8[m] != ImageBefore8[m]) break;
								}
								if (x < dest_width) break;
							}

							dest_height = y - dest_y + 1;
						}
						else {
							dest_y = 0;
							dest_height = 0;
						}

					}

					*((CANI_LONG*)SrcNow) = dest_x;
					SrcNow += sizeof(CANI_LONG);
					*((CANI_LONG*)SrcNow) = dest_y;
					SrcNow += sizeof(CANI_LONG);
					*((CANI_LONG*)SrcNow) = dest_width;
					SrcNow += sizeof(CANI_LONG);
					*((CANI_LONG*)SrcNow) = dest_height;
					SrcNow += sizeof(CANI_LONG);


					for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * ((CANI_LONG)animation->Width) + (dest_x + x);

							if (animation->Format.format == CAF_FORMAT_GRAY_ND) {
								*((CANI_BYTE*)SrcNow) = CAF_Mask(ImageNow8[m], animation->Format.gray.Gray);
							}
							else {
								*((CANI_BYTE*)SrcNow) = CAF_Mask(ImageNow8[m], animation->Format.gray.Gray) - ImageBefore8[m];
							}

							SrcNow += sizeof(CANI_BYTE);
						}
					}

					for (m = 0; m < BitMapSize; m++) {

						ImageBefore8[m] = CAF_Mask(ImageNow8[m], animation->Format.gray.Gray);

					}

					/*for (m = 0; m < BitMapSize; m++) {

						//ImageNow8[m] = ImageGray8[m];

						if (animation->Format.format == CAF_FORMAT_GRAY_ND) {
							*((CANI_BYTE*)SrcNow) = CAF_Mask(ImageNow8[m], animation->Format.gray.Gray);
						}
						else {
							*((CANI_BYTE*)SrcNow) = CAF_Mask(ImageNow8[m], animation->Format.gray.Gray) - ImageBefore8[m];
						}

						SrcNow += sizeof(CANI_BYTE);

					}

					for (m = 0; m < BitMapSize; m++) {

						ImageBefore8[m] = CAF_Mask(ImageNow8[m], animation->Format.gray.Gray);

					}*/

					*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
					SrcNow += sizeof(CANI_DWORD);

					CANI_RGBA_2_GRAY(&BgColor, &animation->Images[n].bgColorRGB, 1);
					*((CANI_BYTE*)SrcNow) = CAF_Mask(BgColor, animation->Format.gray.Gray);
					SrcNow += sizeof(CANI_BYTE);


					*((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
					SrcNow += sizeof(CANI_DWORD);

					if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

						if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);

							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							//if (ImageGray8) CANI_FREE(ImageGray8);

							if (ImageBefore8) CANI_FREE(ImageBefore8);
							if (ImageNow8) CANI_FREE(ImageNow8);
							CANI_FREE(Src);
							return FALSE;
						}

						CANI_FREE(Src); Src = 0;

						SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

						if (SrcLen) {
							Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
							if (Src == 0) {
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return FALSE;
							}
							SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

							if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);

								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								//if (ImageGray8) CANI_FREE(ImageGray8);

								if (ImageBefore8) CANI_FREE(ImageBefore8);
								if (ImageNow8) CANI_FREE(ImageNow8);
								CANI_FREE(Src);
								return FALSE;
							}

							CANI_FREE(Src); Src = 0;
						}
						else {
							CANI_WriteZeroData(File);
						}

					}
					else {
						SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
					}
				}

				if (Image8) CANI_FREE(Image8);
				if (Image16) CANI_FREE(Image16);
				//if (ImageGray8) CANI_FREE(ImageGray8);

				if (ImageBefore8) CANI_FREE(ImageBefore8);
				if (ImageNow8) CANI_FREE(ImageNow8);
			}

			if (AlphaBefore8) CANI_FREE(AlphaBefore8);
			if (AlphaBefore16) CANI_FREE(AlphaBefore16);

			/*SrcLen += animation->ImagesNR * ((BitMapSize * 1) + sizeof(CANI_DWORD) + (3 * sizeof(CANI_BYTE)) + sizeof(CANI_BYTE));

			if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {
				SrcLen += animation->ImagesNR * BitMapSize;
			}
			else {
				for (n = 0; n < animation->ImagesNR; n++) {
					if (animation->Images[n].Style & CANI_STYLE_ALPHA) SrcLen += BitMapSize;
				}
			}

			Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));// CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
			SrcNow = Src;

			if (animation->Format.options & CANI_OPTION_ALPHA) {
				CANI_MEMCPY(SrcNow, animation->gAlpha, BitMapSize); 
				SrcNow += BitMapSize;
			}

			CANI_BYTE* ImageBefore = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE)); //CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			CANI_BYTE* ImageNow = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE)); //new CANI_BYTE[BitMapSize];
			CANI_BYTE* AlphaBefore = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE)); //new CANI_BYTE[BitMapSize];

			for (m = 0; m < BitMapSize; m++) {
				ImageBefore[m] = 0;
				AlphaBefore[m] = 0;
			}

			for (n = 0; n < animation->ImagesNR; n++) {
				CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image);

				for (m = 0; m < BitMapSize; m++) {

					CANI_WORD ColorNow = (((CANI_WORD)Image[m].Red + (CANI_WORD)Image[m].Green + (CANI_WORD)Image[m].Blue) / 3);
					if (ColorNow > 0xFF) ColorNow = 0xFF;

					ImageNow[m] = (CANI_BYTE)ColorNow;

					if (animation->Format.format == CAF_FORMAT_GRAY_ND) *((CANI_BYTE*)SrcNow) = CAF_Mask(ImageNow[m], animation->Format.gray.Gray);
					else *((CANI_BYTE*)SrcNow) = CAF_Mask(ImageNow[m], animation->Format.gray.Gray) - ImageBefore[m];

					SrcNow += sizeof(CANI_BYTE);

				}

				for (m = 0; m < BitMapSize; m++) {

					ImageBefore[m] = CAF_Mask(ImageNow[m], animation->Format.gray.Gray);

				}

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				SrcNow += sizeof(CANI_DWORD);

				*SrcNow = CAF_Mask(((CANI_BYTE*)&animation->Images[n].bgColor)[0], animation->Format.gray.Gray);
				SrcNow++;

				*SrcNow = CAF_Mask(((CANI_BYTE*)&animation->Images[n].bgColor)[1], animation->Format.gray.Gray);
				SrcNow++;

				*SrcNow = CAF_Mask(((CANI_BYTE*)&animation->Images[n].bgColor)[2], animation->Format.gray.Gray);
				SrcNow++;

				*((CANI_BYTE*)SrcNow) = animation->Images[n].Style;
				SrcNow += sizeof(CANI_BYTE);

				if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {
					for (m = 0; m < BitMapSize; m++) {
						*SrcNow = ((CANI_BGRAQUAD*)animation->Images[n].Image)[m].Alpha - AlphaBefore[m];
						SrcNow++;
					}
					for (m = 0; m < BitMapSize; m++) {
						AlphaBefore[m] = ((CANI_BGRAQUAD*)animation->Images[n].Image)[m].Alpha;
					}
				}
				else if (animation->Images[n].Style & CANI_STYLE_ALPHA) {
					for (m = 0; m < BitMapSize; m++) {
						*SrcNow = ((CANI_BGRAQUAD*)animation->Images[n].Image)[m].Alpha;
						SrcNow++;
					}
				}
			}

			CANI_FREE(ImageBefore);
			CANI_FREE(ImageNow);*/

		}
		else if (animation->Format.format == CAF_FORMAT_RGB16 || animation->Format.format == CAF_FORMAT_RGB16_ND) {

			CANI_DWORD ColorSize = 2;
			//CANI_CHAR Colors16;
			CANI_CHAR Alpha16;
			CANI_DWORD BackgroundColorSize;
			CANI_DWORD AlphaSize;
			CANI_BYTE* AlphaBefore8;
			CANI_WORD* AlphaBefore16;
			CANI_DWORD PosAndSize = 4 * sizeof(CANI_DWORD);
			CANI_BYTE ProgressNow = 0;
			CANI_LONG dest_x, dest_y, dest_width, dest_height, x, y;
			CANI_BGRAQUAD* Image8;
			CANI_BGRA16QUAD* Image16;
			CANI_WORD* ImageBefore16;
			CANI_WORD* ImageNow16;

			//Colors16 = (ColorSize > 1 ? TRUE : FALSE);
			Alpha16 = (animation->Format.gray.Alpha > 8 ? TRUE : FALSE);

			BackgroundColorSize = ColorSize;// (Colors16 || Alpha16) ? (1 * sizeof(CANI_WORD)) : (1 * sizeof(CANI_BYTE));
			AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

			if (Progress) Progress(0);

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				SrcLen = CANI_CalculateGlobalAlphaSize(animation, Alpha16);

				if (SrcLen) {
					Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					if (Src == 0) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}
					SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, Src);

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						CANI_FREE(Src); return FALSE;
					}

					CANI_FREE(Src); Src = 0;
				}
				else {
					CANI_WriteZeroData(File);
				}
			}
			else {

				SrcLen64 = (CANI_QWORD)animation->ImagesNR * (CANI_QWORD)(PosAndSize + (BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

				SrcLen64 += CANI_CalculateAlphaSize(animation, Alpha16);

				if (SrcLen64 > CANI_MAX_32BIT_SIZE) {
					CANI_SetLastError(CANI_ERROR_MEMORY_TOO_LARGE);
					return 0; // (CANI_QWORD)
				}

				SrcLen = (CANI_DWORD)SrcLen64;

				Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
				if (Src == 0) {
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}
				SrcNow = Src;

				SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, SrcNow);
			}

			AlphaBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			AlphaBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));

			if (AlphaBefore8 == 0 || AlphaBefore16 == 0) {
				if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				if (AlphaBefore16) CANI_FREE(AlphaBefore16);
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

			Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
			Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
			//CANI_WORD* ImageGray16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));

			ImageBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD)); //CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			ImageNow16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD)); //new CANI_BYTE[BitMapSize];

			if (Image8 == 0 || Image16 == 0 /*|| ImageGray16 == 0 */
				|| ImageBefore16 == 0 || ImageNow16 == 0) {

				if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				if (AlphaBefore16) CANI_FREE(AlphaBefore16);

				if (Image8) CANI_FREE(Image8);
				if (Image16) CANI_FREE(Image16);
				//if (ImageGray16) CANI_FREE(ImageGray16);

				if (ImageBefore16) CANI_FREE(ImageBefore16);
				if (ImageNow16) CANI_FREE(ImageNow16);

				CANI_FREE(Src); Src = 0;
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMSET(ImageBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));
			CANI_MEMSET(ImageNow16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

			for (n = 0; n < animation->ImagesNR; n++) {

				CANI_WORD BgColor;

				if (Progress && (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				CANI_ConvertImage(TRUE, Image8, Image16, &animation->Images[n], BitMapSize);

				//CANI_BGRA16_2_GRAY16(ImageGray16, Image16, BitMapSize );
				CANI_BGRA16_2_RGB16(ImageNow16, Image16, BitMapSize);

				//MessageBoxA(0, "CANI_BGRA16_2_RGB16", 0, 0);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					SrcLen = (PosAndSize + (BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);

							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							//if (ImageGray16) CANI_FREE(ImageGray16);

							if (ImageBefore16) CANI_FREE(ImageBefore16);
							if (ImageNow16) CANI_FREE(ImageNow16);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = Src;
					}
				}

				dest_x = 0;
				dest_y = 0;
				dest_width = animation->Width;
				dest_height = animation->Height;

				//char szCaniBuffer[1024];

				if ((animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA) && n > 0) {
					for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
						for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							m = y * animation->Width + x;
							if (ImageNow16[m] != ImageBefore16[m]) break;
						}

						if (y < ((CANI_LONG)animation->Height)) break;
					}

					dest_x = x;

					if (dest_x < ((CANI_LONG)animation->Width)) {
						for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
							for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								m = y * ((CANI_LONG)animation->Width) + x;
								if (ImageNow16[m] != ImageBefore16[m]) break;
							}

							if (y < ((CANI_LONG)animation->Height)) break;
						}

						dest_width = x - dest_x + 1;
					}
					else {
						dest_x = 0;
						dest_width = 0;
					}

					for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
						for (x = 0; x < dest_width; x++) {
							m = y * animation->Width + dest_x + x;
							if (ImageNow16[m] != ImageBefore16[m]) break;
						}
						if (x < dest_width) break;
					}

					dest_y = y;

					if (dest_y < ((CANI_LONG)animation->Height)) {
						for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
							for (x = 0; x < dest_width; x++) {
								m = y * animation->Width + dest_x + x;
								if (ImageNow16[m] != ImageBefore16[m]) break;
							}
							if (x < dest_width) break;
						}

						dest_height = y - dest_y + 1;
					}
					else {
						dest_y = 0;
						dest_height = 0;
					}

				}

				*((CANI_LONG*)SrcNow) = dest_x;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_y;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_width;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_height;
				SrcNow += sizeof(CANI_LONG);


				for (y = 0; y < dest_height; y++) {
					for (x = 0; x < dest_width; x++) {
						m = (dest_y + y) * animation->Width + (dest_x + x);

						if (animation->Format.format == CAF_FORMAT_RGB16_ND) {
							*((CANI_WORD*)SrcNow) = ImageNow16[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);
						}
						else {
							*((CANI_WORD*)SrcNow) = ImageNow16[m] - ImageBefore16[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray) - ImageBefore16[m];
						}

						SrcNow += sizeof(CANI_WORD);
					}
				}

				/*for (m = 0; m < BitMapSize; m++) {

					//ImageNow16[m] = ImageGray16[m];

					if (animation->Format.format == CAF_FORMAT_RGB16_ND ) {
						*((CANI_WORD*)SrcNow) = ImageNow16[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);
					}
					else {
						*((CANI_WORD*)SrcNow) = ImageNow16[m] - ImageBefore16[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray) - ImageBefore16[m];
					}

					SrcNow += sizeof(CANI_WORD);

				}*/

				for (m = 0; m < BitMapSize; m++) {

					ImageBefore16[m] = ImageNow16[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);

				}

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				SrcNow += sizeof(CANI_DWORD);

				CANI_RGBA16_2_RGB16(&BgColor, &animation->Images[n].bgColorRGB16, 1);
				*((CANI_WORD*)SrcNow) = BgColor;// CAF_Mask16(BgColor, animation->Format.gray.Gray);
				SrcNow += sizeof(CANI_WORD);


				*((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
				SrcNow += sizeof(CANI_DWORD);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);

						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);
						//if (ImageGray16) CANI_FREE(ImageGray16);

						if (ImageBefore16) CANI_FREE(ImageBefore16);
						if (ImageNow16) CANI_FREE(ImageNow16);
						CANI_FREE(Src);
						return FALSE;
					}

					CANI_FREE(Src); Src = 0;

					SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

						if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);

							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							//if (ImageGray16) CANI_FREE(ImageGray16);

							if (ImageBefore16) CANI_FREE(ImageBefore16);
							if (ImageNow16) CANI_FREE(ImageNow16);
							CANI_FREE(Src);
							return FALSE;
						}

						CANI_FREE(Src); Src = 0;
					}
					else {
						CANI_WriteZeroData(File);
					}

				}
				else {
					SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
				}
			}

			if (Image8) CANI_FREE(Image8);
			if (Image16) CANI_FREE(Image16);
			//if (ImageGray16) CANI_FREE(ImageGray16);

			if (ImageBefore16) CANI_FREE(ImageBefore16);
			if (ImageNow16) CANI_FREE(ImageNow16);

			if (AlphaBefore8) CANI_FREE(AlphaBefore8);
			if (AlphaBefore16) CANI_FREE(AlphaBefore16);

			/*SrcLen += animation->ImagesNR * ((BitMapSize * 2) + sizeof(CANI_DWORD) + (3 * sizeof(CANI_BYTE)) + sizeof(CANI_BYTE));

			if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {
				SrcLen += animation->ImagesNR * BitMapSize;
			}
			else {
				for (n = 0; n < animation->ImagesNR; n++) {
					if (animation->Images[n].Style & CANI_STYLE_ALPHA) SrcLen += BitMapSize;
				}
			}

			Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE)); // CANI_MALLOC(SrcLen * sizeof(CANI_BYTE))
			SrcNow = Src;

			if (animation->Format.options & CANI_OPTION_ALPHA) {
				CANI_MEMCPY(SrcNow, animation->gAlpha, BitMapSize);
				SrcNow += BitMapSize;
			}

			CANI_WORD* ImageBefore = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));
			CANI_WORD* ImageNow = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));
			CANI_BYTE* AlphaBefore = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));

			for (m = 0; m < BitMapSize; m++) {

				ImageBefore[m] = 0;

				AlphaBefore[m] = 0;
			}

			for (n = 0; n < animation->ImagesNR; n++) {
				CANI_BGRAQUAD* Image = (CANI_BGRAQUAD*)(animation->Images[n].Image);

				for (m = 0; m < BitMapSize; m++) {

					CANI_WORD ColorNow = ((((CANI_WORD)Image[m].Red) >> 3) & 0x1F);
					ColorNow |= (((((CANI_WORD)Image[m].Green) >> 2) & 0x3F) << 5);
					ColorNow |= (((((CANI_WORD)Image[m].Blue) >> 3) & 0x1F) << 11);

					ImageNow[m] = ColorNow;

					if (animation->Format.format == CAF_FORMAT_RGB16_ND) *((CANI_WORD*)SrcNow) = ColorNow;
					else *((CANI_WORD*)SrcNow) = ColorNow - ImageBefore[m];

					SrcNow += sizeof(CANI_WORD);

				}

				for (m = 0; m < BitMapSize; m++) {

					ImageBefore[m] = ImageNow[m];

				}

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				SrcNow += sizeof(CANI_DWORD);

				*SrcNow = CAF_Mask(((CANI_BYTE*)&animation->Images[n].bgColor)[0], animation->Format.rgb.Red);
				SrcNow++;

				*SrcNow = CAF_Mask(((CANI_BYTE*)&animation->Images[n].bgColor)[1], animation->Format.rgb.Green);
				SrcNow++;

				*SrcNow = CAF_Mask(((CANI_BYTE*)&animation->Images[n].bgColor)[2], animation->Format.rgb.Blue);
				SrcNow++;

				*((CANI_BYTE*)SrcNow) = (CANI_BYTE)animation->Images[n].Style;
				SrcNow += sizeof(CANI_BYTE);

				if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {
					for (m = 0; m < BitMapSize; m++) {
						*SrcNow = ((CANI_BGRAQUAD*)animation->Images[n].Image)[m].Alpha - AlphaBefore[m];
						SrcNow++;
					}
					for (m = 0; m < BitMapSize; m++) {
						AlphaBefore[m] = ((CANI_BGRAQUAD*)animation->Images[n].Image)[m].Alpha;
					}
				}
				else if (animation->Images[n].Style & CANI_STYLE_ALPHA) {
					for (m = 0; m < BitMapSize; m++) {
						*SrcNow = ((CANI_BGRAQUAD*)animation->Images[n].Image)[m].Alpha;
						SrcNow++;
					}
				}
			}

			CANI_FREE(ImageBefore);
			CANI_FREE(ImageNow);*/
		} 
		else if (animation->Format.format == CAF_FORMAT_PALETTE || animation->Format.format == CAF_FORMAT_PALETTE_ND) {

			CANI_DWORD ColorSize = 1;
			//CANI_CHAR Colors16;
			CANI_CHAR Alpha16;
			CANI_DWORD BackgroundColorSize;
			CANI_DWORD AlphaSize;
			CANI_BYTE* AlphaBefore8=0;
			CANI_WORD* AlphaBefore16=0;
			CANI_BYTE* PaletteNow=0;
			CANI_BYTE* PaletteBefore=0;
			CANI_BYTE ProgressNow = 0;

			CANI_BGRAQUAD* Image8=0;
			CANI_BGRA16QUAD* Image16=0;
			CANI_DWORD PosAndSize = 4 * sizeof(CANI_DWORD);
			CANI_DWORD ColorosNumber = (1 << animation->Format.palette.Palette);
			CANI_DWORD i,n;
			CANI_BGRAQUAD Palette[256];
			CANI_DWORD PaletteNR = 0;
			CANI_DWORD PaletteSize;
			CANI_LONG dest_x, dest_y, dest_width, dest_height, x, y;

			if (animation->Format.palette.Palette > 8) {
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_PALLET_SIZE_TOO_LARGE);
				return FALSE;
			}

			if (animation->Format.palette.Palette > 8) ColorSize++;

			//Colors16 = (ColorSize > 1 ? TRUE : FALSE);
			Alpha16 = (animation->Format.palette.Alpha > 8 ? TRUE : FALSE);

			BackgroundColorSize = 2;// ColorSize;// (Colors16 || Alpha16) ? (1 * sizeof(CANI_WORD)) : (1 * sizeof(CANI_BYTE));
			AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

			if (Progress) Progress(0);

			for (n = 0; n < animation->ImagesNR; n++) {

				if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {

					CANI_BGRA16QUAD* ImageNow = (CANI_BGRA16QUAD*)animation->Images[n].Image;
					CANI_BGRA16QUAD* ImageMax = &((CANI_BGRA16QUAD*)(animation->Images[n].Image))[BitMapSize];

					while (ImageNow < ImageMax) {

						CANI_BYTE Red = (CANI_BYTE)((ImageNow->Red & 0xFF00) >> 8);
						CANI_BYTE Green = (CANI_BYTE)((ImageNow->Green & 0xFF00) >> 8);
						CANI_BYTE Blue = (CANI_BYTE)((ImageNow->Blue & 0xFF00) >> 8);

						for (i = 0; i < PaletteNR; i++) {
							if (Palette[i].Red == Red && Palette[i].Green == Green && Palette[i].Blue == Blue) break;
						}

						if (i >= PaletteNR) {
							if (PaletteNR >= 256) { CANI_FREE(Src); CANI_SetLastError(CANI_ERROR_TOO_MANY_COLORS); return FALSE; }

							//sprintf(szCaniBuffer, "Dodano =  %u [%u,%u,%u]", PaletteNR, Red,Green, Blue );

							//MessageBoxA(0, szCaniBuffer, "", 0);

							Palette[PaletteNR].Red = Red;
							Palette[PaletteNR].Green = Green;
							Palette[PaletteNR].Blue = Blue;
							PaletteNR++;
						}

						ImageNow++;

					}
				}
				else {

					CANI_BGRAQUAD* ImageNow = (CANI_BGRAQUAD*)animation->Images[n].Image;
					CANI_BGRAQUAD* ImageMax = &((CANI_BGRAQUAD*)(animation->Images[n].Image))[BitMapSize];

					while (ImageNow < ImageMax) {

						CANI_BYTE Red = ImageNow->Red;
						CANI_BYTE Green = ImageNow->Green;
						CANI_BYTE Blue = ImageNow->Blue;

						for (i = 0; i < PaletteNR; i++) {
							if (Palette[i].Red == Red && Palette[i].Green == Green && Palette[i].Blue == Blue) break;
						}

						if (i >= PaletteNR) {
							if (PaletteNR >= 256) { CANI_FREE(Src); CANI_SetLastError(CANI_ERROR_TOO_MANY_COLORS); return FALSE; }
							Palette[PaletteNR].Red = Red;
							Palette[PaletteNR].Green = Green;
							Palette[PaletteNR].Blue = Blue;

							PaletteNR++;
						}

						ImageNow++;

					}
				}
			}

			if (PaletteNR > ColorosNumber) {
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_TOO_MANY_COLORS);
				return FALSE;
			}

			ColorosNumber = PaletteNR;

			PaletteSize = ColorosNumber * 3;

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				SrcLen = CANI_CalculateGlobalAlphaSize(animation, Alpha16);

				if (SrcLen) {
					Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					if (Src == 0) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}
					SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, Src);

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						CANI_FREE(Src); return FALSE;
					}

					CANI_FREE(Src); Src = 0;
				}
				else {
					CANI_WriteZeroData(File);
				}
			}
			else {

				SrcLen64 = sizeof(CANI_DWORD) + (CANI_QWORD)PaletteSize + (CANI_QWORD)animation->ImagesNR * (CANI_QWORD)(PosAndSize + (BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

				SrcLen64 += CANI_CalculateAlphaSize(animation, Alpha16);

				if (SrcLen64 > CANI_MAX_32BIT_SIZE) {
					CANI_SetLastError(CANI_ERROR_MEMORY_TOO_LARGE);
					return 0; // (CANI_QWORD)
				}

				SrcLen = (CANI_DWORD)SrcLen64;

				Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
				if (Src == 0) {
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}
				SrcNow = Src;

				SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, SrcNow);
			}

			AlphaBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			AlphaBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));
			PaletteNow = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			PaletteBefore = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
			Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

			if (AlphaBefore8 == 0 || AlphaBefore16 == 0) {
				if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				if (AlphaBefore16) CANI_FREE(AlphaBefore16);
				if (PaletteNow) CANI_FREE(PaletteNow);
				if (PaletteBefore) CANI_FREE(PaletteBefore);
				if (Image8) CANI_FREE(Image8);
				if (Image16) CANI_FREE(Image16);
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				SrcLen = sizeof(CANI_DWORD) + PaletteSize;

				if (SrcLen) {
					Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					if (Src == 0) {
						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (PaletteNow) CANI_FREE(PaletteNow);
						if (PaletteBefore) CANI_FREE(PaletteBefore);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}
					SrcNow = Src;
				}
			}

			//InitGraphicsReuce();

			//sprintf(szCaniBuffer, "ColorosNumber =  %u", ColorosNumber );

			//MessageBoxA(0, szCaniBuffer, "", 0);

			*((CANI_DWORD*)SrcNow) = ColorosNumber;
			SrcNow += sizeof(CANI_DWORD);

			for (n = 0; n < ColorosNumber; n++) {

				*((CANI_BYTE*)SrcNow) = Palette[n].Red;
				SrcNow += sizeof(CANI_BYTE);

				*((CANI_BYTE*)SrcNow) = Palette[n].Green;
				SrcNow += sizeof(CANI_BYTE);

				*((CANI_BYTE*)SrcNow) = Palette[n].Blue;
				SrcNow += sizeof(CANI_BYTE);
			}

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
					if (AlphaBefore8) CANI_FREE(AlphaBefore8);
					if (AlphaBefore16) CANI_FREE(AlphaBefore16);
					if (PaletteNow) CANI_FREE(PaletteNow);
					if (PaletteBefore) CANI_FREE(PaletteBefore);
					if (Image8) CANI_FREE(Image8);
					if (Image16) CANI_FREE(Image16);
					CANI_FREE(Src);
					return FALSE;
				}

				CANI_FREE(Src); Src = 0;
			}

			CANI_MEMSET(PaletteBefore, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			CANI_MEMSET(PaletteNow, 0x00, (BitMapSize * sizeof(CANI_BYTE)));

			for (n = 0; n < animation->ImagesNR; n++) {

				CANI_CHAR quarter_byte;
				CANI_CHAR half_byte;
				CANI_BYTE byte_now;
				CANI_BYTE bytes[4];

				CANI_LONG Red;
				CANI_LONG Green;
				CANI_LONG Blue;

				int diff, max, pal;

				if (Progress && (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				CANI_ConvertImage(Alpha16, Image8, Image16, &animation->Images[n], BitMapSize);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					SrcLen = (PosAndSize + (BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (PaletteNow) CANI_FREE(PaletteNow);
							if (PaletteBefore) CANI_FREE(PaletteBefore);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = Src;
					}
				}
				
				if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {

					CANI_BGRA16QUAD* ImageNow = (CANI_BGRA16QUAD*)animation->Images[n].Image;//(ImageNow->Red & 0xF000)

					for (m = 0; m < BitMapSize; m++) {

						CANI_BYTE Red = (CANI_BYTE)((ImageNow->Red & 0xFF00) >> 8);
						CANI_BYTE Green = (CANI_BYTE)((ImageNow->Green & 0xFF00) >> 8);
						CANI_BYTE Blue = (CANI_BYTE)((ImageNow->Blue & 0xFF00) >> 8);

						for (i = 0; i < PaletteNR; i++) {
							if (Palette[i].Red == Red && Palette[i].Green == Green && Palette[i].Blue == Blue) break;
						}

						PaletteNow[m] = (CANI_BYTE)i;

						//c = CANI_NearestColor(ImageNow, palette, ColorosNumber);
						/*for (c = 0; c < ColorosNumber; c++) {
							if ( ( palette[c].Red & 0xF0 ) == ( ( ImageNow->Red & 0xF000 ) >> 8 ) 
								&& ( palette[c].Green & 0xF0) == ( (ImageNow->Green & 0xF000) >> 8)
								&& ( palette[c].Blue & 0xF0 ) == ( (ImageNow->Blue & 0xF000) >> 8) ) {
								break;
							}
						}*/

						//if (c >= ColorosNumber ) MessageBoxA(0, "CAF_FORMAT_PALETTE", "", 0);
						//if (c>0) MessageBoxA(0, "c>0", "CAF_FORMAT_PALETTE", 0);

						ImageNow++;
					}
				}
				else {

					CANI_BGRAQUAD* ImageNow = (CANI_BGRAQUAD*)animation->Images[n].Image;

					for (m = 0; m < BitMapSize; m++) {

						CANI_BYTE Red = ImageNow->Red;
						CANI_BYTE Green = ImageNow->Green;
						CANI_BYTE Blue = ImageNow->Blue;

						for (i = 0; i < PaletteNR; i++) {
							if (Palette[i].Red == Red && Palette[i].Green == Green && Palette[i].Blue == Blue) break;
						}

						PaletteNow[m] = (CANI_BYTE)i;

						ImageNow++;
					}
				}

				dest_x = 0;
				dest_y = 0;
				dest_width = animation->Width;
				dest_height = animation->Height;

				if ((animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA) && n > 0) {
					for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
						for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							m = y * animation->Width + x;
							if (PaletteNow[m] != PaletteBefore[m]) break;
						}

						if (y < ((CANI_LONG)animation->Height)) break;
					}

					dest_x = x;

					if (dest_x < ((CANI_LONG)animation->Width)) {
						for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
							for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								m = y * ((CANI_LONG)animation->Width) + x;
								if (PaletteNow[m] != PaletteBefore[m]) break;
							}

							if (y < ((CANI_LONG)animation->Height)) break;
						}

						dest_width = x - dest_x + 1;
					}
					else {
						dest_x = 0;
						dest_width = 0;
					}

					for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
						for (x = 0; x < dest_width; x++) {
							m = y * ((CANI_LONG)animation->Width) + dest_x + x;
							if (PaletteNow[m] != PaletteBefore[m]) break;
						}
						if (x < dest_width) break;
					}

					dest_y = y;

					if (dest_y < ((CANI_LONG)animation->Height)) {
						for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
							for (x = 0; x < dest_width; x++) {
								m = y * ((CANI_LONG)animation->Width) + dest_x + x;
								if (PaletteNow[m] != PaletteBefore[m]) break;
							}
							if (x < dest_width) break;
						}

						dest_height = y - dest_y + 1;
					}
					else {
						dest_y = 0;
						dest_height = 0;
					}

				}

				//sprintf(szCaniBuffer, "%i,%i  %i x %i", dest_x, dest_y, dest_width, dest_height);

				//MessageBoxA(0, szCaniBuffer, "", 0);

				*((CANI_LONG*)SrcNow) = dest_x;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_y;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_width;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_height;
				SrcNow += sizeof(CANI_LONG);

				quarter_byte = 
					(animation->Format.format == CAF_FORMAT_PALETTE_ND && animation->Format.palette.Palette <= 2 && ColorosNumber <= 4 ) ? TRUE : FALSE;

				half_byte = 
					( animation->Format.format == CAF_FORMAT_PALETTE_ND && animation->Format.palette.Palette <= 4 && ColorosNumber <= 16 ) ? TRUE : FALSE;

				//CANI_CHAR four_in_three =
				//	(animation->Format.format == CAF_FORMAT_PALETTE_ND && animation->Format.palette.Palette <= 6 && ColorosNumber <= 64) ? TRUE : FALSE;

				byte_now = 0;

				CANI_MEMSET(bytes,0x00,sizeof(bytes));


				for (y = 0; y < dest_height; y++) {
					for (x = 0; x < dest_width; x++) {
						m = (dest_y + y) * animation->Width + (dest_x + x);

						if (animation->Format.format == CAF_FORMAT_PALETTE_ND) {
							bytes[byte_now] = PaletteNow[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);
						}
						else {
							bytes[byte_now] = PaletteNow[m] - PaletteBefore[m];// -ImageBefore16[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray) - ImageBefore16[m];
						}

						if (quarter_byte) {

							if (byte_now == 0) byte_now = 1;
							else if (byte_now == 1) byte_now = 2;
							else if (byte_now == 2) byte_now = 3;
							else {
								byte_now = 0;
								*((CANI_BYTE*)SrcNow) =
									((bytes[0] & 0x03) << 6) | ((bytes[1] & 0x03) << 4) | ((bytes[2] & 0x03) << 2) | (bytes[3] & 0x03);
								SrcNow += sizeof(CANI_BYTE);
								CANI_MEMSET(bytes, 0x00, sizeof(bytes));
							}

						}
						else if (half_byte) {

							if (byte_now == 0) byte_now = 1;
							else {
								byte_now = 0;
								*((CANI_BYTE*)SrcNow) = ((bytes[0] & 0x0F) << 4) | (bytes[1] & 0x0F);
								SrcNow += sizeof(CANI_BYTE);
								CANI_MEMSET(bytes, 0x00, sizeof(bytes));
							}
						}
						/*else if (four_in_three) {

							if (byte_now == 0) byte_now = 1;
							else if (byte_now == 1) byte_now = 2;
							else if (byte_now == 2) byte_now = 3;
							else {
								byte_now = 0;

								CANI_DWORD Value = 0;

								Value = ((((CANI_DWORD)bytes[0]) & 0x3F) << 18) | ((((CANI_DWORD)bytes[1]) & 0x3F) << 12)
									| ((((CANI_DWORD)bytes[2]) & 0x3F) << 6) | ((((CANI_DWORD)bytes[3]) & 0x3F));

								*((CANI_BYTE*)SrcNow) = ((CANI_BYTE*)&Value)[0];
								SrcNow += sizeof(CANI_BYTE);

								*((CANI_BYTE*)SrcNow) = ((CANI_BYTE*)&Value)[1];
								SrcNow += sizeof(CANI_BYTE);

								*((CANI_BYTE*)SrcNow) = ((CANI_BYTE*)&Value)[2];
								SrcNow += sizeof(CANI_BYTE);

								CANI_MEMSET(bytes, 0x00, sizeof(bytes));
							}
						}*/ 
						else {

							*((CANI_BYTE*)SrcNow) = bytes[byte_now];

							SrcNow += sizeof(CANI_BYTE);
						}
					}
				}

				if (quarter_byte) {
					if (byte_now) {
						*((CANI_BYTE*)SrcNow) =
							((bytes[0] & 0x03) << 6) || ((bytes[1] & 0x03) << 4) || ((bytes[2] & 0x03) << 2) || (bytes[3] & 0x03);
						SrcNow += sizeof(CANI_BYTE);
					}
				}
				else if (half_byte) {
					if (byte_now) {
						*((CANI_BYTE*)SrcNow) = ((bytes[0] & 0x0F) << 4) || (bytes[1] & 0x0F);
						SrcNow += sizeof(CANI_BYTE);
					}
				} 
				/*else if (four_in_three) {
					if (byte_now) {

						CANI_DWORD Value = 0;

						Value = ((((CANI_DWORD)bytes[0]) & 0x3F) << 18) | ((((CANI_DWORD)bytes[1]) & 0x3F) << 12)
							| ((((CANI_DWORD)bytes[2]) & 0x3F) << 6) | ((((CANI_DWORD)bytes[3]) & 0x3F));

						*((CANI_BYTE*)SrcNow) = ((CANI_BYTE*)&Value)[0];
						SrcNow += sizeof(CANI_BYTE);

						*((CANI_BYTE*)SrcNow) = ((CANI_BYTE*)&Value)[1];
						SrcNow += sizeof(CANI_BYTE);

						*((CANI_BYTE*)SrcNow) = ((CANI_BYTE*)&Value)[2];
						SrcNow += sizeof(CANI_BYTE);
					}
				}*/
				
				for (m = 0; m < BitMapSize; m++) {

					PaletteBefore[m] = PaletteNow[m];// CAF_Mask16(ImageNow16[m], animation->Format.gray.Gray);

				}

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				SrcNow += sizeof(CANI_DWORD);

				Red = animation->Images[n].bgColorRGB.Red;
				Green = animation->Images[n].bgColorRGB.Green;
				Blue = animation->Images[n].bgColorRGB.Blue;

				max = 100000000;
				pal = 0;

				for (i = 0; i < PaletteNR; i++) {

					long r = ((CANI_LONG)Palette[i].Red) - Red;
					long g = ((CANI_LONG)Palette[i].Green) - Green;
					long b = ((CANI_LONG)Palette[i].Blue) - Blue;

					if (r < 0) r = -r;
					if (g < 0) g = -g;
					if (b < 0) b = -b;

					if (r > 0xFF) r = 0xFF;
					if (g > 0xFF) g = 0xFF;
					if (b > 0xFF) b = 0xFF;

					diff = (int)sqrt( (float)( r + g + b ) );// sqrt(r * r + g * g + b * b);

					if (diff < max) {
						max = diff;
						pal = i;
					}
					//if (Palette[i].Red == Red && Palette[i].Green == Green && Palette[i].Blue == Blue) break;
				}

				*((CANI_WORD*)SrcNow) = pal;
				SrcNow += sizeof(CANI_WORD);

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
				SrcNow += sizeof(CANI_DWORD);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (PaletteNow) CANI_FREE(PaletteNow);
						if (PaletteBefore) CANI_FREE(PaletteBefore);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);
						CANI_FREE(Src);
						return FALSE;
					}

					CANI_FREE(Src); Src = 0;

					SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

						if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (PaletteNow) CANI_FREE(PaletteNow);
							if (PaletteBefore) CANI_FREE(PaletteBefore);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							CANI_FREE(Src);
							return FALSE;
						}

						CANI_FREE(Src); Src = 0;
					}
					else {
						CANI_WriteZeroData(File);
					}

				}
				else {
					SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
				}
			}

			if (AlphaBefore8) CANI_FREE(AlphaBefore8);
			if (AlphaBefore16) CANI_FREE(AlphaBefore16);
			if (PaletteNow) CANI_FREE(PaletteNow);
			if (PaletteBefore) CANI_FREE(PaletteBefore);
			if (Image8) CANI_FREE(Image8);
			if (Image16) CANI_FREE(Image16);

		} else if (animation->Format.format == CAF_FORMAT_RGB || animation->Format.format == CAF_FORMAT_RGB_ND) {

			CANI_DWORD ColorSize = 3;
			CANI_CHAR Red16, Green16, Blue16;
			CANI_CHAR Colors16;
			CANI_CHAR Alpha16;
			CANI_DWORD BackgroundColorSize;
			CANI_DWORD AlphaSize;
			CANI_DWORD PosAndSize = 4 * sizeof(CANI_DWORD);

			CANI_BGRAQUAD* ImageBefore8;
			CANI_BGRA16QUAD* ImageBefore16;
			CANI_BYTE* AlphaBefore8;
			CANI_WORD* AlphaBefore16;
			CANI_BGRAQUAD* Image8;
			CANI_BGRA16QUAD* Image16;
			CANI_BYTE ProgressNow = 0;
			CANI_LONG dest_x, dest_y,dest_width, dest_height,x,y;

			if (animation->Format.rgb.Red > 8) {
				Red16 = TRUE;
				ColorSize++;
			} else Red16 = FALSE;

			if (animation->Format.rgb.Green > 8) {
				Green16 = TRUE;
				ColorSize++;
			} else Green16 = FALSE;

			if (animation->Format.rgb.Blue > 8) {
				Blue16 = TRUE;
				ColorSize++;
			} else Blue16 = FALSE;

			Colors16 = (ColorSize > 3 ? TRUE : FALSE);
			Alpha16 = (animation->Format.rgb.Alpha > 8 ? TRUE : FALSE);

			BackgroundColorSize = ColorSize;// (Colors16 /*|| Alpha16*/) ? (3 * sizeof(CANI_WORD)) : (3 * sizeof(CANI_BYTE));
			AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

			if (Progress) Progress(0);

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				SrcLen = CANI_CalculateGlobalAlphaSize(animation, Alpha16);

				if (SrcLen) {
					Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					if (Src == 0) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}
					SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, Src);

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						CANI_FREE(Src); return FALSE;
					}

					CANI_FREE(Src); Src = 0;
				}
				else {
					CANI_WriteZeroData(File);
				}
			}
			else {

				SrcLen64 = (CANI_QWORD)animation->ImagesNR * (CANI_QWORD)(PosAndSize + (BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

				SrcLen64 += CANI_CalculateAlphaSize(animation, Alpha16);

				if (SrcLen64 > CANI_MAX_32BIT_SIZE) {
					CANI_SetLastError(CANI_ERROR_MEMORY_TOO_LARGE);
					return 0; // (CANI_QWORD)
				}

				SrcLen = (CANI_DWORD)SrcLen64;

				Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
				if (Src == 0) {
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}
				SrcNow = Src;

				SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, SrcNow);
			}

			ImageBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
			ImageBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
			AlphaBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			AlphaBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));
			Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
			Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

			if (ImageBefore8 == 0 || ImageBefore16 == 0 || AlphaBefore8 == 0 || AlphaBefore16 == 0
				|| Image8 == 0 || Image16 == 0) {

				if (ImageBefore8) CANI_FREE(ImageBefore8);
				if (ImageBefore16) CANI_FREE(ImageBefore16);
				if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				if (AlphaBefore16) CANI_FREE(AlphaBefore16);
				if (Image8) CANI_FREE(Image8);
				if (Image16) CANI_FREE(Image16);
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMSET( ImageBefore8, 0x00, (BitMapSize * sizeof(CANI_BGRAQUAD)));
			CANI_MEMSET( ImageBefore16, 0x00, (BitMapSize * sizeof(CANI_BGRA16QUAD)));
			CANI_MEMSET( AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			CANI_MEMSET( AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));
			CANI_MEMSET( Image8, 0x00, (BitMapSize * sizeof(CANI_BGRAQUAD)));
			CANI_MEMSET( Image16, 0x00, (BitMapSize * sizeof(CANI_BGRA16QUAD)));

			for (n = 0; n < animation->ImagesNR; n++) {

				if ( Progress && ( animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY ) ) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				CANI_ConvertImage(Colors16, Image8, Image16, &animation->Images[n], BitMapSize);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					SrcLen = ( PosAndSize + (BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							if (ImageBefore8) CANI_FREE(ImageBefore8);
							if (ImageBefore16) CANI_FREE(ImageBefore16);
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = Src;
					}
				}

				dest_x = 0;
				dest_y = 0;
				dest_width = animation->Width;
				dest_height = animation->Height;

				//char szCaniBuffer[1024];

				if ((animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA) && n > 0) {
					for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
						for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							m = y * animation->Width + x;
							if (Red16) {
								if (ImageBefore16[m].Red != CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red)) break;
							}
							else {
								if (ImageBefore8[m].Red != CAF_Mask(Image8[m].Red, animation->Format.rgb.Red)) break;
							}
							if (Green16) {
								if (ImageBefore16[m].Green != CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green)) break;
							}
							else {
								if (ImageBefore8[m].Green != CAF_Mask(Image8[m].Green, animation->Format.rgb.Green)) break;
							}
							if (Blue16) {
								if (ImageBefore16[m].Blue != CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue)) break;
							}
							else {
								if (ImageBefore8[m].Blue != CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue)) break;
							}
						}

						if (y < ((CANI_LONG)animation->Height)) break;
					}

					dest_x = x;

					if (dest_x < ((CANI_LONG)animation->Width)) {
						for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
							for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								m = y * ((CANI_LONG)animation->Width) + x;
								if (Red16) {
									if (ImageBefore16[m].Red != CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red)) break;
								}
								else {
									if (ImageBefore8[m].Red != CAF_Mask(Image8[m].Red, animation->Format.rgb.Red)) break;
								}
								if (Green16) {
									if (ImageBefore16[m].Green != CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green)) break;
								}
								else {
									if (ImageBefore8[m].Green != CAF_Mask(Image8[m].Green, animation->Format.rgb.Green)) break;
								}
								if (Blue16) {
									if (ImageBefore16[m].Blue != CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue)) break;
								}
								else {
									if (ImageBefore8[m].Blue != CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue)) break;
								}
							}

							if (y < ((CANI_LONG)animation->Height)) break;
						}

						dest_width = x - dest_x  + 1;

						//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_height, dest_height);

						//MessageBoxA(0, szCaniBuffer, szCaniBuffer, 0);
					}
					else {
						dest_x = 0;
						dest_width = 0;
					}

					for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
						for (x = 0; x < dest_width; x++) {
							m = y * animation->Width + dest_x + x;
							if (Red16) {
								if (ImageBefore16[m].Red != CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red)) break;
							}
							else {
								if (ImageBefore8[m].Red != CAF_Mask(Image8[m].Red, animation->Format.rgb.Red)) break;
							}
							if (Green16) {
								if (ImageBefore16[m].Green != CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green)) break;
							}
							else {
								if (ImageBefore8[m].Green != CAF_Mask(Image8[m].Green, animation->Format.rgb.Green)) break;
							}
							if (Blue16) {
								if (ImageBefore16[m].Blue != CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue)) break;
							}
							else {
								if (ImageBefore8[m].Blue != CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue)) break;
							}
						}
						if (x < dest_width) break;
					}

					dest_y = y;

					if (dest_y < ((CANI_LONG)animation->Height)) {
						for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
							for (x = 0; x < dest_width; x++) {
								m = y * animation->Width + dest_x + x;
								if (Red16) {
									if (ImageBefore16[m].Red != CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red)) break;
								}
								else {
									if (ImageBefore8[m].Red != CAF_Mask(Image8[m].Red, animation->Format.rgb.Red)) break;
								}
								if (Green16) {
									if (ImageBefore16[m].Green != CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green)) break;
								}
								else {
									if (ImageBefore8[m].Green != CAF_Mask(Image8[m].Green, animation->Format.rgb.Green)) break;
								}
								if (Blue16) {
									if (ImageBefore16[m].Blue != CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue)) break;
								}
								else {
									if (ImageBefore8[m].Blue != CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue)) break;
								}
							}
							if (x < dest_width) break;
						}

						dest_height = y - dest_y  + 1;
					}
					else {
						dest_y = 0;
						dest_height = 0;
					}
					
				}

				//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_width, dest_height);

				//MessageBoxA(0, szCaniBuffer, szCaniBuffer,0);

				*((CANI_LONG*)SrcNow) = dest_x;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_y;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_width;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_height;
				SrcNow += sizeof(CANI_LONG);


				if ( dest_width > 0 && dest_height > 0 ) for (y = 0; y < dest_height; y++) {
					for (x = 0; x < dest_width; x++) {
						m = (dest_y + y) * animation->Width + ( dest_x + x );
						if (animation->Format.format == CAF_FORMAT_RGB_ND) {
							if (Red16) {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red);
								SrcNow += 2;
							}
							else {
								*SrcNow = CAF_Mask(Image8[m].Red, animation->Format.rgb.Red);
								SrcNow++;
							}
							if (Green16) {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green);
								SrcNow += 2;
							}
							else {
								*SrcNow = CAF_Mask(Image8[m].Green, animation->Format.rgb.Green);
								SrcNow++;
							}
							if (Blue16) {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue);
								SrcNow += 2;
							}
							else {
								*SrcNow = CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue);
								SrcNow++;
							}
						}
						else {
							if (Red16) {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red) - ImageBefore16[m].Red;
								SrcNow += 2;
							}
							else {
								*SrcNow = CAF_Mask(Image8[m].Red, animation->Format.rgb.Red) - ImageBefore8[m].Red;
								SrcNow++;
							}
							if (Green16) {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green) - ImageBefore16[m].Green;
								SrcNow += 2;
							}
							else {
								*SrcNow = CAF_Mask(Image8[m].Green, animation->Format.rgb.Green) - ImageBefore8[m].Green;
								SrcNow++;
							}
							if (Blue16) {
								*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue) - ImageBefore16[m].Blue;
								SrcNow += 2;
							}
							else {
								*SrcNow = CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue) - ImageBefore8[m].Blue;
								SrcNow++;
							}
						}
					}
				}


				/*for (m = 0; m < BitMapSize; m++) {

					if (animation->Format.format == CAF_FORMAT_RGB_ND) {
						if (Red16) {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red);
							SrcNow+=2;
						} else {
							*SrcNow = CAF_Mask(Image8[m].Red, animation->Format.rgb.Red);
							SrcNow++;
						}
						if (Green16) {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green);
							SrcNow += 2;
						} else {
							*SrcNow = CAF_Mask(Image8[m].Green, animation->Format.rgb.Green);
							SrcNow++;
						}
						if (Blue16) {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue);
							SrcNow += 2;
						}
						else {
							*SrcNow = CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue);
							SrcNow++;
						}
					}
					else {
						if (Red16) {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red) - ImageBefore16[m].Red;
							SrcNow += 2;
						}
						else {
							*SrcNow = CAF_Mask(Image8[m].Red, animation->Format.rgb.Red) - ImageBefore8[m].Red;
							SrcNow++;
						}
						if (Green16) {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green) - ImageBefore16[m].Green;
							SrcNow += 2;
						}
						else {
							*SrcNow = CAF_Mask(Image8[m].Green, animation->Format.rgb.Green) - ImageBefore8[m].Green;
							SrcNow++;
						}
						if (Blue16) {
							*((CANI_WORD*)SrcNow) = CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue) - ImageBefore16[m].Blue;
							SrcNow += 2;
						}
						else {
							*SrcNow = CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue) - ImageBefore8[m].Blue;
							SrcNow++;
						}
					}

				}*/

				for (m = 0; m < BitMapSize; m++) {

					if (Red16) ImageBefore16[m].Red = CAF_Mask16(Image16[m].Red, animation->Format.rgb.Red);
					else ImageBefore8[m].Red = CAF_Mask(Image8[m].Red, animation->Format.rgb.Red);

					if (Green16) ImageBefore16[m].Green = CAF_Mask16(Image16[m].Green, animation->Format.rgb.Green);
					else ImageBefore8[m].Green = CAF_Mask(Image8[m].Green, animation->Format.rgb.Green);

					if (Blue16) ImageBefore16[m].Blue = CAF_Mask16(Image16[m].Blue, animation->Format.rgb.Blue);
					else ImageBefore8[m].Blue = CAF_Mask(Image8[m].Blue, animation->Format.rgb.Blue);

				}

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				SrcNow += sizeof(CANI_DWORD);

				//SrcNow = CANI_WriteBackgroundColor(Colors16 /*|| Alpha16*/, SrcNow, &animation->Images[n]);

				SrcNow = CANI_WriteBackgroundColor(Red16,Green16,Blue16,SrcNow, &animation->Images[n], &animation->Format );

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
				SrcNow += sizeof(CANI_DWORD);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					if (!CANI_CompressAndWrite(File, Src, SrcNow )) {
						if (ImageBefore8) CANI_FREE(ImageBefore8);
						if (ImageBefore16) CANI_FREE(ImageBefore16);
						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);
						CANI_FREE(Src);
						return FALSE;
					}

					CANI_FREE(Src); Src = 0;

					SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

						if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
							if (ImageBefore8) CANI_FREE(ImageBefore8);
							if (ImageBefore16) CANI_FREE(ImageBefore16);
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							CANI_FREE(Src);
							return FALSE;
						}

						CANI_FREE(Src); Src = 0;
					}
					else {
						CANI_WriteZeroData(File);
					}

				} else {

					SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
				}
			}

			if (ImageBefore8) CANI_FREE(ImageBefore8);
			if (ImageBefore16) CANI_FREE(ImageBefore16);
			if (AlphaBefore8) CANI_FREE(AlphaBefore8);
			if (AlphaBefore16) CANI_FREE(AlphaBefore16);
			if (Image8) CANI_FREE(Image8);
			if (Image16) CANI_FREE(Image16);
			//CANI_FREE(Image);

		} else if (animation->Format.format == CAF_FORMAT_CMYK || animation->Format.format == CAF_FORMAT_CMYK_ND) {

			 CANI_DWORD ColorSize = 4;
			 CANI_CHAR Cyan16, Magenta16, Yellow16, KeyColour16;
			 CANI_CHAR Colors16;
			 CANI_CHAR Alpha16;
			 CANI_DWORD BackgroundColorSize;
			 CANI_DWORD AlphaSize;
			 CANI_DWORD PosAndSize = 4 * sizeof(CANI_DWORD);

			 CANI_BYTE* AlphaBefore8;
			 CANI_WORD* AlphaBefore16;
			 CANI_BGRAQUAD* Image8;
			 CANI_CMYKAQUAD* ImageCMYK8;
			 CANI_CMYKAQUAD* ImageBeforeCMYK8;
			 CANI_BGRA16QUAD* Image16;
			 CANI_CMYKA16QUAD *ImageCMYK16;
			 CANI_CMYKA16QUAD *ImageBeforeCMYK16;
			 CANI_BYTE ProgressNow = 0;
			 CANI_LONG dest_x, dest_y, dest_width, dest_height, x, y;

			 if (animation->Format.cmyk.C > 8) {
				 Cyan16 = TRUE;
				 ColorSize++;
			 }
			 else Cyan16 = FALSE;

			 if (animation->Format.cmyk.M > 8) {
				 Magenta16 = TRUE;
				 ColorSize++;
			 }
			 else Magenta16 = FALSE;

			 if (animation->Format.cmyk.Y > 8) {
				 Yellow16 = TRUE;
				 ColorSize++;
			 }
			 else Yellow16 = FALSE;

			 if (animation->Format.cmyk.K > 8) {
				 KeyColour16 = TRUE;
				 ColorSize++;
			 }
			 else KeyColour16 = FALSE;

			 Colors16 = (ColorSize > 4 ? TRUE : FALSE);
			 Alpha16 = (animation->Format.cmyk.Alpha > 8 ? TRUE : FALSE);

			 BackgroundColorSize = ColorSize;// (Colors16 /*|| Alpha16*/) ? (3 * sizeof(CANI_WORD)) : (3 * sizeof(CANI_BYTE));
			 AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

			 //MessageBoxA(0, "CAF_FORMAT_CMYK", "", 0);

			 if (Progress) Progress(0);

			 if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				 SrcLen = CANI_CalculateGlobalAlphaSize(animation, Alpha16);

				 if (SrcLen) {
					 Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					 if (Src == 0) {
						 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						 return FALSE;
					 }
					 SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, Src);

					 if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						 CANI_FREE(Src); return FALSE;
					 }

					 CANI_FREE(Src); Src = 0;
				 }
				 else {
					 CANI_WriteZeroData(File);
				 }
			 }
			 else {

				 SrcLen64 = (CANI_QWORD)animation->ImagesNR * (CANI_QWORD)(PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

				 SrcLen64 += CANI_CalculateAlphaSize(animation, Alpha16);

				 if (SrcLen64 > CANI_MAX_32BIT_SIZE) {
					 CANI_SetLastError(CANI_ERROR_MEMORY_TOO_LARGE);
					 return 0; // (CANI_QWORD)
				 }

				 SrcLen = (CANI_DWORD)SrcLen64;

				 Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
				 if (Src == 0) {
					 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					 return FALSE;
				 }
				 SrcNow = Src;

				 SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, SrcNow);
			 }

			 AlphaBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			 AlphaBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));

			 Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
			 ImageCMYK8 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKAQUAD));
			 ImageBeforeCMYK8 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKAQUAD));

			 Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
			 ImageCMYK16 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKA16QUAD));
			 ImageBeforeCMYK16 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKA16QUAD));

			 if ( AlphaBefore8 == 0 || AlphaBefore16 == 0
				 || Image8 == 0 || ImageCMYK8 == 0 || ImageBeforeCMYK8 == 0 
				 || Image16 == 0 || ImageCMYK16 == 0 || ImageBeforeCMYK16 == 0 ) {

				 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
				 if (Image8) CANI_FREE(Image8);
				 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
				 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
				 if (Image16) CANI_FREE(Image16);
				 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
				 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);
				 CANI_FREE(Src);
				 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				 return FALSE;
			 }

			 CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			 CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

			 CANI_MEMSET(Image8, 0x00, (BitMapSize * sizeof(CANI_BGRAQUAD)));
			 CANI_MEMSET(ImageCMYK8, 0x00, (BitMapSize * sizeof(CANI_CMYKAQUAD)));
			 CANI_MEMSET(ImageBeforeCMYK8, 0x00, (BitMapSize * sizeof(CANI_CMYKAQUAD)));

			 CANI_MEMSET(Image16, 0x00, (BitMapSize * sizeof(CANI_BGRA16QUAD)));
			 CANI_MEMSET(ImageCMYK16, 0x00, (BitMapSize * sizeof(CANI_CMYKA16QUAD)));
			 CANI_MEMSET(ImageBeforeCMYK16, 0x00, (BitMapSize * sizeof(CANI_CMYKA16QUAD)));
			 
			for (n = 0; n < animation->ImagesNR; n++) {

				 if (Progress && (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

					 CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

					 if (ProgressNow2 != ProgressNow) {
						 ProgressNow = ProgressNow2;
						 Progress(ProgressNow);
					 }

				 }

				 //CANI_ConvertImage(TRUE, Image8, Image16, &animation->Images[n], BitMapSize);

				 if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {
					 if (Image8) CANI_BGRA16_2_BGRA(Image8, (CANI_BGRA16QUAD*)animation->Images[n].Image, BitMapSize);
					 if (Image16) CANI_MEMCPY(Image16, (CANI_BGRA16QUAD*)animation->Images[n].Image, BitMapSize * sizeof(CANI_BGRA16QUAD));
					 CANI_RGBA16_2_RGBA(&animation->Images[n].bgColorRGB, &animation->Images[n].bgColorRGB16, 1);
				 }
				 else {
					 if (Image8) CANI_MEMCPY(Image8, (CANI_BGRAQUAD*)animation->Images[n].Image, BitMapSize * sizeof(CANI_BGRAQUAD));
					 if (Image16) CANI_BGRA_2_BGRA16(Image16, (CANI_BGRAQUAD*)animation->Images[n].Image, BitMapSize);
					 CANI_RGBA_2_RGBA16(&animation->Images[n].bgColorRGB16, &animation->Images[n].bgColorRGB, 1);
				 }

				 CANI_BGRA16_2_CMYKA16(ImageCMYK16, Image16, BitMapSize);

				 CANI_BGRA_2_CMYKA(ImageCMYK8, Image8, BitMapSize);

				 if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					 SrcLen = (PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

					 if (SrcLen) {
						 Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						 if (Src == 0) {
							 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							 if (Image8) CANI_FREE(Image8);
							 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
							 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
							 if (Image16) CANI_FREE(Image16);
							 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
							 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);
							 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							 return FALSE;
						 }
						 SrcNow = Src;
					 }
				 }

				 /*for (m = 0; m < BitMapSize; m++) {
					 //CANI_CHAR Cyan16, Magenta16, Yellow16, KeyColour16;

					 if (animation->Format.format == CAF_FORMAT_CMYK_ND) {
						 if (Cyan16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C );
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].C), animation->Format.cmyk.C);
							 SrcNow++;
						 }
						 if (Magenta16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M);
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].M), animation->Format.cmyk.M);
							 SrcNow++;
						 }
						 if (Yellow16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y);
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].Y), animation->Format.cmyk.Y);
							 SrcNow++;
						 }
						 if (KeyColour16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K);
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].K), animation->Format.cmyk.K);
							 SrcNow++;
						 }
					 }
					 else {
						 if (Cyan16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C) - ImageBeforeCMYK16[m].C;
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].C), animation->Format.cmyk.C) - (ImageBeforeCMYK8[m].C);
							 SrcNow++;
						 }
						 if (Magenta16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M) - ImageBeforeCMYK16[m].M;
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].M), animation->Format.cmyk.M) - (ImageBeforeCMYK8[m].M );
							 SrcNow++;
						 }
						 if (Yellow16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y) - ImageBeforeCMYK16[m].Y;
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].Y), animation->Format.cmyk.Y) - (ImageBeforeCMYK8[m].Y);
							 SrcNow++;
						 }
						 if (KeyColour16) {
							 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K) - ImageBeforeCMYK16[m].K;
							 SrcNow += 2;
						 }
						 else {
							 *SrcNow = CAF_Mask((ImageCMYK8[m].K), animation->Format.cmyk.K) - (ImageBeforeCMYK8[m].K);
							 SrcNow++;
						 }
					 }

				 }

				for (m = 0; m < BitMapSize; m++) {

					 if (Cyan16) ImageBeforeCMYK16[m].C = CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C);
					 else ImageBeforeCMYK8[m].C = CAF_Mask(ImageCMYK8[m].C, animation->Format.cmyk.C);

					 if (Magenta16) ImageBeforeCMYK16[m].M = CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M);
					 else ImageBeforeCMYK8[m].M = CAF_Mask(ImageCMYK8[m].M, animation->Format.cmyk.M);

					 if (Yellow16) ImageBeforeCMYK16[m].Y = CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y);
					 else ImageBeforeCMYK8[m].Y = CAF_Mask(ImageCMYK8[m].Y, animation->Format.cmyk.Y);

					 if (KeyColour16) ImageBeforeCMYK16[m].K = CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K);
					 else ImageBeforeCMYK8[m].K = CAF_Mask(ImageCMYK8[m].K, animation->Format.cmyk.K);


				 }*/

				 dest_x = 0;
				 dest_y = 0;
				 dest_width = animation->Width;
				 dest_height = animation->Height;

				 //char szCaniBuffer[1024];

				 if ((animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA) && n > 0) {
					 for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
						 for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							 m = y * animation->Width + x;
							 if (Cyan16) {
								 if (ImageBeforeCMYK16[m].C != CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].C != CAF_Mask(ImageCMYK8[m].C, animation->Format.cmyk.C)) break;
							 }
							 if (Magenta16) {
								 if (ImageBeforeCMYK16[m].M != CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].M != CAF_Mask(ImageCMYK8[m].M, animation->Format.cmyk.M)) break;
							 }
							 if (Yellow16) {
								 if (ImageBeforeCMYK16[m].Y != CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].Y != CAF_Mask(ImageCMYK8[m].Y, animation->Format.cmyk.Y)) break;
							 }
							 if (KeyColour16) {
								 if (ImageBeforeCMYK16[m].K != CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].K != CAF_Mask(ImageCMYK8[m].K, animation->Format.cmyk.K)) break;
							 }
						 }

						 if (y < ((CANI_LONG)animation->Height)) break;
					 }

					 dest_x = x;

					 if (dest_x < ((CANI_LONG)animation->Width)) {
						 for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
							 for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								 m = y * ((CANI_LONG)animation->Width) + x;
								 if (Cyan16) {
									 if (ImageBeforeCMYK16[m].C != CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].C != CAF_Mask(ImageCMYK8[m].C, animation->Format.cmyk.C)) break;
								 }
								 if (Magenta16) {
									 if (ImageBeforeCMYK16[m].M != CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].M != CAF_Mask(ImageCMYK8[m].M, animation->Format.cmyk.M)) break;
								 }
								 if (Yellow16) {
									 if (ImageBeforeCMYK16[m].Y != CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].Y != CAF_Mask(ImageCMYK8[m].Y, animation->Format.cmyk.Y)) break;
								 }
								 if (KeyColour16) {
									 if (ImageBeforeCMYK16[m].K != CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].K != CAF_Mask(ImageCMYK8[m].K, animation->Format.cmyk.K)) break;
								 }
							 }

							 if (y < ((CANI_LONG)animation->Height)) break;
						 }

						 dest_width = x - dest_x + 1;

						 //sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_height, dest_height);

						 //MessageBoxA(0, szCaniBuffer, szCaniBuffer, 0);
					 }
					 else {
						 dest_x = 0;
						 dest_width = 0;
					 }

					 for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
						 for (x = 0; x < dest_width; x++) {
							 m = y * animation->Width + dest_x + x;
							 if (Cyan16) {
								 if (ImageBeforeCMYK16[m].C != CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].C != CAF_Mask(ImageCMYK8[m].C, animation->Format.cmyk.C)) break;
							 }
							 if (Magenta16) {
								 if (ImageBeforeCMYK16[m].M != CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].M != CAF_Mask(ImageCMYK8[m].M, animation->Format.cmyk.M)) break;
							 }
							 if (Yellow16) {
								 if (ImageBeforeCMYK16[m].Y != CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].Y != CAF_Mask(ImageCMYK8[m].Y, animation->Format.cmyk.Y)) break;
							 }
							 if (KeyColour16) {
								 if (ImageBeforeCMYK16[m].K != CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K)) break;
							 }
							 else {
								 if (ImageBeforeCMYK8[m].K != CAF_Mask(ImageCMYK8[m].K, animation->Format.cmyk.K)) break;
							 }
						 }
						 if (x < dest_width) break;
					 }

					 dest_y = y;

					 if (dest_y < ((CANI_LONG)animation->Height)) {
						 for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
							 for (x = 0; x < dest_width; x++) {
								 m = y * animation->Width + dest_x + x;
								 if (Cyan16) {
									 if (ImageBeforeCMYK16[m].C != CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].C != CAF_Mask(ImageCMYK8[m].C, animation->Format.cmyk.C)) break;
								 }
								 if (Magenta16) {
									 if (ImageBeforeCMYK16[m].M != CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].M != CAF_Mask(ImageCMYK8[m].M, animation->Format.cmyk.M)) break;
								 }
								 if (Yellow16) {
									 if (ImageBeforeCMYK16[m].Y != CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].Y != CAF_Mask(ImageCMYK8[m].Y, animation->Format.cmyk.Y)) break;
								 }
								 if (KeyColour16) {
									 if (ImageBeforeCMYK16[m].K != CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K)) break;
								 }
								 else {
									 if (ImageBeforeCMYK8[m].K != CAF_Mask(ImageCMYK8[m].K, animation->Format.cmyk.K)) break;
								 }
							 }
							 if (x < dest_width) break;
						 }

						 dest_height = y - dest_y + 1;
					 }
					 else {
						 dest_y = 0;
						 dest_height = 0;
					 }

				 }

				 //sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_width, dest_height);

				 //MessageBoxA(0, szCaniBuffer, szCaniBuffer,0);

				 *((CANI_LONG*)SrcNow) = dest_x;
				 SrcNow += sizeof(CANI_LONG);
				 *((CANI_LONG*)SrcNow) = dest_y;
				 SrcNow += sizeof(CANI_LONG);
				 *((CANI_LONG*)SrcNow) = dest_width;
				 SrcNow += sizeof(CANI_LONG);
				 *((CANI_LONG*)SrcNow) = dest_height;
				 SrcNow += sizeof(CANI_LONG);


				 if (dest_width > 0 && dest_height > 0) for (y = 0; y < dest_height; y++) {
					 for (x = 0; x < dest_width; x++) {
						 m = (dest_y + y) * animation->Width + (dest_x + x);
						 if (animation->Format.format == CAF_FORMAT_CMYK_ND) {
							 if (Cyan16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C);
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].C), animation->Format.cmyk.C);
								 SrcNow++;
							 }
							 if (Magenta16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M);
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].M), animation->Format.cmyk.M);
								 SrcNow++;
							 }
							 if (Yellow16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y);
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].Y), animation->Format.cmyk.Y);
								 SrcNow++;
							 }
							 if (KeyColour16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K);
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].K), animation->Format.cmyk.K);
								 SrcNow++;
							 }
						 }
						 else {
							 if (Cyan16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C) - ImageBeforeCMYK16[m].C;
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].C), animation->Format.cmyk.C) - (ImageBeforeCMYK8[m].C);
								 SrcNow++;
							 }
							 if (Magenta16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M) - ImageBeforeCMYK16[m].M;
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].M), animation->Format.cmyk.M) - (ImageBeforeCMYK8[m].M);
								 SrcNow++;
							 }
							 if (Yellow16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y) - ImageBeforeCMYK16[m].Y;
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].Y), animation->Format.cmyk.Y) - (ImageBeforeCMYK8[m].Y);
								 SrcNow++;
							 }
							 if (KeyColour16) {
								 *((CANI_WORD*)SrcNow) = CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K) - ImageBeforeCMYK16[m].K;
								 SrcNow += 2;
							 }
							 else {
								 *SrcNow = CAF_Mask((ImageCMYK8[m].K), animation->Format.cmyk.K) - (ImageBeforeCMYK8[m].K);
								 SrcNow++;
							 }
						 }
					 }
				 }

				 for (m = 0; m < BitMapSize; m++) {

					 if (Cyan16) ImageBeforeCMYK16[m].C = CAF_Mask16(ImageCMYK16[m].C, animation->Format.cmyk.C);
					 else ImageBeforeCMYK8[m].C = CAF_Mask(ImageCMYK8[m].C, animation->Format.cmyk.C);

					 if (Magenta16) ImageBeforeCMYK16[m].M = CAF_Mask16(ImageCMYK16[m].M, animation->Format.cmyk.M);
					 else ImageBeforeCMYK8[m].M = CAF_Mask(ImageCMYK8[m].M, animation->Format.cmyk.M);

					 if (Yellow16) ImageBeforeCMYK16[m].Y = CAF_Mask16(ImageCMYK16[m].Y, animation->Format.cmyk.Y);
					 else ImageBeforeCMYK8[m].Y = CAF_Mask(ImageCMYK8[m].Y, animation->Format.cmyk.Y);

					 if (KeyColour16) ImageBeforeCMYK16[m].K = CAF_Mask16(ImageCMYK16[m].K, animation->Format.cmyk.K);
					 else ImageBeforeCMYK8[m].K = CAF_Mask(ImageCMYK8[m].K, animation->Format.cmyk.K);


				 }

				 *((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				 SrcNow += sizeof(CANI_DWORD);

				 SrcNow = CANI_WriteBackgroundColorCMYK(Cyan16, Magenta16, Yellow16, KeyColour16, SrcNow, &animation->Images[n], &animation->Format);

				 *((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
				 SrcNow += sizeof(CANI_DWORD);

				 if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					 if (!CANI_CompressAndWrite(File, Src, SrcNow)) {

						 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						 if (Image8) CANI_FREE(Image8);
						 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
						 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
						 if (Image16) CANI_FREE(Image16);
						 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
						 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);
						 CANI_FREE(Src);
						 return FALSE;
					 }

					 CANI_FREE(Src); Src = 0;

					 SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

					 if (SrcLen) {
						 Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						 if (Src == 0) {
							 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							 return FALSE;
						 }
						 SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

						 if (!CANI_CompressAndWrite(File, Src, SrcNow)) {

							 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							 if (Image8) CANI_FREE(Image8);
							 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
							 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
							 if (Image16) CANI_FREE(Image16);
							 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
							 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);
							 CANI_FREE(Src);
							 return FALSE;
						 }

						 CANI_FREE(Src); Src = 0;
					 }
					 else {
						 CANI_WriteZeroData(File);
					 }

				 }
				 else {

					 SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
				 }
			 }

			 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
			 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
			 if (Image8) CANI_FREE(Image8);
			 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
			 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
			 if (Image16) CANI_FREE(Image16);
			 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
			 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);

		}
		else if (animation->Format.format == CAF_FORMAT_YUV444 || animation->Format.format == CAF_FORMAT_YUV422 || animation->Format.format == CAF_FORMAT_YUV411
			|| animation->Format.format == CAF_FORMAT_YUV444_ND || animation->Format.format == CAF_FORMAT_YUV422_ND || animation->Format.format == CAF_FORMAT_YUV411_ND) {

			CANI_DWORD ColorSize = 3;
			CANI_CHAR Y16, U16, V16;
			CANI_CHAR Colors16;
			CANI_CHAR Alpha16;
			CANI_DWORD BackgroundColorSize;
			CANI_DWORD AlphaSize;
			CANI_DWORD PosAndSize = 4 * sizeof(CANI_DWORD);

			CANI_BYTE* AlphaBefore8;
			CANI_WORD* AlphaBefore16;
			CANI_BGRAQUAD* Image8;
			CANI_YUVAQUAD* ImageYUV8;
			CANI_YUVAQUAD* ImageBeforeYUV8;
			CANI_BGRA16QUAD* Image16;
			CANI_YUVA16QUAD* ImageYUV16;
			CANI_YUVA16QUAD* ImageBeforeYUV16;
			CANI_BYTE ProgressNow = 0;
			CANI_LONG dest_x, dest_y, dest_width, dest_height, x, y;

			if (animation->Format.yuv.Y > 8) {
				Y16 = TRUE;
				ColorSize++;
			}
			else Y16 = FALSE;

			if (animation->Format.yuv.U > 8) {
				U16 = TRUE;
				ColorSize++;
			}
			else U16 = FALSE;

			if (animation->Format.yuv.V > 8) {
				V16 = TRUE;
				ColorSize++;
			}
			else V16 = FALSE;

			Colors16 = (ColorSize > 3 ? TRUE : FALSE);
			Alpha16 = (animation->Format.yuv.Alpha > 8 ? TRUE : FALSE);

			BackgroundColorSize = ColorSize;// (Colors16 /*|| Alpha16*/) ? (3 * sizeof(CANI_WORD)) : (3 * sizeof(CANI_BYTE));
			AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

			//MessageBoxA(0, "CAF_FORMAT_CMYK", "", 0);

			if (Progress) Progress(0);

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				SrcLen = CANI_CalculateGlobalAlphaSize(animation, Alpha16);

				if (SrcLen) {
					Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					if (Src == 0) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}
					SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, Src);

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						CANI_FREE(Src); return FALSE;
					}

					CANI_FREE(Src); Src = 0;
				}
				else {
					CANI_WriteZeroData(File);
				}
			}
			else {

				SrcLen64 = (CANI_QWORD)animation->ImagesNR * (CANI_QWORD)(PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

				SrcLen64 += CANI_CalculateAlphaSize(animation, Alpha16);

				if (SrcLen64 > CANI_MAX_32BIT_SIZE) {
					CANI_SetLastError(CANI_ERROR_MEMORY_TOO_LARGE);
					return 0; // (CANI_QWORD)
				}

				SrcLen = (CANI_DWORD)SrcLen64;

				Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
				if (Src == 0) {
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}
				SrcNow = Src;

				SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, SrcNow);
			}

			AlphaBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			AlphaBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));

			Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
			ImageYUV8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVAQUAD));
			ImageBeforeYUV8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVAQUAD));

			Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
			ImageYUV16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVA16QUAD));
			ImageBeforeYUV16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVA16QUAD));

			if (AlphaBefore8 == 0 || AlphaBefore16 == 0
				|| Image8 == 0 || ImageYUV8 == 0 || ImageBeforeYUV8 == 0
				|| Image16 == 0 || ImageYUV16 == 0 || ImageBeforeYUV16 == 0) {

				if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				if (AlphaBefore16) CANI_FREE(AlphaBefore16);
				if (Image8) CANI_FREE(Image8);
				if (ImageYUV8) CANI_FREE(ImageYUV8);
				if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
				if (Image16) CANI_FREE(Image16);
				if (ImageYUV16) CANI_FREE(ImageYUV16);
				if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

			CANI_MEMSET(Image8, 0x00, (BitMapSize * sizeof(CANI_BGRAQUAD)));
			CANI_MEMSET(ImageYUV8, 0x00, (BitMapSize * sizeof(CANI_YUVAQUAD)));
			CANI_MEMSET(ImageBeforeYUV8, 0x00, (BitMapSize * sizeof(CANI_YUVAQUAD)));

			CANI_MEMSET(Image16, 0x00, (BitMapSize * sizeof(CANI_BGRA16QUAD)));
			CANI_MEMSET(ImageYUV16, 0x00, (BitMapSize * sizeof(CANI_YUVA16QUAD)));
			CANI_MEMSET(ImageBeforeYUV16, 0x00, (BitMapSize * sizeof(CANI_YUVA16QUAD)));

			for (n = 0; n < animation->ImagesNR; n++) {

				if (Progress && (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				//CANI_ConvertImage(TRUE, Image8, Image16, &animation->Images[n], BitMapSize);

				if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {
					if (Image8) CANI_BGRA16_2_BGRA(Image8, (CANI_BGRA16QUAD*)animation->Images[n].Image, BitMapSize);
					if (Image16) CANI_MEMCPY(Image16, (CANI_BGRA16QUAD*)animation->Images[n].Image, BitMapSize * sizeof(CANI_BGRA16QUAD));
					CANI_RGBA16_2_RGBA(&animation->Images[n].bgColorRGB, &animation->Images[n].bgColorRGB16, 1);
				}
				else {
					if (Image8) CANI_MEMCPY(Image8, (CANI_BGRAQUAD*)animation->Images[n].Image, BitMapSize * sizeof(CANI_BGRAQUAD));
					if (Image16) CANI_BGRA_2_BGRA16(Image16, (CANI_BGRAQUAD*)animation->Images[n].Image, BitMapSize);
					CANI_RGBA_2_RGBA16(&animation->Images[n].bgColorRGB16, &animation->Images[n].bgColorRGB, 1);
				}

				CANI_BGRA16_2_YUVA16(ImageYUV16, Image16, BitMapSize);

				CANI_BGRA_2_YUVA(ImageYUV8, Image8, BitMapSize);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					SrcLen = (PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (ImageYUV8) CANI_FREE(ImageYUV8);
							if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
							if (Image16) CANI_FREE(Image16);
							if (ImageYUV16) CANI_FREE(ImageYUV16);
							if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = Src;
					}
				}

				dest_x = 0;
				dest_y = 0;
				dest_width = animation->Width;
				dest_height = animation->Height;

				//char szCaniBuffer[1024];

				if ((animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA) && n > 0) {
					for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
						for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							m = y * animation->Width + x;
							if (Y16) {
								if (ImageBeforeYUV16[m].Y != CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y)) break;
							}
							else {
								if (ImageBeforeYUV8[m].Y != CAF_Mask(ImageYUV8[m].Y, animation->Format.yuv.Y)) break;
							}
							if (U16) {
								if (ImageBeforeYUV16[m].U != CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U)) break;
							}
							else {
								if (ImageBeforeYUV8[m].U != CAF_Mask(ImageYUV8[m].U, animation->Format.yuv.U)) break;
							}
							if (V16) {
								if (ImageBeforeYUV16[m].V != CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V)) break;
							}
							else {
								if (ImageBeforeYUV8[m].V != CAF_Mask(ImageYUV8[m].V, animation->Format.yuv.V)) break;
							}
						}

						if (y < ((CANI_LONG)animation->Height)) break;
					}

					dest_x = x;

					if (dest_x < ((CANI_LONG)animation->Width)) {
						for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
							for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								m = y * ((CANI_LONG)animation->Width) + x;
								if (Y16) {
									if (ImageBeforeYUV16[m].Y != CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y)) break;
								}
								else {
									if (ImageBeforeYUV8[m].Y != CAF_Mask(ImageYUV8[m].Y, animation->Format.yuv.Y)) break;
								}
								if (U16) {
									if (ImageBeforeYUV16[m].U != CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U)) break;
								}
								else {
									if (ImageBeforeYUV8[m].U != CAF_Mask(ImageYUV8[m].U, animation->Format.yuv.U)) break;
								}
								if (V16) {
									if (ImageBeforeYUV16[m].V != CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V)) break;
								}
								else {
									if (ImageBeforeYUV8[m].V != CAF_Mask(ImageYUV8[m].V, animation->Format.yuv.V)) break;
								}
							}

							if (y < ((CANI_LONG)animation->Height)) break;
						}

						dest_width = x - dest_x + 1;

						//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_height, dest_height);

						//MessageBoxA(0, szCaniBuffer, szCaniBuffer, 0);
					}
					else {
						dest_x = 0;
						dest_width = 0;
					}

					for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
						for (x = 0; x < dest_width; x++) {
							m = y * animation->Width + dest_x + x;
							if (Y16) {
								if (ImageBeforeYUV16[m].Y != CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y)) break;
							}
							else {
								if (ImageBeforeYUV8[m].Y != CAF_Mask(ImageYUV8[m].Y, animation->Format.yuv.Y)) break;
							}
							if (U16) {
								if (ImageBeforeYUV16[m].U != CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U)) break;
							}
							else {
								if (ImageBeforeYUV8[m].U != CAF_Mask(ImageYUV8[m].U, animation->Format.yuv.U)) break;
							}
							if (V16) {
								if (ImageBeforeYUV16[m].V != CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V)) break;
							}
							else {
								if (ImageBeforeYUV8[m].V != CAF_Mask(ImageYUV8[m].V, animation->Format.yuv.V)) break;
							}
						}
						if (x < dest_width) break;
					}

					dest_y = y;

					if (dest_y < ((CANI_LONG)animation->Height)) {
						for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
							for (x = 0; x < dest_width; x++) {
								m = y * animation->Width + dest_x + x;
								if (Y16) {
									if (ImageBeforeYUV16[m].Y != CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y)) break;
								}
								else {
									if (ImageBeforeYUV8[m].Y != CAF_Mask(ImageYUV8[m].Y, animation->Format.yuv.Y)) break;
								}
								if (U16) {
									if (ImageBeforeYUV16[m].U != CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U)) break;
								}
								else {
									if (ImageBeforeYUV8[m].U != CAF_Mask(ImageYUV8[m].U, animation->Format.yuv.U)) break;
								}
								if (V16) {
									if (ImageBeforeYUV16[m].V != CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V)) break;
								}
								else {
									if (ImageBeforeYUV8[m].V != CAF_Mask(ImageYUV8[m].V, animation->Format.yuv.V)) break;
								}
							}
							if (x < dest_width) break;
						}

						dest_height = y - dest_y + 1;
					}
					else {
						dest_y = 0;
						dest_height = 0;
					}

				}

				//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_width, dest_height);

				//MessageBoxA(0, szCaniBuffer, szCaniBuffer,0);

				*((CANI_LONG*)SrcNow) = dest_x;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_y;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_width;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_height;
				SrcNow += sizeof(CANI_LONG);


				if (animation->Format.format == CAF_FORMAT_YUV444 || animation->Format.format == CAF_FORMAT_YUV444_ND) {

					if (dest_width > 0 && dest_height > 0) for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * animation->Width + (dest_x + x);
							if (animation->Format.format == CAF_FORMAT_YUV444) {
								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y) - ImageBeforeYUV16[m].Y;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].Y), animation->Format.yuv.Y) - (ImageBeforeYUV8[m].Y);
									SrcNow++;
								}
								if (U16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U) - ImageBeforeYUV16[m].U;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].U), animation->Format.yuv.U) - (ImageBeforeYUV8[m].U);
									SrcNow++;
								}
								if (V16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V) - ImageBeforeYUV16[m].V;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].V), animation->Format.yuv.V) - (ImageBeforeYUV8[m].V);
									SrcNow++;
								}
							}
							else {
								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].Y), animation->Format.yuv.Y);
									SrcNow++;
								}
								if (U16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].U), animation->Format.yuv.U);
									SrcNow++;
								}
								if (V16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].V), animation->Format.yuv.V);
									SrcNow++;
								}
							}
						}
					}

				} else if (animation->Format.format == CAF_FORMAT_YUV422 || animation->Format.format == CAF_FORMAT_YUV422_ND) {

					if (dest_width > 0 && dest_height > 0) for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * animation->Width + (dest_x + x);

							if (animation->Format.format == CAF_FORMAT_YUV422) {

								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y) - ImageBeforeYUV16[m].Y;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].Y), animation->Format.yuv.Y) - (ImageBeforeYUV8[m].Y);
									SrcNow++;
								}

								if ((dest_x + x) & 0x01) {
									if (V16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V) - ImageBeforeYUV16[m].V;
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYUV8[m].V), animation->Format.yuv.V) - (ImageBeforeYUV8[m].V);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.yuv.V) - ImageBefore[m].yuvV;
									//SrcNow++;
								}
								else {
									if (U16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U) - ImageBeforeYUV16[m].U;
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYUV8[m].U), animation->Format.yuv.U) - (ImageBeforeYUV8[m].U);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.yuv.U) - ImageBefore[m].yuvU;
									//SrcNow++;
								}
							}
							else {
								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].Y), animation->Format.yuv.Y);
									SrcNow++;
								}

								//*SrcNow = CAF_Mask((CANI_BYTE)yuvY, animation->Format.yuv.Y);
								//SrcNow++;

								if ((dest_x + x) & 0x01) {
									if (V16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V);
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYUV8[m].V), animation->Format.yuv.V);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.yuv.V);
									//SrcNow++;
								}
								else {
									if (U16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U);
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYUV8[m].U), animation->Format.yuv.U);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.yuv.U);
									//SrcNow++;
								}
							}

						}
					}

				} else if (animation->Format.format == CAF_FORMAT_YUV411 || animation->Format.format == CAF_FORMAT_YUV411_ND) {

					if (dest_width > 0 && dest_height > 0) for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * animation->Width + (dest_x + x);

							if (animation->Format.format == CAF_FORMAT_YUV411) {

								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y) - ImageBeforeYUV16[m].Y;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].Y), animation->Format.yuv.Y) - (ImageBeforeYUV8[m].Y);
									SrcNow++;
								}
								//*SrcNow = CAF_Mask((CANI_BYTE)yuvY, animation->Format.yuv.Y) - ImageBefore[m].yuvY;
								//SrcNow++;

								if (((dest_x + x) & 0x01) == 0) {
									if ((dest_y + y) & 0x01) {
										if (V16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V) - ImageBeforeYUV16[m].V;
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYUV8[m].V), animation->Format.yuv.V) - (ImageBeforeYUV8[m].V);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.yuv.V) - ImageBefore[m].yuvV;
										//SrcNow++;
									}
									else {
										if (U16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U) - ImageBeforeYUV16[m].U;
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYUV8[m].U), animation->Format.yuv.U) - (ImageBeforeYUV8[m].U);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.yuv.U) - ImageBefore[m].yuvU;
										//SrcNow++;
									}
								}

							}
							else {

								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYUV8[m].Y), animation->Format.yuv.Y);
									SrcNow++;
								}
								//*SrcNow = CAF_Mask((CANI_BYTE)yuvY, animation->Format.yuv.Y);
								//SrcNow++;

								if (((dest_x + x) & 0x01) == 0) {
									if ((dest_y + y) & 0x01) {
										if (V16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V);
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYUV8[m].V), animation->Format.yuv.V);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.yuv.V);
										//SrcNow++;
									}
									else {
										if (U16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U);
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYUV8[m].U), animation->Format.yuv.U);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.yuv.U);
										//SrcNow++;
									}
								}

							}
						}
					}
				}

				for (m = 0; m < BitMapSize; m++) {

					if (Y16) ImageBeforeYUV16[m].Y = CAF_Mask16(ImageYUV16[m].Y, animation->Format.yuv.Y);
					else ImageBeforeYUV8[m].Y = CAF_Mask(ImageYUV8[m].Y, animation->Format.yuv.Y);

					if (U16) ImageBeforeYUV16[m].U = CAF_Mask16(ImageYUV16[m].U, animation->Format.yuv.U);
					else ImageBeforeYUV8[m].U = CAF_Mask(ImageYUV8[m].U, animation->Format.yuv.U);

					if (Y16) ImageBeforeYUV16[m].V = CAF_Mask16(ImageYUV16[m].V, animation->Format.yuv.V);
					else ImageBeforeYUV8[m].V = CAF_Mask(ImageYUV8[m].V, animation->Format.yuv.V);

				}

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				SrcNow += sizeof(CANI_DWORD);

				SrcNow = CANI_WriteBackgroundColorYUV( Y16, U16, V16, SrcNow, &animation->Images[n], &animation->Format);

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
				SrcNow += sizeof(CANI_DWORD);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {

						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (ImageYUV8) CANI_FREE(ImageYUV8);
						if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
						if (Image16) CANI_FREE(Image16);
						if (ImageYUV16) CANI_FREE(ImageYUV16);
						if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);
						CANI_FREE(Src);
						return FALSE;
					}

					CANI_FREE(Src); Src = 0;

					SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

						if (!CANI_CompressAndWrite(File, Src, SrcNow)) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (ImageYUV8) CANI_FREE(ImageYUV8);
							if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
							if (Image16) CANI_FREE(Image16);
							if (ImageYUV16) CANI_FREE(ImageYUV16);
							if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);
							CANI_FREE(Src);
							return FALSE;
						}

						CANI_FREE(Src); Src = 0;
					}
					else {
						CANI_WriteZeroData(File);
					}

				}
				else {

					SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
				}
			}

			if (AlphaBefore8) CANI_FREE(AlphaBefore8);
			if (AlphaBefore16) CANI_FREE(AlphaBefore16);
			if (Image8) CANI_FREE(Image8);
			if (ImageYUV8) CANI_FREE(ImageYUV8);
			if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
			if (Image16) CANI_FREE(Image16);
			if (ImageYUV16) CANI_FREE(ImageYUV16);
			if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);

		}
		else if (animation->Format.format == CAF_FORMAT_YCbCr || animation->Format.format == CAF_FORMAT_YCbCr422 || animation->Format.format == CAF_FORMAT_YCbCr411
			|| animation->Format.format == CAF_FORMAT_YCbCr_ND || animation->Format.format == CAF_FORMAT_YCbCr422_ND || animation->Format.format == CAF_FORMAT_YCbCr411_ND) {

			CANI_DWORD ColorSize = 3;
			CANI_CHAR Y16, Cb16, Cr16;
			CANI_CHAR Colors16;
			CANI_CHAR Alpha16;
			CANI_DWORD BackgroundColorSize;
			CANI_DWORD AlphaSize;
			CANI_DWORD PosAndSize = 4 * sizeof(CANI_DWORD);

			CANI_BYTE* AlphaBefore8;
			CANI_WORD* AlphaBefore16;
			CANI_BGRAQUAD* Image8;
			CANI_YCbCrAQUAD* ImageYCbCr8;
			CANI_YCbCrAQUAD* ImageBeforeYCbCr8;
			CANI_BGRA16QUAD* Image16;
			CANI_YCbCrA16QUAD* ImageYCbCr16;
			CANI_YCbCrA16QUAD* ImageBeforeYCbCr16;
			CANI_BYTE ProgressNow = 0;
			CANI_LONG dest_x, dest_y, dest_width, dest_height, x, y;

			if (animation->Format.ycbcr.Y > 8) {
				Y16 = TRUE;
				ColorSize++;
			}
			else Y16 = FALSE;

			if (animation->Format.ycbcr.Cb > 8) {
				Cb16 = TRUE;
				ColorSize++;
			}
			else Cb16 = FALSE;

			if (animation->Format.ycbcr.Cr > 8) {
				Cr16 = TRUE;
				ColorSize++;
			}
			else Cr16 = FALSE;

			Colors16 = (ColorSize > 3 ? TRUE : FALSE);
			Alpha16 = (animation->Format.ycbcr.Alpha > 8 ? TRUE : FALSE);

			BackgroundColorSize = ColorSize;// (Colors16 /*|| Alpha16*/) ? (3 * sizeof(CANI_WORD)) : (3 * sizeof(CANI_BYTE));
			AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

			//MessageBoxA(0, "CAF_FORMAT_CMYK", "", 0);

			if (Progress) Progress(0);

			if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

				SrcLen = CANI_CalculateGlobalAlphaSize(animation, Alpha16);

				if (SrcLen) {
					Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
					if (Src == 0) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}
					SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, Src);

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {
						CANI_FREE(Src); return FALSE;
					}

					CANI_FREE(Src); Src = 0;
				}
				else {
					CANI_WriteZeroData(File);
				}
			}
			else {

				SrcLen64 = (CANI_QWORD)animation->ImagesNR * (CANI_QWORD)(PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

				SrcLen64 += CANI_CalculateAlphaSize(animation, Alpha16);

				if (SrcLen64 > CANI_MAX_32BIT_SIZE) {
					CANI_SetLastError(CANI_ERROR_MEMORY_TOO_LARGE);
					return 0; // (CANI_QWORD)
				}

				SrcLen = (CANI_DWORD)SrcLen64;

				Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
				if (Src == 0) {
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return FALSE;
				}
				SrcNow = Src;

				SrcNow = CANI_WriteGlobalAlpha(animation, Alpha16, SrcNow);
			}

			AlphaBefore8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BYTE));
			AlphaBefore16 = CANI_MALLOC(BitMapSize * sizeof(CANI_WORD));

			Image8 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
			ImageYCbCr8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrAQUAD));
			ImageBeforeYCbCr8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrAQUAD));

			Image16 = CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
			ImageYCbCr16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrA16QUAD));
			ImageBeforeYCbCr16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrA16QUAD));

			if (AlphaBefore8 == 0 || AlphaBefore16 == 0
				|| Image8 == 0 || ImageYCbCr8 == 0 || ImageBeforeYCbCr8 == 0
				|| Image16 == 0 || ImageYCbCr16 == 0 || ImageBeforeYCbCr16 == 0) {

				if (AlphaBefore8) CANI_FREE(AlphaBefore8);
				if (AlphaBefore16) CANI_FREE(AlphaBefore16);
				if (Image8) CANI_FREE(Image8);
				if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
				if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
				if (Image16) CANI_FREE(Image16);
				if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
				if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
			CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

			CANI_MEMSET(Image8, 0x00, (BitMapSize * sizeof(CANI_BGRAQUAD)));
			CANI_MEMSET(ImageYCbCr8, 0x00, (BitMapSize * sizeof(CANI_YCbCrAQUAD)));
			CANI_MEMSET(ImageBeforeYCbCr8, 0x00, (BitMapSize * sizeof(CANI_YCbCrAQUAD)));

			CANI_MEMSET(Image16, 0x00, (BitMapSize * sizeof(CANI_BGRA16QUAD)));
			CANI_MEMSET(ImageYCbCr16, 0x00, (BitMapSize * sizeof(CANI_YCbCrA16QUAD)));
			CANI_MEMSET(ImageBeforeYCbCr16, 0x00, (BitMapSize * sizeof(CANI_YCbCrA16QUAD)));

			for (n = 0; n < animation->ImagesNR; n++) {

				if (Progress && (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				//CANI_ConvertImage(TRUE, Image8, Image16, &animation->Images[n], BitMapSize);

				if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {
					if (Image8) CANI_BGRA16_2_BGRA(Image8, (CANI_BGRA16QUAD*)animation->Images[n].Image, BitMapSize);
					if (Image16) CANI_MEMCPY(Image16, (CANI_BGRA16QUAD*)animation->Images[n].Image, BitMapSize * sizeof(CANI_BGRA16QUAD));
					CANI_RGBA16_2_RGBA(&animation->Images[n].bgColorRGB, &animation->Images[n].bgColorRGB16, 1);
				}
				else {
					if (Image8) CANI_MEMCPY(Image8, (CANI_BGRAQUAD*)animation->Images[n].Image, BitMapSize * sizeof(CANI_BGRAQUAD));
					if (Image16) CANI_BGRA_2_BGRA16(Image16, (CANI_BGRAQUAD*)animation->Images[n].Image, BitMapSize);
					CANI_RGBA_2_RGBA16(&animation->Images[n].bgColorRGB16, &animation->Images[n].bgColorRGB, 1);
				}

				CANI_BGRA16_2_YCbCrA16(ImageYCbCr16, Image16, BitMapSize);

				CANI_BGRA_2_YCbCrA(ImageYCbCr8, Image8, BitMapSize);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					SrcLen = (PosAndSize+(BitMapSize * ColorSize) + sizeof(CANI_DWORD) + BackgroundColorSize + sizeof(CANI_DWORD));

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
							if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
							if (Image16) CANI_FREE(Image16);
							if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
							if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = Src;
					}
				}

				dest_x = 0;
				dest_y = 0;
				dest_width = animation->Width;
				dest_height = animation->Height;

				//char szCaniBuffer[1024];

				if ((animation->Format.options & CANI_OPTION_CALCULATE_CHANGE_AREA) && n > 0) {
					for (x = 0; x < ((CANI_LONG)animation->Width); x++) {
						for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
							m = y * animation->Width + x;
							if (Y16) {
								if (ImageBeforeYCbCr16[m].Y != CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y)) break;
							}
							else {
								if (ImageBeforeYCbCr8[m].Y != CAF_Mask(ImageYCbCr8[m].Y, animation->Format.ycbcr.Y)) break;
							}
							if (Cb16) {
								if (ImageBeforeYCbCr16[m].Cb != CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb)) break;
							}
							else {
								if (ImageBeforeYCbCr8[m].Cb != CAF_Mask(ImageYCbCr8[m].Cb, animation->Format.ycbcr.Cb)) break;
							}
							if (Cr16) {
								if (ImageBeforeYCbCr16[m].Cr != CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr)) break;
							}
							else {
								if (ImageBeforeYCbCr8[m].Cr != CAF_Mask(ImageYCbCr8[m].Cr, animation->Format.ycbcr.Cr)) break;
							}
						}

						if (y < ((CANI_LONG)animation->Height)) break;
					}

					dest_x = x;

					if (dest_x < ((CANI_LONG)animation->Width)) {
						for (x = ((CANI_LONG)animation->Width) - 1; x > dest_x; x--) {
							for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
								m = y * ((CANI_LONG)animation->Width) + x;
								if (Y16) {
									if (ImageBeforeYCbCr16[m].Y != CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y)) break;
								}
								else {
									if (ImageBeforeYCbCr8[m].Y != CAF_Mask(ImageYCbCr8[m].Y, animation->Format.ycbcr.Y)) break;
								}
								if (Cb16) {
									if (ImageBeforeYCbCr16[m].Cb != CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb)) break;
								}
								else {
									if (ImageBeforeYCbCr8[m].Cb != CAF_Mask(ImageYCbCr8[m].Cb, animation->Format.ycbcr.Cb)) break;
								}
								if (Cr16) {
									if (ImageBeforeYCbCr16[m].Cr != CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr)) break;
								}
								else {
									if (ImageBeforeYCbCr8[m].Cr != CAF_Mask(ImageYCbCr8[m].Cr, animation->Format.ycbcr.Cr)) break;
								}
							}

							if (y < ((CANI_LONG)animation->Height)) break;
						}

						dest_width = x - dest_x + 1;

						//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_height, dest_height);

						//MessageBoxA(0, szCaniBuffer, szCaniBuffer, 0);
					}
					else {
						dest_x = 0;
						dest_width = 0;
					}

					for (y = 0; y < ((CANI_LONG)animation->Height); y++) {
						for (x = 0; x < dest_width; x++) {
							m = y * animation->Width + dest_x + x;
							if (Y16) {
								if (ImageBeforeYCbCr16[m].Y != CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y)) break;
							}
							else {
								if (ImageBeforeYCbCr8[m].Y != CAF_Mask(ImageYCbCr8[m].Y, animation->Format.ycbcr.Y)) break;
							}
							if (Cb16) {
								if (ImageBeforeYCbCr16[m].Cb != CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb)) break;
							}
							else {
								if (ImageBeforeYCbCr8[m].Cb != CAF_Mask(ImageYCbCr8[m].Cb, animation->Format.ycbcr.Cb)) break;
							}
							if (Cr16) {
								if (ImageBeforeYCbCr16[m].Cr != CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr)) break;
							}
							else {
								if (ImageBeforeYCbCr8[m].Cr != CAF_Mask(ImageYCbCr8[m].Cr, animation->Format.ycbcr.Cr)) break;
							}
						}
						if (x < dest_width) break;
					}

					dest_y = y;

					if (dest_y < ((CANI_LONG)animation->Height)) {
						for (y = ((CANI_LONG)animation->Height) - 1; y > dest_y; y--) {
							for (x = 0; x < dest_width; x++) {
								m = y * animation->Width + dest_x + x;
								if (Y16) {
									if (ImageBeforeYCbCr16[m].Y != CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y)) break;
								}
								else {
									if (ImageBeforeYCbCr8[m].Y != CAF_Mask(ImageYCbCr8[m].Y, animation->Format.ycbcr.Y)) break;
								}
								if (Cb16) {
									if (ImageBeforeYCbCr16[m].Cb != CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb)) break;
								}
								else {
									if (ImageBeforeYCbCr8[m].Cb != CAF_Mask(ImageYCbCr8[m].Cb, animation->Format.ycbcr.Cb)) break;
								}
								if (Cr16) {
									if (ImageBeforeYCbCr16[m].Cr != CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr)) break;
								}
								else {
									if (ImageBeforeYCbCr8[m].Cr != CAF_Mask(ImageYCbCr8[m].Cr, animation->Format.ycbcr.Cr)) break;
								}
							}
							if (x < dest_width) break;
						}

						dest_height = y - dest_y + 1;
					}
					else {
						dest_y = 0;
						dest_height = 0;
					}

				}

				//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_width, dest_height);

				//MessageBoxA(0, szCaniBuffer, szCaniBuffer,0);

				*((CANI_LONG*)SrcNow) = dest_x;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_y;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_width;
				SrcNow += sizeof(CANI_LONG);
				*((CANI_LONG*)SrcNow) = dest_height;
				SrcNow += sizeof(CANI_LONG);


				if (animation->Format.format == CAF_FORMAT_YCbCr || animation->Format.format == CAF_FORMAT_YCbCr_ND) {

					if (dest_width > 0 && dest_height > 0) for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * animation->Width + (dest_x + x);
							if (animation->Format.format == CAF_FORMAT_YCbCr) {
								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y) - ImageBeforeYCbCr16[m].Y;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Y), animation->Format.ycbcr.Y) - (ImageBeforeYCbCr8[m].Y);
									SrcNow++;
								}
								if (Cb16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb) - ImageBeforeYCbCr16[m].Cb;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Cb), animation->Format.ycbcr.Cb) - (ImageBeforeYCbCr8[m].Cb);
									SrcNow++;
								}
								if (Cr16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr) - ImageBeforeYCbCr16[m].Cr;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Cr), animation->Format.ycbcr.Cr) - (ImageBeforeYCbCr8[m].Cr);
									SrcNow++;
								}
							}
							else {
								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Y), animation->Format.ycbcr.Y);
									SrcNow++;
								}
								if (Cb16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Cb), animation->Format.ycbcr.Cb);
									SrcNow++;
								}
								if (Cr16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Cr), animation->Format.ycbcr.Cr);
									SrcNow++;
								}
							}
						}
					}

				}
				else if (animation->Format.format == CAF_FORMAT_YCbCr422 || animation->Format.format == CAF_FORMAT_YCbCr422_ND) {

					if (dest_width > 0 && dest_height > 0) for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * animation->Width + (dest_x + x);

							if (animation->Format.format == CAF_FORMAT_YCbCr422) {

								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y) - ImageBeforeYCbCr16[m].Y;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Y), animation->Format.ycbcr.Y) - (ImageBeforeYCbCr8[m].Y);
									SrcNow++;
								}

								if ((dest_x + x) & 0x01) {
									if (Cr16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr) - ImageBeforeYCbCr16[m].Cr;
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYCbCr8[m].Cr), animation->Format.ycbcr.Cr) - (ImageBeforeYCbCr8[m].Cr);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.ycbcr.Cr) - ImageBefore[m].yuvV;
									//SrcNow++;
								}
								else {
									if (Cb16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb) - ImageBeforeYCbCr16[m].Cb;
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYCbCr8[m].Cb), animation->Format.ycbcr.Cb) - (ImageBeforeYCbCr8[m].Cb);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.ycbcr.Cb) - ImageBefore[m].yuvU;
									//SrcNow++;
								}
							}
							else {
								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Y), animation->Format.ycbcr.Y);
									SrcNow++;
								}

								//*SrcNow = CAF_Mask((CANI_BYTE)yuvY, animation->Format.ycbcr.Y);
								//SrcNow++;

								if ((dest_x + x) & 0x01) {
									if (Cr16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr);
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYCbCr8[m].Cr), animation->Format.ycbcr.Cr);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.ycbcr.Cr);
									//SrcNow++;
								}
								else {
									if (Cb16) {
										*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb);
										SrcNow += 2;
									}
									else {
										*SrcNow = CAF_Mask((ImageYCbCr8[m].Cb), animation->Format.ycbcr.Cb);
										SrcNow++;
									}
									//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.ycbcr.Cb);
									//SrcNow++;
								}
							}

						}
					}

				}
				else if (animation->Format.format == CAF_FORMAT_YCbCr411 || animation->Format.format == CAF_FORMAT_YCbCr411_ND) {

					if (dest_width > 0 && dest_height > 0) for (y = 0; y < dest_height; y++) {
						for (x = 0; x < dest_width; x++) {
							m = (dest_y + y) * animation->Width + (dest_x + x);

							if (animation->Format.format == CAF_FORMAT_YCbCr411) {

								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y) - ImageBeforeYCbCr16[m].Y;
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Y), animation->Format.ycbcr.Y) - (ImageBeforeYCbCr8[m].Y);
									SrcNow++;
								}
								//*SrcNow = CAF_Mask((CANI_BYTE)yuvY, animation->Format.ycbcr.Y) - ImageBefore[m].yuvY;
								//SrcNow++;

								if (((dest_x + x) & 0x01) == 0) {
									if ((dest_y + y) & 0x01) {
										if (Cr16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr) - ImageBeforeYCbCr16[m].Cr;
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYCbCr8[m].Cr), animation->Format.ycbcr.Cr) - (ImageBeforeYCbCr8[m].Cr);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.ycbcr.Cr) - ImageBefore[m].yuvV;
										//SrcNow++;
									}
									else {
										if (Cb16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb) - ImageBeforeYCbCr16[m].Cb;
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYCbCr8[m].Cb), animation->Format.ycbcr.Cb) - (ImageBeforeYCbCr8[m].Cb);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.ycbcr.Cb) - ImageBefore[m].yuvU;
										//SrcNow++;
									}
								}

							}
							else {

								if (Y16) {
									*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y);
									SrcNow += 2;
								}
								else {
									*SrcNow = CAF_Mask((ImageYCbCr8[m].Y), animation->Format.ycbcr.Y);
									SrcNow++;
								}
								//*SrcNow = CAF_Mask((CANI_BYTE)yuvY, animation->Format.ycbcr.Y);
								//SrcNow++;

								if (((dest_x + x) & 0x01) == 0) {
									if ((dest_y + y) & 0x01) {
										if (Cr16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr);
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYCbCr8[m].Cr), animation->Format.ycbcr.Cr);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvV, animation->Format.ycbcr.Cr);
										//SrcNow++;
									}
									else {
										if (Cb16) {
											*((CANI_WORD*)SrcNow) = CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb);
											SrcNow += 2;
										}
										else {
											*SrcNow = CAF_Mask((ImageYCbCr8[m].Cb), animation->Format.ycbcr.Cb);
											SrcNow++;
										}
										//*SrcNow = CAF_Mask((CANI_BYTE)yuvU, animation->Format.ycbcr.Cb);
										//SrcNow++;
									}
								}

							}
						}
					}
				}

				for (m = 0; m < BitMapSize; m++) {

					if (Y16) ImageBeforeYCbCr16[m].Y = CAF_Mask16(ImageYCbCr16[m].Y, animation->Format.ycbcr.Y);
					else ImageBeforeYCbCr8[m].Y = CAF_Mask(ImageYCbCr8[m].Y, animation->Format.ycbcr.Y);

					if (Cb16) ImageBeforeYCbCr16[m].Cb = CAF_Mask16(ImageYCbCr16[m].Cb, animation->Format.ycbcr.Cb);
					else ImageBeforeYCbCr8[m].Cb = CAF_Mask(ImageYCbCr8[m].Cb, animation->Format.ycbcr.Cb);

					if (Y16) ImageBeforeYCbCr16[m].Cr = CAF_Mask16(ImageYCbCr16[m].Cr, animation->Format.ycbcr.Cr);
					else ImageBeforeYCbCr8[m].Cr = CAF_Mask(ImageYCbCr8[m].Cr, animation->Format.ycbcr.Cr);

				}

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Time;
				SrcNow += sizeof(CANI_DWORD);

				SrcNow = CANI_WriteBackgroundColorYCbCr(Y16, Cb16, Cr16, SrcNow, &animation->Images[n], &animation->Format);

				*((CANI_DWORD*)SrcNow) = animation->Images[n].Style;
				SrcNow += sizeof(CANI_DWORD);

				if (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) {

					if (!CANI_CompressAndWrite(File, Src, SrcNow)) {

						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
						if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
						if (Image16) CANI_FREE(Image16);
						if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
						if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);
						CANI_FREE(Src);
						return FALSE;
					}

					CANI_FREE(Src); Src = 0;

					SrcLen = CANI_CalculateLocalAlphaSize(animation, n, Alpha16);

					if (SrcLen) {
						Src = CANI_MALLOC(SrcLen * sizeof(CANI_BYTE));
						if (Src == 0) {
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return FALSE;
						}
						SrcNow = CANI_WriteAlpha(animation, n, Alpha16, Src, Image8, Image16, AlphaBefore8, AlphaBefore16);

						if (!CANI_CompressAndWrite(File, Src, SrcNow)) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
							if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
							if (Image16) CANI_FREE(Image16);
							if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
							if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);
							CANI_FREE(Src);
							return FALSE;
						}

						CANI_FREE(Src); Src = 0;
					}
					else {
						CANI_WriteZeroData(File);
					}

				}
				else {

					SrcNow = CANI_WriteAlpha(animation, n, Alpha16, SrcNow, Image8, Image16, AlphaBefore8, AlphaBefore16);
				}
			}

			if (AlphaBefore8) CANI_FREE(AlphaBefore8);
			if (AlphaBefore16) CANI_FREE(AlphaBefore16);
			if (Image8) CANI_FREE(Image8);
			if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
			if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
			if (Image16) CANI_FREE(Image16);
			if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
			if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);
		}

		if ( (animation->Format.options & CANI_OPTION_COMPRESS_SEPARATELY) == 0 ) {

			CANI_DWORD DestLen = 2 * SrcLen + 100; // CANI_QWORD
			CANI_BYTE* Dest = CANI_MALLOC(DestLen * sizeof(CANI_BYTE));// new CANI_BYTE[DestLen];

			if (Dest) {

				SrcLen = (CANI_DWORD)(SrcNow - Src);

				//sprintf( szCaniBuffer, "%u", SrcLen );

				//MessageBoxA( 0, szCaniBuffer, "SrcLen", 0 );

				compress2(Dest, &DestLen, Src, SrcLen, 9);

				if (animation->Format.options & CANI_OPTION_DOUBLE_COMPRESSION) {
					CANI_DWORD DestLen2 = 2 * SrcLen + 100;
					CANI_BYTE* Dest2 = CANI_MALLOC(DestLen2 * sizeof(CANI_BYTE));//new CANI_BYTE[DestLen2];

					if (Dest2) {

						compress2(Dest2, &DestLen2, Dest, DestLen, 9);

						CANI_FWRITE(&DestLen2, 1, sizeof(DestLen2), File);

						CANI_FWRITE(&DestLen, 1, sizeof(DestLen), File);

						CANI_FWRITE(&SrcLen, 1, sizeof(SrcLen), File);

						CANI_FWRITE(Dest2, 1, DestLen2, File);

						CANI_FREE(Dest2);
					}
					else {
						CANI_FREE(Dest);
						CANI_FREE(Src);
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return FALSE;
					}

				}
				else {

					CANI_FWRITE(&DestLen, 1, sizeof(DestLen), File);

					CANI_FWRITE(&SrcLen, 1, sizeof(SrcLen), File);

					CANI_FWRITE(Dest, 1, DestLen, File);

				}

				//delete[] Dest;
				//delete[] Src;
				CANI_FREE(Dest);
				CANI_FREE(Src);

			}
			else {
				CANI_FREE(Src);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}
		}

		if (Progress) Progress(100);

		return TRUE;

	}

	return FALSE;
}


#ifdef CANI_UNICODE_FILE

CANI_EXPORT CANI_CHAR CANI_SaveAnimationW(CANI_WORD* FileName, CANI_ANIMATION* animation, CANI_PROGRESS_FUNC Progress) {

	FILE* File = CANI_FOPEN_UTF16(FileName, L"wb");

	if (File) {
		CANI_CHAR rv = CANI_WriteAnimation(File, animation, Progress );
		CANI_FCLOSE(File);
		if (rv == FALSE) CANI_FDELETE_UTF16(FileName);
		return rv;
	}
	return FALSE;
}

#endif

CANI_EXPORT CANI_CHAR CANI_SaveAnimationA(CANI_CHAR* FileName, CANI_ANIMATION *animation, CANI_PROGRESS_FUNC Progress) {

	FILE* File = CANI_FOPEN_ASCII(FileName, "wb");

	if (File) {
		CANI_CHAR rv = CANI_WriteAnimation(File, animation, Progress );
		CANI_FCLOSE(File);
		if (rv == FALSE) CANI_FDELETE_ASCII(FileName);
		return rv;
	}
	return FALSE;
}