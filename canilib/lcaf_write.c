#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "cani.h"

//#include "..\jpeg\jpeglib.h"

// JCS_RGB

CANI_CHAR CANI_WriteJPEG(FILE* file, CANI_BGRA16QUAD* ImageRGB, int image_width, int image_height, int quality, CANI_BYTE typ)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */
	JSAMPLE* image_buffer;
	if (typ != JCS_RGB && typ != JCS_GRAYSCALE) return FALSE;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, file);
	cinfo.image_width = image_width; 	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = (typ == JCS_RGB ? 3 : 1);		/* # of color components per pixel */
	cinfo.in_color_space = typ;//( input_components == 3 ? JCS_RGB : JCS_GRAYSCALE ); 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = image_width * cinfo.input_components;	/* JSAMPLEs per row in image_buffer */

	image_buffer = CANI_MALLOC(row_stride*sizeof(JSAMPLE));// new JSAMPLE[row_stride];

	if (image_buffer) {
		CANI_DWORD y = cinfo.image_height - 1;
		while (cinfo.next_scanline < cinfo.image_height) {
			CANI_DWORD x;

			if (typ == JCS_RGB) {
				for (x = 0; x < cinfo.image_width; x++) {

					CANI_WORD Value = ImageRGB[y * image_width + x].Red;
					image_buffer[3 * x] = (CANI_BYTE)(Value>>8);

					Value = ImageRGB[y * image_width + x].Green;
					image_buffer[3 * x + 1] = (CANI_BYTE)(Value >> 8);

					Value = ImageRGB[y * image_width + x].Blue;
					image_buffer[3 * x + 2] = (CANI_BYTE)(Value >> 8);
				}
			}
			else {
				CANI_BGRA16_2_GRAY((CANI_BYTE*)image_buffer , &ImageRGB[y * image_width], cinfo.image_width);
				//for (x = 0; x < cinfo.image_width; x++) {
					//CANI_DWORD value = (ImageRGB[y * image_width + x].Red + ImageRGB[y * image_width + x].Green + ImageRGB[y * image_width + x].Blue) / 3;
					
					//image_buffer[x] = (CANI_BYTE)value;
				//}
			}

			row_pointer[0] = image_buffer;//& image_buffer[cinfo.next_scanline * row_stride];
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
			y--;
		}
		CANI_FREE(image_buffer);//delete[] image_buffer;
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	return TRUE;
}

CANI_CHAR CANI_WriteAlphaJPEG(FILE* file, CANI_BYTE* ImageA, int image_width, int image_height, int quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */
	JSAMPLE* image_buffer;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, file);
	cinfo.image_width = image_width; 	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 1;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_GRAYSCALE;//( input_components == 3 ? JCS_RGB : JCS_GRAYSCALE ); 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = image_width * cinfo.input_components;	/* JSAMPLEs per row in image_buffer */

	image_buffer = CANI_MALLOC(row_stride * sizeof(JSAMPLE));// new JSAMPLE[row_stride];

	if (image_buffer) {
		CANI_DWORD y = cinfo.image_height - 1;
		while (cinfo.next_scanline < cinfo.image_height) {
			CANI_DWORD x;

			for (x = 0; x < cinfo.image_width; x++) {
				image_buffer[x] = (CANI_BYTE)ImageA[y * image_width + x];
			}

			row_pointer[0] = image_buffer;//& image_buffer[cinfo.next_scanline * row_stride];
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
			y--;
		}
		CANI_FREE(image_buffer);//delete[] image_buffer;
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	return TRUE;
}

CANI_CHAR CANI_WriteAlphaJPEG_RGB(FILE* file, CANI_BGRA16QUAD* ImageRGB, int image_width, int image_height, int quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */
	JSAMPLE* image_buffer;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, file);
	cinfo.image_width = image_width; 	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 1;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_GRAYSCALE;//( input_components == 3 ? JCS_RGB : JCS_GRAYSCALE ); 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = image_width * cinfo.input_components;	/* JSAMPLEs per row in image_buffer */

	image_buffer = CANI_MALLOC(row_stride * sizeof(JSAMPLE));// new JSAMPLE[row_stride];

	if (image_buffer) {
		CANI_DWORD y = cinfo.image_height - 1;
		while (cinfo.next_scanline < cinfo.image_height) {
			CANI_DWORD x;

			for (x = 0; x < cinfo.image_width; x++) {
				image_buffer[x] = (CANI_BYTE)ImageRGB[y * image_width + x].Alpha;
			}

			row_pointer[0] = image_buffer;//& image_buffer[cinfo.next_scanline * row_stride];
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
			y--;
		}
		CANI_FREE(image_buffer);// delete[] image_buffer;
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	return TRUE;
}

