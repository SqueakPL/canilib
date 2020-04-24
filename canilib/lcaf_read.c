#pragma once

#include "cani.h"

//#include "..\jpeg\jpeglib.h"

CANI_CHAR CANI_JPEG_HeaderIsOkay(FILE* file) {

	CANI_BYTE id[3];
	CANI_FREAD(id, 3, 1, file);
	CANI_FSEEK(file, -3, SEEK_CUR);
	if (id[0] != 0xFF) {
		//MessageBoxA(0, "error", "CANI_JPEG_HeaderIsOkay", 0);
		return FALSE;
	}
	if (id[1] != 0xD8 && id[1] != 0xE0) {
		//MessageBoxA(0, "error", "CANI_JPEG_HeaderIsOkay", 0);
		return FALSE;
	}

	return TRUE;
}

CANI_CHAR CANI_ReadJPEG(FILE* file, CANI_BGRA16QUAD* ImageRGB, CANI_LONG image_width, CANI_LONG image_height) {

	CANI_DWORD width, height;
	CANI_DWORD i, j, n;
	CANI_DWORD row_stride;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPLE* buffer;

	if (!CANI_JPEG_HeaderIsOkay(file)) return 0;

	// Error handler
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file);

	// Read header
	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) return 0;
	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;

	if (width != image_width || height != image_height) return FALSE;

	if (cinfo.output_components != 4 && cinfo.output_components != 3 && cinfo.output_components != 1) return FALSE;

	if (cinfo.out_color_space != JCS_CMYK && cinfo.out_color_space != JCS_RGB && cinfo.out_color_space != JCS_GRAYSCALE)  return FALSE;

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (JSAMPLE*)CANI_MALLOC(row_stride * sizeof(JSAMPLE));// new JSAMPLE[row_stride];
	if (buffer == 0) {
		return FALSE;
	}

	j = cinfo.output_height - 1;

	if (cinfo.output_components == 1) {

		if (cinfo.out_color_space == JCS_GRAYSCALE) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {
					ImageRGB[j * cinfo.output_width + i].Red = (buffer[i]<<8)| buffer[i];
					ImageRGB[j * cinfo.output_width + i].Green = (buffer[i] << 8) | buffer[i];
					ImageRGB[j * cinfo.output_width + i].Blue = (buffer[i] << 8) | buffer[i];
					//ImageRGB[j * cinfo.output_width + i].Alpha = 0xFF;
				}
				j--;
			}
		}

	}
	else if (cinfo.output_components == 4) {

		if (cinfo.out_color_space == JCS_CMYK) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {
					CANI_DWORD tmp = buffer[i * 4 + 3];

					CANI_DWORD Value = ((CANI_DWORD)buffer[i * 4] * tmp / 255);
					ImageRGB[j * cinfo.output_width + i].Red = (CANI_WORD)((Value<<8)| Value);

					Value = ((CANI_DWORD)buffer[i * 4 + 1] * tmp / 255);
					ImageRGB[j * cinfo.output_width + i].Green = (CANI_WORD)((Value << 8) | Value);

					Value = ((CANI_DWORD)buffer[i * 4 + 2] * tmp / 255);
					ImageRGB[j * cinfo.output_width + i].Blue = (CANI_WORD)((Value << 8) | Value);

					//ImageRGB[j * cinfo.output_width + i].Alpha = 0xFF;


				}
				j--;
			}

		}

	}
	else {

		if (cinfo.out_color_space == JCS_RGB) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {

					CANI_WORD Value = ((CANI_WORD)buffer[3 * i]);
					ImageRGB[j * cinfo.output_width + i].Red = (CANI_WORD)((Value << 8) | Value);

					Value = ((CANI_WORD)buffer[3 * i + 1]);
					ImageRGB[j * cinfo.output_width + i].Green = (CANI_WORD)((Value << 8) | Value);

					Value = ((CANI_WORD)buffer[3 * i + 2]);
					ImageRGB[j * cinfo.output_width + i].Blue = (CANI_WORD)((Value << 8) | Value);
				}
				j--;
			}

		}
	}

	// Finish
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	CANI_FREE(buffer);// delete[] buffer;
	return TRUE;
}

