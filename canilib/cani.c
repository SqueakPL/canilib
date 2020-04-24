#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "cani.h"

CANI_EXPORT void CANI_FreeAnimation( CANI_ANIMATION *animation ) {
	if ( animation ) {
		CANI_DWORD n;
		if ( animation->Images ) {
			for ( n = 0; n < animation->ImagesNR; n++ ) {
				if ( animation->Images[n].Image ) CANI_FREE( animation->Images[n].Image );
			}
			CANI_FREE( animation->Images );
		}
		if (animation->gAlpha) CANI_FREE(animation->gAlpha);
		CANI_FREE( animation );
	}
}

CANI_EXPORT CANI_CHAR CANI_ResetAnimation(CANI_ANIMATION* animation) {

	if (animation) {
		animation->Time = 0;
		animation->ImagesNow = 0;

		return TRUE;
	}

	return FALSE;
}

CANI_EXPORT CANI_ANIMATION* CANI_CreateAnimation( CANI_LONG Width, CANI_LONG Height, CANI_FORMAT_V2 *Format ) {

	CANI_QWORD Size;
	CANI_ANIMATION* rv;

	CANI_SetLastError(CANI_ERROR_NO_ERROR);

	if (Width < 1 || Height < 1) {
		CANI_SetLastError(CANI_ERROR_SMALL_IMAGE_SIZES);
		return 0;
	}

	Size = (CANI_QWORD)Width * (CANI_QWORD)Height;

	if (Size > CANI_MAX_32BIT_SIZE) {
		CANI_SetLastError(CANI_ERROR_TOO_BIG_IMAGE);
		return 0;
	}

	rv = (CANI_ANIMATION*)CANI_MALLOC(sizeof(CANI_ANIMATION));
	if (rv) {
		CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
		rv->Width = Width;
		rv->Height = Height;
		CANI_MEMCPY(&rv->Format, Format, sizeof(CANI_FORMAT_V2));
		rv->Format.options &= (~(CANI_OPTION_ALPHA| CANI_OPTION_16BITS_ALPHA));
	} else CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
	return rv;
}

CANI_EXPORT CANI_CHAR CANI_SetGlobalAlpha16(CANI_ANIMATION* animation, CANI_WORD *Alpha ) {

	CANI_SetLastError(CANI_ERROR_NO_ERROR);

	if (animation) {

		if (Alpha) {

			CANI_DWORD Size = animation->Width * animation->Height;

			CANI_WORD* AlphaNew = CANI_MALLOC(Size * sizeof(CANI_WORD));

			if (!AlphaNew) {
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}

			CANI_MEMCPY(AlphaNew,Alpha, Size * sizeof(CANI_WORD));

			if (animation->gAlpha) CANI_FREE(animation->gAlpha);

			animation->gAlpha = (CANI_BYTE*)AlphaNew;

			animation->Format.options |= (CANI_OPTION_ALPHA | CANI_OPTION_16BITS_ALPHA);

			return TRUE;
		}
		else {

			if (animation->gAlpha) CANI_FREE(animation->gAlpha);

			animation->gAlpha = 0;

			animation->Format.options &= (~(CANI_OPTION_ALPHA | CANI_OPTION_16BITS_ALPHA));

			return TRUE;
		}
	}

	CANI_SetLastError(CANI_ERROR_NO_ANIMATION);

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_AddImage16(CANI_ANIMATION* animation, CANI_BGRA16QUAD* Image, CANI_DWORD Style,
	CANI_DWORD Time, CANI_RGBA16QUAD* bg_color ) {

	CANI_SetLastError(CANI_ERROR_NO_ERROR);

	if (animation) {
		CANI_DWORD Size;

		if ( animation->Images && animation->ImagesNR > 0 ) {
			CANI_FRAME_DATA* Images = CANI_REALLOC(animation->Images, (animation->ImagesNR + 1) * sizeof(CANI_FRAME_DATA));
			if (Images) {
				animation->Images = Images;
			} else {
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return FALSE;
			}
		}
		else {
			animation->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));
			animation->ImagesNR = 0;
		}

		if (animation->Images == 0) {
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return FALSE;
		}

		CANI_MEMSET(&animation->Images[animation->ImagesNR],0x00,sizeof(CANI_FRAME_DATA));

		Size = animation->Width * animation->Height;

		animation->Images[animation->ImagesNR].Image = CANI_MALLOC(Size*sizeof(CANI_BGRA16QUAD));

		if (animation->Images[animation->ImagesNR].Image == 0) {
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return FALSE;
		}

		CANI_MEMCPY(animation->Images[animation->ImagesNR].Image, Image, Size * sizeof(CANI_BGRA16QUAD));

		Style |= CANI_STYLE_16BITS_IMAGE;

		animation->Images[animation->ImagesNR].Style = Style;

		animation->Images[animation->ImagesNR].Time = Time;

		CANI_MEMCPY(&animation->Images[animation->ImagesNR].bgColorRGB16, bg_color, sizeof(CANI_RGBA16QUAD));

		CANI_RGBA16_2_RGBA((CANI_RGBAQUAD*)&animation->Images[animation->ImagesNR].bgColorRGB,
			(CANI_RGBA16QUAD*)&animation->Images[animation->ImagesNR].bgColorRGB16, 1);

		animation->ImagesNR++;

		return TRUE;
	}

	CANI_SetLastError(CANI_ERROR_NO_ANIMATION);

	return FALSE;
}

CANI_EXPORT void CANI_ConvertBackgroundColorToAlpha( CANI_ANIMATION *animation ) {
	if ( animation ) {
		CANI_DWORD n;
		if ( animation->Images ) {
			for ( n = 0; n < animation->ImagesNR; n++ ) {
				if ( animation->Images[n].Image && ( animation->Images[n].Style & CANI_STYLE_TRANSPARENT ) ) {
					if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {
						CANI_RGBA16QUAD* bg_color = &animation->Images[n].bgColorRGB16;
						CANI_BGRA16QUAD* ImageNow = ((CANI_BGRA16QUAD*)animation->Images[n].Image);
						CANI_BGRA16QUAD* ImageMax = &((CANI_BGRA16QUAD*)animation->Images[n].Image)[animation->Width * animation->Height];

						while (ImageNow < ImageMax) {
							if (ImageNow->Red == bg_color->Red && ImageNow->Green == bg_color->Green && ImageNow->Blue == bg_color->Blue) {
								ImageNow->Alpha = 0x0000;
							}
							else {
								ImageNow->Alpha = 0xFFFF;
							}
							ImageNow++;
						}
					}
					else {
						CANI_RGBAQUAD* bg_color = &animation->Images[n].bgColorRGB;
						CANI_BGRAQUAD* ImageNow = ((CANI_BGRAQUAD*)animation->Images[n].Image);
						CANI_BGRAQUAD* ImageMax = &((CANI_BGRAQUAD*)animation->Images[n].Image)[animation->Width * animation->Height];

						while (ImageNow < ImageMax) {
							if (ImageNow->Red == bg_color->Red && ImageNow->Green == bg_color->Green && ImageNow->Blue == bg_color->Blue) {
								ImageNow->Alpha = 0x00;
							}
							else {
								ImageNow->Alpha = 0xFF;
							}
							ImageNow++;
						}
					}
				}
			}
		}
	}
}

CANI_EXPORT CANI_LONG CANI_NextFrame(CANI_ANIMATION* animation) {
	if (animation) {
		animation->ImagesNow++;
		if (animation->ImagesNow >= animation->ImagesNR) animation->ImagesNow = 0;
		return (CANI_LONG)animation->ImagesNow;
	}
	return -1;
}

CANI_EXPORT CANI_LONG CANI_CalculateNextFrame(CANI_ANIMATION* animation, CANI_DWORD AddTime ) {

	if (animation) {

		animation->Time += AddTime;

		while ( animation->Images[animation->ImagesNow].Time && animation->Time >
			((CANI_LONG)animation->Images[animation->ImagesNow].Time) ) {

			animation->Time -= ((CANI_LONG)animation->Images[animation->ImagesNow].Time);
			animation->ImagesNow++;

			if (animation->ImagesNow >= animation->ImagesNR) animation->ImagesNow = 0;
		}

		return (CANI_LONG)(animation->ImagesNow);
	}

	return -1;
}

CANI_EXPORT CANI_DWORD CANI_GetImageSize(CANI_ANIMATION* animation, CANI_DWORD *Width, CANI_DWORD *Height ) {

	if (animation) {
		if (Width) *Width = animation->Width;
		if (Height) *Height = animation->Height;
		return (animation->Width* animation->Height);
	}

	return 0;
}

CANI_EXPORT CANI_LONG CANI_GetNumberOfFrames(CANI_ANIMATION* animation) {

	if (animation) {
		return (CANI_LONG)(animation->ImagesNR);
	}

	return -1;
}

CANI_EXPORT CANI_LONG CANI_GetCurrentFrame(CANI_ANIMATION* animation) {

	if (animation) {
		return (animation->ImagesNow);
	}

	return -1;
}

CANI_EXPORT CANI_CHAR CANI_GetImageOfCurrentFrame(CANI_ANIMATION* animation, CANI_BGRAQUAD* ImageData ) {

	if (animation && (animation->ImagesNow < animation->ImagesNR) && animation->Images[animation->ImagesNow].Image) {
		//CANI_MEMCPY(ImageData, animation->Images[animation->ImagesNow].Image, animation->Width * animation->Height * sizeof(CANI_BGRAQUAD) );

		if (animation->Images[animation->ImagesNow].Style & CANI_STYLE_16BITS_IMAGE) {

			CANI_BGRA16_2_BGRA(ImageData, (CANI_BGRA16QUAD*)animation->Images[animation->ImagesNow].Image, animation->Width * animation->Height);

		}
		else {
			CANI_MEMCPY(ImageData, animation->Images[animation->ImagesNow].Image, animation->Width * animation->Height * sizeof(CANI_BGRAQUAD));

		}
		return TRUE;
	}

	return FALSE;
}

CANI_EXPORT CANI_LONG CANI_GetFrameDisplayTime(CANI_ANIMATION* animation, CANI_DWORD FrameNR) {

	if (animation && FrameNR < animation->ImagesNR) {
		return (CANI_LONG)animation->Images[FrameNR].Time;
	}

	return -1;
}

