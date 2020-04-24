#include "cani.h"

CANI_BYTE* CANI_ReadLine(FILE* file) {
	CANI_DWORD Size = 1024,Pos = 0;
	CANI_BYTE* rv = malloc(1024+1);

	do {
		int c = CANI_FGETC(file);
		if (c == EOF) break;
		rv[Pos] = c;
		Pos++;
		if (Pos >= Size) {
			Size += 1024;
			rv = CANI_REALLOC(rv, Size+1 );
		}
	} while (rv[Pos - 1] != 0x0A);

	rv[Pos] = 0;

	return rv;
}

CANI_CHAR CANI_FlipImageVertically(CANI_ANIMATION* animation, CANI_DWORD FrameNR) {
	if (animation && FrameNR < animation->ImagesNR) {

		//if (animation->Format.options & CANI_OPTION_16BITS_ALPHA) {
		if (animation->Images[FrameNR].Style & CANI_STYLE_16BITS_IMAGE) {

			CANI_BGRA16QUAD* src = (CANI_BGRA16QUAD*)animation->Images[FrameNR].Image;
			CANI_BGRA16QUAD* dest = malloc(animation->Width* animation->Height*sizeof(CANI_BGRA16QUAD));

			if (dest) {

				CANI_DWORD n;
				for (n = 0; n < animation->Height; n++) {
					CANI_MEMCPY(&dest[n * animation->Width], &src[(animation->Height - 1 - n) * animation->Width],
						animation->Width * sizeof(CANI_BGRA16QUAD));
				}
				CANI_FREE(animation->Images[FrameNR].Image);
				animation->Images[FrameNR].Image = (CANI_BYTE*)dest;

				return TRUE;
			}

			return TRUE;
		}
		else {

			CANI_BGRAQUAD* src = (CANI_BGRAQUAD*)animation->Images[FrameNR].Image;
			CANI_BGRAQUAD* dest = malloc(animation->Width * animation->Height * sizeof(CANI_BGRAQUAD));

			if (dest) {

				CANI_DWORD n;
				for (n = 0; n < animation->Height; n++) {
					CANI_MEMCPY(&dest[n * animation->Width], &src[(animation->Height - 1 - n) * animation->Width],
						animation->Width * sizeof(CANI_BGRAQUAD));
				}
				CANI_FREE(animation->Images[FrameNR].Image);
				animation->Images[FrameNR].Image = (CANI_BYTE*)dest;

				return TRUE;
			}

		}
	}
	return FALSE;
}