CANI_CHAR CANI_ReadAlphaJPEG(FILE* file, CANI_BYTE* ImageA, CANI_LONG image_width, CANI_LONG image_height) {

	CANI_DWORD width, height;
	CANI_DWORD i, j, n;
	CANI_DWORD row_stride;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPLE* buffer;

	if (!CANI_JPEG_HeaderIsOkay(file)) return 0;

	// Error handler
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file);

	// Read header
	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) return 0;
	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;

	if (width != image_width || height != image_height) return FALSE;

	if (cinfo.output_components != 4 && cinfo.output_components != 3 && cinfo.output_components != 1) return FALSE;
	if (cinfo.out_color_space != JCS_GRAYSCALE)  return FALSE;

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (JSAMPLE*)CANI_MALLOC(row_stride * sizeof(JSAMPLE));// new JSAMPLE[row_stride];
	if (buffer == 0) {
		return FALSE;
	}

	j = cinfo.output_height - 1;

	if (cinfo.output_components == 1) {

		if (cinfo.out_color_space == JCS_GRAYSCALE) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {
					ImageA[j * cinfo.output_width + i] = buffer[i];
				}
				j--;
			}
		}

	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	CANI_FREE(buffer);// delete[] buffer;
	return TRUE;
}

CANI_CHAR CANI_ReadAlphaJPEG_RGB(FILE* file, CANI_BGRA16QUAD* ImageRGB, CANI_LONG image_width, CANI_LONG image_height) {

	CANI_DWORD width, height;
	CANI_DWORD i, j, n;
	CANI_DWORD row_stride;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPLE* buffer;

	if (!CANI_JPEG_HeaderIsOkay(file)) return 0;

	// Error handler
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file);

	// Read header
	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) return 0;
	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;

	if (width != image_width || height != image_height) return FALSE;

	if (cinfo.output_components != 4 && cinfo.output_components != 3 && cinfo.output_components != 1) return FALSE;
	if (cinfo.out_color_space != JCS_GRAYSCALE)  return FALSE;

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (JSAMPLE*)CANI_MALLOC(row_stride * sizeof(JSAMPLE));// new JSAMPLE[row_stride];
	if (buffer == 0) {
		return FALSE;
	}

	j = cinfo.output_height - 1;

	if (cinfo.output_components == 1) {

		if (cinfo.out_color_space == JCS_GRAYSCALE) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {
					ImageRGB[j * cinfo.output_width + i].Alpha = buffer[i];
				}
				j--;
			}
		}

	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	CANI_FREE(buffer);// delete[] buffer;
	return TRUE;
}


