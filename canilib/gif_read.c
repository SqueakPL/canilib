#include "cani.h"

#ifdef CANI_USE_GIFLIB

CANI_ANIMATION* CANI_ReadGIF_FILE(FILE* file, CANI_PROGRESS_FUNC Progress) {
	if (file) {

		int Error;

		GifFileType* gifFile = DGifOpenFile(file, &Error);

		if (gifFile) {
			CANI_ANIMATION* rv = 0;
			CANI_BGRAQUAD* Image;
			CANI_DWORD Size;
			CANI_LONG n;
			CANI_CHAR disposal_method = -1;
			int delay_time = 0;// 67;
			int trans_value = -1;
			int i;
			CANI_BYTE ProgressNow = 0;
			GifColorType rgb_bg;

			//char szCaniBuffer[1024];
			//sprintf(szCaniBuffer, "%u %u", gft->SWidth, gft->SHeight);
			//MessageBoxA(0, szCaniBuffer, "", 0);

			if (DGifSlurp(gifFile) == GIF_ERROR) {
				DGifCloseFile(gifFile, &Error);
				return 0;
			}

			rv = CANI_MALLOC(sizeof(CANI_ANIMATION));

			if (!rv) {
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));

			rv->Width = gifFile->SWidth;
			rv->Height = gifFile->SHeight;

			rv->Format.format = CAF_FORMAT_RGB;
			rv->Format.rgb.Red = 8;
			rv->Format.rgb.Green = 8;
			rv->Format.rgb.Blue = 8;
			rv->Format.rgb.Alpha = 8;
			rv->ImagesNR = gifFile->ImageCount;

			Size = rv->Width * rv->Height;

			rv->Images = CANI_MALLOC(gifFile->ImageCount*sizeof(CANI_FRAME_DATA));

			if (!rv->Images) {
				CANI_FREE(rv->Images);
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			CANI_MEMSET(rv->Images, 0x00, gifFile->ImageCount * sizeof(CANI_FRAME_DATA));

			for ( i = 0; i < gifFile->ImageCount; ++i) {

				rv->Images[i].Image = (CANI_BYTE*)CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));

				if (rv->Images[i].Image == 0) {
					CANI_DWORD m;
					for (m = i; m > 0; m--) {
						CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
					}
					CANI_FREE(rv->Images);
					CANI_FREE(rv);
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}
			}

			for ( i = 0; i < gifFile->ImageCount; ++i) {

				const SavedImage *saved;

				const GifImageDesc *desc;

				const ColorMapObject *colorMap;

				int v,u,c;

				if (Progress) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)( i * 100 / gifFile->ImageCount );

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				rgb_bg = gifFile->SColorMap->Colors[gifFile->SBackGroundColor];

				Image = (CANI_BGRAQUAD*)( rv->Images[i].Image );

				if (i == 0) {

					CANI_DWORD pos;

					for ( pos = 0; pos < Size; pos++ ) {

						Image[pos].Red = rgb_bg.Red;
						Image[pos].Green = rgb_bg.Green;
						Image[pos].Blue = rgb_bg.Blue;
						Image[pos].Alpha = 0xFF;

					}
				}

				saved = &gifFile->SavedImages[i];

				desc = &saved->ImageDesc;

				colorMap = desc->ColorMap ? desc->ColorMap : gifFile->SColorMap;

				for (n = 0; n < saved->ExtensionBlockCount; n++) {

					ExtensionBlock* ExtensionBlock = &saved->ExtensionBlocks[n];

					if (ExtensionBlock->Function == GRAPHICS_EXT_FUNC_CODE) {

						disposal_method = ((ExtensionBlock->Bytes[0] >> 2) & 0x07);

						delay_time = *((CANI_WORD*)&(ExtensionBlock->Bytes[1]));
						if (delay_time) delay_time *= 10;
						else delay_time = 67;

						if (ExtensionBlock->Bytes[0] & 0x01) {
							trans_value = ExtensionBlock->Bytes[3];
						}// else trans_value = -1;
					}
				}

				rv->Images[i].Time = delay_time;

				//if (bg_colour) {

					rv->Images[i].bgColorRGB.Red = rgb_bg.Red;
					rv->Images[i].bgColorRGB.Green = rgb_bg.Green;
					rv->Images[i].bgColorRGB.Blue = rgb_bg.Blue;
					rv->Images[i].bgColorRGB.Alpha = 0xFF;
					rv->Images[i].Style = CANI_STYLE_TRANSPARENT;

				/*}
				else {

					rv->Images[image].bgColor = 0x00000000;
					rv->Images[image].Style = 0;
				}*/

				for (v = 0; v < desc->Height; ++v) {

					for (u = 0; u < desc->Width; ++u) {

						c = saved->RasterBits[v * desc->Width + u];

						//printf(" %02X", c);

						if (colorMap) {

							GifColorType rgb = colorMap->Colors[c];

							if (trans_value<0 || trans_value != c ) { // colour->alpha) {

								long pos = (rv->Height - (v + desc->Top) - 1) * rv->Width + desc->Left + u;

								Image[pos].Red = rgb.Red;
								Image[pos].Green = rgb.Green;
								Image[pos].Blue = rgb.Blue;
								Image[pos].Alpha = 0xFF;

							}

							//ss << " [" << (int)rgb.Red << "," << (int)rgb.Green << "," << (int)rgb.Blue << "]";

						}

					}

				}



				if (i < (gifFile->ImageCount - 1)) CANI_MEMCPY(rv->Images[i+1].Image, rv->Images[i].Image, Size * sizeof(CANI_BGRAQUAD));


				if (disposal_method == 2 && i < (gifFile->ImageCount-1) ) {

					GifColorType rgb = colorMap->Colors[gifFile->SBackGroundColor];

					int v,u;

					Image = (CANI_BGRAQUAD*)(rv->Images[i+1].Image);

					for ( v = 0; v < desc->Height; ++v) {

						for ( u = 0; u < desc->Width; ++u) {

							long pos = (rv->Height - (v + desc->Top) - 1) * rv->Width + desc->Left + u;

							Image[pos].Red = rgb.Red;
							Image[pos].Green = rgb.Green;
							Image[pos].Blue = rgb.Blue;
							Image[pos].Alpha = 0xFF;

						}
					}
				}


			}

			DGifCloseFile(gifFile,&Error);

			return rv;
		}
	}

	return 0;
}

#endif