CANI_ANIMATION* CANI_ReadPXM_FILE(FILE* file) {
	if (file) {
		CANI_BYTE Type[3];
		CANI_FREAD(Type, 1, 3, file);
		if (Type[0] != 'P' || Type[2] != 0x0A) return 0;
		if (Type[1] >= '1' && Type[1] <= '6' ) {
			CANI_BYTE* Line = 0;
			CANI_DWORD Width=0, Height=0;
			CANI_DWORD n;
			do {
				if (Line) CANI_FREE(Line);
				Line = CANI_ReadLine(file);
			} while (Line[0]=='#');
			for (n = 0; Line[n] >= '0' && Line[n] <= '9'; n++) {
				Width *= 10;
				Width += (Line[n] - '0');
			}
			while (Line[n] == ' ') n++;
			for (; Line[n] >= '0' && Line[n] <= '9'; n++) {
				Height *= 10;
				Height += (Line[n] - '0');
			}
			//char szCaniBuffer[1024];
			if (Type[1] == '1') {
				if (Width > 0 && Height > 0) {

					CANI_BGRAQUAD* Image;
					CANI_BGRAQUAD* ImageNow;
					CANI_BGRAQUAD* ImageMax;
					CANI_MEMORY_SIZE Size;

					CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));
					if (!rv) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return 0;
					}
					CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
					rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));
					if (!rv->Images) { CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

					rv->ImagesNR = 1;
					rv->Width = Width;
					rv->Height = Height;
					Size = Width * Height;

					Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));
					if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					ImageNow = Image;
					ImageMax = &Image[Size];

					rv->Format.format = CAF_FORMAT_RGB;
					rv->Format.rgb.Red = 8;
					rv->Format.rgb.Green = 8;
					rv->Format.rgb.Blue = 8;
					rv->Format.rgb.Alpha = 8;

					do {
						CANI_BYTE* Now;
						if (Line) CANI_FREE(Line);
						Line = CANI_ReadLine(file);
						Now = Line;
						while (*Now != 0 && *Now != 0x0a) {
							if (*Now == '0') {
								ImageNow->Red = 0xFF;
								ImageNow->Green = 0xFF;
								ImageNow->Blue = 0xFF;
								ImageNow->Alpha = 0xFF;
								ImageNow++;
							}
							else if (*Now == '1') {
								ImageNow->Red = 0x00;
								ImageNow->Green = 0x00;
								ImageNow->Blue = 0x00;
								ImageNow->Alpha = 0xFF;
								ImageNow++;
							}
							if (ImageNow >= ImageMax) break;
							Now++;
						}
					} while (Line[0]);

					if (Line) CANI_FREE(Line);
					CANI_FlipImageVertically(rv, 0);
					return rv;

				}

			}
			else if (Type[1] == '4') {
				if (Width > 0 && Height > 0) {

					CANI_BGRAQUAD* Image;
					CANI_BGRAQUAD* ImageNow;
					CANI_BGRAQUAD* ImageMax;
					CANI_DWORD LineSize;
					CANI_BYTE* data;
					CANI_MEMORY_SIZE Size;

					CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));
					if (!rv) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return 0;
					}
					CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
					rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));
					if (!rv->Images) { CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

					rv->ImagesNR = 1;
					rv->Width = Width;
					rv->Height = Height;
					Size = Width * Height;

					Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));
					if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					ImageNow = Image;
					ImageMax = &Image[Size];

					rv->Format.format = CAF_FORMAT_RGB;
					rv->Format.rgb.Red = 8;
					rv->Format.rgb.Green = 8;
					rv->Format.rgb.Blue = 8;
					rv->Format.rgb.Alpha = 8;

					LineSize = Width / 8;
					if (Width % 8) LineSize++;
					;
					if (Width % 8) {
						LineSize =
							Width + (8 - (Width % 8));
					}
					else {
						LineSize = Width;
					}

					data = CANI_MALLOC(LineSize * Height);

					if (data) {
						unsigned char maska[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
						CANI_DWORD x, y;
						CANI_DWORD nr = 0;

						CANI_FREAD(data, LineSize, Height, file);
						for (y = 0; y < Height; y++) {

							nr = (y) * (LineSize);

							for (x = 0; x < Width; x++) {
								CANI_BYTE wartosc = data[nr / 8] & maska[nr % 8] ? 0 : 1;

								if (wartosc) {
									ImageNow->Red = 0xFF;
									ImageNow->Green = 0xFF;
									ImageNow->Blue = 0xFF;
									ImageNow->Alpha = 0xFF;
								}
								else {
									ImageNow->Red = 0x00;
									ImageNow->Green = 0x00;
									ImageNow->Blue = 0x00;
									ImageNow->Alpha = 0xFF;
								}

								ImageNow++;
								nr++;
							}
						}

						CANI_FREE(data);
					}

					if (Line) CANI_FREE(Line);
					CANI_FlipImageVertically(rv, 0);
					return rv;
				}

			}
			else if (Type[1] == '2') {
				if (Width > 0 && Height > 0) {

					CANI_DWORD Maximum = 0;
					CANI_DWORD n;
					CANI_MEMORY_SIZE Size;

					CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));
					if (!rv) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return 0;
					}
					CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
					rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));
					if (!rv->Images) { CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

					rv->ImagesNR = 1;
					rv->Width = Width;
					rv->Height = Height;
					Size = Width * Height;

					rv->Format.format = CAF_FORMAT_RGB;
					rv->Format.rgb.Red = 8;
					rv->Format.rgb.Green = 8;
					rv->Format.rgb.Blue = 8;
					rv->Format.rgb.Alpha = 8;

					if (Line) CANI_FREE(Line);
					Line = CANI_ReadLine(file);
					for (n = 0; Line[n] >= '0' && Line[n] <= '9'; n++) {
						Maximum *= 10;
						Maximum += (Line[n] - '0');
					}

					if (Maximum == 0) {
						CANI_FREE(rv->Images); CANI_FREE(rv); return 0;
					}

					if (Maximum > 0xFF) {

						CANI_BGRA16QUAD* ImageNow;
						CANI_BGRA16QUAD* ImageMax;
						CANI_BGRA16QUAD* Image = ((CANI_BGRA16QUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						rv->Images->Style |= CANI_STYLE_16BITS_IMAGE;
						ImageNow = Image;
						ImageMax = &Image[Size];

						do {
							CANI_BYTE* Now;
							CANI_CONVERT_VARIABLE Value = 0;
							if (Line) CANI_FREE(Line);
							Line = CANI_ReadLine(file);
							Now = Line;
							while (*Now != 0 && *Now != 0x0a) {

								if (*Now >= '0' && *Now <= '9') {
									Value *= 10;
									Value += (*Now - '0');
								}
								else if (*Now == ' ') {
									Value = ((Value * 0xFFFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Red = (CANI_WORD)Value;
									ImageNow->Green = (CANI_WORD)Value;
									ImageNow->Blue = (CANI_WORD)Value;
									ImageNow->Alpha = 0xFF;
									Value = 0;
									ImageNow++;
									if (ImageNow >= ImageMax) break;
								}
								Now++;
							}
						} while (Line[0]);

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}
					else {

						CANI_BGRAQUAD* ImageNow;
						CANI_BGRAQUAD* ImageMax;
						CANI_BGRAQUAD* Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						ImageNow = Image;
						ImageMax = &Image[Size];

						do {
							CANI_BYTE* Now = Line;
							CANI_CONVERT_VARIABLE Value = 0;
							if (Line) CANI_FREE(Line);
							Line = CANI_ReadLine(file);
							Now = Line;
							while (*Now != 0 && *Now != 0x0a) {

								if (*Now >= '0' && *Now <= '9') {
									Value *= 10;
									Value += (*Now - '0');
								}
								else if (*Now == ' ') {
									Value = ((Value * 0xFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Red = (CANI_BYTE)Value;
									ImageNow->Green = (CANI_BYTE)Value;
									ImageNow->Blue = (CANI_BYTE)Value;
									ImageNow->Alpha = 0xFF;
									Value = 0;
									ImageNow++;
									if (ImageNow >= ImageMax) break;
								}
								Now++;
							}
						} while (Line[0]);

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}

				}
			} else if (Type[1] == '5') {
				if (Width > 0 && Height > 0) {
					CANI_MEMORY_SIZE Size;
					CANI_DWORD Maximum = 0;
					CANI_DWORD n;

					CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));
					if (!rv) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return 0;
					}
					CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
					rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));
					if (!rv->Images) { CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

					rv->ImagesNR = 1;
					rv->Width = Width;
					rv->Height = Height;
					Size = Width * Height;

					if (Line) CANI_FREE(Line);
					Line = CANI_ReadLine(file);

					rv->Format.format = CAF_FORMAT_RGB;
					rv->Format.rgb.Red = 8;
					rv->Format.rgb.Green = 8;
					rv->Format.rgb.Blue = 8;
					rv->Format.rgb.Alpha = 8;

					for (n = 0; Line[n] >= '0' && Line[n] <= '9'; n++) {
						Maximum *= 10;
						Maximum += (Line[n] - '0');
					}

					if (Maximum == 0) {
						CANI_FREE(rv->Images); CANI_FREE(rv); return 0;
					}

					if (Maximum > 0xFF) {

						CANI_BGRA16QUAD* ImageNow;
						CANI_BGRA16QUAD* ImageMax;
						CANI_DWORD LineSize;
						CANI_BYTE* data;

						CANI_BGRA16QUAD* Image = ((CANI_BGRA16QUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						rv->Images->Style |= CANI_STYLE_16BITS_IMAGE;
						ImageNow = Image;
						ImageMax = &Image[Size];

						LineSize = Width;

						data = CANI_MALLOC(LineSize * Height * sizeof(CANI_WORD));

						if (data) {
							CANI_DWORD x, y;
							CANI_DWORD nr = 0;

							CANI_FREAD(data, LineSize, Height, file);

							for (y = 0; y < Height; y++) {

								nr = (y) * (LineSize);

								for (x = 0; x < Width; x++) {
									CANI_CONVERT_VARIABLE Value = ( ( ((CANI_CONVERT_VARIABLE)((CANI_WORD*)data)[nr]) * 0xFFFF ) / ((CANI_CONVERT_VARIABLE)Maximum) );
									ImageNow->Red = (CANI_WORD)Value;
									ImageNow->Green = (CANI_WORD)Value;
									ImageNow->Blue = (CANI_WORD)Value;
									ImageNow->Alpha = 0xFF;

									ImageNow++;
									if (ImageNow >= ImageMax) break;
									nr++;
								}
								if (ImageNow >= ImageMax) break;
							}

							CANI_FREE(data);
						}

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}
					else {
						CANI_BGRAQUAD* ImageNow;
						CANI_BGRAQUAD* ImageMax;
						CANI_DWORD LineSize;
						CANI_BYTE* data;

						CANI_BGRAQUAD* Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						ImageNow = Image;
						ImageMax = &Image[Size];

						LineSize = Width;

						data = CANI_MALLOC(LineSize * Height);

						if (data) {
							CANI_DWORD x, y;
							CANI_DWORD nr = 0;

							CANI_FREAD(data, LineSize, Height, file);

							for (y = 0; y < Height; y++) {

								nr = (y) * (LineSize);

								for (x = 0; x < Width; x++) {
									CANI_CONVERT_VARIABLE Value = ((((CANI_CONVERT_VARIABLE)((CANI_BYTE*)data)[nr]) * 0xFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Red = (CANI_BYTE)Value;
									ImageNow->Green = (CANI_BYTE)Value;
									ImageNow->Blue = (CANI_BYTE)Value;
									ImageNow->Alpha = 0xFF;

									ImageNow++;
									if (ImageNow >= ImageMax) break;
									nr++;
								}
								if (ImageNow >= ImageMax) break;
							}

							CANI_FREE(data);
						}

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}
				}
			} else if (Type[1] == '3') {
				if (Width > 0 && Height > 0) {

					CANI_MEMORY_SIZE Size;
					CANI_DWORD Maximum = 0;
					CANI_DWORD n;

					CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));
					if (!rv) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return 0;
					}
					CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
					rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));
					if (!rv->Images) { CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

					rv->ImagesNR = 1;
					rv->Width = Width;
					rv->Height = Height;
					Size = Width * Height;

					rv->Format.format = CAF_FORMAT_RGB;
					rv->Format.rgb.Red = 8;
					rv->Format.rgb.Green = 8;
					rv->Format.rgb.Blue = 8;
					rv->Format.rgb.Alpha = 8;

					if (Line) CANI_FREE(Line);
					Line = CANI_ReadLine(file);

					for (n = 0; Line[n] >= '0' && Line[n] <= '9'; n++) {
						Maximum *= 10;
						Maximum += (Line[n] - '0');
					}

					if (Maximum == 0) {
						CANI_FREE(rv->Images); CANI_FREE(rv); return 0;
					}

					if (Maximum > 0xFF) {

						CANI_BGRA16QUAD* ImageNow;
						CANI_BGRA16QUAD* ImageMax;
						CANI_BYTE color = 0;
						CANI_BGRA16QUAD* Image = ((CANI_BGRA16QUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						rv->Images->Style |= CANI_STYLE_16BITS_IMAGE;
						ImageNow = Image;
						ImageMax = &Image[Size];

						do {
							CANI_CONVERT_VARIABLE Value = 0;
							CANI_BYTE* Now;
							if (Line) CANI_FREE(Line);
							Line = CANI_ReadLine(file);
							Now = Line;
							while (*Now != 0 && *Now != 0x0a) {

								if (*Now >= '0' && *Now <= '9') {
									Value *= 10;
									Value += (*Now - '0');
								}
								else if (*Now == ' ') {
									Value = ((Value * 0xFFFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									if (color == 0) {
										ImageNow->Alpha = 0xFFFF;
										ImageNow->Red = (CANI_WORD)Value;
										color = 1;
									}
									else if (color == 1) {
										ImageNow->Green = (CANI_WORD)Value;
										color = 2;
									}
									else {
										ImageNow->Blue = (CANI_WORD)Value;
										ImageNow++;
										if (ImageNow >= ImageMax) break;
										color = 0;

									}
									Value = 0;
								}
								Now++;
							}
						} while (Line[0]);

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}
					else {
						CANI_BGRAQUAD* ImageNow;
						CANI_BGRAQUAD* ImageMax;
						CANI_BYTE color = 0;

						CANI_BGRAQUAD* Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						ImageNow = Image;
						ImageMax = &Image[Size];

						do {
							CANI_CONVERT_VARIABLE Value = 0;
							CANI_BYTE* Now;
							if (Line) CANI_FREE(Line);
							Line = CANI_ReadLine(file);
							Now = Line;
							while (*Now != 0 && *Now != 0x0a) {

								if (*Now >= '0' && *Now <= '9') {
									Value *= 10;
									Value += (*Now - '0');
								}
								else if (*Now == ' ') {
									Value = ((Value * 0xFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									if (color == 0) {
										ImageNow->Alpha = 0xFF;
										ImageNow->Red = (CANI_BYTE)Value;
										color = 1;
									}
									else if (color == 1) {
										ImageNow->Green = (CANI_BYTE)Value;
										color = 2;
									}
									else {
										ImageNow->Blue = (CANI_BYTE)Value;
										ImageNow++;
										if (ImageNow >= ImageMax) break;
										color = 0;

									}
									Value = 0;
								}
								Now++;
							}
						} while (Line[0]);

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}
				}

			}
			else if (Type[1] == '6') {
				if (Width > 0 && Height > 0) {
					CANI_MEMORY_SIZE Size;
					CANI_DWORD Maximum = 0;
					CANI_DWORD n;

					CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));
					if (!rv) {
						CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
						return 0;
					}
					CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
					rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));
					if (!rv->Images) { CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
					CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

					rv->ImagesNR = 1;
					rv->Width = Width;
					rv->Height = Height;
					Size = Width * Height;

					if (Line) CANI_FREE(Line);
					Line = CANI_ReadLine(file);

					rv->Format.format = CAF_FORMAT_RGB;
					rv->Format.rgb.Red = 8;
					rv->Format.rgb.Green = 8;
					rv->Format.rgb.Blue = 8;
					rv->Format.rgb.Alpha = 8;

					for (n = 0; Line[n] >= '0' && Line[n] <= '9'; n++) {
						Maximum *= 10;
						Maximum += (Line[n] - '0');
					}

					if (Maximum == 0) {
						CANI_FREE(rv->Images); CANI_FREE(rv); return 0;
					}

					if (Maximum > 0xFF) {
						CANI_BGRA16QUAD* ImageNow;
						CANI_BGRA16QUAD* ImageMax;
						CANI_DWORD LineSize;
						CANI_BYTE* data;

						CANI_BGRA16QUAD* Image = ((CANI_BGRA16QUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						rv->Images->Style |= CANI_STYLE_16BITS_IMAGE;
						ImageNow = Image;
						ImageMax = &Image[Size];

						LineSize = Width;

						data = CANI_MALLOC(LineSize * Height * sizeof(CANI_WORD));

						if (data) {
							CANI_DWORD x, y;
							CANI_DWORD nr = 0;
							CANI_BYTE color = 0;
							CANI_FREAD(data, LineSize, Height, file);
							Size = LineSize * Height;

							for (y = 0; y < Height; y++) {

								nr = (y) * (LineSize);

								for (x = 0; x < Width && nr <= (Size - 3); x++) {
									CANI_CONVERT_VARIABLE Value = ((((CANI_CONVERT_VARIABLE)((CANI_BYTE*)data)[nr]) * 0xFFFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Red = (CANI_BYTE)Value;
									nr++;
									Value = ((((CANI_CONVERT_VARIABLE)((CANI_BYTE*)data)[nr]) * 0xFFFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Green = (CANI_BYTE)Value;
									nr++;
									Value = ((((CANI_CONVERT_VARIABLE)((CANI_BYTE*)data)[nr]) * 0xFFFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Blue = (CANI_BYTE)Value;
									nr++;
									ImageNow->Alpha = 0xFFFF;
									ImageNow++;
									if (ImageNow >= ImageMax) break;
								}
								if (ImageNow >= ImageMax) break;
							}

							CANI_FREE(data);
						}

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}
					else {
						CANI_BGRAQUAD* ImageNow;
						CANI_BGRAQUAD* ImageMax;
						CANI_DWORD LineSize;
						CANI_BYTE* data;

						CANI_BGRAQUAD* Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));
						if (!Image) { (rv->Images); CANI_FREE(rv); CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY); return 0; }
						ImageNow = Image;
						ImageMax = &Image[Size];

						LineSize = 3 * Width;

						data = CANI_MALLOC(LineSize * Height);

						if (data) {
							CANI_DWORD x, y;
							CANI_DWORD nr = 0;
							CANI_BYTE color = 0;
							CANI_FREAD(data, LineSize, Height, file);
							Size = LineSize * Height;

							for (y = 0; y < Height; y++) {

								nr = (y) * (LineSize);

								for (x = 0; x < Width && nr <= ( Size - 3 ); x++) {
									CANI_CONVERT_VARIABLE Value = ((((CANI_CONVERT_VARIABLE)((CANI_BYTE*)data)[nr]) * 0xFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Red = (CANI_BYTE)Value;
									nr++;
									Value = ((((CANI_CONVERT_VARIABLE)((CANI_BYTE*)data)[nr]) * 0xFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Green = (CANI_BYTE)Value;
									nr++;
									Value = ((((CANI_CONVERT_VARIABLE)((CANI_BYTE*)data)[nr]) * 0xFF) / ((CANI_CONVERT_VARIABLE)Maximum));
									ImageNow->Blue = (CANI_BYTE)Value;
									nr++;
									ImageNow->Alpha = 0xFF;
									ImageNow++;
									if (ImageNow >= ImageMax) break;
								}
								if (ImageNow >= ImageMax) break;
							}

							CANI_FREE(data);
						}

						if (Line) CANI_FREE(Line);
						CANI_FlipImageVertically(rv, 0);
						return rv;
					}
				}
			}
		}
	}
	return 0;
}