CANI_ANIMATION *CANI_ReadJPEG_FILE(FILE* file) {

	CANI_DWORD width, height;
	CANI_DWORD i, j, n;
	CANI_DWORD row_stride;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPLE* buffer;
	CANI_ANIMATION* rv;
	//MessageBoxA(0, "1", "CANI_ReadJPEG_FILE", 0);

	if (!CANI_JPEG_HeaderIsOkay(file)) return 0;

	// Error handler
	cinfo.err = jpeg_std_error(&jerr);
	//MessageBoxA(0, "2", "CANI_ReadJPEG_FILE", 0);

	jpeg_create_decompress(&cinfo);
	//MessageBoxA(0, "3", "CANI_ReadJPEG_FILE", 0);
	jpeg_stdio_src(&cinfo, file);
	//MessageBoxA(0, "4", "CANI_ReadJPEG_FILE", 0);

	// Read header

	//char szCaniBuffer[ 1024];
	//sprintf(szCaniBuffer, "%u", jpeg_read_header(&cinfo, TRUE));
	//MessageBoxA(0, szCaniBuffer, "jpeg_read_header", 0);
	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) return 0;
	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;

	if (cinfo.output_components != 4 && cinfo.output_components != 3 && cinfo.output_components != 1) return 0;

	if (cinfo.out_color_space != JCS_CMYK && cinfo.out_color_space != JCS_RGB && cinfo.out_color_space != JCS_GRAYSCALE)  return 0;

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (JSAMPLE*)CANI_MALLOC(row_stride * sizeof(JSAMPLE));// new JSAMPLE[row_stride];
	if (buffer == 0) {
		return 0;
	}

	rv = (CANI_ANIMATION*)CANI_MALLOC(sizeof(CANI_ANIMATION));
	if (rv == 0) {
		CANI_FREE(buffer);
		return 0;
	}

	CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));

	rv->Images = (CANI_FRAME_DATA*)CANI_MALLOC(sizeof(CANI_FRAME_DATA));
	if (rv->Images == 0) {
		CANI_FREE(rv);
		CANI_FREE(buffer);
		return 0;
	}

	rv->Images[0].Image = (CANI_BYTE*)CANI_MALLOC(width* height*sizeof(CANI_BGRAQUAD));
	if (rv->Images[0].Image == 0) {
		CANI_FREE(rv->Images);
		CANI_FREE(rv);
		CANI_FREE(buffer);
		return 0;
	}

	rv->Format.format = CAF_FORMAT_RGB;
	rv->Format.rgb.Red = 8;
	rv->Format.rgb.Green = 8;
	rv->Format.rgb.Blue = 8;
	rv->Format.rgb.Alpha = 8;

	rv->Width = width;
	rv->Height = height;
	rv->ImagesNR = 1;
	rv->ImagesNow = 0;
	rv->Time = 0;

	rv->Images[0].bgColor = 0;
	rv->Images[0].Style = 0;
	rv->Images[0].Time = 0;

	j = cinfo.output_height - 1;

	if (cinfo.output_components == 1) {

		if (cinfo.out_color_space == JCS_GRAYSCALE) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Red = buffer[i];
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Green = buffer[i];
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Blue = buffer[i];
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Alpha = 0xFF;
				}
				j--;
			}
		}

	}
	else if (cinfo.output_components == 4) {

		if (cinfo.out_color_space == JCS_CMYK) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {
					CANI_DWORD tmp = buffer[i * 4 + 3];
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Red = (CANI_BYTE)((CANI_DWORD)buffer[i * 4] * tmp / 255);
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Green = (CANI_BYTE)((CANI_DWORD)buffer[i * 4 + 1] * tmp / 255);
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Blue = (CANI_BYTE)((CANI_DWORD)buffer[i * 4 + 2] * tmp / 255);
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Alpha = 0xFF;

					//ImageRGB[j * cinfo.output_width + i].Alpha = 0xFF;


				}
				j--;
			}

		}

	}
	else {

		if (cinfo.out_color_space == JCS_RGB) {

			while (cinfo.output_scanline < cinfo.output_height) {

				n = jpeg_read_scanlines(&cinfo, &buffer, 1);

				for (i = 0; i < cinfo.output_width; i++) {
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Red = buffer[3 * i];
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Green = buffer[3 * i + 1];
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Blue = buffer[3 * i + 2];
					((CANI_BGRAQUAD*)rv->Images[0].Image)[j * cinfo.output_width + i].Alpha = 0xFF;
				}
				j--;
			}

		}
	}

	// Finish
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	CANI_FREE(buffer);// delete[] buffer;
	return rv;
}