CANI_EXPORT CANI_CHAR LCAF_WriteAnimation(FILE* File, CANI_ANIMATION* animation, CANI_WORD ImageQuality, CANI_WORD AlphaQuality, CANI_PROGRESS_FUNC Progress) {

	if (File) {

		CANI_DWORD KeyFrame = 10000;
		CANI_FILE_POS beg, end;
		CANI_SIZE AniSize = { 0, 0 };
		CANI_DWORD Size;
		CANI_BYTE* Alpha8;
		CANI_BGRA16QUAD* Image16;
		CANI_DWORD FileSize;
		CANI_DWORD n, m;
		CANI_BYTE JCS_COLOR_SPACE;// JCS_RGB
		CANI_BYTE ProgressNow = 0;

		AniSize.Width = animation->Width;
		AniSize.Height = animation->Height;

		CANI_SetLastError(CANI_ERROR_NO_ERROR);

		Size = AniSize.Width * AniSize.Height;

		Alpha8 = CANI_MALLOC(Size * sizeof(CANI_BYTE));
		Image16 = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));

		if (Alpha8 == 0 || Image16 == 0) {
			if (Alpha8) CANI_FREE(Alpha8);
			if (Image16) CANI_FREE(Image16);
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return FALSE;
		}

		CANI_FWRITE("LCAF1", 1, 5, File);

		CANI_FWRITE(&AniSize, 1, sizeof(AniSize), File);
		CANI_FWRITE(&KeyFrame, 1, sizeof(KeyFrame), File);
		CANI_FWRITE(&animation->ImagesNR, 1, sizeof(animation->ImagesNR), File);

		if (!CANI_GetGlobalAlpha(animation, Alpha8) || (animation->Format.options & CANI_OPTION_NO_ALPHA) ) {
			animation->Format.options &= (~(CANI_OPTION_ALPHA));
		}

		//animation->Format.options |= CANI_OPTION_ALPHA_DIFFERENTATION | CANI_OPTION_JPEG_DIFFERENTATION;

		CANI_FWRITE(&animation->Format.options, 1, sizeof(animation->Format.options), File);

		if (animation->Format.options & CANI_OPTION_ALPHA) {

			//MessageBoxA(0, "CANI_OPTION_ALPHA", "", 0);

			beg = CANI_FTELL(File);

			CANI_FSEEK(File, sizeof(CANI_DWORD), SEEK_CUR);

			/*FILE* file = fopen("C:\\Users\\squea\\Desktop\\zarobek\\CANI\\Examples\\apng\\samples\\moje\\1a.alpha","wb");

			if (file) {
				fwrite(Alpha8, AniSize.Width, AniSize.Height,file);
				fclose(file);
			}*/

			//MessageBox(0,L"CANI_WriteAlphaJPEG",0,0);

			CANI_WriteAlphaJPEG(File, Alpha8, AniSize.Width, AniSize.Height, AlphaQuality);

			end = CANI_FTELL(File);

			FileSize = ((CANI_DWORD)(end - beg - sizeof(CANI_DWORD)));

			CANI_FSEEK(File, beg, SEEK_SET);

			CANI_FWRITE(&FileSize, 1, sizeof(CANI_DWORD), File);

			CANI_FSEEK(File, beg + sizeof(FileSize) + FileSize, SEEK_SET);

			// 

			//CANI_MEMCPY(SrcNow, animation->gAlpha, BitMapSize);
			//SrcNow += BitMapSize;
		}

		if (animation->Format.options & CANI_OPTION_NO_ALPHA) {

			animation->Format.options &= (~(CANI_OPTION_ALPHA_DIFFERENTATION));
		}

		if (animation->Format.options & CANI_OPTION_JPEG_GRAYSCALE) {
			JCS_COLOR_SPACE = JCS_GRAYSCALE;
		}
		else {
			JCS_COLOR_SPACE = JCS_RGB;
		}

		if (Progress) Progress(0);

		//#define CANI_OPTION_NO_ALPHA 0x00000020 // don't write alpha
		//#define CANI_OPTION_JPEG_GRAYSCALE 0x00000040

		//if (animation->Format.options & CANI_OPTION_JPEG_DIFFERENTATION) {
		if (1) {

			CANI_BGRA16QUAD* LastFrame = CANI_MALLOC(Size*sizeof(CANI_BGRA16QUAD));// CANI_MALLOC(Size*sizeof(CANI_BGRAQUAD))
			CANI_BGRA16QUAD* DiffFrame = CANI_MALLOC(Size*sizeof(CANI_BGRA16QUAD));

			CANI_BYTE* LastFrameAlpha = CANI_MALLOC(Size*sizeof(CANI_BYTE));// CANI_MALLOC(Size*sizeof(CANI_BYTE))
			CANI_BYTE* DiffFrameAlpha = CANI_MALLOC(Size*sizeof(CANI_BYTE));
			CANI_BYTE* NowFrameAlpha = CANI_MALLOC(Size * sizeof(CANI_BYTE));// CANI_MALLOC(Size*sizeof(CANI_BYTE))

			if (LastFrame && DiffFrame && LastFrameAlpha && DiffFrameAlpha && NowFrameAlpha ) {

				for (n = 0; n < animation->ImagesNR; n++) {

					CANI_BYTE Style;

					if (Progress) {

						CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

						if (ProgressNow2 != ProgressNow) {
							ProgressNow = ProgressNow2;
							Progress(ProgressNow);
						}

					}

					CANI_GetImageOfFrame16(animation, n, Image16 );

					if (animation->Format.options & CANI_OPTION_NO_ALPHA) Style = 0;
					else if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) Style = LCAF_STYLE_ALPHA;
					else if (animation->Images[n].Style & ( CANI_STYLE_TRANSPARENT | CANI_STYLE_ALPHA ) ) Style = LCAF_STYLE_ALPHA;
					else Style = 0;

					if (animation->Images[n].Style &CANI_STYLE_16BITS_IMAGE) Style |= LCAF_STYLE_16BITS_IMAGE;

					CANI_FWRITE(&Style, 1, 1, File);
					CANI_FWRITE(&animation->Images[n].Time, 1, 4, File);

					if (animation->Format.options & CANI_OPTION_NO_ALPHA) {
					}
					else if (animation->Format.options & CANI_OPTION_ALPHA_DIFFERENTATION) {

						CANI_GetAlphaOfFrame(animation, n, NowFrameAlpha);

						if ( ( n % KeyFrame ) == 0 ) {

							beg = CANI_FTELL(File);

							CANI_FSEEK(File, sizeof(CANI_DWORD), SEEK_CUR);

							CANI_WriteAlphaJPEG(File, NowFrameAlpha, AniSize.Width, AniSize.Height, AlphaQuality);

							end = CANI_FTELL(File);

							FileSize = (CANI_DWORD)(end - beg - sizeof(CANI_DWORD));

							CANI_FSEEK(File, beg, SEEK_SET);

							CANI_FWRITE(&FileSize, 1, sizeof(CANI_DWORD), File);

							CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

							if (!CANI_ReadAlphaJPEG(File, LastFrameAlpha, AniSize.Width, AniSize.Height)) {

								CANI_FREE(LastFrame);
								CANI_FREE(DiffFrame); //CANI_FREE(DiffFrame);
								CANI_FREE(LastFrameAlpha);//CANI_FREE(LastFrameAlpha);
								CANI_FREE(DiffFrameAlpha);//CANI_FREE(DiffFrameAlpha);
								CANI_FREE(NowFrameAlpha);

								//CANI_FCLOSE(File);
								CANI_SetLastError(CANI_ERROR_ERROR_DURING_WRITING);

								return FALSE;
							}

							CANI_FSEEK(File, beg + sizeof(FileSize) + FileSize, SEEK_SET);
						}
						else {

							for (m = 0; m < Size; m++) {
								long diff;

								diff = ((long)NowFrameAlpha[m]) - ((long)LastFrameAlpha[m]);
								diff += 256;
								diff /= 2;
								if (diff < 0) diff = 0;
								if (diff > 255) diff = 255;
								DiffFrameAlpha[m] = (CANI_BYTE)diff;
							}

							beg = CANI_FTELL(File);

							CANI_FSEEK(File, sizeof(CANI_DWORD), SEEK_CUR);

							CANI_WriteAlphaJPEG(File, DiffFrameAlpha, AniSize.Width, AniSize.Height, AlphaQuality);

							end = CANI_FTELL(File);

							FileSize = (CANI_DWORD)(end - beg - sizeof(CANI_DWORD));

							CANI_FSEEK(File, beg, SEEK_SET);

							CANI_FWRITE(&FileSize, 1, sizeof(CANI_DWORD), File);

							CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

							if (!CANI_ReadAlphaJPEG(File, DiffFrameAlpha, AniSize.Width, AniSize.Height)) {

								CANI_FREE(LastFrame);
								CANI_FREE(DiffFrame);
								CANI_FREE(LastFrameAlpha);// CANI_FREE(LastFrameAlpha)
								CANI_FREE(DiffFrameAlpha);// CANI_FREE(DiffFrameAlpha)
								CANI_FREE(NowFrameAlpha);

								//CANI_FCLOSE(File);
								CANI_SetLastError(CANI_ERROR_ERROR_DURING_WRITING);

								return FALSE;
							}

							CANI_FSEEK(File, beg + sizeof(FileSize) + FileSize, SEEK_SET);

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

						}
					}
					else if (animation->Images[n].Style & ( CANI_STYLE_TRANSPARENT | CANI_STYLE_ALPHA ) ) {

						CANI_GetAlphaOfFrame(animation, n, NowFrameAlpha);

						//CANI_MEMSET(NowFrameAlpha, 0xff, Size);

						beg = CANI_FTELL(File);

						CANI_FSEEK(File, sizeof(CANI_DWORD), SEEK_CUR);

						CANI_WriteAlphaJPEG(File, NowFrameAlpha, AniSize.Width, AniSize.Height, AlphaQuality);

						end = CANI_FTELL(File);

						FileSize = (CANI_DWORD)(end - beg - sizeof(CANI_DWORD));

						CANI_FSEEK(File, beg, SEEK_SET);

						CANI_FWRITE(&FileSize, 1, sizeof(CANI_DWORD), File);

						CANI_FSEEK(File, beg + sizeof(FileSize) + FileSize, SEEK_SET);
					}

					if ((n % KeyFrame) == 0 || (animation->Format.options & CANI_OPTION_JPEG_DIFFERENTATION) == 0 ) {

						beg = CANI_FTELL(File);

						CANI_FSEEK(File, sizeof(CANI_DWORD), SEEK_CUR);

						CANI_WriteJPEG(File, Image16, AniSize.Width, AniSize.Height, ImageQuality, JCS_COLOR_SPACE);

						end = CANI_FTELL(File);

						FileSize = (CANI_DWORD)(end - beg - sizeof(CANI_DWORD));

						CANI_FSEEK(File, beg, SEEK_SET);

						CANI_FWRITE(&FileSize, 1, sizeof(CANI_DWORD), File);

						CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

						if (!CANI_ReadJPEG(File, LastFrame, AniSize.Width, AniSize.Height)) {

							CANI_FREE(LastFrame);
							CANI_FREE(DiffFrame);
							CANI_FREE(LastFrameAlpha);
							CANI_FREE(DiffFrameAlpha);
							CANI_FREE(NowFrameAlpha);

							//CANI_FCLOSE(File);
							CANI_SetLastError(CANI_ERROR_ERROR_DURING_WRITING);

							return FALSE;
						}

						CANI_FSEEK(File, beg + sizeof(FileSize) + FileSize, SEEK_SET);
					}
					else {

						CANI_BGRA16QUAD* Image = Image16;

						for (m = 0; m < Size; m++) {
							long diff;

							diff = ((long)Image[m].Red) - ((long)LastFrame[m].Red);
							diff += ((long)0x00010000);
							diff /= 2;
							if (diff < 0) diff = 0;
							if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
							DiffFrame[m].Red = (CANI_WORD)diff;

							diff = ((long)Image[m].Green) - ((long)LastFrame[m].Green);
							diff += ((long)0x00010000);
							diff /= 2;
							if (diff < 0) diff = 0;
							if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
							DiffFrame[m].Green = (CANI_WORD)diff;

							diff = ((long)Image[m].Blue) - ((long)LastFrame[m].Blue);
							diff += ((long)0x00010000);
							diff /= 2;
							if (diff < 0) diff = 0;
							if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
							DiffFrame[m].Blue = (CANI_WORD)diff;
						}

						beg = CANI_FTELL(File);

						CANI_FSEEK(File, sizeof(CANI_DWORD), SEEK_CUR);

						CANI_WriteJPEG(File, DiffFrame, AniSize.Width, AniSize.Height, ImageQuality, JCS_COLOR_SPACE);

						end = CANI_FTELL(File);

						FileSize = (CANI_DWORD)(end - beg - sizeof(CANI_DWORD));

						CANI_FSEEK(File, beg, SEEK_SET);

						CANI_FWRITE(&FileSize, 1, sizeof(CANI_DWORD), File);

						CANI_FSEEK(File, beg + sizeof(CANI_DWORD), SEEK_SET);

						if (!CANI_ReadJPEG(File, DiffFrame, AniSize.Width, AniSize.Height)) {

							CANI_FREE(LastFrame);
							CANI_FREE(DiffFrame);
							CANI_FREE(LastFrameAlpha);
							CANI_FREE(DiffFrameAlpha);
							CANI_FREE(NowFrameAlpha);

							//CANI_FCLOSE(File);
							CANI_SetLastError(CANI_ERROR_ERROR_DURING_WRITING);

							return FALSE;
						}

						CANI_FSEEK(File, beg + sizeof(FileSize) + FileSize, SEEK_SET);

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
							LastFrame[m].Red = (CANI_WORD)now;

							diff = DiffFrame[m].Green;
							diff *= 2;
							diff -= ((long)0x00010000);
							if (diff < -((long)0xFFFF)) diff = -((long)0xFFFF);
							if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
							now = ((long)LastFrame[m].Green) + diff;
							if (now < 0) now = 0;
							if (now > ((long)0xFFFF)) now = ((long)0xFFFF);
							LastFrame[m].Green = (CANI_WORD)now;

							diff = DiffFrame[m].Blue;
							diff *= 2;
							diff -= ((long)0x00010000);
							if (diff < -((long)0xFFFF)) diff = -((long)0xFFFF);
							if (diff > ((long)0xFFFF)) diff = ((long)0xFFFF);
							now = ((long)LastFrame[m].Blue) + diff;
							if (now < 0) now = 0;
							if (now > ((long)0xFFFF)) now = ((long)0xFFFF);
							LastFrame[m].Blue = (CANI_WORD)now;

							//diff = ((long)Image[m].Red) - ((long)LastFrame[m].Red);
						}

						//CANI_MEMCPY(LastFrame, NowFrame, Size * sizeof(CANI_BGRAQUAD));

					}
				}
			}
			else {

				if (LastFrame) CANI_FREE(LastFrame);// delete[]LastFrame;
				if (DiffFrame) CANI_FREE(DiffFrame);//delete[]DiffFrame;
				if (LastFrameAlpha) CANI_FREE(LastFrameAlpha);//delete[]LastFrameAlpha;
				if (DiffFrameAlpha) CANI_FREE(DiffFrameAlpha);//delete[]DiffFrameAlpha;
				if (NowFrameAlpha) CANI_FREE(NowFrameAlpha);

				if (Alpha8) CANI_FREE(Alpha8);
				if (Image16) CANI_FREE(Image16);

				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);

				return FALSE;
			}

			if (LastFrame) CANI_FREE(LastFrame);// delete[]LastFrame;
			if (DiffFrame) CANI_FREE(DiffFrame);//delete[]DiffFrame;
			if (LastFrameAlpha) CANI_FREE(LastFrameAlpha);//delete[]LastFrameAlpha;
			if (DiffFrameAlpha) CANI_FREE(DiffFrameAlpha);//delete[]DiffFrameAlpha;
			if (NowFrameAlpha) CANI_FREE(NowFrameAlpha);
		}
		if (Alpha8) CANI_FREE(Alpha8);
		if (Image16) CANI_FREE(Image16);

		if (Progress) {

			Progress(100);

		}

		return TRUE;
	}
	return FALSE;
}


