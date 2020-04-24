#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "cani.h"

#ifdef CANI_WINDOWS_GDI

CANI_EXPORT CANI_CHAR CANI_WinDrawImage(CANI_ANIMATION* animation, HDC dest, CANI_LONG x, CANI_LONG y) {

	if (animation && (animation->ImagesNow < animation->ImagesNR) && animation->Images[animation->ImagesNow].Image) {

		BITMAPINFO bmpI;
		bmpI.bmiHeader.biBitCount = 32;
		bmpI.bmiHeader.biCompression = BI_RGB;
		bmpI.bmiHeader.biWidth = animation->Width;
		bmpI.bmiHeader.biHeight = animation->Height;
		bmpI.bmiHeader.biPlanes = 1;
		bmpI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpI.bmiHeader.biSizeImage = 0;
		bmpI.bmiHeader.biClrUsed = 0;

		if ( animation->Images[animation->ImagesNow].Style & CANI_STYLE_16BITS_IMAGE ) {

			//MessageBoxA(0,"CANI_WinDrawImage","CANI_STYLE_16BITS_IMAGE",0);

			CANI_DWORD ImageSize = animation->Width * animation->Height;

			RGBQUAD* Image = (RGBQUAD*)CANI_MALLOC(ImageSize * sizeof(RGBQUAD));

			if (Image) {

				CANI_BGRA16_2_BGRA((CANI_BGRAQUAD*)Image, (CANI_BGRA16QUAD*)animation->Images[animation->ImagesNow].Image, ImageSize);

				SetDIBitsToDevice(dest,
					x, y,
					animation->Width, animation->Height,
					0, 0, 0, animation->Height,
					Image,
					&bmpI, DIB_RGB_COLORS);

				CANI_FREE(Image);

				return TRUE;
			}
		}
		else {

			//MessageBoxA(0, "CANI_WinDrawImage", "", 0);

			SetDIBitsToDevice(dest,
				x, y,
				animation->Width, animation->Height,
				0, 0, 0, animation->Height,
				animation->Images[animation->ImagesNow].Image,
				&bmpI, DIB_RGB_COLORS);

			return TRUE;

		}
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_WinDrawImageResized(CANI_ANIMATION* animation, HDC dest,
	CANI_LONG xDest, CANI_LONG yDest, CANI_LONG DestWidth, CANI_LONG DestHeight, CANI_LONG xSrc, CANI_LONG ySrc, CANI_LONG SrcWidth, CANI_LONG SrcHeight ) {

	if (animation && (animation->ImagesNow < animation->ImagesNR) && animation->Images[animation->ImagesNow].Image) {

		BITMAPINFO bmpI;
		bmpI.bmiHeader.biBitCount = 32;
		bmpI.bmiHeader.biCompression = BI_RGB;
		bmpI.bmiHeader.biWidth = animation->Width;
		bmpI.bmiHeader.biHeight = animation->Height;
		bmpI.bmiHeader.biPlanes = 1;
		bmpI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpI.bmiHeader.biSizeImage = 0;
		bmpI.bmiHeader.biClrUsed = 0;

		if (animation->Images[animation->ImagesNow].Style & CANI_STYLE_16BITS_IMAGE) {

			//MessageBoxA(0,"CANI_WinDrawImage","CANI_STYLE_16BITS_IMAGE",0);

			CANI_DWORD ImageSize = animation->Width * animation->Height;

			RGBQUAD* Image = (RGBQUAD*)CANI_MALLOC(ImageSize * sizeof(RGBQUAD));

			if (Image) {

				CANI_BGRA16_2_BGRA((CANI_BGRAQUAD*)Image, (CANI_BGRA16QUAD*)animation->Images[animation->ImagesNow].Image, ImageSize);

				StretchDIBits(dest,
					xDest, yDest,
					DestWidth, DestHeight,
					xSrc, ySrc,
					SrcWidth, SrcHeight,
					Image,
					&bmpI, DIB_RGB_COLORS, SRCCOPY );

				CANI_FREE(Image);

				return TRUE;
			}
		}
		else {

			//MessageBoxA(0, "CANI_WinDrawImage", "", 0);

			StretchDIBits(dest,
				xDest, yDest,
				DestWidth, DestHeight,
				xSrc, ySrc,
				SrcWidth, SrcHeight,
				animation->Images[animation->ImagesNow].Image,
				&bmpI, DIB_RGB_COLORS, SRCCOPY);

			return TRUE;

		}
	}

	return FALSE;
}

CANI_EXPORT CANI_CHAR CANI_WinDrawMask(CANI_ANIMATION* animation, HDC dest, CANI_LONG x, CANI_LONG y) {

	if (animation && (animation->ImagesNow < animation->ImagesNR) && animation->Images[animation->ImagesNow].Image) {

		CANI_DWORD ImageSize = animation->Width * animation->Height;

		RGBQUAD* Mask = (RGBQUAD*)CANI_MALLOC(ImageSize * sizeof(RGBQUAD));

		if (Mask) {
			BITMAPINFO bmpI;

			if (animation->Images[animation->ImagesNow].Style & CANI_STYLE_16BITS_IMAGE) {
				RGBQUAD* MaskNow = Mask;
				RGBQUAD* MaskMax = &Mask[ImageSize];
				CANI_BGRA16QUAD* Src = (CANI_BGRA16QUAD*)animation->Images[animation->ImagesNow].Image;

				while (MaskNow < MaskMax) {
					MaskNow->rgbRed = MaskNow->rgbGreen = MaskNow->rgbBlue = (CANI_BYTE)(Src->Alpha >> 8);
					MaskNow++;
					Src++;
				}
			}
			else {
				RGBQUAD* MaskNow = Mask;
				RGBQUAD* MaskMax = &Mask[ImageSize];
				CANI_BGRAQUAD* Src = (CANI_BGRAQUAD*)animation->Images[animation->ImagesNow].Image;

				while (MaskNow < MaskMax) {
					MaskNow->rgbRed = MaskNow->rgbGreen = MaskNow->rgbBlue = Src->Alpha;
					MaskNow++;
					Src++;
				}
			}

			bmpI.bmiHeader.biBitCount = 32;
			bmpI.bmiHeader.biCompression = BI_RGB;
			bmpI.bmiHeader.biWidth = animation->Width;
			bmpI.bmiHeader.biHeight = animation->Height;
			bmpI.bmiHeader.biPlanes = 1;
			bmpI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmpI.bmiHeader.biSizeImage = 0;
			bmpI.bmiHeader.biClrUsed = 0;

			SetDIBitsToDevice(dest,
				x, y,
				animation->Width, animation->Height,
				0, 0, 0, animation->Height,
				Mask,
				&bmpI, DIB_RGB_COLORS);

			CANI_FREE(Mask);

			return TRUE;
		}
	}

	return FALSE;
}

#endif