CANI_EXPORT CANI_CHAR CANI_GetFrameBackgroundColor(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_RGBAQUAD *bgColorRGB ) {

	if (animation && FrameNR < animation->ImagesNR) {

		CANI_MEMCPY(bgColorRGB, &animation->Images[FrameNR].bgColorRGB, sizeof(CANI_RGBAQUAD) );
		
		if ( animation->Images[FrameNR].Style & CANI_STYLE_TRANSPARENT ) return TRUE;
		else return FALSE;
	}

	return -1;
}

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrame(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_BGRAQUAD* ImageData) {

	if ( animation && FrameNR < animation->ImagesNR && animation->Images[FrameNR].Image ) {

		if (animation->Images[animation->ImagesNow].Style & CANI_STYLE_16BITS_IMAGE) {

			CANI_BGRA16_2_BGRA(ImageData, (CANI_BGRA16QUAD*)animation->Images[FrameNR].Image, animation->Width * animation->Height);

		}
		else {
			CANI_MEMCPY(ImageData, animation->Images[FrameNR].Image, animation->Width * animation->Height * sizeof(CANI_BGRAQUAD));

		}
		return TRUE;
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrame16(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_BGRA16QUAD* ImageData) {

	if (animation && FrameNR < animation->ImagesNR && animation->Images[FrameNR].Image ) {

		if (animation->Images[animation->ImagesNow].Style & CANI_STYLE_16BITS_IMAGE) {

			CANI_MEMCPY(ImageData, animation->Images[FrameNR].Image, animation->Width * animation->Height * sizeof(CANI_BGRA16QUAD));

		}
		else {

			CANI_BGRA_2_BGRA16(ImageData, (CANI_BGRAQUAD*)animation->Images[FrameNR].Image, animation->Width * animation->Height);

		}
		return TRUE;
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_GetAlphaOfFrame(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_BYTE* ImageData ) {

	if (animation && FrameNR < animation->ImagesNR && animation->Images[FrameNR].Image ) {

		CANI_BYTE* ImageDataNow = ImageData;

		if (animation->Images[FrameNR].Style & CANI_STYLE_16BITS_IMAGE) {
			CANI_BGRA16QUAD* ImageNow = ((CANI_BGRA16QUAD*)animation->Images[FrameNR].Image);
			CANI_BGRA16QUAD* ImageMax = &((CANI_BGRA16QUAD*)animation->Images[FrameNR].Image)[animation->Width * animation->Height];

			if (animation->Images[FrameNR].Style & CANI_STYLE_TRANSPARENT) {

				CANI_RGBA16QUAD* bg_color = &animation->Images[FrameNR].bgColorRGB16;
				CANI_BYTE* ImageDataNow = ImageData;

				while (ImageNow < ImageMax) {
					if (ImageNow->Red == bg_color->Red && ImageNow->Green == bg_color->Green && ImageNow->Blue == bg_color->Blue) {
						*ImageDataNow = 0x00;
					}
					else *ImageDataNow = 0xFF;
					ImageDataNow++;
					ImageNow++;
				}
			}
			else {

				while (ImageNow < ImageMax) {
					*ImageDataNow = (ImageNow->Alpha >> 8);
					ImageDataNow++;
					ImageNow++;
				}
			}
		}
		else {
			CANI_BGRAQUAD* ImageNow = ((CANI_BGRAQUAD*)animation->Images[FrameNR].Image);
			CANI_BGRAQUAD* ImageMax = &((CANI_BGRAQUAD*)animation->Images[FrameNR].Image)[animation->Width * animation->Height];

			if (animation->Images[FrameNR].Style & CANI_STYLE_TRANSPARENT) {

				CANI_RGBAQUAD* bg_color = &animation->Images[FrameNR].bgColorRGB;
				CANI_BYTE* ImageDataNow = ImageData;

				while (ImageNow < ImageMax) {
					if (ImageNow->Red == bg_color->Red && ImageNow->Green == bg_color->Green && ImageNow->Blue == bg_color->Blue) {
						*ImageDataNow = 0x00;
					}
					else *ImageDataNow = 0xFF;
					ImageDataNow++;
					ImageNow++;
				}
			}
			else {
				while (ImageNow < ImageMax) {
					*ImageDataNow = ImageNow->Alpha;
					ImageDataNow++;
					ImageNow++;
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_GetGlobalAlpha16(CANI_ANIMATION* animation, CANI_WORD* ImageData ) {

	if (animation&& (animation->Format.options & CANI_OPTION_ALPHA)) {

		if (animation->Format.options & CANI_OPTION_16BITS_ALPHA) {

			if (animation->gAlpha) CANI_MEMCPY(ImageData, animation->gAlpha, animation->Width * animation->Height * sizeof(CANI_WORD));
			else return FALSE;

		} 
		else {
			if (animation->gAlpha) CANI_ALPHA_2_ALPHA16(ImageData, animation->gAlpha, animation->Width * animation->Height);
			else return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_GetGlobalAlpha(CANI_ANIMATION* animation, CANI_BYTE* ImageData) {

	if (animation && (animation->Format.options & CANI_OPTION_ALPHA)) {

		if (animation->Format.options & CANI_OPTION_16BITS_ALPHA) {
			//MessageBox(0, L"1", L"CANI_GetGlobalAlpha", 0);
			if (animation->gAlpha)
				CANI_ALPHA16_2_ALPHA(ImageData, (CANI_WORD*)animation->gAlpha, animation->Width * animation->Height);
			else return FALSE;
		}
		else {
			//MessageBox(0, L"2", L"CANI_GetGlobalAlpha", 0);
			//MessageBox(0, L"1", L"CANI_GetGlobalAlpha", 0);
			if (animation->gAlpha)
				CANI_MEMCPY(ImageData, animation->gAlpha, animation->Width * animation->Height * sizeof(CANI_BYTE));
			else return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_GetImageOfCurrentFrameRGBA(CANI_ANIMATION* animation, CANI_RGBAQUAD* ImageData) {

	if (animation&& (animation->ImagesNow< animation->ImagesNR ) && animation->Images[animation->ImagesNow].Image) {

		//if (animation->Format.options & CANI_OPTION_16BITS_ALPHA) {
		if (animation->Images[animation->ImagesNow].Style & CANI_STYLE_16BITS_IMAGE) {

			CANI_BGRA16QUAD* src = (CANI_BGRA16QUAD*)animation->Images[animation->ImagesNow].Image;
			CANI_BGRA16QUAD* src_max = &src[animation->Width * animation->Height];
			CANI_RGBAQUAD* dest = ImageData;

			while (src < src_max) {
				dest->Red = ( src->Red >> 8 );
				dest->Green = ( src->Green >> 8 );
				dest->Blue = ( src->Blue >> 8 );
				dest->Alpha = ( src->Alpha >> 8 );
				src++;
				dest++;
			}

			return TRUE;
		}
		else {

			CANI_BGRAQUAD* src = (CANI_BGRAQUAD*)animation->Images[animation->ImagesNow].Image;
			CANI_BGRAQUAD* src_max = &src[animation->Width * animation->Height];
			CANI_RGBAQUAD* dest = ImageData;

			while (src < src_max) {
				dest->Red = src->Red;
				dest->Green = src->Green;
				dest->Blue = src->Blue;
				dest->Alpha = src->Alpha;
				src++;
				dest++;
			}

			return TRUE;

		}
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrameRGBA(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_RGBAQUAD* ImageData) {

	if (animation && (FrameNR < animation->ImagesNR) && animation->Images[FrameNR].Image) {

		//if (animation->Format.options & CANI_OPTION_16BITS_ALPHA) {
		if (animation->Images[FrameNR].Style & CANI_STYLE_16BITS_IMAGE) {

			CANI_BGRA16QUAD* src = (CANI_BGRA16QUAD*)animation->Images[FrameNR].Image;
			CANI_BGRA16QUAD* src_max = &src[animation->Width * animation->Height];
			CANI_RGBAQUAD* dest = ImageData;

			while (src < src_max) {
				dest->Red = (src->Red >> 8);
				dest->Green = (src->Green >> 8);
				dest->Blue = (src->Blue >> 8);
				dest->Alpha = (src->Alpha >> 8);
				src++;
				dest++;
			}

			return TRUE;
		}
		else {

			CANI_BGRAQUAD* src = (CANI_BGRAQUAD*)animation->Images[FrameNR].Image;
			CANI_BGRAQUAD* src_max = &src[animation->Width * animation->Height];
			CANI_RGBAQUAD* dest = ImageData;

			while (src < src_max) {
				dest->Red = src->Red;
				dest->Green = src->Green;
				dest->Blue = src->Blue;
				dest->Alpha = src->Alpha;
				src++;
				dest++;
			}

			return TRUE;

		}
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_GetImageOfFrameRGBA16(CANI_ANIMATION* animation, CANI_DWORD FrameNR, CANI_RGBA16QUAD* ImageData) {

	if (animation && (FrameNR < animation->ImagesNR) && animation->Images[FrameNR].Image) {

		//if (animation->Format.options & CANI_OPTION_16BITS_ALPHA) {
		if (animation->Images[FrameNR].Style & CANI_STYLE_16BITS_IMAGE) {

			CANI_BGRA16QUAD* src = (CANI_BGRA16QUAD*)animation->Images[FrameNR].Image;
			CANI_BGRA16QUAD* src_max = &src[animation->Width * animation->Height];
			CANI_RGBA16QUAD* dest = ImageData;

			while (src < src_max) {
				dest->Red = src->Red;
				dest->Green = src->Green;
				dest->Blue = src->Blue;
				dest->Alpha = src->Alpha;
				src++;
				dest++;
			}

			return TRUE;
		}
		else {

			CANI_BGRAQUAD* src = (CANI_BGRAQUAD*)animation->Images[FrameNR].Image;
			CANI_BGRAQUAD* src_max = &src[animation->Width * animation->Height];
			CANI_RGBA16QUAD* dest = ImageData;

			while (src < src_max) {
				dest->Red = (((CANI_WORD)src->Red)<<8) | ((CANI_WORD)src->Red);
				dest->Green = (((CANI_WORD)src->Green) << 8) | ((CANI_WORD)src->Green);
				dest->Blue = (((CANI_WORD)src->Blue) << 8) | ((CANI_WORD)src->Blue);
				dest->Alpha = (((CANI_WORD)src->Alpha) << 8) | ((CANI_WORD)src->Alpha);
				src++;
				dest++;
			}

			return TRUE;

		}
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_IsImageOfFrame16Bits(CANI_ANIMATION* animation, CANI_DWORD FrameNR ) {

	if (animation && (FrameNR < animation->ImagesNR) && animation->Images[FrameNR].Image) {

		//if (animation->Format.options & CANI_OPTION_16BITS_ALPHA) {
		if (animation->Images[FrameNR].Style & CANI_STYLE_16BITS_IMAGE) return CANI_IMAGE_IS_16_BITS;
		else return CANI_IMAGE_IS_8_BITS;
	}

	return 0;
}

CANI_EXPORT CANI_ANIMATION* CANI_CreateResizedAnimation(CANI_ANIMATION* animation, CANI_LONG dest_cx, CANI_LONG dest_cy,
	CANI_BYTE method, CANI_PROGRESS_FUNC Progress) {

	if (animation) {

		CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));

		if (rv) {
			CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));
			rv->Images = CANI_MALLOC(animation->ImagesNR * sizeof(CANI_FRAME_DATA));

			if (rv->Images) {
				CANI_DWORD size = dest_cx * dest_cy;
				CANI_DWORD n;
				CANI_BYTE ProgressNow;
				CANI_MEMSET(rv->Images, 0x00, animation->ImagesNR * sizeof(CANI_FRAME_DATA));

				for (n = 0; n < animation->ImagesNR; n++) {
					if (animation->Images[n].Style & CANI_STYLE_16BITS_IMAGE) rv->Images[n].Image = CANI_MALLOC(size * sizeof(CANI_BGRA16QUAD));
					else rv->Images[n].Image = CANI_MALLOC(size * sizeof(CANI_BGRAQUAD));

					if (!rv->Images[n].Image) {
						if (n > 0) {
							n--;
							while (n >= 0) {
								CANI_FREE(rv->Images[n].Image);
								n--;
							}
						}
						CANI_FREE(rv->Images);
						CANI_FREE(rv);
						return 0;
					}
				}

				rv->Width = dest_cx;
				rv->Height = dest_cy;
				rv->ImagesNR = animation->ImagesNR;
				rv->ImagesNow = 0;
				rv->Time = 0;
				CANI_MEMCPY(&rv->Format, &animation->Format, sizeof(CANI_FORMAT_V2));

				ProgressNow = 0;
				if (Progress) Progress(0);

				for (n = 0; n < animation->ImagesNR; n++) {
					rv->Images[n].bgColor = animation->Images[n].bgColor;
					rv->Images[n].Time = animation->Images[n].Time;
					rv->Images[n].Style = animation->Images[n].Style;

					if (rv->Images[n].Style & CANI_STYLE_16BITS_IMAGE) {

						switch (method) {
						case CANI_INTERPOLATION:
							CANI_InterpolationResize16((CANI_BGRA16QUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRA16QUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						case CANI_SQUARE_INTERPOLATION:
							CANI_SquareInterpolationResize16((CANI_BGRA16QUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRA16QUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						case CANI_CUBIC_INTERPOLATION:
							CANI_CubicInterpolationResize16((CANI_BGRA16QUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRA16QUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						case CANI_LAGRANGE_INTERPOLATION:
							CANI_LagrangeInterpolationResize16((CANI_BGRA16QUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRA16QUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						default://case CANI_FAST_RESIZE:
							CANI_FastResize16((CANI_BGRA16QUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRA16QUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						}
					}
					else {

						switch (method) {
						case CANI_INTERPOLATION:
							CANI_InterpolationResize((CANI_BGRAQUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRAQUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						case CANI_SQUARE_INTERPOLATION:
							CANI_SquareInterpolationResize((CANI_BGRAQUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRAQUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						case CANI_CUBIC_INTERPOLATION:
							CANI_CubicInterpolationResize((CANI_BGRAQUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRAQUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						case CANI_LAGRANGE_INTERPOLATION:
							CANI_LagrangeInterpolationResize((CANI_BGRAQUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRAQUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						default:
							CANI_FastResize((CANI_BGRAQUAD*)(animation->Images[n].Image), animation->Width, animation->Height, (CANI_BGRAQUAD*)(rv->Images[n].Image), dest_cx, dest_cy);
							break;
						}

					}

					if (Progress) {

						CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / animation->ImagesNR);

						if (ProgressNow2 != ProgressNow) {
							ProgressNow = ProgressNow2;
							Progress(ProgressNow);
						}

					}
				}

				if ( (rv->Format.options & CANI_OPTION_ALPHA) && animation->gAlpha ) {
					if (rv->Format.options & CANI_OPTION_16BITS_ALPHA) {
						rv->gAlpha = CANI_MALLOC(dest_cx * dest_cy * sizeof(CANI_WORD));

						if (rv->gAlpha) switch (method) {
						case CANI_INTERPOLATION:
							CANI_InterpolationResizeAlpha16((CANI_WORD*)animation->gAlpha, animation->Width, animation->Height, (CANI_WORD*)rv->gAlpha, dest_cx, dest_cy);
							break;
						case CANI_SQUARE_INTERPOLATION:
							CANI_SquareInterpolationResizeAlpha16((CANI_WORD*)animation->gAlpha, animation->Width, animation->Height, (CANI_WORD*)rv->gAlpha, dest_cx, dest_cy);
							break;
						case CANI_CUBIC_INTERPOLATION:
							CANI_CubicInterpolationResizeAlpha16((CANI_WORD*)animation->gAlpha, animation->Width, animation->Height, (CANI_WORD*)rv->gAlpha, dest_cx, dest_cy);
							break;
						case CANI_LAGRANGE_INTERPOLATION:
							CANI_LagrangeInterpolationResizeAlpha16((CANI_WORD*)animation->gAlpha, animation->Width, animation->Height, (CANI_WORD*)rv->gAlpha, dest_cx, dest_cy);
							break;
						default:
							CANI_FastResizeAlpha16((CANI_WORD*)animation->gAlpha, animation->Width, animation->Height, (CANI_WORD*)rv->gAlpha, dest_cx, dest_cy);
							break;
						}
					}
					else {
						rv->gAlpha = CANI_MALLOC(dest_cx * dest_cy * sizeof(CANI_BYTE));

						if (rv->gAlpha) switch (method) {
						case CANI_INTERPOLATION:
							CANI_InterpolationResizeAlpha(animation->gAlpha, animation->Width, animation->Height, rv->gAlpha, dest_cx, dest_cy);
							break;
						case CANI_SQUARE_INTERPOLATION:
							CANI_SquareInterpolationResizeAlpha(animation->gAlpha, animation->Width, animation->Height, rv->gAlpha, dest_cx, dest_cy);
							break;
						case CANI_CUBIC_INTERPOLATION:
							CANI_CubicInterpolationResizeAlpha(animation->gAlpha, animation->Width, animation->Height, rv->gAlpha, dest_cx, dest_cy);
							break;
						case CANI_LAGRANGE_INTERPOLATION:
							CANI_LagrangeInterpolationResizeAlpha(animation->gAlpha, animation->Width, animation->Height, rv->gAlpha, dest_cx, dest_cy);
							break;
						default:
							CANI_FastResizeAlpha(animation->gAlpha, animation->Width, animation->Height, rv->gAlpha, dest_cx, dest_cy);
							break;
						}
					}
					//rv->Format.options &= (~(CANI_OPTION_ALPHA));
					//for (n = 0; n < animation->ImagesNR; n++) rv->Images[n].Style |= CANI_STYLE_ALPHA;
				}
				else {
					rv->Format.options &= (~(CANI_OPTION_ALPHA));
					rv->gAlpha = 0;
				}

				if (Progress) Progress(100);

			}
			else {
				CANI_FREE(rv);
				return 0;
			}

		}

		return rv;

	}

	return 0;
}

//#include "lcaf.h"
		
CANI_EXPORT CANI_ANIMATION* CANI_ReadAnimation( FILE* File, CANI_PROGRESS_FUNC Progress ) {


	if ( File ) {
		size_t NumberOfBytesRead;

		CANI_ANIMATION *rv = 0;
		CANI_SIZE AniSize = { 0, 0 };
		CANI_DWORD ImagesNR = 0;
		CANI_FORMAT_V2 settings;
		CANI_BYTE *Src=0;
		CANI_BYTE *Dest=0;
		CANI_BYTE *Dest2=0;
		char id[5];
		CANI_FILE_POS pos = CANI_FTELL(File);

		CANI_BYTE ProgressNow = 0;
		if (Progress) Progress(0);

		CANI_SetLastError( CANI_ERROR_NO_ERROR );

		CANI_FREAD( id, 1, 5, File );
		
		if ( id[0] != 'C' || id[1] != 'A' || id[2] != 'N' || id[3] != 'I' ) {
			CANI_ANIMATION *rv;
			CANI_FSEEK(File, pos, SEEK_SET);
			//CANI_FCLOSE( File );
			// CANI_SetLastError( CANI_ERROR_NO_FREE_MEMORY );
			rv = CANI_ReadLCAF( File );
			if (rv == 0 && CANI_GetLastError() == CANI_ERROR_NO_FREE_MEMORY) {
				//if (Progress) Progress(100);
				return 0;
			}
			if (!rv) {
				CANI_FSEEK(File, pos, SEEK_SET);
				rv = CANI_ReadBMP_FILE(File);
				if (rv == 0 && CANI_GetLastError() == CANI_ERROR_NO_FREE_MEMORY) {
					//if (Progress) Progress(100);
					return 0;
				}
			}
			if (!rv) {
				CANI_FSEEK(File, pos, SEEK_SET);
				rv = CANI_ReadPXM_FILE(File);
				if (rv == 0 && CANI_GetLastError() == CANI_ERROR_NO_FREE_MEMORY) {
					//if (Progress) Progress(100);
					return 0;
				}
			}
#ifdef CANI_USE_LIBAPNG
			if (!rv) {
				CANI_FSEEK(File, pos, SEEK_SET);
				rv = CANI_ReadAPNG_FILE(File, Progress);
				if (rv == 0 && CANI_GetLastError() == CANI_ERROR_NO_FREE_MEMORY) {
					//if (Progress) Progress(100);
					return 0;
				}
			}
#endif
#ifdef CANI_USE_LIBPNG
			if (!rv) {
				CANI_FSEEK(File, pos, SEEK_SET);
				rv = CANI_ReadPNG_FILE(File);
				if (rv == 0 && CANI_GetLastError() == CANI_ERROR_NO_FREE_MEMORY) {
					//if (Progress) Progress(100);
					return 0;
				}
			}
#endif
#ifdef CANI_USE_GIFLIB
			if (!rv) {
				CANI_FSEEK(File, pos, SEEK_SET);
				rv = CANI_ReadGIF_FILE(File, Progress);
				if (rv == 0 && CANI_GetLastError() == CANI_ERROR_NO_FREE_MEMORY) {
					//if (Progress) Progress(100);
					return 0;
				}
			}
#endif
#ifdef CANI_USE_LIBMNG
			if (!rv) {
				CANI_FSEEK(File, pos, SEEK_SET);
				rv = CANI_ReadMNG_FILE(File, Progress);
				if (rv == 0 && CANI_GetLastError() == CANI_ERROR_NO_FREE_MEMORY) {
					//if (Progress) Progress(100);
					return 0;
				}
			}
#endif
			if (!rv) {
				CANI_SetLastError(CANI_ERROR_UNSUPPORTED_FILE);
				//CANI_FCLOSE(File);
			}

			if (rv&&Progress) Progress(100);
			return rv;
		}


		CANI_FREAD( &AniSize, 1, sizeof(AniSize), File );

		CANI_FREAD( &ImagesNR, 1, sizeof(ImagesNR), File );

		//if ( id[4] >= '3' ) CANI_FREAD( &settings, sizeof(settings), 1, File );
		//else CANI_MEMSET( &settings, 0x00, sizeof(settings) );
		//char szCaniBuffer[1024];

		if (id[4] >= '4') {
			CANI_FREAD(&settings, sizeof(settings), 1, File);
		}
		else if (id[4] >= '3')  {
			//sprintf(szCaniBuffer, "%u %u", sizeof(CANI_FORMAT), sizeof(CANI_FORMAT_V2));
			//MessageBoxA(0, szCaniBuffer, 0, 0);

			CANI_FORMAT settings_old;
			CANI_FREAD(&settings_old, 1, sizeof(CANI_FORMAT), File);
			settings.format = settings_old.format;
			settings.options = settings_old.options;
			//CANI_MEMCPY(&settings.rgb, &settings_old.rgb, 3);
			settings.rgb.Red = settings_old.rgb.Red;
			settings.rgb.Green = settings_old.rgb.Green;
			settings.rgb.Blue = settings_old.rgb.Blue;
			settings.rgb.Alpha = 8;
		} else CANI_MEMSET(&settings, 0x00, sizeof(settings));

		///sprintf(szCaniBuffer, "%u %u %u %u", settings.rgb.Red, settings.rgb.Green, settings.rgb.Blue, settings.rgb.Alpha);
		//MessageBoxA(0, szCaniBuffer, "settings", 0);

		if ( ImagesNR > 0 ) {
			CANI_DWORD SrcLen=0, DestLen=0,DestLen2=0; // CANI_MEMORY_SIZE

			if ( ( settings.options & CANI_OPTION_COMPRESS_SEPARATELY ) == 0 ) {

				//MessageBox(0, L"1", 0, 0);

				/*if (id[4] >= '4') {

					CANI_FREAD(&SrcLen, 1, sizeof(CANI_QWORD), File);

					CANI_FREAD(&DestLen, 1, sizeof(CANI_QWORD), File);

					if (settings.options & CANI_OPTION_DOUBLE_COMPRESSION) {

						CANI_FREAD(&DestLen2, 1, sizeof(CANI_QWORD), File);
					}
				}
				else */ {

					CANI_FREAD(&SrcLen, 1, sizeof(CANI_DWORD), File);

					CANI_FREAD(&DestLen, 1, sizeof(CANI_DWORD), File);

					if (settings.options & CANI_OPTION_DOUBLE_COMPRESSION) {

						CANI_FREAD(&DestLen2, 1, sizeof(CANI_DWORD), File);
					}
					else DestLen2 = 0;
				}
				//sprintf(szCaniBuffer, "%u %u %u", SrcLen, DestLen, DestLen2);
				//MessageBoxA(0, szCaniBuffer, 0, 0);

				Src = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * SrcLen);//new CANI_BYTE[ SrcLen ];

				if (!Src) {
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}

				Dest = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * DestLen + 8);//new CANI_BYTE[ DestLen ];

				if (!Dest) {
					CANI_FREE(Src);//delete [] Src;
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}
				//MessageBox(0, L"2", 0, 0);

				NumberOfBytesRead = CANI_FREAD(Src, 1, SrcLen, File);
			}

			if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY) || ( SrcLen > 0 && NumberOfBytesRead >= SrcLen )  ) {
				//MessageBox(0, L"3", 0, 0);
				CANI_MEMORY_SIZE BitMapSize;
				CANI_DWORD n,m;
				CANI_BYTE *DestNow,*DestMax;
				CANI_BYTE *alph4all;
				CANI_CHAR Alpha16;
				//CANI_DWORD msSrcLen = 0, msDestLen = 0, msDestLen2 = 0; // CANI_MEMORY_SIZE

				//msSrcLen = ((CANI_MEMORY_SIZE)SrcLen);
				//msDestLen = ((CANI_MEMORY_SIZE)DestLen);
				//msDestLen2 = ((CANI_MEMORY_SIZE)DestLen2);

				if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

					Dest = CANI_ReadAndDeCompress(File, &DestLen);

					if (DestLen) {
						DestNow = Dest;
						DestMax = &Dest[DestLen];
					}
					else {
						DestNow = 0;
						DestMax = 0;
					}

				} else {

					uncompress(Dest, &DestLen, Src, SrcLen);

					if (settings.options & CANI_OPTION_DOUBLE_COMPRESSION) {

						Dest2 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * DestLen2 + 8);//CANI_BYTE *Dest2 = new CANI_BYTE[ DestLen2 ];

						if (!Dest2) {
							CANI_FREE(Src);//delete [] Src;
							CANI_FREE(Dest);//delete [] Dest;
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}

						uncompress(Dest2, &DestLen2, Dest, DestLen);

						CANI_FREE(Dest);

						Dest = Dest2;

						DestLen = DestLen2;
					}

					CANI_FREE(Src);
					Src = 0;

					DestNow = Dest;
					DestMax = &Dest[DestLen];
				}

				rv = (CANI_ANIMATION*)CANI_MALLOC( sizeof(CANI_ANIMATION) );//new CANI_ANIMATION;

				if ( !rv ) {
					//CANI_FREE( Src );//delete [] Src;
					CANI_FREE( Dest );//delete [] Dest;
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}

				rv->Width = AniSize.Width;
				rv->Height = AniSize.Height;
				rv->gAlpha = 0;

				BitMapSize = rv->Width * rv->Height;

				rv->ImagesNR = ImagesNR;
				rv->ImagesNow = 0;
				rv->Time = 0;
				rv->Images = (CANI_FRAME_DATA*)CANI_MALLOC( sizeof(CANI_FRAME_DATA) * ImagesNR );//new CANI_FRAME_DATA[ImagesNR];

				if ( !rv->Images ) {
					//CANI_FREE( Src );//delete [] Src;
					CANI_FREE( Dest );//delete [] Dest;
					CANI_FREE( rv );//delete [] rv;
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}

				Alpha16 = (settings.rgb.Alpha > 8 ? TRUE : FALSE);

				if ( DestNow && settings.options & CANI_OPTION_ALPHA ) {
					if (Alpha16) {
						if ((DestNow + (sizeof(CANI_WORD) * BitMapSize) ) <= DestMax) {
							alph4all = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);//new CANI_BYTE[BitMapSize];
							if (alph4all) {
								CANI_MEMCPY(alph4all, DestNow, sizeof(CANI_WORD) * BitMapSize);
								for (m = 0; m < BitMapSize; m++) {
									((CANI_WORD*)alph4all)[m] = CAF_UnMask16(((CANI_WORD*)alph4all)[m], settings.rgb.Alpha);
								}
							}
							DestNow += (sizeof(CANI_WORD) * BitMapSize);
						}
						else {
							CANI_FREE(Dest);//delete [] Dest;
							CANI_FREE(rv->Images);
							CANI_FREE(rv);//delete [] rv;
							CANI_SetLastError(CANI_ERROR_END_OF_FILE);
							return 0;
						}

						settings.options |= CANI_OPTION_16BITS_ALPHA;
					}
					else if (id[4] >= '4' && settings.rgb.Alpha == 1) {

						CANI_DWORD b;
						CANI_MEMORY_SIZE Size = BitMapSize / 8;
						if (BitMapSize % 8) Size++;

						if ((DestNow + Size) <= DestMax) {
							alph4all = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);//new CANI_BYTE[BitMapSize];
							if (alph4all) {

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

								for (b = 0; b < BitMapSize;) {
									CANI_BYTE byte = *DestNow;
									for (m = 0; b < BitMapSize && m < 8; m++, b++) {
										if (byte & BitMask[m]) alph4all[b] = 0xFF;
										else alph4all[b] = 0x00;
									}
									DestNow++;
								}
							}
							else {
								DestNow += Size;
							}
						}
						else {
							CANI_FREE(Dest);//delete [] Dest;
							CANI_FREE(rv->Images);
							CANI_FREE(rv);//delete [] rv;
							CANI_SetLastError(CANI_ERROR_END_OF_FILE);
							return 0;
						}

						settings.options &= (~(CANI_OPTION_16BITS_ALPHA));

						
					}
					else {
						if ((DestNow + BitMapSize) <= DestMax) {
							alph4all = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);//new CANI_BYTE[BitMapSize];
							if (alph4all) {
								CANI_MEMCPY(alph4all, DestNow, BitMapSize);
								for (m = 0; m < BitMapSize; m++) {
									((CANI_BYTE*)alph4all)[m] = CAF_UnMask(((CANI_BYTE*)alph4all)[m], settings.rgb.Alpha);
								}
							}
							DestNow += BitMapSize;
						}
						else {
							CANI_FREE(Dest);//delete [] Dest;
							CANI_FREE(rv->Images);
							CANI_FREE(rv);//delete [] rv;
							CANI_SetLastError(CANI_ERROR_END_OF_FILE);
							return 0;
						}

						settings.options &= (~(CANI_OPTION_16BITS_ALPHA));

					}
				} else alph4all = 0;

				if ( id[4] == '1' ) {

					for ( n = 0; n < ImagesNR; n++ ) {
						CANI_BGRAQUAD *Image = (CANI_BGRAQUAD*)(rv->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC( sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
						
						if ( !Image ) {
							CANI_LONG m;
							//CANI_FREE( Src );//delete [] Src;
							CANI_FREE( Dest );//delete [] Dest;
							if (n > 0) if (n > 0) for ( m = n - 1; m > 0; m-- ) {
								CANI_FREE( rv->Images[m].Image );//delete [] rv->Images[m].Image;
							}
							CANI_FREE( rv->Images );//delete [] rv->Images;
							CANI_FREE( rv );//delete [] rv;
							if ( alph4all ) CANI_FREE( alph4all );
							return 0;
						}

						if (DestNow < DestMax) for ( m = 0; m < BitMapSize; m++ ) {

							CANI_MEMCPY( Image, DestNow, 3 );

							Image->Alpha = 0xFF;

							DestNow+=3;

							if (DestNow > DestMax) break;

							Image++;
						}

						if (DestNow < DestMax) {
							rv->Images[n].Time = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD);
						}

						rv->Images[n].bgColor = 0;

						rv->Images[n].Style = 0;
					}

				} else if ( id[4] == '2' ) {

					for ( n = 0; n < ImagesNR; n++ ) {
						CANI_BGRAQUAD *Image = (CANI_BGRAQUAD*)(rv->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC( sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
						
						if ( !Image ) {
							CANI_LONG m;
							//CANI_FREE( Src );//delete [] Src;
							CANI_FREE( Dest );//delete [] Dest;
							if (n > 0) for ( m = n - 1; m > 0; m-- ) {
								CANI_FREE( rv->Images[m].Image );//delete [] rv->Images[m].Image;
							}
							CANI_FREE( rv->Images );//delete [] rv->Images;
							CANI_FREE( rv );//delete [] rv;
							if ( alph4all ) CANI_FREE( alph4all );
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}


						if (DestNow < DestMax) for ( m = 0; m < BitMapSize; m++ ) {

							CANI_MEMCPY( Image, DestNow, 3 );

							Image->Alpha = 0xFF;

							DestNow+=3;

							if (DestNow > DestMax) break;

							Image++;
						}

						if (DestNow < DestMax) {
							rv->Images[n].Time = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD);
						}


						if (DestNow < DestMax) {
							rv->Images[n].bgColor = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD) ;
						}


						if (DestNow < DestMax) {
							rv->Images[n].Style = *((CANI_BYTE*)DestNow);
							DestNow += sizeof(CANI_BYTE) ;
						}
					}

				} else if ( id[4] == '3' || id[4] == '4' ) {

					if (settings.format == CAF_FORMAT_PALETTE || settings.format == CAF_FORMAT_PALETTE_ND) {

						CANI_DWORD ColorSize = 1;
						CANI_CHAR Alpha16;

						CANI_BYTE* AlphaBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						CANI_WORD* AlphaBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
						CANI_BGRAQUAD* Image8 = (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
						CANI_BGRA16QUAD* Image16 = (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
						CANI_BYTE* PaletteNow = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						CANI_BYTE* PaletteBefore = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						CANI_PALETTE_2 palette[256];// 4096
						CANI_DWORD palette_nr = 0;

						if (AlphaBefore8 == 0 || AlphaBefore16 == 0 || Image8 == 0 || Image16 == 0 || PaletteNow == 0 || PaletteBefore == 0 ) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							if (PaletteNow) CANI_FREE(PaletteNow);
							if (PaletteBefore) CANI_FREE(PaletteBefore);
							CANI_FREE(Dest);
							CANI_FREE(rv->Images);
							CANI_FREE(rv);
							if (alph4all) CANI_FREE(alph4all);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}

						CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
						CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

						Alpha16 = (settings.palette.Alpha > 8 ? TRUE : FALSE);

						if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

							if (Dest) CANI_FREE(Dest);

							Dest = CANI_ReadAndDeCompress(File, &DestLen);

							if (DestLen) {
								DestNow = Dest;
								DestMax = &Dest[DestLen];
							}
							else {
								DestNow = 0;
								DestMax = 0;
							}

						}

						memset(palette, 0x00, sizeof(palette));

						if (DestNow < DestMax) {
							palette_nr = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD);

							//sprintf(szCaniBuffer, "palette_nr =  %u", palette_nr);

							//MessageBoxA(0, szCaniBuffer, "", 0);

							for (n = 0; n < palette_nr; n++) {
								palette[n].Red = *((CANI_BYTE*)DestNow);
								DestNow += sizeof(CANI_BYTE);
								palette[n].Green = *((CANI_BYTE*)DestNow);
								DestNow += sizeof(CANI_BYTE);
								palette[n].Blue = *((CANI_BYTE*)DestNow);
								DestNow += sizeof(CANI_BYTE);
							}
						}

						CANI_MEMSET(Image8, 0x00, (BitMapSize * sizeof(CANI_BGRAQUAD)));

						for (n = 0; n < ImagesNR; n++) {

							CANI_LONG dest_x, dest_y, dest_max_x, dest_max_y, dest_width, dest_height, x, y, width, height;

							CANI_CHAR quarter_byte;
							CANI_CHAR half_byte;
							CANI_CHAR byte_now;
							CANI_CHAR bytes[4];
							CANI_WORD pal;
							CANI_BGRAQUAD* Image;

							if (Progress) {

								CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

								if (ProgressNow2 != ProgressNow) {
									ProgressNow = ProgressNow2;
									Progress(ProgressNow);
								}

							}

							CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}

							if (id[4] >= '4') {

								dest_x = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_y = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_width = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_height = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
							}
							else {

								dest_x = 0;
								dest_y = 0;
								dest_width = rv->Width;
								dest_height = rv->Height;
							}

							width = rv->Width;
							height = rv->Height;

							dest_max_x = dest_x + dest_width;
							dest_max_y = dest_y + dest_height;

							quarter_byte =
								(settings.format == CAF_FORMAT_PALETTE_ND && settings.palette.Palette <= 2 && palette_nr <= 4) ? TRUE : FALSE;

							half_byte =
								(settings.format == CAF_FORMAT_PALETTE_ND && settings.palette.Palette <= 4 && palette_nr <= 16) ? TRUE : FALSE;

							byte_now = 0;

							CANI_MEMSET(bytes, 0x00, sizeof(bytes));

							if (DestNow < DestMax) {

								CANI_BYTE* PaletteBeforeNow = PaletteBefore;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);//new CANI_WORD[ BitMapSize ];
								CANI_BYTE* PaletteNowNow = PaletteNow;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);

								for (y = 0; y < height; y++) {

									if (y >= dest_y && y < dest_max_y) {

										for (x = 0; x < width; x++) {

											if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

												CANI_BYTE IndexNow;

												if (quarter_byte) {
													if (byte_now == 0) {

														IndexNow = *((CANI_BYTE*)DestNow);

														DestNow += sizeof(CANI_BYTE);

														if (DestNow > DestMax) break;

														bytes[0] = (IndexNow >> 6) & 0x03;
														bytes[1] = (IndexNow >> 4) & 0x03;
														bytes[2] = (IndexNow >> 2) & 0x03;
														bytes[3] = (IndexNow) & 0x03;

														IndexNow = bytes[0];
														byte_now = 1;
													}
													else if (byte_now == 1) {
														IndexNow = bytes[1];
														byte_now = 2;
													}
													else if (byte_now == 2) {
														IndexNow = bytes[2];
														byte_now = 3;
													}
													else {
														IndexNow = bytes[3];
														byte_now = 0;
													}
												}
												else if (half_byte) {
													if (byte_now == 0) {

														IndexNow = *((CANI_BYTE*)DestNow);

														DestNow += sizeof(CANI_BYTE);

														if (DestNow > DestMax) break;

														bytes[0] = (IndexNow >> 4) & 0x0F;
														bytes[1] = (IndexNow) & 0x0F;

														IndexNow = bytes[0];
														byte_now = 1;
													}
													else {
														IndexNow = bytes[1];
														byte_now = 0;
													}
												}
												else {
													if (settings.format == CAF_FORMAT_PALETTE_ND) IndexNow = *((CANI_BYTE*)DestNow);
													else IndexNow = *((CANI_BYTE*)DestNow) + *PaletteBeforeNow;

													DestNow += sizeof(CANI_BYTE);

													if (DestNow > DestMax) break;
												}

												*PaletteNowNow = IndexNow;

											}
											else {

												*PaletteNowNow = *PaletteBeforeNow;

											}

											*PaletteBeforeNow = *PaletteNowNow;

											PaletteBeforeNow++;
											PaletteNowNow++;
										}
									}
									else {

										for (x = 0; x < width; x++) {

											*PaletteNowNow = *PaletteBeforeNow;

											*PaletteBeforeNow = *PaletteNowNow;

											PaletteBeforeNow++;
											PaletteNowNow++;
										}
									}


									if (DestNow > DestMax) break;
								}
							}

							if (DestNow > DestMax) continue;

							rv->Images[n].Time = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD);
							if (DestNow > DestMax) continue;

							pal = *((CANI_WORD*)DestNow);
							DestNow += sizeof(CANI_WORD);
							if (DestNow > DestMax) continue;

							//sprintf(szCaniBuffer, "%u %u", pal, palette_nr);
							//MessageBoxA(0, szCaniBuffer, "", 0);

							if (pal < palette_nr) {

								rv->Images[n].bgColorRGB.Red = palette[pal].Red;
								rv->Images[n].bgColorRGB.Green = palette[pal].Green;
								rv->Images[n].bgColorRGB.Blue = palette[pal].Blue;
							}

							CANI_RGBA_2_RGBA16((CANI_RGBA16QUAD*)&rv->Images[n].bgColorRGB16, (CANI_RGBAQUAD*)&rv->Images[n].bgColorRGB, 1);

							if (id[4] >= '4') {
								rv->Images[n].Style = *((CANI_DWORD*)DestNow);
								DestNow += sizeof(CANI_DWORD);
							}
							else {
								rv->Images[n].Style = *((CANI_BYTE*)DestNow);
								DestNow += sizeof(CANI_BYTE);
							}
							if (DestNow > DestMax) continue;

							//CANI_GRAY_2_BGRA(Image8, ImageNow8, BitMapSize);

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];

									DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);

									CANI_FREE(Dest);
									Dest = 0;

									if (DestNow > DestMax) continue;
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}
							else {

								DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
									Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
								if (DestNow > DestMax) continue;

							}

							Image = (CANI_BGRAQUAD*)(rv->Images[n].Image) = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);//new CANI_BGRAQUAD[BitMapSize];
							if (Image) {
								//CANI_MEMCPY(Image, Image8, sizeof(CANI_BGRAQUAD) * BitMapSize);

								for (m = 0; m < BitMapSize; m++) {
									Image[m].Red = palette[PaletteNow[m]].Red;
									Image[m].Green = palette[PaletteNow[m]].Green;
									Image[m].Blue = palette[PaletteNow[m]].Blue;
									if (Alpha16) {
										Image[m].Alpha = (CANI_BYTE)(Image16[m].Alpha >> 8 );
									}
									else {
										Image[m].Alpha = Image8[m].Alpha;

									}
								}

								rv->Images[n].Style &= (~(CANI_STYLE_16BITS_IMAGE));
							}
						}

						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);
						if (PaletteNow) CANI_FREE(PaletteNow);
						if (PaletteBefore) CANI_FREE(PaletteBefore);

					} else if (settings.format == CAF_FORMAT_GRAY || settings.format == CAF_FORMAT_GRAY_ND) {

						CANI_DWORD ColorSize = 1;
						CANI_CHAR Gray16;
						CANI_CHAR Colors16;
						CANI_CHAR Alpha16;
						//CANI_DWORD BackgroundColorSize;
						//CANI_DWORD AlphaSize;

						CANI_BYTE* AlphaBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						CANI_WORD *AlphaBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
						CANI_BGRAQUAD* Image8 = (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
						CANI_BGRA16QUAD* Image16 = (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

						CANI_LONG dest_x, dest_y, dest_max_x, dest_max_y, dest_width, dest_height, x, y, width, height;

						CANI_WORD BgColor;
						CANI_WORD Which16;

						if ( AlphaBefore8 == 0 || AlphaBefore16 == 0 || Image8 == 0 || Image16 == 0 ) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							CANI_FREE(Dest);
							CANI_FREE(rv->Images);
							CANI_FREE(rv);
							if (alph4all) CANI_FREE(alph4all);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}

						CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
						CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

						if (settings.gray.Gray > 8) {
							Gray16 = TRUE;
							ColorSize++;
						}
						else Gray16 = FALSE;

						Colors16 = (ColorSize > 1 ? TRUE : FALSE);
						Alpha16 = (settings.gray.Alpha > 8 ? TRUE : FALSE);

						if (Colors16) {
							CANI_WORD* ImageBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);//new CANI_WORD[ BitMapSize ];
							CANI_WORD* ImageNow16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);

							if ( ImageNow16 == 0 || ImageBefore16 == 0 ) {

								if (ImageNow16) CANI_FREE(ImageNow16);
								if (ImageBefore16) CANI_FREE(ImageBefore16);
								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);
								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								CANI_FREE(Dest);
								CANI_FREE(rv->Images);
								CANI_FREE(rv);
								if (alph4all) CANI_FREE(alph4all);
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return 0;
							}

							CANI_MEMSET(ImageBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));
							CANI_MEMSET(ImageNow16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

							for (n = 0; n < ImagesNR; n++) {

								if (Progress) {

									CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

									if (ProgressNow2 != ProgressNow) {
										ProgressNow = ProgressNow2;
										Progress(ProgressNow);
									}

								}

								CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

								if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

									if (Dest) CANI_FREE(Dest);

									Dest = CANI_ReadAndDeCompress(File, &DestLen);

									if (DestLen) {
										DestNow = Dest;
										DestMax = &Dest[DestLen];
									}
									else {
										DestNow = 0;
										DestMax = 0;
									}

								}

								if (id[4] >= '4') {

									dest_x = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
									dest_y = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
									dest_width = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
									dest_height = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
								}
								else {

									dest_x = 0;
									dest_y = 0;
									dest_width = rv->Width;
									dest_height = rv->Height;
								}

								//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_width, dest_height);

							    //MessageBoxA(0, szCaniBuffer, szCaniBuffer, 0);

								width = rv->Width;
								height = rv->Height;

								dest_max_x = dest_x + dest_width;
								dest_max_y = dest_y + dest_height;

								if (DestNow < DestMax) {

									CANI_WORD* ImageBefore16Now = ImageBefore16;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);//new CANI_WORD[ BitMapSize ];
									CANI_WORD* ImageNow16Now = ImageNow16;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);

									for (y = 0; y < height; y++) {

										if (y >= dest_y && y < dest_max_y) {

											for (x = 0; x < width; x++) {

												if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

													CANI_WORD ColorNow;

													if (settings.format == CAF_FORMAT_GRAY_ND) ColorNow = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.gray.Gray);
													else ColorNow = CAF_UnMask16(*((CANI_WORD*)DestNow) + *ImageBefore16Now, settings.gray.Gray);

													DestNow += sizeof(CANI_WORD);

													if (DestNow > DestMax) break;

													*ImageNow16Now = ColorNow;

												}
												else {

													*ImageNow16Now = CAF_UnMask16( *ImageBefore16Now, settings.gray.Gray);

												}

												*ImageBefore16Now = CAF_Mask16(*ImageNow16Now, settings.gray.Gray);

												ImageBefore16Now++;
												ImageNow16Now++;
											}
										}
										else {

											for (x = 0; x < width; x++) {

												*ImageNow16Now = CAF_UnMask16(*ImageBefore16Now, settings.gray.Gray);

												*ImageBefore16Now = CAF_Mask16(*ImageNow16Now, settings.gray.Gray);

												ImageBefore16Now++;
												ImageNow16Now++;
											}
										}


										if (DestNow > DestMax) break;
									}
								}

								if (DestNow > DestMax) continue;

								rv->Images[n].Time = *((CANI_DWORD*)DestNow);
								DestNow += sizeof(CANI_DWORD);
								if (DestNow > DestMax) continue;

								BgColor = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.gray.Gray);
								CANI_GRAY16_2_RGBA16(&rv->Images[n].bgColorRGB16, &BgColor, 1);
								CANI_RGBA16_2_RGBA(&rv->Images[n].bgColorRGB, &rv->Images[n].bgColorRGB16, 1);
								DestNow += sizeof(CANI_WORD);

								if (DestNow > DestMax) continue;

								if (id[4] >= '4') {
									rv->Images[n].Style = *((CANI_DWORD*)DestNow);
									DestNow += sizeof(CANI_DWORD);
								}
								else {
									rv->Images[n].Style = *((CANI_BYTE*)DestNow);
									DestNow += sizeof(CANI_BYTE);
								}
								if (DestNow > DestMax) continue;

								CANI_GRAY16_2_BGRA16(Image16, ImageNow16, BitMapSize );

								//CANI_BGRA16_2_BGRA(Image8, Image16, BitMapSize);

								if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

									if (Dest) CANI_FREE(Dest);

									Dest = CANI_ReadAndDeCompress(File, &DestLen);

									if (DestLen) {
										DestNow = Dest;
										DestMax = &Dest[DestLen];

										DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
											Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4] );

										CANI_FREE(Dest);
										Dest = 0;

										if (DestNow > DestMax) continue;
									}
									else {
										DestNow = 0;
										DestMax = 0;
									}

								}
								else {

									DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
									if (DestNow > DestMax) continue;

								}

								Which16 = 0;
								if (Gray16) Which16 |= CANI_IMAGE_GRAY16;
								if (Alpha16) Which16 |= CANI_IMAGE_ALPHA16;

								if (!CANI_DeConvertImageGray(rv, n, Colors16 || Alpha16, Image8, Image16, Which16 )) {

									if (ImageNow16) CANI_FREE(ImageNow16);
									if (ImageBefore16) CANI_FREE(ImageBefore16);
									if (AlphaBefore8) CANI_FREE(AlphaBefore8);
									if (AlphaBefore16) CANI_FREE(AlphaBefore16);
									if (Image8) CANI_FREE(Image8);
									if (Image16) CANI_FREE(Image16);
									CANI_FREE(Dest);
									//if (n > 0) for (m = n - 1; m > 0; m--) {
									for (m = n; m > 0; m--) {
										CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
									}
									CANI_FREE(rv->Images);
									CANI_FREE(rv);
									if (alph4all) CANI_FREE(alph4all);
									CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
									return 0;
								}
							}

							if (ImageNow16) CANI_FREE(ImageNow16);
							if (ImageBefore16) CANI_FREE(ImageBefore16);
						}
						else {

							CANI_BYTE* ImageBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);//new CANI_WORD[ BitMapSize ];
							CANI_BYTE* ImageNow8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);

							if (ImageNow8 == 0 || ImageBefore8 == 0) {

								if (ImageNow8) CANI_FREE(ImageNow8);
								if (ImageBefore8) CANI_FREE(ImageBefore8);
								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);
								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								CANI_FREE(Dest);
								CANI_FREE(rv->Images);
								CANI_FREE(rv);
								if (alph4all) CANI_FREE(alph4all);
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return 0;
							}

							CANI_MEMSET(ImageBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
							CANI_MEMSET(ImageNow8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));

							for (n = 0; n < ImagesNR; n++) {

								if (Progress) {

									CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

									if (ProgressNow2 != ProgressNow) {
										ProgressNow = ProgressNow2;
										Progress(ProgressNow);
									}

								}

								CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

								if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

									if (Dest) CANI_FREE(Dest);

									Dest = CANI_ReadAndDeCompress(File, &DestLen);

									if (DestLen) {
										DestNow = Dest;
										DestMax = &Dest[DestLen];
									}
									else {
										DestNow = 0;
										DestMax = 0;
									}

								}

								if (id[4] >= '4') {

									dest_x = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
									dest_y = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
									dest_width = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
									dest_height = *((CANI_LONG*)DestNow);
									DestNow += sizeof(CANI_LONG);
								}
								else {

									dest_x = 0;
									dest_y = 0;
									dest_width = rv->Width;
									dest_height = rv->Height;
								}

								width = rv->Width;
								height = rv->Height;

								dest_max_x = dest_x + dest_width;
								dest_max_y = dest_y + dest_height;

								if (DestNow < DestMax) {

									CANI_BYTE* ImageBefore8Now = ImageBefore8;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);//new CANI_WORD[ BitMapSize ];
									CANI_BYTE* ImageNow8Now = ImageNow8;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);

									for (y = 0; y < height; y++) {

										if (y >= dest_y && y < dest_max_y) {

											for (x = 0; x < width; x++) {

												if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

													CANI_BYTE ColorNow;

													if (settings.format == CAF_FORMAT_GRAY_ND) ColorNow = CAF_UnMask(*((CANI_BYTE*)DestNow), settings.gray.Gray);
													else ColorNow = CAF_UnMask(*((CANI_BYTE*)DestNow) + *ImageBefore8Now, settings.gray.Gray);

													DestNow += sizeof(CANI_BYTE);

													if (DestNow > DestMax) break;

													*ImageNow8Now = ColorNow;

												}
												else {

													*ImageNow8Now = CAF_UnMask(*ImageBefore8Now, settings.gray.Gray);

												}

												*ImageBefore8Now = CAF_Mask(*ImageNow8Now, settings.gray.Gray);

												ImageBefore8Now++;
												ImageNow8Now++;
											}
										}
										else {

											for (x = 0; x < width; x++) {

												*ImageNow8Now = CAF_UnMask(*ImageBefore8Now, settings.gray.Gray);

												*ImageBefore8Now = CAF_Mask(*ImageNow8Now, settings.gray.Gray);

												ImageBefore8Now++;
												ImageNow8Now++;
											}
										}


										if (DestNow > DestMax) break;
									}
								}

								if (DestNow > DestMax) continue;

								rv->Images[n].Time = *((CANI_DWORD*)DestNow);
								DestNow += sizeof(CANI_DWORD);
								if (DestNow > DestMax) continue;

								if (id[4] == '3') {
									rv->Images[n].bgColorRGB.Red = CAF_UnMask(*((CANI_BYTE*)DestNow), settings.rgb.Red);
									DestNow++;
									rv->Images[n].bgColorRGB.Green = CAF_UnMask(*((CANI_BYTE*)DestNow), settings.rgb.Green);
									DestNow++;
									rv->Images[n].bgColorRGB.Blue = CAF_UnMask(*((CANI_BYTE*)DestNow), settings.rgb.Blue);
									DestNow++;

									CANI_RGBA_2_RGBA16(&rv->Images[n].bgColorRGB16, &rv->Images[n].bgColorRGB, 1);
								}
								else {
									CANI_BYTE BgColor;
									BgColor = CAF_UnMask(*((CANI_BYTE*)DestNow), settings.gray.Gray);
									CANI_GRAY_2_RGBA(&rv->Images[n].bgColorRGB, &BgColor, 1);
									CANI_RGBA_2_RGBA16(&rv->Images[n].bgColorRGB16, &rv->Images[n].bgColorRGB, 1);
									DestNow += sizeof(CANI_BYTE);
								}

								//DestNow = CANI_ReadBackgroundColor(Colors16/* || Alpha16*/, DestNow, DestMax, &rv->Images[n]);
								if (DestNow > DestMax) continue;

								if (id[4] >= '4') {
									rv->Images[n].Style = *((CANI_DWORD*)DestNow);
									DestNow += sizeof(CANI_DWORD);
								}
								else {
									rv->Images[n].Style = *((CANI_BYTE*)DestNow);
									DestNow += sizeof(CANI_BYTE);
								}
								if (DestNow > DestMax) continue;

								CANI_GRAY_2_BGRA(Image8, ImageNow8, BitMapSize);

								//CANI_BGRA16_2_BGRA(Image8, Image16, BitMapSize);

								if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

									if (Dest) CANI_FREE(Dest);

									Dest = CANI_ReadAndDeCompress(File, &DestLen);

									if (DestLen) {
										DestNow = Dest;
										DestMax = &Dest[DestLen];

										DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
											Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);

										CANI_FREE(Dest);
										Dest = 0;

										if (DestNow > DestMax) continue;
									}
									else {
										DestNow = 0;
										DestMax = 0;
									}

								}
								else {

									DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
									if (DestNow > DestMax) continue;

								}

								Which16 = 0;
								if (Gray16) Which16 |= CANI_IMAGE_GRAY16;
								if (Alpha16) Which16 |= CANI_IMAGE_ALPHA16;

								if (!CANI_DeConvertImageGray(rv, n, Colors16 || Alpha16, Image8, Image16, Which16)) {

									if (ImageNow8) CANI_FREE(ImageNow8);
									if (ImageBefore8) CANI_FREE(ImageBefore8);
									if (AlphaBefore8) CANI_FREE(AlphaBefore8);
									if (AlphaBefore16) CANI_FREE(AlphaBefore16);
									if (Image8) CANI_FREE(Image8);
									if (Image16) CANI_FREE(Image16);
									CANI_FREE(Dest);
									//if (n > 0) for (m = n - 1; m > 0; m--) {
									for (m = n; m > 0; m--) {
										CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
									}
									CANI_FREE(rv->Images);
									CANI_FREE(rv);
									if (alph4all) CANI_FREE(alph4all);
									CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
									return 0;
								}
							}

							if (ImageNow8) CANI_FREE(ImageNow8);
							if (ImageBefore8) CANI_FREE(ImageBefore8);
						}

						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);

					} else if ( settings.format == CAF_FORMAT_RGB16 || settings.format == CAF_FORMAT_RGB16_ND ) {

						CANI_DWORD ColorSize = 1;
						CANI_CHAR Gray16;
						CANI_CHAR Colors16;
						CANI_CHAR Alpha16;
						//CANI_DWORD BackgroundColorSize;
						//CANI_DWORD AlphaSize;

						CANI_BYTE* AlphaBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						CANI_WORD* AlphaBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
						CANI_BGRAQUAD* Image8 = (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
						CANI_BGRA16QUAD* Image16 = (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
						CANI_WORD* ImageBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);//new CANI_WORD[ BitMapSize ];
						CANI_WORD* ImageNow16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
							
						CANI_LONG dest_x, dest_y, dest_max_x, dest_max_y, dest_width, dest_height, x, y, width, height;
						CANI_WORD Which16;

						if (AlphaBefore8 == 0 || AlphaBefore16 == 0 || Image8 == 0 || Image16 == 0 || ImageBefore16 == 0 || ImageNow16 == 0 ) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							if (ImageBefore16) CANI_FREE(ImageBefore16);
							if (ImageNow16) CANI_FREE(ImageNow16);
							CANI_FREE(Dest);
							CANI_FREE(rv->Images);
							CANI_FREE(rv);
							if (alph4all) CANI_FREE(alph4all);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}

						CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
						CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));
						CANI_MEMSET(ImageBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));
						CANI_MEMSET(ImageNow16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

						if (settings.gray.Gray > 8) {
							Gray16 = TRUE;
							ColorSize++;
						}
						else Gray16 = FALSE;

						Colors16 = (ColorSize > 1 ? TRUE : FALSE);
						Alpha16 = (settings.gray.Alpha > 8 ? TRUE : FALSE);

						for (n = 0; n < ImagesNR; n++) {

							if (Progress) {

								CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

								if (ProgressNow2 != ProgressNow) {
									ProgressNow = ProgressNow2;
									Progress(ProgressNow);
								}

							}

							CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}

							if (id[4] >= '4') {

								dest_x = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_y = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_width = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_height = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
							}
							else {

								dest_x = 0;
								dest_y = 0;
								dest_width = rv->Width;
								dest_height = rv->Height;
							}

							width = rv->Width;
							height = rv->Height;

							dest_max_x = dest_x + dest_width;
							dest_max_y = dest_y + dest_height;

							if (DestNow < DestMax) {

								CANI_WORD* ImageBefore16Now = ImageBefore16;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);//new CANI_WORD[ BitMapSize ];
								CANI_WORD* ImageNow16Now = ImageNow16;// (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);

								for (y = 0; y < height; y++) {

									if (y >= dest_y && y < dest_max_y) {

										for (x = 0; x < width; x++) {

											if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

												CANI_WORD ColorNow;

												if (settings.format == CAF_FORMAT_RGB16_ND) ColorNow = *((CANI_WORD*)DestNow);// CAF_UnMask16(*((CANI_WORD*)DestNow), settings.gray.Gray);
												else ColorNow = *((CANI_WORD*)DestNow) + *ImageBefore16Now;// CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBefore16[m], settings.gray.Gray);

												DestNow += sizeof(CANI_WORD);

												if (DestNow > DestMax) break;

												*ImageNow16Now = ColorNow;

											}
											else {

												*ImageNow16Now = *ImageBefore16Now;

											}

											*ImageBefore16Now = *ImageNow16Now;

											ImageBefore16Now++;
											ImageNow16Now++;
										}
									}
									else {

										for (x = 0; x < width; x++) {

											*ImageNow16Now = *ImageBefore16Now;
											*ImageBefore16Now = *ImageNow16Now;
											ImageBefore16Now++;
											ImageNow16Now++;
										}
									}


									if (DestNow > DestMax) break;
								}
							}

							if (DestNow > DestMax) continue;

							rv->Images[n].Time = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD);
							if (DestNow > DestMax) continue;

							if (id[4] == '3') {
								rv->Images[n].bgColorRGB.Red = *((CANI_BYTE*)DestNow);// CAF_UnMask(*((CANI_BYTE*)DestNow), settings.rgb.Red);
								DestNow++;
								rv->Images[n].bgColorRGB.Green = *((CANI_BYTE*)DestNow);// CAF_UnMask(*((CANI_BYTE*)DestNow), settings.rgb.Green);
								DestNow++;
								rv->Images[n].bgColorRGB.Blue = *((CANI_BYTE*)DestNow);// CAF_UnMask(*((CANI_BYTE*)DestNow), settings.rgb.Blue);
								DestNow++;

								CANI_RGBA_2_RGBA16(&rv->Images[n].bgColorRGB16, &rv->Images[n].bgColorRGB, 1);
							}
							else {
								CANI_WORD BgColor;
								BgColor = *((CANI_WORD*)DestNow);// CAF_UnMask16(*((CANI_WORD*)DestNow), settings.gray.Gray);
								CANI_RGB16_2_RGBA16(&rv->Images[n].bgColorRGB16, &BgColor, 1);
								CANI_RGBA16_2_RGBA(&rv->Images[n].bgColorRGB, &rv->Images[n].bgColorRGB16, 1);
								DestNow += sizeof(CANI_WORD);
							}

							//DestNow = CANI_ReadBackgroundColor(Colors16/* || Alpha16*/, DestNow, DestMax, &rv->Images[n]);
							if (DestNow > DestMax) continue;

							if (id[4] >= '4') {
								rv->Images[n].Style = *((CANI_DWORD*)DestNow);
								DestNow += sizeof(CANI_DWORD);
							}
							else {
								rv->Images[n].Style = *((CANI_BYTE*)DestNow);
								DestNow += sizeof(CANI_BYTE);
							}
							if (DestNow > DestMax) continue;

							//CANI_GRAY16_2_BGRA16(Image16, ImageNow16, BitMapSize);

							CANI_RGB16_2_BGRA16(Image16, ImageNow16, BitMapSize);

							//CANI_GRAY16_2_BGRA16(Image16, ImageNow16, BitMapSize);

							//CANI_BGRA16_2_BGRA(Image8, Image16, BitMapSize);

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];

									DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);

									CANI_FREE(Dest);
									Dest = 0;

									if (DestNow > DestMax) continue;
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}
							else {

								DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
									Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
								if (DestNow > DestMax) continue;

							}

							Which16 = CANI_IMAGE_RGB16;
							//if (Gray16) Which16 |= CANI_IMAGE_GRAY16;
							if (Alpha16) Which16 |= CANI_IMAGE_ALPHA16;

							//CANI_RGB16_2_BGRA16(Image16, ImageNow16, BitMapSize);

							// ImageNow16

							//CANI_BGRA16_2_BGRA(Image8, Image16, BitMapSize);

							if (!CANI_DeConvertImageRGB16(rv, n, Alpha16, Image8, Image16, Which16)) {

								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);
								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								if (ImageBefore16) CANI_FREE(ImageBefore16);
								if (ImageNow16) CANI_FREE(ImageNow16);
								CANI_FREE(Dest);
								//if (n > 0) for (m = n - 1; m > 0; m--) {
								for (m = n; m > 0; m--) {
									CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
								}
								CANI_FREE(rv->Images);
								CANI_FREE(rv);
								if (alph4all) CANI_FREE(alph4all);
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return 0;
							}
						}

						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);
						if (ImageBefore16) CANI_FREE(ImageBefore16);
						if (ImageNow16) CANI_FREE(ImageNow16);

					} else if ( settings.format == CAF_FORMAT_RGB || settings.format == CAF_FORMAT_RGB_ND ) {

						CANI_DWORD ColorSize = 3;
						CANI_CHAR Red16, Green16, Blue16;
						CANI_CHAR Colors16;
						CANI_BGRAQUAD* ImageBefore8 = (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
						CANI_BGRA16QUAD* ImageBefore16 = (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
						CANI_BYTE* AlphaBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						CANI_WORD* AlphaBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
						CANI_BGRAQUAD* Image8 = (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
						CANI_BGRA16QUAD* Image16 = (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));
						CANI_WORD Which16;
						CANI_LONG dest_x, dest_y, dest_max_x, dest_max_y, dest_width, dest_height, x, y, width, height;

						if (ImageBefore8 == 0 || ImageBefore16 == 0 || AlphaBefore8 == 0 || AlphaBefore16 == 0
							|| Image8 == 0 || Image16 == 0 ) {

							if (ImageBefore8) CANI_FREE(ImageBefore8);
							if (ImageBefore16) CANI_FREE(ImageBefore16);
							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							CANI_FREE(Dest);
							CANI_FREE(rv->Images);
							CANI_FREE(rv);
							if (alph4all) CANI_FREE(alph4all);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}

						if (settings.rgb.Red > 8) {
							Red16 = TRUE;
							ColorSize++;
						}
						else Red16 = FALSE;

						if (settings.rgb.Green > 8) {
							Green16 = TRUE;
							ColorSize++;
						}
						else Green16 = FALSE;

						if (settings.rgb.Blue > 8) {
							Blue16 = TRUE;
							ColorSize++;
						}
						else Blue16 = FALSE;

						Colors16 = (ColorSize > 3 ? TRUE : FALSE);

						CANI_MEMSET(ImageBefore8, 0x00, (BitMapSize * sizeof(CANI_BGRAQUAD)));
						CANI_MEMSET(ImageBefore16, 0x00, (BitMapSize * sizeof(CANI_BGRA16QUAD)));
						CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
						CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

						for (n = 0; n < ImagesNR; n++) {

							if (Progress) {

								CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

								if (ProgressNow2 != ProgressNow) {
									ProgressNow = ProgressNow2;
									Progress(ProgressNow);
								}

							}

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}

							CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

							CANI_MEMSET(Image8, 0x00, sizeof(CANI_BGRAQUAD)* BitMapSize);
							CANI_MEMSET(Image16, 0x00, sizeof(CANI_BGRA16QUAD)* BitMapSize);

							if (id[4] >= '4') {

								dest_x = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_y = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_width = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_height = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
							}
							else {

								dest_x = 0;
								dest_y = 0;
								dest_width = rv->Width;
								dest_height = rv->Height;
							}

							width = rv->Width;
							height = rv->Height;

							dest_max_x = dest_x + dest_width;
							dest_max_y = dest_y + dest_height;

							if (DestNow < DestMax) {

								CANI_BGRAQUAD* ImageBefore8Now = ImageBefore8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
								CANI_BGRA16QUAD* ImageBefore16Now = ImageBefore16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
								CANI_BGRAQUAD* Image8Now = Image8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
								CANI_BGRA16QUAD* Image16Now = Image16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

								for (y = 0; y < height; y++) {

									if (y >= dest_y && y < dest_max_y) {

										for (x = 0; x < width; x++) {

											if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

												if (settings.format == CAF_FORMAT_RGB_ND) {

													if (Red16) {
														Image16Now->Red = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.rgb.Red);
														DestNow += 2;
													}
													else {
														Image8Now->Red = CAF_UnMask(*DestNow, settings.rgb.Red);
														DestNow++;
													}
													if (DestNow > DestMax) break;

													if (Green16) {
														Image16Now->Green = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.rgb.Green);
														DestNow += 2;
													}
													else {
														Image8Now->Green = CAF_UnMask(*DestNow, settings.rgb.Green);
														DestNow++;
													}
													if (DestNow > DestMax) break;

													if (Blue16) {
														Image16Now->Blue = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.rgb.Blue);
														DestNow += 2;
													}
													else {
														Image8Now->Blue = CAF_UnMask(*DestNow, settings.rgb.Blue);
														DestNow++;
													}
													if (DestNow > DestMax) break;

												}
												else {

													if (Red16) {
														Image16Now->Red = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBefore16Now->Red, settings.rgb.Red);
														DestNow += 2;
													}
													else {
														Image8Now->Red = CAF_UnMask(*DestNow + ImageBefore8Now->Red, settings.rgb.Red);
														DestNow++;
													}
													if (DestNow > DestMax) break;

													if (Green16) {
														Image16Now->Green = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBefore16Now->Green, settings.rgb.Green);
														DestNow += 2;
													}
													else {
														Image8Now->Green = CAF_UnMask(*DestNow + ImageBefore8Now->Green, settings.rgb.Green);
														DestNow++;
													}
													if (DestNow > DestMax) break;

													if (Green16) {
														Image16Now->Blue = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBefore16Now->Blue, settings.rgb.Blue);
														DestNow += 2;
													}
													else {
														Image8Now->Blue = CAF_UnMask(*DestNow + ImageBefore8Now->Blue, settings.rgb.Blue);
														DestNow++;
													}
													if (DestNow > DestMax) break;

												}

											}
											else {

												if (Red16) {
													Image16Now->Red = CAF_UnMask16( ImageBefore16Now->Red, settings.rgb.Red);
												}
												else {
													Image8Now->Red = CAF_UnMask(ImageBefore8Now->Red, settings.rgb.Red);
												}

												if (Green16) {
													Image16Now->Green = CAF_UnMask16(ImageBefore16Now->Green, settings.rgb.Green);
												}
												else {
													Image8Now->Green = CAF_UnMask(ImageBefore8Now->Green, settings.rgb.Green);
												}

												if (Blue16) {
													Image16Now->Blue = CAF_UnMask16(ImageBefore16Now->Blue, settings.rgb.Blue);
												}
												else {
													Image8Now->Blue = CAF_UnMask(ImageBefore8Now->Blue, settings.rgb.Blue);
												}

											}

											if (Red16) {
												ImageBefore16Now->Red = CAF_Mask16(Image16Now->Red, settings.rgb.Red);
											}
											else {
												ImageBefore8Now->Red = CAF_Mask(Image8Now->Red, settings.rgb.Red);
											}

											if (Green16) {
												ImageBefore16Now->Green = CAF_Mask16(Image16Now->Green, settings.rgb.Green);
											}
											else {
												ImageBefore8Now->Green = CAF_Mask(Image8Now->Green, settings.rgb.Green);
											}

											if (Blue16) {
												ImageBefore16Now->Blue = CAF_Mask16(Image16Now->Blue, settings.rgb.Blue);
											}
											else {
												ImageBefore8Now->Blue = CAF_Mask(Image8Now->Blue, settings.rgb.Blue);
											}

											ImageBefore8Now++;
											ImageBefore16Now++;
											Image8Now++;
											Image16Now++;
										}
									}
									else {

										for (x = 0; x < width; x++) {

											if (Red16) {
												Image16Now->Red = CAF_UnMask16(ImageBefore16Now->Red, settings.rgb.Red);
											}
											else {
												Image8Now->Red = CAF_UnMask(ImageBefore8Now->Red, settings.rgb.Red);
											}

											if (Green16) {
												Image16Now->Green = CAF_UnMask16(ImageBefore16Now->Green, settings.rgb.Green);
											}
											else {
												Image8Now->Green = CAF_UnMask(ImageBefore8Now->Green, settings.rgb.Green);
											}

											if (Blue16) {
												Image16Now->Blue = CAF_UnMask16(ImageBefore16Now->Blue, settings.rgb.Blue);
											}
											else {
												Image8Now->Blue = CAF_UnMask(ImageBefore8Now->Blue, settings.rgb.Blue);
											}

											if (Red16) {
												ImageBefore16Now->Red = CAF_Mask16(Image16Now->Red, settings.rgb.Red);
											}
											else {
												ImageBefore8Now->Red = CAF_Mask(Image8Now->Red, settings.rgb.Red);
											}

											if (Green16) {
												ImageBefore16Now->Green = CAF_Mask16(Image16Now->Green, settings.rgb.Green);
											}
											else {
												ImageBefore8Now->Green = CAF_Mask(Image8Now->Green, settings.rgb.Green);
											}

											if (Blue16) {
												ImageBefore16Now->Blue = CAF_Mask16(Image16Now->Blue, settings.rgb.Blue);
											}
											else {
												ImageBefore8Now->Blue = CAF_Mask(Image8Now->Blue, settings.rgb.Blue);
											}

											ImageBefore8Now++;
											ImageBefore16Now++;
											Image8Now++;
											Image16Now++;
										}
									}


									if (DestNow > DestMax) break;
								}
							}

							if (DestNow > DestMax) continue;

							rv->Images[n].Time = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD);
							if (DestNow > DestMax) continue;


							// CANI_WriteBackgroundColor(Red16,Green16,Blue16,SrcNow, &animation->Images[n]);

							DestNow = CANI_ReadBackgroundColor(Red16, Green16, Blue16, DestNow, DestMax, &rv->Images[n], &settings );
							if (DestNow > DestMax) continue;

							if (id[4] >= '4') {
								rv->Images[n].Style = *((CANI_DWORD*)DestNow);
								DestNow += sizeof(CANI_DWORD);
							}
							else {
								rv->Images[n].Style = *((CANI_BYTE*)DestNow);
								DestNow += sizeof(CANI_BYTE);
							}
							if (DestNow > DestMax) continue;

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];

									DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);

									CANI_FREE(Dest);
									Dest = 0;

									if (DestNow > DestMax) continue;
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}
							else {

								DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
									Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
								if (DestNow > DestMax) continue;

							}

							Which16 = 0;
							if (Red16) Which16 |= CANI_IMAGE_RED16;
							if (Green16) Which16 |= CANI_IMAGE_GREEN16;
							if (Blue16) Which16 |= CANI_IMAGE_BLUE16;
							if (Alpha16) Which16 |= CANI_IMAGE_ALPHA16;

							if (!CANI_DeConvertImage(rv, n, Colors16 || Alpha16, Image8, Image16, Which16)) {

								if (ImageBefore8) CANI_FREE(ImageBefore8);
								if (ImageBefore16) CANI_FREE(ImageBefore16);
								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);
								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								CANI_FREE(Dest);
								//if (n > 0) for (m = n - 1; m > 0; m--) {
								for (m = n; m > 0; m--) {
									CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
								}
								CANI_FREE(rv->Images);
								CANI_FREE(rv);
								if (alph4all) CANI_FREE(alph4all);
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return 0;
							}
						}

						if (ImageBefore8) CANI_FREE(ImageBefore8);
						if (ImageBefore16) CANI_FREE(ImageBefore16);
						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);

					} else if (settings.format == CAF_FORMAT_CMYK || settings.format == CAF_FORMAT_CMYK_ND) {

						 CANI_DWORD ColorSize = 4;
						 CANI_CHAR Cyan16, Magenta16, Yellow16, KeyColour16;
						 CANI_CHAR Colors16;

						 CANI_BYTE* AlphaBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						 CANI_WORD* AlphaBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
						 CANI_CMYKAQUAD *ImageCMYK8 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKAQUAD));
						 CANI_CMYKAQUAD *ImageBeforeCMYK8 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKAQUAD));
						 CANI_CMYKA16QUAD * ImageCMYK16 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKA16QUAD));
						 CANI_CMYKA16QUAD * ImageBeforeCMYK16 = CANI_MALLOC(BitMapSize * sizeof(CANI_CMYKA16QUAD));
						 CANI_BGRAQUAD* Image8 = (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
						 CANI_BGRA16QUAD* Image16 = (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

						 CANI_LONG dest_x, dest_y, dest_max_x, dest_max_y, dest_width, dest_height, x, y, width, height;
						 CANI_WORD Which16;

						 if ( AlphaBefore8 == 0 || AlphaBefore16 == 0 || Image8 == 0 || Image16 == 0 
							 || ImageCMYK8 == 0 || ImageCMYK16 == 0 || ImageBeforeCMYK8 == 0 || ImageBeforeCMYK16 == 0 ) {

							 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							 if (Image8) CANI_FREE(Image8);
							 if (Image16) CANI_FREE(Image16);
							 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
							 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
							 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
							 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);
							 CANI_FREE(Dest);
							 CANI_FREE(rv->Images);
							 CANI_FREE(rv);
							 if (alph4all) CANI_FREE(alph4all);
							 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							 return 0;
						 }

						 if (settings.cmyk.C > 8) {
							 Cyan16 = TRUE;
							 ColorSize++;
						 }
						 else Cyan16 = FALSE;

						 if (settings.cmyk.M > 8) {
							 Magenta16 = TRUE;
							 ColorSize++;
						 }
						 else Magenta16 = FALSE;

						 if (settings.cmyk.Y > 8) {
							 Yellow16 = TRUE;
							 ColorSize++;
						 }
						 else Yellow16 = FALSE;

						 if (settings.cmyk.K > 8) {
							 KeyColour16 = TRUE;
							 ColorSize++;
						 }
						 else KeyColour16 = FALSE;

						 Colors16 = (ColorSize > 4 ? TRUE : FALSE);
						 Alpha16 = (settings.cmyk.Alpha > 8 ? TRUE : FALSE);

						 //BackgroundColorSize = ColorSize;// (Colors16 /*|| Alpha16*/) ? (3 * sizeof(CANI_WORD)) : (3 * sizeof(CANI_BYTE));
						// AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

						 CANI_MEMSET(ImageBeforeCMYK8, 0x00, (BitMapSize * sizeof(CANI_CMYKAQUAD)));
						 CANI_MEMSET(ImageBeforeCMYK16, 0x00, (BitMapSize * sizeof(CANI_CMYKA16QUAD)));
						 CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
						 CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

						 for (n = 0; n < ImagesNR; n++) {

							 if (Progress) {

								 CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

								 if (ProgressNow2 != ProgressNow) {
									 ProgressNow = ProgressNow2;
									 Progress(ProgressNow);
								 }

							 }

							 if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								 if (Dest) CANI_FREE(Dest);

								 Dest = CANI_ReadAndDeCompress(File, &DestLen);

								 if (DestLen) {
									 DestNow = Dest;
									 DestMax = &Dest[DestLen];
								 }
								 else {
									 DestNow = 0;
									 DestMax = 0;
								 }

							 }

							 CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

							 //CANI_MEMSET(Image8, 0x00, sizeof(CANI_BGRAQUAD) * BitMapSize);
							 //CANI_MEMSET(Image16, 0x00, sizeof(CANI_BGRA16QUAD) * BitMapSize);

							 //CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

							 CANI_MEMSET(Image8, 0x00, sizeof(CANI_BGRAQUAD)* BitMapSize);
							 CANI_MEMSET(Image16, 0x00, sizeof(CANI_BGRA16QUAD)* BitMapSize);

							 if (id[4] >= '4') {

								 dest_x = *((CANI_LONG*)DestNow);
								 DestNow += sizeof(CANI_LONG);
								 dest_y = *((CANI_LONG*)DestNow);
								 DestNow += sizeof(CANI_LONG);
								 dest_width = *((CANI_LONG*)DestNow);
								 DestNow += sizeof(CANI_LONG);
								 dest_height = *((CANI_LONG*)DestNow);
								 DestNow += sizeof(CANI_LONG);
							 }
							 else {

								 dest_x = 0;
								 dest_y = 0;
								 dest_width = rv->Width;
								 dest_height = rv->Height;
							 }

							 width = rv->Width;
							 height = rv->Height;

							 dest_max_x = dest_x + dest_width;
							 dest_max_y = dest_y + dest_height;

							 if (DestNow < DestMax) {

								 CANI_CMYKAQUAD* ImageBeforeCMYK8Now = ImageBeforeCMYK8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
								 CANI_CMYKA16QUAD* ImageBeforeCMYK16Now = ImageBeforeCMYK16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
								 CANI_CMYKAQUAD* ImageCMYK8Now = ImageCMYK8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
								 CANI_CMYKA16QUAD* ImageCMYK16Now = ImageCMYK16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

								 for (y = 0; y < height; y++) {

									 if (y >= dest_y && y < dest_max_y) {

										 for (x = 0; x < width; x++) {

											 if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

												 if (settings.format == CAF_FORMAT_CMYK_ND) {

													 if (Cyan16) {
														 ImageCMYK16Now->C = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.cmyk.C);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->C = CAF_UnMask(*DestNow, settings.cmyk.C);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

													 if (Magenta16) {
														 ImageCMYK16Now->M = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.cmyk.M);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->M = CAF_UnMask(*DestNow, settings.cmyk.M);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

													 if (Yellow16) {
														 ImageCMYK16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.cmyk.Y);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->Y = CAF_UnMask(*DestNow, settings.cmyk.Y);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

													 if (KeyColour16) {
														 ImageCMYK16Now->K = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.cmyk.K);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->K = CAF_UnMask(*DestNow, settings.cmyk.K);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

												 }
												 else {

													 if (Cyan16) {
														 ImageCMYK16Now->C = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeCMYK16Now->C, settings.cmyk.C);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->C = CAF_UnMask(*DestNow + ImageBeforeCMYK8Now->C, settings.cmyk.C);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

													 if (Magenta16) {
														 ImageCMYK16Now->M = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeCMYK16Now->M, settings.cmyk.M);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->M = CAF_UnMask(*DestNow + ImageBeforeCMYK8Now->M, settings.cmyk.M);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

													 if (Yellow16) {
														 ImageCMYK16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeCMYK16Now->Y, settings.cmyk.Y);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->Y = CAF_UnMask(*DestNow + ImageBeforeCMYK8Now->Y, settings.cmyk.Y);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

													 if (KeyColour16) {
														 ImageCMYK16Now->K = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeCMYK16Now->K, settings.cmyk.K);
														 DestNow += 2;
													 }
													 else {
														 ImageCMYK8Now->K = CAF_UnMask(*DestNow + ImageBeforeCMYK8Now->K, settings.cmyk.K);
														 DestNow++;
													 }
													 if (DestNow > DestMax) break;

												 }

											 }
											 else {

												 if (Cyan16) {
													 ImageCMYK16Now->C = CAF_UnMask16( ImageBeforeCMYK16Now->C, settings.cmyk.C);
												 }
												 else {
													 ImageCMYK8Now->C = CAF_UnMask( ImageBeforeCMYK8Now->C, settings.cmyk.C);
												 }

												 if (Magenta16) {
													 ImageCMYK16Now->M = CAF_UnMask16( ImageBeforeCMYK16Now->M, settings.cmyk.M);
												 }
												 else {
													 ImageCMYK8Now->M = CAF_UnMask( ImageBeforeCMYK8Now->M, settings.cmyk.M);
												 }

												 if (Yellow16) {
													 ImageCMYK16Now->Y = CAF_UnMask16( ImageBeforeCMYK16Now->Y, settings.cmyk.Y);
												 }
												 else {
													 ImageCMYK8Now->Y = CAF_UnMask( ImageBeforeCMYK8Now->Y, settings.cmyk.Y);
												 }

												 if (KeyColour16) {
													 ImageCMYK16Now->K = CAF_UnMask16( ImageBeforeCMYK16Now->K, settings.cmyk.K);
												 }
												 else {
													 ImageCMYK8Now->K = CAF_UnMask( ImageBeforeCMYK8Now->K, settings.cmyk.K);
												 }

											 }

											 if (Cyan16) ImageBeforeCMYK16Now->C = CAF_Mask16(ImageCMYK16Now->C, settings.cmyk.C);
											 else ImageBeforeCMYK8Now->C = CAF_Mask(ImageCMYK8Now->C, settings.cmyk.C);

											 if (Magenta16) ImageBeforeCMYK16Now->M = CAF_Mask16(ImageCMYK16Now->M, settings.cmyk.M);
											 else ImageBeforeCMYK8Now->M = CAF_Mask(ImageCMYK8Now->M, settings.cmyk.M);

											 if (Yellow16) ImageBeforeCMYK16Now->Y = CAF_Mask16(ImageCMYK16Now->Y, settings.cmyk.Y);
											 else ImageBeforeCMYK8Now->Y = CAF_Mask(ImageCMYK8Now->Y, settings.cmyk.Y);

											 if (KeyColour16) ImageBeforeCMYK16Now->K = CAF_Mask16(ImageCMYK16Now->K, settings.cmyk.K);
											 else ImageBeforeCMYK8Now->K = CAF_Mask(ImageCMYK8Now->K, settings.cmyk.K);

											 ImageBeforeCMYK8Now++;
											 ImageBeforeCMYK16Now++;
											 ImageCMYK8Now++;
											 ImageCMYK16Now++;
										 }
									 }
									 else {

										 for (x = 0; x < width; x++) {



											 if (Cyan16) {
												 ImageCMYK16Now->C = CAF_UnMask16(ImageBeforeCMYK16Now->C, settings.cmyk.C);
											 }
											 else {
												 ImageCMYK8Now->C = CAF_UnMask(ImageBeforeCMYK8Now->C, settings.cmyk.C);
											 }

											 if (Magenta16) {
												 ImageCMYK16Now->M = CAF_UnMask16(ImageBeforeCMYK16Now->M, settings.cmyk.M);
											 }
											 else {
												 ImageCMYK8Now->M = CAF_UnMask(ImageBeforeCMYK8Now->M, settings.cmyk.M);
											 }

											 if (Yellow16) {
												 ImageCMYK16Now->Y = CAF_UnMask16(ImageBeforeCMYK16Now->Y, settings.cmyk.Y);
											 }
											 else {
												 ImageCMYK8Now->Y = CAF_UnMask(ImageBeforeCMYK8Now->Y, settings.cmyk.Y);
											 }

											 if (KeyColour16) {
												 ImageCMYK16Now->K = CAF_UnMask16(ImageBeforeCMYK16Now->K, settings.cmyk.K);
											 }
											 else {
												 ImageCMYK8Now->K = CAF_UnMask(ImageBeforeCMYK8Now->K, settings.cmyk.K);
											 }

											 if (Cyan16) ImageBeforeCMYK16Now->C = CAF_Mask16(ImageCMYK16Now->C, settings.cmyk.C);
											 else ImageBeforeCMYK8Now->C = CAF_Mask(ImageCMYK8Now->C, settings.cmyk.C);

											 if (Magenta16) ImageBeforeCMYK16Now->M = CAF_Mask16(ImageCMYK16Now->M, settings.cmyk.M);
											 else ImageBeforeCMYK8Now->M = CAF_Mask(ImageCMYK8Now->M, settings.cmyk.M);

											 if (Yellow16) ImageBeforeCMYK16Now->Y = CAF_Mask16(ImageCMYK16Now->Y, settings.cmyk.Y);
											 else ImageBeforeCMYK8Now->Y = CAF_Mask(ImageCMYK8Now->Y, settings.cmyk.Y);

											 if (KeyColour16) ImageBeforeCMYK16Now->K = CAF_Mask16(ImageCMYK16Now->K, settings.cmyk.K);
											 else ImageBeforeCMYK8Now->K = CAF_Mask(ImageCMYK8Now->K, settings.cmyk.K);

											 ImageBeforeCMYK8Now++;
											 ImageBeforeCMYK16Now++;
											 ImageCMYK8Now++;
											 ImageCMYK16Now++;
										 }
									 }


									 if (DestNow > DestMax) break;
								 }
							 }

							 if (DestNow > DestMax) continue;

							 rv->Images[n].Time = *((CANI_DWORD*)DestNow);
							 DestNow += sizeof(CANI_DWORD);
							 if (DestNow > DestMax) continue;

							 // CANI_WriteBackgroundColor(Red16,Green16,Blue16,SrcNow, &animation->Images[n]);

							 DestNow = CANI_ReadBackgroundColorCMYK(Cyan16, Magenta16, Yellow16, KeyColour16, DestNow, DestMax, &rv->Images[n], &settings);
							 if (DestNow > DestMax) continue;

							 if (id[4] >= '4') {
								 rv->Images[n].Style = *((CANI_DWORD*)DestNow);
								 DestNow += sizeof(CANI_DWORD);
							 }
							 else {
								 rv->Images[n].Style = *((CANI_BYTE*)DestNow);
								 DestNow += sizeof(CANI_BYTE);
							 }
							 if (DestNow > DestMax) continue;

							 if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								 if (Dest) CANI_FREE(Dest);

								 Dest = CANI_ReadAndDeCompress(File, &DestLen);

								 if (DestLen) {
									 DestNow = Dest;
									 DestMax = &Dest[DestLen];

									 DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										 Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);

									 CANI_FREE(Dest);
									 Dest = 0;

									 if (DestNow > DestMax) continue;
								 }
								 else {
									 DestNow = 0;
									 DestMax = 0;
								 }

							 }
							 else {

								 DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
									 Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
								 if (DestNow > DestMax) continue;

							 }

							 Which16 = 0;
							 if (Cyan16) Which16 |= CANI_IMAGE_CYAN16;
							 if (Magenta16) Which16 |= CANI_IMAGE_MAGENTA16;
							 if (Yellow16) Which16 |= CANI_IMAGE_YELLOW16;
							 if (KeyColour16) Which16 |= CANI_IMAGE_KEYCOLOUR16;
							 if (Alpha16) Which16 |= CANI_IMAGE_ALPHA16;

							 if (!CANI_DeConvertImageCMYK(rv, n, Colors16 || Alpha16, Image8, Image16, ImageCMYK8, ImageCMYK16, Which16)) {

								 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
								 if (Image8) CANI_FREE(Image8);
								 if (Image16) CANI_FREE(Image16);
								 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
								 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
								 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
								 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);
								 CANI_FREE(Dest);
								 //if (n > 0) for (m = n - 1; m > 0; m--) {
								 for (m = n; m > 0; m--) {
									 CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
								 }
								 CANI_FREE(rv->Images);
								 CANI_FREE(rv);
								 if (alph4all) CANI_FREE(alph4all);
								 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								 return 0;
							 }
						 }

						 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						 if (Image8) CANI_FREE(Image8);
						 if (Image16) CANI_FREE(Image16);
						 if (ImageCMYK8) CANI_FREE(ImageCMYK8);
						 if (ImageBeforeCMYK8) CANI_FREE(ImageBeforeCMYK8);
						 if (ImageCMYK16) CANI_FREE(ImageCMYK16);
						 if (ImageBeforeCMYK16) CANI_FREE(ImageBeforeCMYK16);

					 } else if (settings.format == CAF_FORMAT_YUV444 || settings.format == CAF_FORMAT_YUV422 || settings.format == CAF_FORMAT_YUV411
						 || settings.format == CAF_FORMAT_YUV444_ND || settings.format == CAF_FORMAT_YUV422_ND || settings.format == CAF_FORMAT_YUV411_ND) {

							 CANI_DWORD ColorSize = 3;
							 CANI_CHAR Y16,U16,V16;
							 CANI_CHAR Colors16;

							 CANI_BYTE* AlphaBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
							 CANI_WORD* AlphaBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
							 CANI_YUVAQUAD* ImageYUV8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVAQUAD));
							 CANI_YUVAQUAD* ImageBeforeYUV8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVAQUAD));
							 CANI_YUVA16QUAD* ImageYUV16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVA16QUAD));
							 CANI_YUVA16QUAD* ImageBeforeYUV16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YUVA16QUAD));
							 CANI_BGRAQUAD* Image8 = (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
							 CANI_BGRA16QUAD* Image16 = (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

							 CANI_LONG dest_x, dest_y, dest_max_x, dest_max_y, dest_width, dest_height, x, y, width, height;
							 CANI_WORD Which16;

							 if (AlphaBefore8 == 0 || AlphaBefore16 == 0 || Image8 == 0 || Image16 == 0
								 || ImageYUV8 == 0 || ImageYUV16 == 0 || ImageBeforeYUV8 == 0 || ImageBeforeYUV16 == 0) {

								 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
								 if (Image8) CANI_FREE(Image8);
								 if (Image16) CANI_FREE(Image16);
								 if (ImageYUV8) CANI_FREE(ImageYUV8);
								 if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
								 if (ImageYUV16) CANI_FREE(ImageYUV16);
								 if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);
								 CANI_FREE(Dest);
								 CANI_FREE(rv->Images);
								 CANI_FREE(rv);
								 if (alph4all) CANI_FREE(alph4all);
								 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								 return 0;
							 }

							 if (settings.yuv.Y > 8) {
								 Y16 = TRUE;
								 ColorSize++;
							 }
							 else Y16 = FALSE;

							 if (settings.yuv.U > 8) {
								 U16 = TRUE;
								 ColorSize++;
							 }
							 else U16 = FALSE;

							 if (settings.yuv.V > 8) {
								 V16 = TRUE;
								 ColorSize++;
							 }
							 else V16 = FALSE;

							 Colors16 = (ColorSize > 4 ? TRUE : FALSE);
							 Alpha16 = (settings.yuv.Alpha > 8 ? TRUE : FALSE);

							 //BackgroundColorSize = ColorSize;// (Colors16 /*|| Alpha16*/) ? (3 * sizeof(CANI_WORD)) : (3 * sizeof(CANI_BYTE));
							// AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

							 CANI_MEMSET(ImageBeforeYUV8, 0x00, (BitMapSize * sizeof(CANI_YUVAQUAD)));
							 CANI_MEMSET(ImageBeforeYUV16, 0x00, (BitMapSize * sizeof(CANI_YUVA16QUAD)));
							 CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
							 CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

							 for (n = 0; n < ImagesNR; n++) {

								 if (Progress) {

									 CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

									 if (ProgressNow2 != ProgressNow) {
										 ProgressNow = ProgressNow2;
										 Progress(ProgressNow);
									 }

								 }

								 if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

									 if (Dest) CANI_FREE(Dest);

									 Dest = CANI_ReadAndDeCompress(File, &DestLen);

									 if (DestLen) {
										 DestNow = Dest;
										 DestMax = &Dest[DestLen];
									 }
									 else {
										 DestNow = 0;
										 DestMax = 0;
									 }

								 }

								 CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

								 CANI_MEMSET(Image8, 0x00, sizeof(CANI_BGRAQUAD)* BitMapSize);
								 CANI_MEMSET(Image16, 0x00, sizeof(CANI_BGRA16QUAD)* BitMapSize);

								 if (id[4] >= '4') {

									 dest_x = *((CANI_LONG*)DestNow);
									 DestNow += sizeof(CANI_LONG);
									 dest_y = *((CANI_LONG*)DestNow);
									 DestNow += sizeof(CANI_LONG);
									 dest_width = *((CANI_LONG*)DestNow);
									 DestNow += sizeof(CANI_LONG);
									 dest_height = *((CANI_LONG*)DestNow);
									 DestNow += sizeof(CANI_LONG);
								 }
								 else {

									 dest_x = 0;
									 dest_y = 0;
									 dest_width = rv->Width;
									 dest_height = rv->Height;
								 }

								 width = rv->Width;
								 height = rv->Height;

								 dest_max_x = dest_x + dest_width;
								 dest_max_y = dest_y + dest_height;

								 //sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_width, dest_height);

								// MessageBoxA(0, szCaniBuffer, szCaniBuffer, 0);

								 //CANI_MEMSET(Image8, 0x00, sizeof(CANI_BGRAQUAD) * BitMapSize);
								 //CANI_MEMSET(Image16, 0x00, sizeof(CANI_BGRA16QUAD) * BitMapSize);

								 if (settings.format == CAF_FORMAT_YUV444 || settings.format == CAF_FORMAT_YUV444_ND) {

									 if (DestNow < DestMax) {

										 CANI_YUVAQUAD* ImageBeforeYUV8Now = ImageBeforeYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
										 CANI_YUVA16QUAD* ImageBeforeYUV16Now = ImageBeforeYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
										 CANI_YUVAQUAD* ImageYUV8Now = ImageYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
										 CANI_YUVA16QUAD* ImageYUV16Now = ImageYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

										 for (y = 0; y < height; y++) {

											 if (y >= dest_y && y < dest_max_y) {

												 for (x = 0; x < width; x++) {

													 if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

														 if (settings.format == CAF_FORMAT_YUV444) {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->Y, settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->Y, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if (U16) {
																 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->U, settings.yuv.U);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->U = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->U, settings.yuv.U);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if (V16) {
																 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->V, settings.yuv.V);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->V = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->V, settings.yuv.V);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

														 }
														 else {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if (U16) {
																 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.U);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->U = CAF_UnMask(*DestNow, settings.yuv.U);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if (V16) {
																 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.V);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->V = CAF_UnMask(*DestNow, settings.yuv.V);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

														 }

													 } else {

														 if (Y16) {
															 ImageYUV16Now->Y = CAF_UnMask16( ImageBeforeYUV16Now->Y, settings.yuv.Y);
														 }
														 else {
															 ImageYUV8Now->Y = CAF_UnMask( ImageBeforeYUV8Now->Y, settings.yuv.Y);
														 }

														 if (U16) {
															 ImageYUV16Now->U = CAF_UnMask16( ImageBeforeYUV16Now->U, settings.yuv.U);
														 }
														 else {
															 ImageYUV8Now->U = CAF_UnMask( ImageBeforeYUV8Now->U, settings.yuv.U);
														 }

														 if (V16) {
															 ImageYUV16Now->V = CAF_UnMask16( ImageBeforeYUV16Now->V, settings.yuv.V);
														 }
														 else {
															 ImageYUV8Now->V = CAF_UnMask( ImageBeforeYUV8Now->V, settings.yuv.V);
														 }

													 }

													 if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;
												 }
											 }
											 else {

												 for (x = 0; x < width; x++) {

													 if (Y16) {
														 ImageYUV16Now->Y = CAF_UnMask16(ImageBeforeYUV16Now->Y, settings.yuv.Y);
													 }
													 else {
														 ImageYUV8Now->Y = CAF_UnMask(ImageBeforeYUV8Now->Y, settings.yuv.Y);
													 }

													 if (U16) {
														 ImageYUV16Now->U = CAF_UnMask16(ImageBeforeYUV16Now->U, settings.yuv.U);
													 }
													 else {
														 ImageYUV8Now->U = CAF_UnMask(ImageBeforeYUV8Now->U, settings.yuv.U);
													 }

													 if (V16) {
														 ImageYUV16Now->V = CAF_UnMask16(ImageBeforeYUV16Now->V, settings.yuv.V);
													 }
													 else {
														 ImageYUV8Now->V = CAF_UnMask(ImageBeforeYUV8Now->V, settings.yuv.V);
													 }

													 if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;

												 }
											 }


											 if (DestNow > DestMax) break;
										 }
									 }

								 } else if (settings.format == CAF_FORMAT_YUV422 || settings.format == CAF_FORMAT_YUV422_ND) {

									 if (DestNow < DestMax) {

										 CANI_YUVAQUAD* ImageBeforeYUV8Now = ImageBeforeYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
										 CANI_YUVA16QUAD* ImageBeforeYUV16Now = ImageBeforeYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
										 CANI_YUVAQUAD* ImageYUV8Now = ImageYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
										 CANI_YUVA16QUAD* ImageYUV16Now = ImageYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

										 for (y = 0; y < height; y++) {

											 if (y >= dest_y && y < dest_max_y) {

												 for (x = 0; x < width; x++) {

													 if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

														 if (settings.format == CAF_FORMAT_YUV422) {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->Y, settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->Y, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if ((x) & 0x01) {

																 if (V16) {
																	 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->V, settings.yuv.V);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->V = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->V, settings.yuv.V);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;

															 } else {

																 if (U16) {
																	 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->U, settings.yuv.U);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->U = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->U, settings.yuv.U);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;

															 }

														 }
														 else {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if ((x) & 0x01) {

																 if (V16) {
																	 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.V);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->V = CAF_UnMask(*DestNow, settings.yuv.V);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;
															 }
															 else {

																 if (U16) {
																	 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.U);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->U = CAF_UnMask(*DestNow, settings.yuv.U);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;
															 }

														 }

													 }
													 else {

														 if (Y16) {
															 ImageYUV16Now->Y = CAF_UnMask16(ImageBeforeYUV16Now->Y, settings.yuv.Y);
														 }
														 else {
															 ImageYUV8Now->Y = CAF_UnMask(ImageBeforeYUV8Now->Y, settings.yuv.Y);
														 }

														 if (U16) {
															 ImageYUV16Now->U = CAF_UnMask16(ImageBeforeYUV16Now->U, settings.yuv.U);
														 }
														 else {
															 ImageYUV8Now->U = CAF_UnMask(ImageBeforeYUV8Now->U, settings.yuv.U);
														 }

														 if (V16) {
															 ImageYUV16Now->V = CAF_UnMask16(ImageBeforeYUV16Now->V, settings.yuv.V);
														 }
														 else {
															 ImageYUV8Now->V = CAF_UnMask(ImageBeforeYUV8Now->V, settings.yuv.V);
														 }

													 }

													 /*if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);*/

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;
												 }
											 }
											 else {

												 for (x = 0; x < width; x++) {

													 if (Y16) {
														 ImageYUV16Now->Y = CAF_UnMask16(ImageBeforeYUV16Now->Y, settings.yuv.Y);
													 }
													 else {
														 ImageYUV8Now->Y = CAF_UnMask(ImageBeforeYUV8Now->Y, settings.yuv.Y);
													 }

													 if (U16) {
														 ImageYUV16Now->U = CAF_UnMask16(ImageBeforeYUV16Now->U, settings.yuv.U);
													 }
													 else {
														 ImageYUV8Now->U = CAF_UnMask(ImageBeforeYUV8Now->U, settings.yuv.U);
													 }

													 if (V16) {
														 ImageYUV16Now->V = CAF_UnMask16(ImageBeforeYUV16Now->V, settings.yuv.V);
													 }
													 else {
														 ImageYUV8Now->V = CAF_UnMask(ImageBeforeYUV8Now->V, settings.yuv.V);
													 }

													 /*if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);*/

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;

												 }
											 }


											 if (DestNow > DestMax) break;
										 }
									 }

									 m = 0;

									 // if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

									 for (y = 0; y < dest_height; y++) {
										 for (x = 0; x < dest_width; x++) {

											 m = (dest_y + y) * AniSize.Width + (dest_x + x);

											 if (( x + dest_x ) & 0x01) {
												 if (U16) ImageYUV16[m].U = ImageYUV16[m - 1].U;
												 else ImageYUV8[m].U = ImageYUV8[m - 1].U;
											 }
											 else {
												 if ((x + dest_x) < (AniSize.Width - 1)) {
													 if (V16) ImageYUV16[m].V = ImageYUV16[m + 1].V;
													 else ImageYUV8[m].V = ImageYUV8[m + 1].V;
												 }
												 else {
													 if (V16) ImageYUV16[m].V = ImageYUV16[m - 1].V;
													 else ImageYUV8[m].V = ImageYUV8[m - 1].V;
												 }
											 }
											 m++;
										 }
									 }

									 for (m = 0; m < BitMapSize; m++) {

										 if (Y16) ImageBeforeYUV16[m].Y = CAF_Mask16(ImageYUV16[m].Y, settings.yuv.Y);
										 else ImageBeforeYUV8[m].Y = CAF_Mask(ImageYUV8[m].Y, settings.yuv.Y);

										 if (U16) ImageBeforeYUV16[m].U = CAF_Mask16(ImageYUV16[m].U, settings.yuv.U);
										 else ImageBeforeYUV8[m].U = CAF_Mask(ImageYUV8[m].U, settings.yuv.U);

										 if (V16) ImageBeforeYUV16[m].V = CAF_Mask16(ImageYUV16[m].V, settings.yuv.V);
										 else ImageBeforeYUV8[m].V = CAF_Mask(ImageYUV8[m].V, settings.yuv.V);

									 }

								 }
								 else if (settings.format == CAF_FORMAT_YUV422 || settings.format == CAF_FORMAT_YUV422_ND) {

									 if (DestNow < DestMax) {

										 CANI_YUVAQUAD* ImageBeforeYUV8Now = ImageBeforeYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
										 CANI_YUVA16QUAD* ImageBeforeYUV16Now = ImageBeforeYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
										 CANI_YUVAQUAD* ImageYUV8Now = ImageYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
										 CANI_YUVA16QUAD* ImageYUV16Now = ImageYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

										 for (y = 0; y < height; y++) {

											 if (y >= dest_y && y < dest_max_y) {

												 for (x = 0; x < width; x++) {

													 if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

														 if (settings.format == CAF_FORMAT_YUV422) {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->Y, settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->Y, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if ((x) & 0x01) {

																 if (V16) {
																	 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->V, settings.yuv.V);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->V = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->V, settings.yuv.V);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;

															 }
															 else {

																 if (U16) {
																	 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->U, settings.yuv.U);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->U = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->U, settings.yuv.U);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;

															 }

														 }
														 else {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if ((x) & 0x01) {

																 if (V16) {
																	 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.V);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->V = CAF_UnMask(*DestNow, settings.yuv.V);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;
															 }
															 else {

																 if (U16) {
																	 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.U);
																	 DestNow += 2;
																 }
																 else {
																	 ImageYUV8Now->U = CAF_UnMask(*DestNow, settings.yuv.U);
																	 DestNow++;
																 }
																 if (DestNow > DestMax) break;
															 }

														 }

													 }
													 else {

														 if (Y16) {
															 ImageYUV16Now->Y = CAF_UnMask16(ImageBeforeYUV16Now->Y, settings.yuv.Y);
														 }
														 else {
															 ImageYUV8Now->Y = CAF_UnMask(ImageBeforeYUV8Now->Y, settings.yuv.Y);
														 }

														 if (U16) {
															 ImageYUV16Now->U = CAF_UnMask16(ImageBeforeYUV16Now->U, settings.yuv.U);
														 }
														 else {
															 ImageYUV8Now->U = CAF_UnMask(ImageBeforeYUV8Now->U, settings.yuv.U);
														 }

														 if (V16) {
															 ImageYUV16Now->V = CAF_UnMask16(ImageBeforeYUV16Now->V, settings.yuv.V);
														 }
														 else {
															 ImageYUV8Now->V = CAF_UnMask(ImageBeforeYUV8Now->V, settings.yuv.V);
														 }

													 }

													 /*if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);*/

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;
												 }
											 }
											 else {

												 for (x = 0; x < width; x++) {

													 if (Y16) {
														 ImageYUV16Now->Y = CAF_UnMask16(ImageBeforeYUV16Now->Y, settings.yuv.Y);
													 }
													 else {
														 ImageYUV8Now->Y = CAF_UnMask(ImageBeforeYUV8Now->Y, settings.yuv.Y);
													 }

													 if (U16) {
														 ImageYUV16Now->U = CAF_UnMask16(ImageBeforeYUV16Now->U, settings.yuv.U);
													 }
													 else {
														 ImageYUV8Now->U = CAF_UnMask(ImageBeforeYUV8Now->U, settings.yuv.U);
													 }

													 if (V16) {
														 ImageYUV16Now->V = CAF_UnMask16(ImageBeforeYUV16Now->V, settings.yuv.V);
													 }
													 else {
														 ImageYUV8Now->V = CAF_UnMask(ImageBeforeYUV8Now->V, settings.yuv.V);
													 }

													 /*if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);*/

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;

												 }
											 }


											 if (DestNow > DestMax) break;
										 }
									 }

									 m = 0;

									 // if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

									 for (y = 0; y < dest_height; y++) {
										 for (x = 0; x < dest_width; x++) {

											 m = (dest_y + y) * AniSize.Width + (dest_x + x);

											 if ((x + dest_x) & 0x01) {
												 if (U16) ImageYUV16[m].U = ImageYUV16[m - 1].U;
												 else ImageYUV8[m].U = ImageYUV8[m - 1].U;
											 }
											 else {
												 if ((x + dest_x) < (AniSize.Width - 1)) {
													 if (V16) ImageYUV16[m].V = ImageYUV16[m + 1].V;
													 else ImageYUV8[m].V = ImageYUV8[m + 1].V;
												 }
												 else {
													 if (V16) ImageYUV16[m].V = ImageYUV16[m - 1].V;
													 else ImageYUV8[m].V = ImageYUV8[m - 1].V;
												 }
											 }
										 }
									 }

									 for (m = 0; m < BitMapSize; m++) {

										 if (Y16) ImageBeforeYUV16[m].Y = CAF_Mask16(ImageYUV16[m].Y, settings.yuv.Y);
										 else ImageBeforeYUV8[m].Y = CAF_Mask(ImageYUV8[m].Y, settings.yuv.Y);

										 if (U16) ImageBeforeYUV16[m].U = CAF_Mask16(ImageYUV16[m].U, settings.yuv.U);
										 else ImageBeforeYUV8[m].U = CAF_Mask(ImageYUV8[m].U, settings.yuv.U);

										 if (V16) ImageBeforeYUV16[m].V = CAF_Mask16(ImageYUV16[m].V, settings.yuv.V);
										 else ImageBeforeYUV8[m].V = CAF_Mask(ImageYUV8[m].V, settings.yuv.V);

									 }
								 }
								 else if (settings.format == CAF_FORMAT_YUV411 || settings.format == CAF_FORMAT_YUV411_ND) {

									 if (DestNow < DestMax) {

										 CANI_YUVAQUAD* ImageBeforeYUV8Now = ImageBeforeYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
										 CANI_YUVA16QUAD* ImageBeforeYUV16Now = ImageBeforeYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
										 CANI_YUVAQUAD* ImageYUV8Now = ImageYUV8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
										 CANI_YUVA16QUAD* ImageYUV16Now = ImageYUV16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

										 for (y = 0; y < height; y++) {

											 if (y >= dest_y && y < dest_max_y) {

												 for (x = 0; x < width; x++) {

													 if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

														 if (settings.format == CAF_FORMAT_YUV411) {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->Y, settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->Y, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if ( ( x & 0x01) == 0) {
																 if (y & 0x01) {

																	 if (V16) {
																		 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->V, settings.yuv.V);
																		 DestNow += 2;
																	 }
																	 else {
																		 ImageYUV8Now->V = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->V, settings.yuv.V);
																		 DestNow++;
																	 }
																	 if (DestNow > DestMax) break;

																 }
																 else {

																	 if (U16) {
																		 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYUV16Now->U, settings.yuv.U);
																		 DestNow += 2;
																	 }
																	 else {
																		 ImageYUV8Now->U = CAF_UnMask(*DestNow + ImageBeforeYUV8Now->U, settings.yuv.U);
																		 DestNow++;
																	 }
																	 if (DestNow > DestMax) break;

																 }
															 }

														 }
														 else {

															 if (Y16) {
																 ImageYUV16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.Y);
																 DestNow += 2;
															 }
															 else {
																 ImageYUV8Now->Y = CAF_UnMask(*DestNow, settings.yuv.Y);
																 DestNow++;
															 }
															 if (DestNow > DestMax) break;

															 if ((x & 0x01) == 0) {

																 if (y & 0x01) {

																	 if (V16) {
																		 ImageYUV16Now->V = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.V);
																		 DestNow += 2;
																	 }
																	 else {
																		 ImageYUV8Now->V = CAF_UnMask(*DestNow, settings.yuv.V);
																		 DestNow++;
																	 }
																	 if (DestNow > DestMax) break;
																 }
																 else {

																	 if (U16) {
																		 ImageYUV16Now->U = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.yuv.U);
																		 DestNow += 2;
																	 }
																	 else {
																		 ImageYUV8Now->U = CAF_UnMask(*DestNow, settings.yuv.U);
																		 DestNow++;
																	 }
																	 if (DestNow > DestMax) break;
																 }
															 }

														 }

													 }
													 else {

														 if (Y16) {
															 ImageYUV16Now->Y = CAF_UnMask16(ImageBeforeYUV16Now->Y, settings.yuv.Y);
														 }
														 else {
															 ImageYUV8Now->Y = CAF_UnMask(ImageBeforeYUV8Now->Y, settings.yuv.Y);
														 }

														 if (U16) {
															 ImageYUV16Now->U = CAF_UnMask16(ImageBeforeYUV16Now->U, settings.yuv.U);
														 }
														 else {
															 ImageYUV8Now->U = CAF_UnMask(ImageBeforeYUV8Now->U, settings.yuv.U);
														 }

														 if (V16) {
															 ImageYUV16Now->V = CAF_UnMask16(ImageBeforeYUV16Now->V, settings.yuv.V);
														 }
														 else {
															 ImageYUV8Now->V = CAF_UnMask(ImageBeforeYUV8Now->V, settings.yuv.V);
														 }

													 }

													 /*if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);*/

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;
												 }
											 }
											 else {

												 for (x = 0; x < width; x++) {

													 if (Y16) {
														 ImageYUV16Now->Y = CAF_UnMask16(ImageBeforeYUV16Now->Y, settings.yuv.Y);
													 }
													 else {
														 ImageYUV8Now->Y = CAF_UnMask(ImageBeforeYUV8Now->Y, settings.yuv.Y);
													 }

													 if (U16) {
														 ImageYUV16Now->U = CAF_UnMask16(ImageBeforeYUV16Now->U, settings.yuv.U);
													 }
													 else {
														 ImageYUV8Now->U = CAF_UnMask(ImageBeforeYUV8Now->U, settings.yuv.U);
													 }

													 if (V16) {
														 ImageYUV16Now->V = CAF_UnMask16(ImageBeforeYUV16Now->V, settings.yuv.V);
													 }
													 else {
														 ImageYUV8Now->V = CAF_UnMask(ImageBeforeYUV8Now->V, settings.yuv.V);
													 }

													 /*if (Y16) ImageBeforeYUV16Now->Y = CAF_Mask16(ImageYUV16Now->Y, settings.yuv.Y);
													 else ImageBeforeYUV8Now->Y = CAF_Mask(ImageYUV8Now->Y, settings.yuv.Y);

													 if (U16) ImageBeforeYUV16Now->U = CAF_Mask16(ImageYUV16Now->U, settings.yuv.U);
													 else ImageBeforeYUV8Now->U = CAF_Mask(ImageYUV8Now->U, settings.yuv.U);

													 if (V16) ImageBeforeYUV16Now->V = CAF_Mask16(ImageYUV16Now->V, settings.yuv.V);
													 else ImageBeforeYUV8Now->V = CAF_Mask(ImageYUV8Now->V, settings.yuv.V);*/

													 ImageBeforeYUV8Now++;
													 ImageBeforeYUV16Now++;
													 ImageYUV8Now++;
													 ImageYUV16Now++;

												 }
											 }


											 if (DestNow > DestMax) break;
										 }
									 }

									 // if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

									 for (y = 0; y < dest_height; y++) {
										 for (x = 0; x < dest_width; x++) {

											 m = (dest_y + y) * AniSize.Width + (dest_x + x);

											 if ((x + dest_x) & 0x01) {
												 if ((dest_y + y) & 0x01) { // { 1, 1 }

													 if (U16) ImageYUV16[m].U = ImageYUV16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].U;
													 else ImageYUV8[m].U = ImageYUV8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].U;

													 if (V16) ImageYUV16[m].V = ImageYUV16[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].V;
													 else ImageYUV8[m].V = ImageYUV8[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].V;
												 }
												 else { // { 1, 0 }

													 if (U16) ImageYUV16[m].U = ImageYUV16[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].U;
													 else ImageYUV8[m].U = ImageYUV8[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].U;

													 if (((dest_y + y) + 1) < AniSize.Height) {
														 if (V16) ImageYUV16[m].V = ImageYUV16[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x) - 1)].V;
														 else ImageYUV8[m].V = ImageYUV8[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x) - 1)].V;
													 }
													 else {
														 if (V16) ImageYUV16[m].V = ImageYUV16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].V;
														 else ImageYUV8[m].V = ImageYUV8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].V;
													 }
												 }
											 }
											 else {
												 if ((dest_y + y) & 0x01) {  // { 0, 1 }

													 if (U16) ImageYUV16[m].U = ImageYUV16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].U;
													 else ImageYUV8[m].U = ImageYUV8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].U;
												 }
												 else { // { 0, 0 }

													 if (((dest_y + y) + 1) < AniSize.Height) {
														 if (V16) ImageYUV16[m].V = ImageYUV16[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x))].V;
														 else ImageYUV8[m].V = ImageYUV8[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x))].V;
													 }
													 else {
														 if (V16) ImageYUV16[m].V = ImageYUV16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].V;
														 else ImageYUV8[m].V = ImageYUV8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].V;
													 }
												 }
											 }
										 }
									 }

									 for (m = 0; m < BitMapSize; m++) {

										 if (Y16) ImageBeforeYUV16[m].Y = CAF_Mask16(ImageYUV16[m].Y, settings.yuv.Y);
										 else ImageBeforeYUV8[m].Y = CAF_Mask(ImageYUV8[m].Y, settings.yuv.Y);

										 if (U16) ImageBeforeYUV16[m].U = CAF_Mask16(ImageYUV16[m].U, settings.yuv.U);
										 else ImageBeforeYUV8[m].U = CAF_Mask(ImageYUV8[m].U, settings.yuv.U);

										 if (V16) ImageBeforeYUV16[m].V = CAF_Mask16(ImageYUV16[m].V, settings.yuv.V);
										 else ImageBeforeYUV8[m].V = CAF_Mask(ImageYUV8[m].V, settings.yuv.V);

									 }

								 } 

								 if (DestNow > DestMax) continue;

								 rv->Images[n].Time = *((CANI_DWORD*)DestNow);
								 DestNow += sizeof(CANI_DWORD);
								 if (DestNow > DestMax) continue;


								 // CANI_WriteBackgroundColor(Red16,Green16,Blue16,SrcNow, &animation->Images[n]);

								 DestNow = CANI_ReadBackgroundColorYUV( Y16, U16, V16, DestNow, DestMax, &rv->Images[n], &settings);
								 if (DestNow > DestMax) continue;

								 if (id[4] >= '4') {
									 rv->Images[n].Style = *((CANI_DWORD*)DestNow);
									 DestNow += sizeof(CANI_DWORD);
								 }
								 else {
									 rv->Images[n].Style = *((CANI_BYTE*)DestNow);
									 DestNow += sizeof(CANI_BYTE);
								 }
								 if (DestNow > DestMax) continue;

								 if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

									 if (Dest) CANI_FREE(Dest);

									 Dest = CANI_ReadAndDeCompress(File, &DestLen);

									 if (DestLen) {
										 DestNow = Dest;
										 DestMax = &Dest[DestLen];

										 DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
											 Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);

										 CANI_FREE(Dest);
										 Dest = 0;

										 if (DestNow > DestMax) continue;
									 }
									 else {
										 DestNow = 0;
										 DestMax = 0;
									 }

								 }
								 else {

									 DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										 Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
									 if (DestNow > DestMax) continue;

								 }

								 Which16 = 0;
								 if (Y16) Which16 |= CANI_IMAGE_YUV_Y16;
								 if (U16) Which16 |= CANI_IMAGE_YUV_U16;
								 if (V16) Which16 |= CANI_IMAGE_YUV_V16;
								 if (Alpha16) Which16 |= CANI_IMAGE_ALPHA16;

								 if (!CANI_DeConvertImageYUV(rv, n, Colors16 || Alpha16, Image8, Image16, ImageYUV8, ImageYUV16, Which16)) {

									 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
									 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
									 if (Image8) CANI_FREE(Image8);
									 if (Image16) CANI_FREE(Image16);
									 if (ImageYUV8) CANI_FREE(ImageYUV8);
									 if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
									 if (ImageYUV16) CANI_FREE(ImageYUV16);
									 if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);
									 CANI_FREE(Dest);
									 //if (n > 0) for (m = n - 1; m > 0; m--) {
									 for (m = n; m > 0; m--) {
										 CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
									 }
									 CANI_FREE(rv->Images);
									 CANI_FREE(rv);
									 if (alph4all) CANI_FREE(alph4all);
									 CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
									 return 0;
								 }
							 }

							 if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							 if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							 if (Image8) CANI_FREE(Image8);
							 if (Image16) CANI_FREE(Image16);
							 if (ImageYUV8) CANI_FREE(ImageYUV8);
							 if (ImageBeforeYUV8) CANI_FREE(ImageBeforeYUV8);
							 if (ImageYUV16) CANI_FREE(ImageYUV16);
							 if (ImageBeforeYUV16) CANI_FREE(ImageBeforeYUV16);

					} else if ( settings.format == CAF_FORMAT_YCbCr || settings.format == CAF_FORMAT_YCbCr422 || settings.format == CAF_FORMAT_YCbCr411
						|| settings.format == CAF_FORMAT_YCbCr_ND || settings.format == CAF_FORMAT_YCbCr422_ND || settings.format == CAF_FORMAT_YCbCr411_ND ) {

						CANI_DWORD ColorSize = 3;
						CANI_CHAR Y16, Cb16, Cr16;
						CANI_CHAR Colors16;

						CANI_BYTE* AlphaBefore8 = (CANI_BYTE*)CANI_MALLOC(sizeof(CANI_BYTE) * BitMapSize);
						CANI_WORD* AlphaBefore16 = (CANI_WORD*)CANI_MALLOC(sizeof(CANI_WORD) * BitMapSize);
						CANI_YCbCrAQUAD* ImageYCbCr8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrAQUAD));
						CANI_YCbCrAQUAD* ImageBeforeYCbCr8 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrAQUAD));
						CANI_YCbCrA16QUAD* ImageYCbCr16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrA16QUAD));
						CANI_YCbCrA16QUAD* ImageBeforeYCbCr16 = CANI_MALLOC(BitMapSize * sizeof(CANI_YCbCrA16QUAD));
						CANI_BGRAQUAD* Image8 = (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
						CANI_BGRA16QUAD* Image16 = (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

						CANI_LONG dest_x, dest_y, dest_max_x, dest_max_y, dest_width, dest_height, x, y, width, height;
						CANI_WORD Which16;

						if (AlphaBefore8 == 0 || AlphaBefore16 == 0 || Image8 == 0 || Image16 == 0
							|| ImageYCbCr8 == 0 || ImageYCbCr16 == 0 || ImageBeforeYCbCr8 == 0 || ImageBeforeYCbCr16 == 0) {

							if (AlphaBefore8) CANI_FREE(AlphaBefore8);
							if (AlphaBefore16) CANI_FREE(AlphaBefore16);
							if (Image8) CANI_FREE(Image8);
							if (Image16) CANI_FREE(Image16);
							if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
							if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
							if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
							if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);
							CANI_FREE(Dest);
							CANI_FREE(rv->Images);
							CANI_FREE(rv);
							if (alph4all) CANI_FREE(alph4all);
							CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
							return 0;
						}

						if (settings.ycbcr.Y > 8) {
							Y16 = TRUE;
							ColorSize++;
						}
						else Y16 = FALSE;

						if (settings.ycbcr.Cb > 8) {
							Cb16 = TRUE;
							ColorSize++;
						}
						else Cb16 = FALSE;

						if (settings.ycbcr.Cr > 8) {
							Cr16 = TRUE;
							ColorSize++;
						}
						else Cr16 = FALSE;

						Colors16 = (ColorSize > 4 ? TRUE : FALSE);
						Alpha16 = (settings.ycbcr.Alpha > 8 ? TRUE : FALSE);

						//BackgroundColorSize = ColorSize;// (Colors16 /*|| Alpha16*/) ? (3 * sizeof(CANI_WORD)) : (3 * sizeof(CANI_BYTE));
						// AlphaSize = Alpha16 ? (sizeof(CANI_WORD)) : (sizeof(CANI_BYTE));

						CANI_MEMSET(ImageBeforeYCbCr8, 0x00, (BitMapSize * sizeof(CANI_YCbCrAQUAD)));
						CANI_MEMSET(ImageBeforeYCbCr16, 0x00, (BitMapSize * sizeof(CANI_YCbCrA16QUAD)));
						CANI_MEMSET(AlphaBefore8, 0x00, (BitMapSize * sizeof(CANI_BYTE)));
						CANI_MEMSET(AlphaBefore16, 0x00, (BitMapSize * sizeof(CANI_WORD)));

						for (n = 0; n < ImagesNR; n++) {

							if (Progress) {

								CANI_BYTE ProgressNow2 = (CANI_BYTE)(n * 100 / ImagesNR);

								if (ProgressNow2 != ProgressNow) {
									ProgressNow = ProgressNow2;
									Progress(ProgressNow);
								}

							}

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}

							CANI_MEMSET(&rv->Images[n], 0x00, sizeof(CANI_FRAME_DATA));

							CANI_MEMSET(Image8, 0x00, sizeof(CANI_BGRAQUAD) * BitMapSize);
							CANI_MEMSET(Image16, 0x00, sizeof(CANI_BGRA16QUAD) * BitMapSize);

							if (id[4] >= '4') {

								dest_x = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_y = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_width = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
								dest_height = *((CANI_LONG*)DestNow);
								DestNow += sizeof(CANI_LONG);
							}
							else {

								dest_x = 0;
								dest_y = 0;
								dest_width = rv->Width;
								dest_height = rv->Height;
							}

							width = rv->Width;
							height = rv->Height;

							dest_max_x = dest_x + dest_width;
							dest_max_y = dest_y + dest_height;

							//sprintf(szCaniBuffer, "%i %i %i x %i", dest_x, dest_y, dest_width, dest_height);

						   // MessageBoxA(0, szCaniBuffer, szCaniBuffer, 0);

							//CANI_MEMSET(Image8, 0x00, sizeof(CANI_BGRAQUAD) * BitMapSize);
							//CANI_MEMSET(Image16, 0x00, sizeof(CANI_BGRA16QUAD) * BitMapSize);

							if (settings.format == CAF_FORMAT_YCbCr || settings.format == CAF_FORMAT_YCbCr_ND) {

								if (DestNow < DestMax) {

									CANI_YCbCrAQUAD* ImageBeforeYCbCr8Now = ImageBeforeYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
									CANI_YCbCrA16QUAD* ImageBeforeYCbCr16Now = ImageBeforeYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
									CANI_YCbCrAQUAD* ImageYCbCr8Now = ImageYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
									CANI_YCbCrA16QUAD* ImageYCbCr16Now = ImageYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

									for (y = 0; y < height; y++) {

										if (y >= dest_y && y < dest_max_y) {

											for (x = 0; x < width; x++) {

												if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

													if (settings.format == CAF_FORMAT_YCbCr) {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if (Cb16) {
															ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if (Cr16) {
															ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
															DestNow++;
														}
														if (DestNow > DestMax) break;

													}
													else {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if (Cb16) {
															ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cb);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow, settings.ycbcr.Cb);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if (Cr16) {
															ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cr);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow, settings.ycbcr.Cr);
															DestNow++;
														}
														if (DestNow > DestMax) break;

													}

												}
												else {

													if (Y16) {
														ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
													}
													else {
														ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
													}

													if (Cb16) {
														ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
													}
													else {
														ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
													}

													if (Cr16) {
														ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
													}
													else {
														ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
													}

												}

												if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;
											}
										}
										else {

											for (x = 0; x < width; x++) {

												if (Y16) {
													ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
												}
												else {
													ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
												}

												if (Cb16) {
													ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
												}
												else {
													ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
												}

												if (Cr16) {
													ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
												}
												else {
													ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
												}

												if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;

											}
										}


										if (DestNow > DestMax) break;
									}
								}

							}
							else if (settings.format == CAF_FORMAT_YCbCr422 || settings.format == CAF_FORMAT_YCbCr422_ND) {

								if (DestNow < DestMax) {

									CANI_YCbCrAQUAD* ImageBeforeYCbCr8Now = ImageBeforeYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
									CANI_YCbCrA16QUAD* ImageBeforeYCbCr16Now = ImageBeforeYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
									CANI_YCbCrAQUAD* ImageYCbCr8Now = ImageYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
									CANI_YCbCrA16QUAD* ImageYCbCr16Now = ImageYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

									for (y = 0; y < height; y++) {

										if (y >= dest_y && y < dest_max_y) {

											for (x = 0; x < width; x++) {

												if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

													if (settings.format == CAF_FORMAT_YCbCr422) {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if ((x) & 0x01) {

															if (Cr16) {
																ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
																DestNow++;
															}
															if (DestNow > DestMax) break;

														}
														else {

															if (Cb16) {
																ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
																DestNow++;
															}
															if (DestNow > DestMax) break;

														}

													}
													else {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if ((x) & 0x01) {

															if (Cr16) {
																ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cr);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow, settings.ycbcr.Cr);
																DestNow++;
															}
															if (DestNow > DestMax) break;
														}
														else {

															if (Cb16) {
																ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cb);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow, settings.ycbcr.Cb);
																DestNow++;
															}
															if (DestNow > DestMax) break;
														}

													}

												}
												else {

													if (Y16) {
														ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
													}
													else {
														ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
													}

													if (Cb16) {
														ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
													}
													else {
														ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
													}

													if (Cr16) {
														ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
													}
													else {
														ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
													}

												}

												/*if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);*/

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;
											}
										}
										else {

											for (x = 0; x < width; x++) {

												if (Y16) {
													ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
												}
												else {
													ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
												}

												if (Cb16) {
													ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
												}
												else {
													ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
												}

												if (Cr16) {
													ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
												}
												else {
													ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
												}

												/*if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);*/

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;

											}
										}


										if (DestNow > DestMax) break;
									}
								}

								m = 0;

								// if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

								for (y = 0; y < dest_height; y++) {
									for (x = 0; x < dest_width; x++) {

										m = (dest_y + y) * AniSize.Width + (dest_x + x);

										if ((x + dest_x) & 0x01) {
											if (Cb16) ImageYCbCr16[m].Cb = ImageYCbCr16[m - 1].Cb;
											else ImageYCbCr8[m].Cb = ImageYCbCr8[m - 1].Cb;
										}
										else {
											if ((x + dest_x) < (AniSize.Width - 1)) {
												if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[m + 1].Cr;
												else ImageYCbCr8[m].Cr = ImageYCbCr8[m + 1].Cr;
											}
											else {
												if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[m - 1].Cr;
												else ImageYCbCr8[m].Cr = ImageYCbCr8[m - 1].Cr;
											}
										}
										m++;
									}
								}

								for (m = 0; m < BitMapSize; m++) {

									if (Y16) ImageBeforeYCbCr16[m].Y = CAF_Mask16(ImageYCbCr16[m].Y, settings.ycbcr.Y);
									else ImageBeforeYCbCr8[m].Y = CAF_Mask(ImageYCbCr8[m].Y, settings.ycbcr.Y);

									if (Cb16) ImageBeforeYCbCr16[m].Cb = CAF_Mask16(ImageYCbCr16[m].Cb, settings.ycbcr.Cb);
									else ImageBeforeYCbCr8[m].Cb = CAF_Mask(ImageYCbCr8[m].Cb, settings.ycbcr.Cb);

									if (Cr16) ImageBeforeYCbCr16[m].Cr = CAF_Mask16(ImageYCbCr16[m].Cr, settings.ycbcr.Cr);
									else ImageBeforeYCbCr8[m].Cr = CAF_Mask(ImageYCbCr8[m].Cr, settings.ycbcr.Cr);

								}

							}
							else if (settings.format == CAF_FORMAT_YCbCr422 || settings.format == CAF_FORMAT_YCbCr422_ND) {

								if (DestNow < DestMax) {

									CANI_YCbCrAQUAD* ImageBeforeYCbCr8Now = ImageBeforeYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
									CANI_YCbCrA16QUAD* ImageBeforeYCbCr16Now = ImageBeforeYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
									CANI_YCbCrAQUAD* ImageYCbCr8Now = ImageYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
									CANI_YCbCrA16QUAD* ImageYCbCr16Now = ImageYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

									for (y = 0; y < height; y++) {

										if (y >= dest_y && y < dest_max_y) {

											for (x = 0; x < width; x++) {

												if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

													if (settings.format == CAF_FORMAT_YCbCr422) {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if ((x) & 0x01) {

															if (Cr16) {
																ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
																DestNow++;
															}
															if (DestNow > DestMax) break;

														}
														else {

															if (Cb16) {
																ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
																DestNow++;
															}
															if (DestNow > DestMax) break;

														}

													}
													else {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if ((x) & 0x01) {

															if (Cr16) {
																ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cr);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow, settings.ycbcr.Cr);
																DestNow++;
															}
															if (DestNow > DestMax) break;
														}
														else {

															if (Cb16) {
																ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cb);
																DestNow += 2;
															}
															else {
																ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow, settings.ycbcr.Cb);
																DestNow++;
															}
															if (DestNow > DestMax) break;
														}

													}

												}
												else {

													if (Y16) {
														ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
													}
													else {
														ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
													}

													if (Cb16) {
														ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
													}
													else {
														ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
													}

													if (Cr16) {
														ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
													}
													else {
														ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
													}

												}

												/*if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);*/

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;
											}
										}
										else {

											for (x = 0; x < width; x++) {

												if (Y16) {
													ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
												}
												else {
													ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
												}

												if (Cb16) {
													ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
												}
												else {
													ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
												}

												if (Cr16) {
													ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
												}
												else {
													ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
												}

												/*if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);*/

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;

											}
										}


										if (DestNow > DestMax) break;
									}
								}

								m = 0;

								// if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

								for (y = 0; y < dest_height; y++) {
									for (x = 0; x < dest_width; x++) {

										m = (dest_y + y) * AniSize.Width + (dest_x + x);

										if ((x + dest_x) & 0x01) {
											if (Cb16) ImageYCbCr16[m].Cb = ImageYCbCr16[m - 1].Cb;
											else ImageYCbCr8[m].Cb = ImageYCbCr8[m - 1].Cb;
										}
										else {
											if ((x + dest_x) < (AniSize.Width - 1)) {
												if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[m + 1].Cr;
												else ImageYCbCr8[m].Cr = ImageYCbCr8[m + 1].Cr;
											}
											else {
												if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[m - 1].Cr;
												else ImageYCbCr8[m].Cr = ImageYCbCr8[m - 1].Cr;
											}
										}
									}
								}

								for (m = 0; m < BitMapSize; m++) {

									if (Y16) ImageBeforeYCbCr16[m].Y = CAF_Mask16(ImageYCbCr16[m].Y, settings.ycbcr.Y);
									else ImageBeforeYCbCr8[m].Y = CAF_Mask(ImageYCbCr8[m].Y, settings.ycbcr.Y);

									if (Cb16) ImageBeforeYCbCr16[m].Cb = CAF_Mask16(ImageYCbCr16[m].Cb, settings.ycbcr.Cb);
									else ImageBeforeYCbCr8[m].Cb = CAF_Mask(ImageYCbCr8[m].Cb, settings.ycbcr.Cb);

									if (Cr16) ImageBeforeYCbCr16[m].Cr = CAF_Mask16(ImageYCbCr16[m].Cr, settings.ycbcr.Cr);
									else ImageBeforeYCbCr8[m].Cr = CAF_Mask(ImageYCbCr8[m].Cr, settings.ycbcr.Cr);

								}
							}
							else if (settings.format == CAF_FORMAT_YCbCr411 || settings.format == CAF_FORMAT_YCbCr411_ND) {

								if (DestNow < DestMax) {

									CANI_YCbCrAQUAD* ImageBeforeYCbCr8Now = ImageBeforeYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(sizeof(CANI_BGRAQUAD) * BitMapSize);
									CANI_YCbCrA16QUAD* ImageBeforeYCbCr16Now = ImageBeforeYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(sizeof(CANI_BGRA16QUAD) * BitMapSize);
									CANI_YCbCrAQUAD* ImageYCbCr8Now = ImageYCbCr8;// (CANI_BGRAQUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRAQUAD));
									CANI_YCbCrA16QUAD* ImageYCbCr16Now = ImageYCbCr16;// (CANI_BGRA16QUAD*)CANI_MALLOC(BitMapSize * sizeof(CANI_BGRA16QUAD));

									for (y = 0; y < height; y++) {

										if (y >= dest_y && y < dest_max_y) {

											for (x = 0; x < width; x++) {

												if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

													if (settings.format == CAF_FORMAT_YCbCr411) {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if ((x & 0x01) == 0) {
															if (y & 0x01) {

																if (Cr16) {
																	ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
																	DestNow += 2;
																}
																else {
																	ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
																	DestNow++;
																}
																if (DestNow > DestMax) break;

															}
															else {

																if (Cb16) {
																	ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow) + ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
																	DestNow += 2;
																}
																else {
																	ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow + ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
																	DestNow++;
																}
																if (DestNow > DestMax) break;

															}
														}

													}
													else {

														if (Y16) {
															ImageYCbCr16Now->Y = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Y);
															DestNow += 2;
														}
														else {
															ImageYCbCr8Now->Y = CAF_UnMask(*DestNow, settings.ycbcr.Y);
															DestNow++;
														}
														if (DestNow > DestMax) break;

														if ((x & 0x01) == 0) {

															if (y & 0x01) {

																if (Cr16) {
																	ImageYCbCr16Now->Cr = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cr);
																	DestNow += 2;
																}
																else {
																	ImageYCbCr8Now->Cr = CAF_UnMask(*DestNow, settings.ycbcr.Cr);
																	DestNow++;
																}
																if (DestNow > DestMax) break;
															}
															else {

																if (Cb16) {
																	ImageYCbCr16Now->Cb = CAF_UnMask16(*((CANI_WORD*)DestNow), settings.ycbcr.Cb);
																	DestNow += 2;
																}
																else {
																	ImageYCbCr8Now->Cb = CAF_UnMask(*DestNow, settings.ycbcr.Cb);
																	DestNow++;
																}
																if (DestNow > DestMax) break;
															}
														}

													}

												}
												else {

													if (Y16) {
														ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
													}
													else {
														ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
													}

													if (Cb16) {
														ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
													}
													else {
														ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
													}

													if (Cr16) {
														ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
													}
													else {
														ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
													}

												}

												/*if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);*/

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;
											}
										}
										else {

											for (x = 0; x < width; x++) {

												if (Y16) {
													ImageYCbCr16Now->Y = CAF_UnMask16(ImageBeforeYCbCr16Now->Y, settings.ycbcr.Y);
												}
												else {
													ImageYCbCr8Now->Y = CAF_UnMask(ImageBeforeYCbCr8Now->Y, settings.ycbcr.Y);
												}

												if (Cb16) {
													ImageYCbCr16Now->Cb = CAF_UnMask16(ImageBeforeYCbCr16Now->Cb, settings.ycbcr.Cb);
												}
												else {
													ImageYCbCr8Now->Cb = CAF_UnMask(ImageBeforeYCbCr8Now->Cb, settings.ycbcr.Cb);
												}

												if (Cr16) {
													ImageYCbCr16Now->Cr = CAF_UnMask16(ImageBeforeYCbCr16Now->Cr, settings.ycbcr.Cr);
												}
												else {
													ImageYCbCr8Now->Cr = CAF_UnMask(ImageBeforeYCbCr8Now->Cr, settings.ycbcr.Cr);
												}

												/*if (Y16) ImageBeforeYCbCr16Now->Y = CAF_Mask16(ImageYCbCr16Now->Y, settings.ycbcr.Y);
												else ImageBeforeYCbCr8Now->Y = CAF_Mask(ImageYCbCr8Now->Y, settings.ycbcr.Y);

												if (Cb16) ImageBeforeYCbCr16Now->Cb = CAF_Mask16(ImageYCbCr16Now->Cb, settings.ycbcr.Cb);
												else ImageBeforeYCbCr8Now->Cb = CAF_Mask(ImageYCbCr8Now->Cb, settings.ycbcr.Cb);

												if (Cr16) ImageBeforeYCbCr16Now->Cr = CAF_Mask16(ImageYCbCr16Now->Cr, settings.ycbcr.Cr);
												else ImageBeforeYCbCr8Now->Cr = CAF_Mask(ImageYCbCr8Now->Cr, settings.ycbcr.Cr);*/

												ImageBeforeYCbCr8Now++;
												ImageBeforeYCbCr16Now++;
												ImageYCbCr8Now++;
												ImageYCbCr16Now++;

											}
										}


										if (DestNow > DestMax) break;
									}
								}

								// if (x >= dest_x && x < dest_max_x && y >= dest_y && y < dest_max_y) {

								for (y = 0; y < dest_height; y++) {
									for (x = 0; x < dest_width; x++) {

										m = (dest_y + y) * AniSize.Width + (dest_x + x);

										if ((x + dest_x) & 0x01) {
											if ((dest_y + y) & 0x01) { // { 1, 1 }

												if (Cb16) ImageYCbCr16[m].Cb = ImageYCbCr16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].Cb;
												else ImageYCbCr8[m].Cb = ImageYCbCr8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].Cb;

												if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].Cr;
												else ImageYCbCr8[m].Cr = ImageYCbCr8[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].Cr;
											}
											else { // { 1, 0 }

												if (Cb16) ImageYCbCr16[m].Cb = ImageYCbCr16[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].Cb;
												else ImageYCbCr8[m].Cb = ImageYCbCr8[((dest_y + y)) * AniSize.Width + ((x + dest_x) - 1)].Cb;

												if (((dest_y + y) + 1) < AniSize.Height) {
													if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x) - 1)].Cr;
													else ImageYCbCr8[m].Cr = ImageYCbCr8[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x) - 1)].Cr;
												}
												else {
													if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].Cr;
													else ImageYCbCr8[m].Cr = ImageYCbCr8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x) - 1)].Cr;
												}
											}
										}
										else {
											if ((dest_y + y) & 0x01) {  // { 0, 1 }

												if (Cb16) ImageYCbCr16[m].Cb = ImageYCbCr16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].Cb;
												else ImageYCbCr8[m].Cb = ImageYCbCr8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].Cb;
											}
											else { // { 0, 0 }

												if (((dest_y + y) + 1) < AniSize.Height) {
													if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x))].Cr;
													else ImageYCbCr8[m].Cr = ImageYCbCr8[((dest_y + y) + 1) * AniSize.Width + ((x + dest_x))].Cr;
												}
												else {
													if (Cr16) ImageYCbCr16[m].Cr = ImageYCbCr16[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].Cr;
													else ImageYCbCr8[m].Cr = ImageYCbCr8[((dest_y + y) - 1) * AniSize.Width + ((x + dest_x))].Cr;
												}
											}
										}
									}
								}

								for (m = 0; m < BitMapSize; m++) {

									if (Y16) ImageBeforeYCbCr16[m].Y = CAF_Mask16(ImageYCbCr16[m].Y, settings.ycbcr.Y);
									else ImageBeforeYCbCr8[m].Y = CAF_Mask(ImageYCbCr8[m].Y, settings.ycbcr.Y);

									if (Cb16) ImageBeforeYCbCr16[m].Cb = CAF_Mask16(ImageYCbCr16[m].Cb, settings.ycbcr.Cb);
									else ImageBeforeYCbCr8[m].Cb = CAF_Mask(ImageYCbCr8[m].Cb, settings.ycbcr.Cb);

									if (Cr16) ImageBeforeYCbCr16[m].Cr = CAF_Mask16(ImageYCbCr16[m].Cr, settings.ycbcr.Cr);
									else ImageBeforeYCbCr8[m].Cr = CAF_Mask(ImageYCbCr8[m].Cr, settings.ycbcr.Cr);

								}

							}

							if (DestNow > DestMax) continue;

							rv->Images[n].Time = *((CANI_DWORD*)DestNow);
							DestNow += sizeof(CANI_DWORD);
							if (DestNow > DestMax) continue;


							// CANI_WriteBackgroundColor(Red16,Green16,Blue16,SrcNow, &animation->Images[n]);

							DestNow = CANI_ReadBackgroundColorYCbCr(Y16, Cb16, Cr16, DestNow, DestMax, &rv->Images[n], &settings);
							if (DestNow > DestMax) continue;

							if (id[4] >= '4') {
								rv->Images[n].Style = *((CANI_DWORD*)DestNow);
								DestNow += sizeof(CANI_DWORD);
							}
							else {
								rv->Images[n].Style = *((CANI_BYTE*)DestNow);
								DestNow += sizeof(CANI_BYTE);
							}
							if (DestNow > DestMax) continue;

							if ((settings.options & CANI_OPTION_COMPRESS_SEPARATELY)) {

								if (Dest) CANI_FREE(Dest);

								Dest = CANI_ReadAndDeCompress(File, &DestLen);

								if (DestLen) {
									DestNow = Dest;
									DestMax = &Dest[DestLen];

									DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
										Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);

									CANI_FREE(Dest);
									Dest = 0;

									if (DestNow > DestMax) continue;
								}
								else {
									DestNow = 0;
									DestMax = 0;
								}

							}
							else {

								DestNow = CANI_ReadAlpha(rv, &settings, n, Alpha16, DestNow, DestMax,
									Image8, Image16, AlphaBefore8, AlphaBefore16, alph4all, id[4]);
								if (DestNow > DestMax) continue;

							}

							Which16 = 0;
							if (Y16) Which16 |= CANI_IMAGE_YCbCr_Y16;
							if (Cb16) Which16 |= CANI_IMAGE_YCbCr_Cb16;
							if (Cr16) Which16 |= CANI_IMAGE_YCbCr_Cr16;
							if (Alpha16) Which16 |= CANI_IMAGE_ALPHA16;

							if (!CANI_DeConvertImageYCbCr(rv, n, Colors16 || Alpha16, Image8, Image16, ImageYCbCr8, ImageYCbCr16, Which16)) {

								if (AlphaBefore8) CANI_FREE(AlphaBefore8);
								if (AlphaBefore16) CANI_FREE(AlphaBefore16);
								if (Image8) CANI_FREE(Image8);
								if (Image16) CANI_FREE(Image16);
								if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
								if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
								if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
								if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);
								CANI_FREE(Dest);
								//if (n > 0) for (m = n - 1; m > 0; m--) {
								for (m = n; m > 0; m--) {
									CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
								}
								CANI_FREE(rv->Images);
								CANI_FREE(rv);
								if (alph4all) CANI_FREE(alph4all);
								CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
								return 0;
							}
						}

						if (AlphaBefore8) CANI_FREE(AlphaBefore8);
						if (AlphaBefore16) CANI_FREE(AlphaBefore16);
						if (Image8) CANI_FREE(Image8);
						if (Image16) CANI_FREE(Image16);
						if (ImageYCbCr8) CANI_FREE(ImageYCbCr8);
						if (ImageBeforeYCbCr8) CANI_FREE(ImageBeforeYCbCr8);
						if (ImageYCbCr16) CANI_FREE(ImageYCbCr16);
						if (ImageBeforeYCbCr16) CANI_FREE(ImageBeforeYCbCr16);

					} else {

						if (Src) CANI_FREE(Src);
						if (Dest) CANI_FREE(Dest);
						if (rv) CANI_FREE(rv);

						CANI_SetLastError(CANI_ERROR_UNSUPPORTED_VERSION);

						return 0;
					}

				} else { // not supported version

					if (Src) CANI_FREE(Src);
					if (Dest) CANI_FREE(Dest);
					if (rv) CANI_FREE(rv);

					CANI_SetLastError(CANI_ERROR_UNSUPPORTED_VERSION);

					return 0;

					/*CANI_FREE(Src);//delete [] Src;
					CANI_FREE(Dest);//delete [] Dest;
					CANI_FREE(rv);//delete [] rv;
					rv =  0;*/
				}
				
				//if ( alph4all ) CANI_FREE( alph4all );

				rv->gAlpha = alph4all;

				if (Progress) Progress(100);
			}
		}

		if (rv) CANI_MEMCPY(&rv->Format, &settings, sizeof(CANI_FORMAT_V2) );

		// CANI_FORMAT settings;

		if (Src) CANI_FREE(Src);
		if (Dest) CANI_FREE(Dest);

		return rv;
	}
	return 0;
}

#ifdef CANI_UNICODE_FILE

CANI_EXPORT CANI_ANIMATION* CANI_OpenAnimationW(CANI_WORD* FileName, CANI_PROGRESS_FUNC Progress) {

	FILE* File = CANI_FOPEN_UTF16(FileName, L"rb");

	if (File) {
		CANI_ANIMATION* rv = CANI_ReadAnimation(File, Progress);
		CANI_FCLOSE(File);
		return rv;
	}
	return 0;
}

#endif

CANI_EXPORT CANI_ANIMATION* CANI_OpenAnimationA(CANI_CHAR* FileName, CANI_PROGRESS_FUNC Progress ) {

	FILE* File = CANI_FOPEN_ASCII(FileName, "rb" );

	if (File) {
		CANI_ANIMATION* rv = CANI_ReadAnimation(File, Progress);
		CANI_FCLOSE(File);
		return rv;
	}
	return 0;
}

static CANI_DWORD CANI_LastErrorCode = 0;

CANI_EXPORT void CANI_SetLastError( CANI_DWORD ErrorCode ) {
	CANI_LastErrorCode = ErrorCode;
}

CANI_EXPORT CANI_DWORD CANI_GetLastError() {
	return CANI_LastErrorCode;
}