#ifdef CANI_UNICODE_FILE

CANI_EXPORT CANI_CHAR LCAF_SaveAnimationW(CANI_WORD* FileName, CANI_ANIMATION* animation, CANI_WORD ImageQuality, CANI_WORD AlphaQuality, CANI_PROGRESS_FUNC Progress) {

	FILE* File = CANI_FOPEN_UTF16(FileName, L"w+b");

	if (File) {
		CANI_CHAR rv = LCAF_WriteAnimation(File, animation, ImageQuality, AlphaQuality, Progress);
		CANI_FCLOSE(File);
		if (rv == FALSE) CANI_FDELETE_UTF16(FileName);
		return rv;
	}
	return FALSE;
}

#endif

CANI_EXPORT CANI_CHAR LCAF_SaveAnimationA(CANI_CHAR* FileName, CANI_ANIMATION* animation, CANI_WORD ImageQuality, CANI_WORD AlphaQuality, CANI_PROGRESS_FUNC Progress) {

	FILE* File = CANI_FOPEN_ASCII(FileName, "w+b");

	if (File) {
		CANI_CHAR rv = LCAF_WriteAnimation(File, animation, ImageQuality, AlphaQuality, Progress);
		CANI_FCLOSE(File);
		if (rv == FALSE) CANI_FDELETE_ASCII(FileName);
		return rv;
	}
	return FALSE;
}