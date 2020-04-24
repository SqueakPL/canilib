#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "cani.h"

#include <math.h>

CANI_EXPORT CANI_CHAR CANI_FastResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BGRA16QUAD* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	if (src_cx == dest_cx && src_cy == dest_cy) {

		CANI_MEMCPY(dest_adress, src_adress,
			src_cx * src_cy * sizeof(CANI_BGRA16QUAD));
		return TRUE;
	}

	buffor = (CANI_BGRA16QUAD*)CANI_MALLOC((src_cy * dest_cx) * sizeof(CANI_BGRA16QUAD));//new CANI_BGRA16QUAD[src_cy * dest_cx];

	if (buffor) {
		CANI_LONG x, y, x2, y2;

		if (dest_cx > src_cx) {
			CANI_BGRA16QUAD* dest_now = buffor;

			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BGRA16QUAD* src;
					CANI_BGRA16QUAD* dest = dest_now;//&buffor[ y * dest_cx + x ];

					x2 = x * src_cx / dest_cx;
					if (x2 >= src_cx) x2 = src_cx - 1;

					src = //&src_adress[ c[0] ];
						&src_adress[y * src_cx + x2];

					CANI_MEMCPY( dest, src, sizeof(CANI_BGRA16QUAD) );
					//*((CANI_DWORD*)dest) = *((CANI_DWORD*)src);

					dest_now++;
				}
			}
		}
		else if (dest_cx < src_cx) {
			CANI_BGRA16QUAD* dest_now = buffor;

			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BGRA16QUAD* src;
					CANI_BGRA16QUAD* dest = dest_now;//&buffor[ y * dest_cx + x ];

					x2 = x * src_cx / dest_cx;
					if (x2 >= src_cx)
						x2 = src_cx - 1;

					src = &src_adress[y * src_cx + x2];

					CANI_MEMCPY( dest, src, sizeof(CANI_BGRA16QUAD) );
					//*((CANI_DWORD*)dest) = *((CANI_DWORD*)src);

					dest_now++;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress,
				src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}


		if (dest_cy > src_cy) {

			CANI_BGRA16QUAD* dest_now = dest_adress;//[ y * dest_cx + x ];

			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BGRA16QUAD* src;
					//CANI_BGRA16QUAD *dest = dest_now;//&dest_adress[ y * dest_cx + x ];

					y2 = y * src_cy / dest_cy;
					if (y2 >= src_cy) y2 = src_cy - 1;

					src = &buffor[y2 * dest_cx + x];

					//CANI_MEMCPY( dest, src, sizeof(CANI_BGRA16QUAD) );

					//*((CANI_DWORD*)dest_now) = *((CANI_DWORD*)src);
					CANI_MEMCPY(dest_now, src, sizeof(CANI_BGRA16QUAD));

					dest_now++;
				}
			}
		}
		else if (dest_cy < src_cy) {

			CANI_BGRA16QUAD* dest_now = dest_adress;//[ y * dest_cx + x ];

			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BGRA16QUAD* src;
					//CANI_BGRA16QUAD *dest = dest_now;//&dest_adress[ y * dest_cx + x ];

					y2 = y * src_cy / dest_cy;
					if (y2 >= src_cy) y2 = src_cy - 1;

					src = &buffor[y2 * dest_cx + x];

					//CANI_MEMCPY( dest, src, sizeof(CANI_BGRA16QUAD) );

					//*((CANI_DWORD*)dest_now) = *((CANI_DWORD*)src);
					CANI_MEMCPY(dest_now, src, sizeof(CANI_BGRA16QUAD));

					dest_now++;
				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}
		CANI_FREE(buffor);//delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_EXPORT CANI_CHAR CANI_InterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BGRA16QUAD* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BGRA16QUAD*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));//new CANI_BGRA16QUAD[src_cy * dest_cx];

	if (buffor) {
		CANI_LONG x, y, x2, y2;
		CANI_DOUBLE wynik;
		CANI_DOUBLE rx, ry;
		if (src_cx > 1 && dest_cx > 1) {
			rx = ((CANI_DOUBLE)src_cx - 1.0f)
				/ ((CANI_DOUBLE)dest_cx - 1.0f);
		}
		else {
			rx = ((CANI_DOUBLE)src_cx)
				/ ((CANI_DOUBLE)dest_cx);
		}

		if (src_cy > 1 && dest_cy > 1) {
			ry = ((CANI_DOUBLE)src_cy - 1.0f)
				/ ((CANI_DOUBLE)dest_cy - 1.0f);
		}
		else {
			ry = ((CANI_DOUBLE)src_cy)
				/ ((CANI_DOUBLE)dest_cy);
		}

		if (dest_cx > src_cx) {
			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_DOUBLE zaokraglenie;
					CANI_DOUBLE reszta;

					CANI_BGRA16QUAD* dest = &buffor[y * dest_cx + x];

					wynik = ((CANI_DOUBLE)x) * rx;

					zaokraglenie = floor(wynik);
					reszta = wynik - zaokraglenie;

					if (reszta > 0 && wynik >= 0 && (zaokraglenie < src_cx)) {

						CANI_DOUBLE wartosc_a = src_adress[y * src_cx + (CANI_LONG)zaokraglenie].Red;

						CANI_DOUBLE wartosc_b = src_adress[y * src_cx + ((CANI_LONG)zaokraglenie + 1)].Red;

						CANI_DOUBLE wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						buffor[y * dest_cx + x].Red = (CANI_WORD)wartosc;


						wartosc_a = src_adress[y * src_cx + (CANI_LONG)zaokraglenie].Green;

						wartosc_b = src_adress[y * src_cx + ((CANI_LONG)zaokraglenie + 1)].Green;

						wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						buffor[y * dest_cx + x].Green = (CANI_WORD)wartosc;


						wartosc_a = src_adress[y * src_cx + (CANI_LONG)zaokraglenie].Blue;

						wartosc_b = src_adress[y * src_cx + ((CANI_LONG)zaokraglenie + 1)].Blue;

						wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						buffor[y * dest_cx + x].Blue = (CANI_WORD)wartosc;


						wartosc_a = src_adress[y * src_cx + (CANI_LONG)zaokraglenie].Alpha;

						wartosc_b = src_adress[y * src_cx + ((CANI_LONG)zaokraglenie + 1)].Alpha;

						wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						buffor[y * dest_cx + x].Alpha = (CANI_WORD)wartosc;

					}
					else {

						CANI_BGRA16QUAD* src;

						x2 = x * src_cx / dest_cx;
						if (x2 >= src_cx) x2 = src_cx - 1;

						if (x2 >= src_cx) x2 = src_cx - 1;

						src = &src_adress[y * src_cx + x2];

						CANI_MEMCPY(dest, src, sizeof(CANI_BGRA16QUAD));

					}
				}
			}
		}
		else if (dest_cx < src_cx) {
			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BGRA16QUAD* src;
					CANI_BGRA16QUAD* dest = &buffor[y * dest_cx + x];

					x2 = x * src_cx / dest_cx;
					if (x2 >= src_cx) x2 = src_cx - 1;

					src = &src_adress[y * src_cx + x2];

					CANI_MEMCPY(dest, src, sizeof(CANI_BGRA16QUAD));
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}


		if (dest_cy > src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_DOUBLE zaokraglenie;
					CANI_DOUBLE reszta;

					CANI_BGRA16QUAD* dest = &dest_adress[y * dest_cx + x];

					wynik = ((CANI_DOUBLE)y) * ry;

					zaokraglenie = floor(wynik);
					reszta = wynik - zaokraglenie;

					if (reszta > 0 && wynik >= 0 && (zaokraglenie < src_cx)) {

						CANI_DOUBLE wartosc_a = buffor[((CANI_LONG)zaokraglenie + 0) * dest_cx + x].Red;

						CANI_DOUBLE wartosc_b = buffor[((CANI_LONG)zaokraglenie + 1) * dest_cx + x].Red;

						CANI_DOUBLE wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						dest_adress[y * dest_cx + x].Red = (CANI_WORD)wartosc;


						wartosc_a = buffor[((CANI_LONG)zaokraglenie + 0) * dest_cx + x].Green;

						wartosc_b = buffor[((CANI_LONG)zaokraglenie + 1) * dest_cx + x].Green;

						wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						dest_adress[y * dest_cx + x].Green = (CANI_WORD)wartosc;


						wartosc_a = buffor[((CANI_LONG)zaokraglenie + 0) * dest_cx + x].Blue;

						wartosc_b = buffor[((CANI_LONG)zaokraglenie + 1) * dest_cx + x].Blue;

						wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						dest_adress[y * dest_cx + x].Blue = (CANI_WORD)wartosc;


						wartosc_a = buffor[((CANI_LONG)zaokraglenie + 0) * dest_cx + x].Alpha;

						wartosc_b = buffor[((CANI_LONG)zaokraglenie + 1) * dest_cx + x].Alpha;

						wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						dest_adress[y * dest_cx + x].Alpha = (CANI_WORD)wartosc;

					}
					else {
						CANI_BGRA16QUAD* src;

						y2 = y * src_cy / dest_cy;
						if (y2 >= src_cy) y2 = src_cy - 1;

						src = &buffor[y2 * dest_cx + x];

						CANI_MEMCPY(dest, src, sizeof(CANI_BGRA16QUAD));
					}
				}
			}
		}
		else if (dest_cy < src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BGRA16QUAD* src;
					CANI_BGRA16QUAD* dest = &dest_adress[y * dest_cx + x];

					y2 = y * src_cy / dest_cy;
					if (y2 >= src_cy) y2 = src_cy - 1;

					src = &buffor[y2 * dest_cx + x];

					CANI_MEMCPY(dest, src, sizeof(CANI_BGRA16QUAD));
				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}

		CANI_FREE(buffor);// delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_EXPORT CANI_CHAR CANI_SquareInterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BGRA16QUAD* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BGRA16QUAD*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));//new CANI_BGRA16QUAD[src_cy * dest_cx];

	if (buffor) {
		CANI_LONG x, y, x2, y2;
		CANI_DOUBLE wynik, x3, x5, a, b, y3, y5;
		CANI_DOUBLE c4[3];
		CANI_DOUBLE rx, ry;
		if (src_cx > 1 && dest_cx > 1) {
			rx = ((CANI_DOUBLE)src_cx - 1.0f) / ((CANI_DOUBLE)dest_cx - 1.0f);
		}
		else {
			rx = ((CANI_DOUBLE)src_cx) / ((CANI_DOUBLE)dest_cx);
		}

		if (src_cy > 1 && dest_cy > 1) {
			ry = ((CANI_DOUBLE)src_cy - 1.0f) / ((CANI_DOUBLE)dest_cy - 1.0f);
		}
		else {
			ry = ((CANI_DOUBLE)src_cy) / ((CANI_DOUBLE)dest_cy);
		}

		if (dest_cx != src_cx) {
			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BGRA16QUAD* dest = &buffor[y * dest_cx + x];

					x3 = ((CANI_DOUBLE)x) * rx;
					x5 = 1.0f + x3 - floor(x3);
					x2 = ((CANI_LONG)(floor(x3))) - 1;


					if (x2 >= 0) {
						c4[0] = src_adress[y * src_cx + x2].Red;
					}
					else {
						c4[0] = src_adress[y * src_cx + x2 + 1].Red;
					}

					c4[1] = src_adress[
						y * src_cx + x2 + 1].Red;

					if ((x2 + 2) < src_cx) {
						c4[2] = src_adress[y * src_cx + x2 + 2].Red;
					}
					else {
						c4[2] = src_adress[y * src_cx + x2 + 1].Red;
					}

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * x5 * x5) + (b * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Red = (CANI_WORD)wynik;


					if (x2 >= 0) c4[0] = src_adress[y * src_cx + x2].Green;
					else c4[0] = src_adress[y * src_cx + x2 + 1].Green;

					c4[1] = src_adress[y * src_cx + x2 + 1].Green;

					if ((x2 + 2) < src_cx) c4[2] = src_adress[y * src_cx + x2 + 2].Green;
					else c4[2] = src_adress[y * src_cx + x2 + 1].Green;

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * x5 * x5) + (b * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Green = (CANI_WORD)wynik;


					if (x2 >= 0) c4[0] = src_adress[y * src_cx + x2].Blue;

					else c4[0] = src_adress[y * src_cx + x2 + 1].Blue;

					c4[1] = src_adress[y * src_cx + x2 + 1].Blue;

					if ((x2 + 2) < src_cx) c4[2] = src_adress[y * src_cx + x2 + 2].Blue;

					else c4[2] = src_adress[y * src_cx + x2 + 1].Blue;

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * x5 * x5) + (b * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Blue = (CANI_WORD)wynik;


					if (x2 >= 0) c4[0] = src_adress[y * src_cx + x2].Alpha;

					else c4[0] = src_adress[y * src_cx + x2 + 1].Alpha;

					c4[1] = src_adress[y * src_cx + x2 + 1].Alpha;

					if ((x2 + 2) < src_cx) c4[2] = src_adress[y * src_cx + x2 + 2].Alpha;

					else c4[2] = src_adress[y * src_cx + x2 + 1].Alpha;

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * x5 * x5) + (b * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Alpha = (CANI_WORD)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}

		//CANI_LONG y6;

		if (dest_cy != src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BGRA16QUAD* dest = &dest_adress[y * dest_cx + x];


					y3 = ((CANI_DOUBLE)y) * ry;
					y5 = 1.0f + y3 - floor(y3);
					y2 = ((CANI_LONG)(floor(y3))) - 1;

					//y6 = y2;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Red;
					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Red;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Red;

					if ((y2 + 2) < src_cy) c4[2] = buffor[(y2 + 2) * dest_cx + x].Red;
					else c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Red;

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * y5 * y5) + (b * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Red = (CANI_WORD)wynik;



					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Green;

					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Green;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Green;

					if ((y2 + 2) < src_cy) c4[2] = buffor[(y2 + 2) * dest_cx + x].Green;
					else c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Green;

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * y5 * y5) + (b * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Green = (CANI_WORD)wynik;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Blue;

					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Blue;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Blue;

					if ((y2 + 2) < src_cy) c4[2] = buffor[(y2 + 2) * dest_cx + x].Blue;
					else c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Blue;

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * y5 * y5) + (b * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Blue = (CANI_WORD)wynik;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Alpha;

					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Alpha;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Alpha;

					if ((y2 + 2) < src_cy) c4[2] = buffor[(y2 + 2) * dest_cx + x].Alpha;
					else c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Alpha;

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * y5 * y5) + (b * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Alpha = (CANI_WORD)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}

		CANI_FREE(buffor);//delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_EXPORT CANI_CHAR CANI_CubicInterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BGRA16QUAD* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BGRA16QUAD*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));//new CANI_BGRA16QUAD[src_cy * dest_cx];

	if (buffor) {
		CANI_LONG x, y, x2, y2;
		CANI_DOUBLE wynik, x3, x5, a, b, c, y3, y5;
		CANI_DOUBLE c4[4];

		CANI_DOUBLE rx, ry;
		if (src_cx > 1 && dest_cx > 1) {
			rx = ((CANI_DOUBLE)src_cx - 1.0f) / ((CANI_DOUBLE)dest_cx - 1.0f);
		}
		else {
			rx = ((CANI_DOUBLE)src_cx) / ((CANI_DOUBLE)dest_cx);
		}

		if (src_cy > 1 && dest_cy > 1) {
			ry = ((CANI_DOUBLE)src_cy - 1.0f) / ((CANI_DOUBLE)dest_cy - 1.0f);
		}
		else {
			ry = ((CANI_DOUBLE)src_cy) / ((CANI_DOUBLE)dest_cy);
		}

		if (dest_cx != src_cx) {
			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BGRA16QUAD* dest = &buffor[y * dest_cx + x];

					x3 = ((CANI_DOUBLE)x) * rx;
					x5 = 1.0f + x3 - floor(x3);
					x2 = ((CANI_LONG)(floor(x3))) - 1;


					if (x2 >= 0) {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Red;
					}
					else {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;
					}

					c4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;

					if ((x2 + 2) < src_cx) {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Red;
					}
					else {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;
					}

					if ((x2 + 3) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Red;
					}
					else if ((x2 + 2) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Red;
					}
					else {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * x5 * x5 * x5) + (b * x5 * x5) + (c * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Red = (CANI_WORD)wynik;


					if (x2 >= 0) {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Green;
					}
					else {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;
					}

					c4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;

					if ((x2 + 2) < src_cx) {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Green;
					}
					else {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;
					}

					if ((x2 + 3) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Green;
					}
					else if ((x2 + 2) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Green;
					}
					else {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * x5 * x5 * x5) + (b * x5 * x5) + (c * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Green = (CANI_WORD)wynik;



					if (x2 >= 0) {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Blue;
					}
					else {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;
					}

					c4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;

					if ((x2 + 2) < src_cx) {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Blue;
					}
					else {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;
					}

					if ((x2 + 3) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Blue;
					}
					else if ((x2 + 2) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Blue;
					}
					else {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * x5 * x5 * x5) + (b * x5 * x5) + (c * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Blue = (CANI_WORD)wynik;



					if (x2 >= 0) {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Alpha;
					}
					else {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;
					}

					c4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;

					if ((x2 + 2) < src_cx) {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Alpha;
					}
					else {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;
					}

					if ((x2 + 3) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Alpha;
					}
					else if ((x2 + 2) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Alpha;
					}
					else {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * x5 * x5 * x5) + (b * x5 * x5) + (c * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Alpha = (CANI_WORD)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}

		//CANI_LONG y6;

		if (dest_cy != src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BGRA16QUAD* dest = &dest_adress[y * dest_cx + x];


					y3 = ((CANI_DOUBLE)y) * ry;
					y5 = 1.0f + y3 - floor(y3);
					y2 = ((CANI_LONG)(floor(y3))) - 1;

					//y6 = y2;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Red;
					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Red;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Red;

					if ((y2 + 2) < src_cy) {
						c4[2] = buffor[(y2 + 2) * dest_cx + x].Red;
					}
					else {
						c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Red;
					}

					if ((y2 + 3) < src_cy) {
						c4[3] = buffor[(y2 + 3) * dest_cx + x].Red;
					}
					else if ((y2 + 2) < src_cy) {
						c4[3] = buffor[(y2 + 2) * dest_cx + x].Red;
					}
					else {
						c4[3] = buffor[(y2 + 1) * dest_cx + x].Red;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * y5 * y5 * y5) + (b * y5 * y5) + (c * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Red = (CANI_WORD)wynik;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Green;
					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Green;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Green;

					if ((y2 + 2) < src_cy) {
						c4[2] = buffor[(y2 + 2) * dest_cx + x].Green;
					}
					else {
						c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Green;
					}

					if ((y2 + 3) < src_cy) {
						c4[3] = buffor[(y2 + 3) * dest_cx + x].Green;
					}
					else if ((y2 + 2) < src_cy) {
						c4[3] = buffor[(y2 + 2) * dest_cx + x].Green;
					}
					else {
						c4[3] = buffor[(y2 + 1) * dest_cx + x].Green;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * y5 * y5 * y5) + (b * y5 * y5) + (c * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Green = (CANI_WORD)wynik;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Blue;
					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Blue;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Blue;

					if ((y2 + 2) < src_cy) {
						c4[2] = buffor[(y2 + 2) * dest_cx + x].Blue;
					}
					else {
						c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Blue;
					}

					if ((y2 + 3) < src_cy) {
						c4[3] = buffor[(y2 + 3) * dest_cx + x].Blue;
					}
					else if ((y2 + 2) < src_cy) {
						c4[3] = buffor[(y2 + 2) * dest_cx + x].Blue;
					}
					else {
						c4[3] = buffor[(y2 + 1) * dest_cx + x].Blue;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * y5 * y5 * y5) + (b * y5 * y5) + (c * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Blue = (CANI_WORD)wynik;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x].Alpha;
					else c4[0] = buffor[(y2 + 1) * dest_cx + x].Alpha;

					c4[1] = buffor[(y2 + 1) * dest_cx + x].Alpha;

					if ((y2 + 2) < src_cy) {
						c4[2] = buffor[(y2 + 2) * dest_cx + x].Alpha;
					}
					else {
						c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x].Alpha;
					}

					if ((y2 + 3) < src_cy) {
						c4[3] = buffor[(y2 + 3) * dest_cx + x].Alpha;
					}
					else if ((y2 + 2) < src_cy) {
						c4[3] = buffor[(y2 + 2) * dest_cx + x].Alpha;
					}
					else {
						c4[3] = buffor[(y2 + 1) * dest_cx + x].Alpha;
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * y5 * y5 * y5) + (b * y5 * y5) + (c * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Alpha = (CANI_WORD)wynik;

				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}

		CANI_FREE(buffor);// delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_DOUBLE CANI_LagrangeInterpolation16(
	CANI_DOUBLE* xs, CANI_DOUBLE* ys, CANI_LONG length, CANI_DOUBLE x) {
	// length = iloœæ danych x i y
	// x = wartoœæ dla której jest obliczane y
	CANI_DOUBLE t;
	CANI_DOUBLE y = 0.0f;
	CANI_LONG k, j;

	for (k = 0; k < length; k++) {
		t = 1.0f;
		for (j = 0; j < length; j++) {
			if (j != k) {
				t = (t * ((x - xs[j]) / (xs[k] - xs[j])));
				//t = ( t * ( ( x - ((CANI_DOUBLE)j) ) 
				//	/ ( ((CANI_DOUBLE)k) - ((CANI_DOUBLE)j) ) ) );
			}
		}
		y += (t * ys[k]);
	}
	return y;
}



CANI_EXPORT CANI_CHAR CANI_LagrangeInterpolationResize16(CANI_BGRA16QUAD* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BGRA16QUAD* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BGRA16QUAD* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BGRA16QUAD*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));//new CANI_BGRA16QUAD[src_cy * dest_cx];

	if (buffor) {
		//MessageBox( 0, "InterpolacjaLagrange", "", 0 );
		CANI_LONG x, y, x2, y2;
		CANI_DOUBLE wynik, x3, x5, y3, y5;
		CANI_DOUBLE x4[4], y4[4];

		CANI_DOUBLE rx, ry;
		if (src_cx > 1 && dest_cx > 1) {
			rx = ((CANI_DOUBLE)src_cx - 1.0f) / ((CANI_DOUBLE)dest_cx - 1.0f);
		}
		else {
			rx = ((CANI_DOUBLE)src_cx) / ((CANI_DOUBLE)dest_cx);
		}

		if (src_cy > 1 && dest_cy > 1) {
			ry = ((CANI_DOUBLE)src_cy - 1.0f) / ((CANI_DOUBLE)dest_cy - 1.0f);
		}
		else {
			ry = ((CANI_DOUBLE)src_cy) / ((CANI_DOUBLE)dest_cy);
		}

		if (dest_cx != src_cx) {
			x4[0] = 0.0f;
			x4[1] = 1.0f;
			x4[2] = 2.0f;
			x4[3] = 3.0f;

			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BGRA16QUAD* dest = &buffor[y * dest_cx + x];

					x3 = ((CANI_DOUBLE)x) * rx;
					x5 = 1.0f + x3 - floor(x3);
					x2 = ((CANI_LONG)(floor(x3))) - 1;


					if (x2 >= 0) {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Red;
					}
					else {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;
					}

					y4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;

					if ((x2 + 2) < src_cx) {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Red;
					}
					else {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;
					}

					if ((x2 + 3) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Red;
					}
					else if ((x2 + 2) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Red;
					}
					else {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Red;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, x5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Red = (CANI_WORD)wynik;


					if (x2 >= 0) {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Green;
					}
					else {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;
					}

					y4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;

					if ((x2 + 2) < src_cx) {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Green;
					}
					else {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;
					}

					if ((x2 + 3) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Green;
					}
					else if ((x2 + 2) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Green;
					}
					else {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Green;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, x5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Green = (CANI_WORD)wynik;



					if (x2 >= 0) {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Blue;
					}
					else {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;
					}

					y4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;

					if ((x2 + 2) < src_cx) {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Blue;
					}
					else {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;
					}

					if ((x2 + 3) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Blue;
					}
					else if ((x2 + 2) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Blue;
					}
					else {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Blue;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, x5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Blue = (CANI_WORD)wynik;



					if (x2 >= 0) {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2].Alpha;
					}
					else {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;
					}

					y4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;

					if ((x2 + 2) < src_cx) {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Alpha;
					}
					else {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;
					}

					if ((x2 + 3) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3].Alpha;
					}
					else if ((x2 + 2) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2].Alpha;
					}
					else {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1].Alpha;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, x5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Alpha = (CANI_WORD)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}

		//CANI_LONG y6;

		if (dest_cy != src_cy) {
			x4[0] = 0.0f;
			x4[1] = 1.0f;
			x4[2] = 2.0f;
			x4[3] = 3.0f;

			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BGRA16QUAD* dest = &dest_adress[y * dest_cx + x];


					y3 = ((CANI_DOUBLE)y) * ry;
					y5 = 1.0f + y3 - floor(y3);
					y2 = ((CANI_LONG)(floor(y3))) - 1;

					//y6 = y2;


					if (y2 >= 0) y4[0] = buffor[y2 * dest_cx + x].Red;
					else y4[0] = buffor[(y2 + 1) * dest_cx + x].Red;

					y4[1] = buffor[(y2 + 1) * dest_cx + x].Red;

					if ((y2 + 2) < src_cy) {
						y4[2] = buffor[(y2 + 2) * dest_cx + x].Red;
					}
					else {
						y4[2] = y4[2] = buffor[(y2 + 1) * dest_cx + x].Red;
					}

					if ((y2 + 3) < src_cy) {
						y4[3] = buffor[(y2 + 3) * dest_cx + x].Red;
					}
					else if ((y2 + 2) < src_cy) {
						y4[3] = buffor[(y2 + 2) * dest_cx + x].Red;
					}
					else {
						y4[3] = buffor[(y2 + 1) * dest_cx + x].Red;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, y5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Red = (CANI_WORD)wynik;


					if (y2 >= 0) y4[0] = buffor[y2 * dest_cx + x].Green;
					else y4[0] = buffor[(y2 + 1) * dest_cx + x].Green;

					y4[1] = buffor[(y2 + 1) * dest_cx + x].Green;

					if ((y2 + 2) < src_cy) {
						y4[2] = buffor[(y2 + 2) * dest_cx + x].Green;
					}
					else {
						y4[2] = y4[2] = buffor[(y2 + 1) * dest_cx + x].Green;
					}

					if ((y2 + 3) < src_cy) {
						y4[3] = buffor[(y2 + 3) * dest_cx + x].Green;
					}
					else if ((y2 + 2) < src_cy) {
						y4[3] = buffor[(y2 + 2) * dest_cx + x].Green;
					}
					else {
						y4[3] = buffor[(y2 + 1) * dest_cx + x].Green;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, y5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Green = (CANI_WORD)wynik;


					if (y2 >= 0) y4[0] = buffor[y2 * dest_cx + x].Blue;
					else y4[0] = buffor[(y2 + 1) * dest_cx + x].Blue;

					y4[1] = buffor[(y2 + 1) * dest_cx + x].Blue;

					if ((y2 + 2) < src_cy) {
						y4[2] = buffor[(y2 + 2) * dest_cx + x].Blue;
					}
					else {
						y4[2] = y4[2] = buffor[(y2 + 1) * dest_cx + x].Blue;
					}

					if ((y2 + 3) < src_cy) {
						y4[3] = buffor[(y2 + 3) * dest_cx + x].Blue;
					}
					else if ((y2 + 2) < src_cy) {
						y4[3] = buffor[(y2 + 2) * dest_cx + x].Blue;
					}
					else {
						y4[3] = buffor[(y2 + 1) * dest_cx + x].Blue;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, y5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Blue = (CANI_WORD)wynik;


					if (y2 >= 0) y4[0] = buffor[y2 * dest_cx + x].Alpha;
					else y4[0] = buffor[(y2 + 1) * dest_cx + x].Alpha;

					y4[1] = buffor[(y2 + 1) * dest_cx + x].Alpha;

					if ((y2 + 2) < src_cy) {
						y4[2] = buffor[(y2 + 2) * dest_cx + x].Alpha;
					}
					else {
						y4[2] = y4[2] = buffor[(y2 + 1) * dest_cx + x].Alpha;
					}

					if ((y2 + 3) < src_cy) {
						y4[3] = buffor[(y2 + 3) * dest_cx + x].Alpha;
					}
					else if ((y2 + 2) < src_cy) {
						y4[3] = buffor[(y2 + 2) * dest_cx + x].Alpha;
					}
					else {
						y4[3] = buffor[(y2 + 1) * dest_cx + x].Alpha;
					}

					wynik = CANI_LagrangeInterpolation16(x4, y4, 4, y5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFFFF) wynik = 0xFFFF;

					dest->Alpha = (CANI_WORD)wynik;

				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BGRA16QUAD));
		}

		CANI_FREE(buffor);// delete[] buffor;

		return TRUE;
	}

	return FALSE;
}