CANI_ANIMATION* CANI_ReadLCAF(FILE* File) {

	if (File) {
		CANI_ANIMATION* rv = 0;
		CANI_SIZE AniSize = { 0, 0 };
		CANI_DWORD ImagesNR = 0;
		CANI_DWORD settings;
		CANI_DWORD Size;
		CANI_BYTE* g_alpha=0;
		CANI_DWORD FileSize;
		CANI_CHAR id[5];
		CANI_DWORD n, m;
		CANI_DWORD KeyFrame;
		//CANI_CHAR szCaniBuffer[1024];
		CANI_FILE_POS beg;
		CANI_FILE_POS pos = CANI_FTELL(File);

		CANI_FREAD(id, 1, 5, File);

		if (id[0] != 'L' || id[1] != 'C' || id[2] != 'A' || id[3] != 'F') {
			CANI_FSEEK(File, pos, SEEK_SET);
			return CANI_ReadJPEG_FILE(File);
		}
		CANI_FREAD(&AniSize, 1, sizeof(AniSize), File);
		CANI_FREAD(&KeyFrame, 1, sizeof(KeyFrame), File);
		CANI_FREAD(&ImagesNR, 1, sizeof(ImagesNR), File);
		CANI_FREAD(&settings, 1, sizeof(settings), File);

		if (KeyFrame == 0 || ImagesNR == 0 ) {
			CANI_FSEEK(File, pos, SEEK_SET);
			return 0;// CANI_ReadJPEG_FILE(File);
		}

		rv = CANI_MALLOC(sizeof(CANI_ANIMATION));

		if (rv) {
			//char szCaniBuffer[1024];

			CANI_BYTE* Alpha8;
			CANI_BGRA16QUAD* Image16;

			CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));

			rv->Images = (CANI_FRAME_DATA*)CANI_MALLOC(sizeof(CANI_FRAME_DATA) * ImagesNR);//new CANI_FRAME_DATA[ImagesNR];
			if (!rv->Images) {
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA) * ImagesNR);

			rv->Format.format = CAF_FORMAT_RGB;
			rv->Format.rgb.Red = 8;
			rv->Format.rgb.Green = 8;
			rv->Format.rgb.Blue = 8;
			rv->Format.rgb.Alpha = 8;

			rv->Time = 0;
			rv->ImagesNow = 0;
			rv->ImagesNR = ImagesNR;
			rv->Width = AniSize.Width;
			rv->Height = AniSize.Height;

			Size = AniSize.Width * AniSize.Height;

			Alpha8 = CANI_MALLOC(Size * sizeof(CANI_BYTE));
			Image16 = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));

			if (Alpha8 == 0 || Image16 == 0) {
				if (Alpha8) CANI_FREE(Alpha8);
				if (Image16) CANI_FREE(Image16);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			if (settings & CANI_OPTION_ALPHA) {

				//MessageBoxA(0, "CANI_OPTION_ALPHA", "odczyt", 0);

				g_alpha = CANI_MALLOC(Size);

				if (g_alpha) {

					beg = CANI_FTELL(File);

					CANI_FREAD(&FileSize, 1, sizeof(CANI_DWORD), File);

					CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

					if (!CANI_ReadAlphaJPEG(File, g_alpha, AniSize.Width, AniSize.Height)) {

						CANI_FREE(g_alpha);

						for (n = 0; n < ImagesNR; n++) {
							CANI_FREE(rv->Images[n].Image);
						}
						CANI_FREE(rv->Images);
						CANI_FREE(rv);

						CANI_SetLastError(CANI_ERROR_ERROR_DURING_READING);

						return 0;
					}

					CANI_FSEEK(File, beg + sizeof(CANI_DWORD) + ((CANI_FILE_POS)FileSize), SEEK_SET);

				}
				else {

					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}
			}
			else g_alpha = 0;

			//if (settings & CANI_OPTION_JPEG_DIFFERENTATION) {
			if (1) {

				CANI_BGRA16QUAD* LastFrame = CANI_MALLOC(Size*sizeof(CANI_BGRA16QUAD));// new CANI_BGRA16QUAD[Size];
				CANI_BGRA16QUAD* DiffFrame = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));// new CANI_BGRA16QUAD[Size];

				CANI_BYTE* LastFrameAlpha = CANI_MALLOC(Size * sizeof(CANI_BYTE));// new CANI_BGRA16QUAD[Size];
				CANI_BYTE* DiffFrameAlpha = CANI_MALLOC(Size * sizeof(CANI_BYTE));// new CANI_BGRA16QUAD[Size];

				if (LastFrame && DiffFrame && LastFrameAlpha && DiffFrameAlpha) {

					for (n = 0; n < ImagesNR; n++) {

						//if (feof(File)) MessageBoxA(0, "CANI_FREAD", "eof", 0);

						CANI_BYTE Style;

						CANI_FREAD(&Style, 1, 1, File);
						if (CANI_FREAD(&rv->Images[n].Time, 1, 4, File) < 4) {

							CANI_FREE(Alpha8);
							CANI_FREE(Image16);
							CANI_FREE(LastFrame);// delete[]LastFrame;
							CANI_FREE(DiffFrame); //delete[]DiffFrame;
							CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
							CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
							if (g_alpha) CANI_FREE(g_alpha);

							if (n>0) for (m = n - 1; m > 0; m--) {
								CANI_FREE(rv->Images[m].Image);//delete [] rv->Images[m].Image;
							}
							CANI_FREE(rv->Images);
							CANI_FREE(rv);

							//MessageBoxA(0, "CANI_FREAD", "error", 0);

							CANI_SetLastError(CANI_ERROR_ERROR_DURING_READING);
							return 0;
						}

						if ( settings & CANI_OPTION_ALPHA_DIFFERENTATION ) {

							if ((n % KeyFrame) == 0) {

								beg = CANI_FTELL(File);

								CANI_FREAD(&FileSize, 1, sizeof(CANI_DWORD), File);

								CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

								if (!CANI_ReadAlphaJPEG(File, LastFrameAlpha, AniSize.Width,AniSize.Height)) {

									CANI_FREE(Alpha8);
									CANI_FREE(Image16);
									CANI_FREE(LastFrame);// delete[]LastFrame;
									CANI_FREE(DiffFrame); //delete[]DiffFrame;
									CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
									CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
									if (g_alpha) CANI_FREE(g_alpha);

									for (m = n; m > 0; m--) {
										CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
									}
									CANI_FREE(rv->Images);
									CANI_FREE(rv);

									CANI_SetLastError(CANI_ERROR_ERROR_DURING_READING);
									return 0;
								}

								CANI_FSEEK(File, beg + sizeof(CANI_DWORD) + ((CANI_FILE_POS)FileSize), SEEK_SET);

								/*for (m = 0; m < Size; m++) {
									((CANI_BGRA16QUAD*)rv->Images[n].Image)[m].Alpha = LastFrameAlpha[m];
								}*/
								CANI_MEMCPY(Alpha8, LastFrameAlpha, Size * (sizeof(CANI_BYTE)));
							}
							else {

								beg = CANI_FTELL(File);

								CANI_FREAD(&FileSize, 1, sizeof(CANI_DWORD), File);

								CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

								if (!CANI_ReadAlphaJPEG(File, DiffFrameAlpha, AniSize.Width, AniSize.Height )) {

									CANI_FREE(Alpha8);
									CANI_FREE(Image16);
									CANI_FREE(LastFrame);// delete[]LastFrame;
									CANI_FREE(DiffFrame); //delete[]DiffFrame;
									CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
									CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
									if (g_alpha) CANI_FREE(g_alpha);

									for (m = n; m > 0; m--) {
										CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
									}
									CANI_FREE(rv->Images);
									CANI_FREE(rv);

									CANI_SetLastError(CANI_ERROR_ERROR_DURING_READING); 
									return 0;
								}

								CANI_FSEEK(File, beg + sizeof(CANI_DWORD) + ((CANI_FILE_POS)FileSize), SEEK_SET);

								for (m = 0; m < Size; m++) {
									long diff;
									long now;

									diff = DiffFrameAlpha[m];
									diff *= 2;
									diff -= 256;
									if (diff < -255) diff = -255;
									if (diff > 255) diff = 255;
									now = ((long)LastFrameAlpha[m]) + diff;
									if (now < 0) now = 0;
									if (now > 255) now = 255;
									LastFrameAlpha[m] = (CANI_BYTE)now;
								}
								CANI_MEMCPY(Alpha8, LastFrameAlpha, Size* (sizeof(CANI_BYTE)));

								/*for (m = 0; m < Size; m++) {
									((CANI_BGRA16QUAD*)rv->Images[n].Image)[m].Alpha = LastFrameAlpha[m];
								}*/

							}

							//rv->Images[n].Style = CANI_STYLE_ALPHA;

						} else if ( ( Style & LCAF_STYLE_ALPHA ) ) {

							//MessageBoxA(0,"LCAF_STYLE_ALPHA","",0);

							beg = CANI_FTELL(File);

							CANI_FREAD(&FileSize, 1, sizeof(CANI_DWORD), File);

							CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

							if (!CANI_ReadAlphaJPEG(File, Alpha8, AniSize.Width, AniSize.Height)) {
							//if (!CANI_ReadAlphaJPEG_RGB(File, (CANI_BGRAQUAD*)(rv->Images[n].Image), AniSize.Width, AniSize.Height)) {

								CANI_FREE(Alpha8);
								CANI_FREE(Image16);
								CANI_FREE(LastFrame);// delete[]LastFrame;
								CANI_FREE(DiffFrame); //delete[]DiffFrame;
								CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
								CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
								if (g_alpha) CANI_FREE(g_alpha);

								for (m = n; m > 0; m--) {
									CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
								}
								CANI_FREE(rv->Images);
								CANI_FREE(rv);

								CANI_SetLastError(CANI_ERROR_ERROR_DURING_READING);
								return 0;
							}

							CANI_FSEEK(File, beg + sizeof(CANI_DWORD) + ((CANI_FILE_POS)FileSize), SEEK_SET);
						}
						else if (g_alpha) {
							CANI_MEMCPY(Alpha8, g_alpha, Size * (sizeof(CANI_BYTE)));
							//for (m = 0; m < Size; m++) ((CANI_BGRA16QUAD*)rv->Images[n].Image)[m].Alpha = g_alpha[m];
						}
						else {
							//for (m = 0; m < Size; m++) ((CANI_BGRA16QUAD*)rv->Images[n].Image)[m].Alpha = 0xFF;
							CANI_MEMSET(Alpha8,0xFF, Size* (sizeof(CANI_BYTE)) );
							//MessageBoxA(0,"alpha","",0);
						}

						if ((n % KeyFrame) == 0 || (settings & CANI_OPTION_JPEG_DIFFERENTATION) == 0 ) {
							//MessageBoxA(0, "KeyFrame", "", 0);

							beg = CANI_FTELL(File);

							CANI_FREAD(&FileSize, 1, sizeof(CANI_DWORD), File);

							CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

							if (!CANI_ReadJPEG(File, LastFrame, AniSize.Width, AniSize.Height)) {

								CANI_FREE(Alpha8);
								CANI_FREE(Image16);
								CANI_FREE(LastFrame);// delete[]LastFrame;
								CANI_FREE(DiffFrame); //delete[]DiffFrame;
								CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
								CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
								if (g_alpha) CANI_FREE(g_alpha);

								for (m = n; m > 0; m--) {
									CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
								}
								CANI_FREE(rv->Images);
								CANI_FREE(rv);

								CANI_SetLastError(CANI_ERROR_ERROR_DURING_READING);
								return 0;
							}

							CANI_FSEEK(File, beg + sizeof(CANI_DWORD) + ((CANI_FILE_POS)FileSize), SEEK_SET);

							for (m = 0; m < Size; m++) {
								Image16[m].Red = LastFrame[m].Red;
								Image16[m].Green = LastFrame[m].Green;
								Image16[m].Blue = LastFrame[m].Blue;
								//rv->Images[n].Image[m].Alpha = 0x00;
							}
						}
						else {
							//MessageBoxA(0, "No KeyFrame", "", 0);

							beg = CANI_FTELL(File);

							CANI_FREAD(&FileSize, 1, sizeof(CANI_DWORD), File);

							if (!CANI_ReadJPEG(File, DiffFrame, AniSize.Width, AniSize.Height)) {

								CANI_FREE(Alpha8);
								CANI_FREE(Image16);
								CANI_FREE(LastFrame);// delete[]LastFrame;
								CANI_FREE(DiffFrame); //delete[]DiffFrame;
								CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
								CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
								if (g_alpha) CANI_FREE(g_alpha);

								for (m = n; m > 0; m--) {
									CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
								}
								CANI_FREE(rv->Images);
								CANI_FREE(rv);

								CANI_SetLastError(CANI_ERROR_ERROR_DURING_READING);
								return 0;
							}

							CANI_FSEEK(File, beg + sizeof(CANI_DWORD) + ((CANI_FILE_POS)FileSize), SEEK_SET);

							for (m = 0; m < Size; m++) {
								long diff;
								long now;

								diff = DiffFrame[m].Red;
								diff *= 2;
								diff -= ((long)0x00010000);
								if (diff < -((long)0xFFFF)) diff = -((long)0xFFFF);
								if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
								now = ((long)LastFrame[m].Red) + diff;
								if (now < 0) now = 0;
								if (now > ((long)0xFFFF)) now = ((long)0xFFFF);
								Image16[m].Red = LastFrame[m].Red = (CANI_WORD)now;

								diff = DiffFrame[m].Green;
								diff *= 2;
								diff -= ((long)0x00010000);
								if (diff < -((long)0xFFFF)) diff = -((long)0xFFFF);
								if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
								now = ((long)LastFrame[m].Green) + diff;
								if (now < 0) now = 0;
								if (now > ((long)0xFFFF)) now = 0xFFFF;
								Image16[m].Green = LastFrame[m].Green = (CANI_WORD)now;

								diff = DiffFrame[m].Blue;
								diff *= 2;
								diff -= ((long)0x00010000);
								if (diff < -((long)0xFFFF)) diff = -((long)0xFFFF);
								if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
								now = ((long)LastFrame[m].Blue) + diff;
								if (now < 0) now = 0;
								if (now > ((long)0xFFFF)) now = 0xFFFF;
								Image16[m].Blue = LastFrame[m].Blue = (CANI_WORD)now;
							}

						}

						rv->Images[n].Image = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));

						if (!rv->Images[n].Image) {

							CANI_FREE(Alpha8);
							CANI_FREE(Image16);
							CANI_FREE(LastFrame);// delete[]LastFrame;
							CANI_FREE(DiffFrame); //delete[]DiffFrame;
							CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
							CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
							if (g_alpha) CANI_FREE(g_alpha);

							for (m = n; m > 0; m--) {
								CANI_FREE(rv->Images[m-1].Image);//delete [] rv->Images[m].Image;
							}
							CANI_FREE(rv->Images);
							CANI_FREE(rv);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}

						CANI_MEMCPY(rv->Images[n].Image,Image16,Size * sizeof(CANI_BGRA16QUAD));

						for (m = 0; m < Size; m++) {
							((CANI_BGRA16QUAD*)rv->Images[n].Image)[m].Alpha = (((CANI_WORD)Alpha8[m]) << 8) | ((CANI_WORD)Alpha8[m]);
						}

						rv->Images[n].Style = CANI_STYLE_16BITS_IMAGE;

						if (Style & LCAF_STYLE_ALPHA) rv->Images[n].Style |= CANI_STYLE_ALPHA;

						//CANI_MEMCPY(rv->Images[n].Image, LastFrame, Size * sizeof(CANI_BGRA16QUAD));
					}
				}

				if (LastFrame) CANI_FREE(LastFrame);// delete[]LastFrame;
				if (DiffFrame) CANI_FREE(DiffFrame); //delete[]DiffFrame;

				if (LastFrameAlpha) CANI_FREE(LastFrameAlpha);// delete[]LastFrame;
				if (DiffFrameAlpha) CANI_FREE(DiffFrameAlpha); //delete[]DiffFrame;
			}

			CANI_FREE(Alpha8);
			CANI_FREE(Image16);

			if (settings & CANI_OPTION_16BITS_ALPHA) {

				if (g_alpha) {

					rv->gAlpha = CANI_MALLOC(Size * sizeof(CANI_WORD));

					if (rv->gAlpha) {
						for ( m =0; m < Size; m++) ((CANI_WORD*)rv->gAlpha)[m] = ( ((CANI_WORD)(g_alpha[m])) << 8 ) | ((CANI_WORD)(g_alpha[m]));
					}

					CANI_FREE(g_alpha);

				}
				else rv->gAlpha = 0;
			}
			else {

				rv->gAlpha = g_alpha;
			}

		}

		CANI_MEMCPY(&rv->Format.options, &settings, sizeof(rv->Format.options) );

		//MessageBoxA(0, "CANI_FREAD", "end", 0);

		return rv;
	}

	return 0;
}