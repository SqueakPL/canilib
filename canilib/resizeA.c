#pragma once

/////////////////////////
//                     //
//  Visit cani.com.pl  //
//                     //
/////////////////////////

#include "cani.h"

#include <math.h>

CANI_EXPORT CANI_CHAR CANI_FastResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BYTE* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	if (src_cx == dest_cx && src_cy == dest_cy) {

		CANI_MEMCPY(dest_adress, src_adress,
			src_cx * src_cy * sizeof(CANI_BYTE));
		return TRUE;
	}

	buffor = (CANI_BYTE*)CANI_MALLOC((src_cy * dest_cx) * sizeof(CANI_BYTE));//new CANI_BYTE[src_cy * dest_cx];

	if (buffor) {
		CANI_LONG x, y, x2, y2;

		if (dest_cx > src_cx) {
			CANI_BYTE* dest_now = buffor;

			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BYTE* src;
					CANI_BYTE* dest = dest_now;//&buffor[ y * dest_cx + x ];

					x2 = x * src_cx / dest_cx;
					if (x2 >= src_cx) x2 = src_cx - 1;

					src = //&src_adress[ c[0] ];
						&src_adress[y * src_cx + x2];

					CANI_MEMCPY(dest, src, sizeof(CANI_BYTE));
					//*((CANI_DWORD*)dest) = *((CANI_DWORD*)src);

					dest_now++;
				}
			}
		}
		else if (dest_cx < src_cx) {
			CANI_BYTE* dest_now = buffor;

			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BYTE* src;
					CANI_BYTE* dest = dest_now;//&buffor[ y * dest_cx + x ];

					x2 = x * src_cx / dest_cx;
					if (x2 >= src_cx)
						x2 = src_cx - 1;

					src = &src_adress[y * src_cx + x2];

					CANI_MEMCPY(dest, src, sizeof(CANI_BYTE));
					//*((CANI_DWORD*)dest) = *((CANI_DWORD*)src);

					dest_now++;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress,
				src_cy * dest_cx * sizeof(CANI_BYTE));
		}


		if (dest_cy > src_cy) {

			CANI_BYTE* dest_now = dest_adress;//[ y * dest_cx + x ];

			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BYTE* src;
					//CANI_BYTE *dest = dest_now;//&dest_adress[ y * dest_cx + x ];

					y2 = y * src_cy / dest_cy;
					if (y2 >= src_cy) y2 = src_cy - 1;

					src = &buffor[y2 * dest_cx + x];

					//CANI_MEMCPY( dest, src, sizeof(CANI_BYTE) );

					//*((CANI_DWORD*)dest_now) = *((CANI_DWORD*)src);
					CANI_MEMCPY(dest_now, src, sizeof(CANI_BYTE));

					dest_now++;
				}
			}
		}
		else if (dest_cy < src_cy) {

			CANI_BYTE* dest_now = dest_adress;//[ y * dest_cx + x ];

			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BYTE* src;
					//CANI_BYTE *dest = dest_now;//&dest_adress[ y * dest_cx + x ];

					y2 = y * src_cy / dest_cy;
					if (y2 >= src_cy) y2 = src_cy - 1;

					src = &buffor[y2 * dest_cx + x];

					//CANI_MEMCPY( dest, src, sizeof(CANI_BYTE) );

					//*((CANI_DWORD*)dest_now) = *((CANI_DWORD*)src);
					CANI_MEMCPY(dest_now, src, sizeof(CANI_BYTE));

					dest_now++;
				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BYTE));
		}
		CANI_FREE(buffor);//delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_EXPORT CANI_CHAR CANI_InterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BYTE* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BYTE*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BYTE));//new CANI_BYTE[src_cy * dest_cx];

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

					CANI_BYTE* dest = &buffor[y * dest_cx + x];

					wynik = ((CANI_DOUBLE)x) * rx;

					zaokraglenie = floor(wynik);
					reszta = wynik - zaokraglenie;

					if (reszta > 0 && wynik >= 0 && (zaokraglenie < src_cx)) {

						CANI_DOUBLE wartosc_a = src_adress[y * src_cx + (CANI_LONG)zaokraglenie];

						CANI_DOUBLE wartosc_b = src_adress[y * src_cx + ((CANI_LONG)zaokraglenie + 1)];

						CANI_DOUBLE wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						buffor[y * dest_cx + x] = (CANI_BYTE)wartosc;

					}
					else {

						CANI_BYTE* src;

						x2 = x * src_cx / dest_cx;
						if (x2 >= src_cx) x2 = src_cx - 1;

						if (x2 >= src_cx) x2 = src_cx - 1;

						src = &src_adress[y * src_cx + x2];

						CANI_MEMCPY(dest, src, sizeof(CANI_BYTE));

					}
				}
			}
		}
		else if (dest_cx < src_cx) {
			for (y = 0; y < src_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_BYTE* src;
					CANI_BYTE* dest = &buffor[y * dest_cx + x];

					x2 = x * src_cx / dest_cx;
					if (x2 >= src_cx) x2 = src_cx - 1;

					src = &src_adress[y * src_cx + x2];

					CANI_MEMCPY(dest, src, sizeof(CANI_BYTE));
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BYTE));
		}


		if (dest_cy > src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {

					CANI_DOUBLE zaokraglenie;
					CANI_DOUBLE reszta;

					CANI_BYTE* dest = &dest_adress[y * dest_cx + x];

					wynik = ((CANI_DOUBLE)y) * ry;

					zaokraglenie = floor(wynik);
					reszta = wynik - zaokraglenie;

					if (reszta > 0 && wynik >= 0 && (zaokraglenie < src_cx)) {

						CANI_DOUBLE wartosc_a = buffor[((CANI_LONG)zaokraglenie + 0) * dest_cx + x];

						CANI_DOUBLE wartosc_b = buffor[((CANI_LONG)zaokraglenie + 1) * dest_cx + x];

						CANI_DOUBLE wartosc = (1 - reszta) * wartosc_a + (reszta)*wartosc_b;

						dest_adress[y * dest_cx + x] = (CANI_BYTE)wartosc;

					}
					else {
						CANI_BYTE* src;

						y2 = y * src_cy / dest_cy;
						if (y2 >= src_cy) y2 = src_cy - 1;

						src = &buffor[y2 * dest_cx + x];

						CANI_MEMCPY(dest, src, sizeof(CANI_BYTE));
					}
				}
			}
		}
		else if (dest_cy < src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BYTE* src;
					CANI_BYTE* dest = &dest_adress[y * dest_cx + x];

					y2 = y * src_cy / dest_cy;
					if (y2 >= src_cy) y2 = src_cy - 1;

					src = &buffor[y2 * dest_cx + x];

					CANI_MEMCPY(dest, src, sizeof(CANI_BYTE));
				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BYTE));
		}

		CANI_FREE(buffor);// delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_EXPORT CANI_CHAR CANI_SquareInterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BYTE* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BYTE*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BYTE));//new CANI_BYTE[src_cy * dest_cx];

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

					CANI_BYTE* dest = &buffor[y * dest_cx + x];

					x3 = ((CANI_DOUBLE)x) * rx;
					x5 = 1.0f + x3 - floor(x3);
					x2 = ((CANI_LONG)(floor(x3))) - 1;


					if (x2 >= 0) {
						c4[0] = src_adress[y * src_cx + x2];
					}
					else {
						c4[0] = src_adress[y * src_cx + x2 + 1];
					}

					c4[1] = src_adress[
						y * src_cx + x2 + 1];

					if ((x2 + 2) < src_cx) {
						c4[2] = src_adress[y * src_cx + x2 + 2];
					}
					else {
						c4[2] = src_adress[y * src_cx + x2 + 1];
					}

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * x5 * x5) + (b * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFF) wynik = 0xFF;

					*dest = (CANI_BYTE)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BYTE));
		}

		//CANI_LONG y6;

		if (dest_cy != src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BYTE* dest = &dest_adress[y * dest_cx + x];


					y3 = ((CANI_DOUBLE)y) * ry;
					y5 = 1.0f + y3 - floor(y3);
					y2 = ((CANI_LONG)(floor(y3))) - 1;

					//y6 = y2;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x];
					else c4[0] = buffor[(y2 + 1) * dest_cx + x];

					c4[1] = buffor[(y2 + 1) * dest_cx + x];

					if ((y2 + 2) < src_cy) c4[2] = buffor[(y2 + 2) * dest_cx + x];
					else c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x];

					b = (2.0f * ((CANI_DOUBLE)(c4[1] - c4[0]))) - (0.5f * ((CANI_DOUBLE)(c4[2] - c4[0])));

					a = ((CANI_DOUBLE)(c4[1] - c4[0])) - b;

					wynik = (a * y5 * y5) + (b * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFF) wynik = 0xFF;

					*dest = (CANI_BYTE)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BYTE));
		}

		CANI_FREE(buffor);//delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_EXPORT CANI_CHAR CANI_CubicInterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BYTE* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BYTE*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BYTE));//new CANI_BYTE[src_cy * dest_cx];

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

					CANI_BYTE* dest = &buffor[y * dest_cx + x];

					x3 = ((CANI_DOUBLE)x) * rx;
					x5 = 1.0f + x3 - floor(x3);
					x2 = ((CANI_LONG)(floor(x3))) - 1;


					if (x2 >= 0) {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2];
					}
					else {
						c4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];
					}

					c4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];

					if ((x2 + 2) < src_cx) {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2];
					}
					else {
						c4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];
					}

					if ((x2 + 3) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3];
					}
					else if ((x2 + 2) < src_cx) {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2];
					}
					else {
						c4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * x5 * x5 * x5) + (b * x5 * x5) + (c * x5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFF) wynik = 0xFF;

					*dest = (CANI_BYTE)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BYTE));
		}

		//CANI_LONG y6;

		if (dest_cy != src_cy) {
			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BYTE* dest = &dest_adress[y * dest_cx + x];


					y3 = ((CANI_DOUBLE)y) * ry;
					y5 = 1.0f + y3 - floor(y3);
					y2 = ((CANI_LONG)(floor(y3))) - 1;

					//y6 = y2;


					if (y2 >= 0) c4[0] = buffor[y2 * dest_cx + x];
					else c4[0] = buffor[(y2 + 1) * dest_cx + x];

					c4[1] = buffor[(y2 + 1) * dest_cx + x];

					if ((y2 + 2) < src_cy) {
						c4[2] = buffor[(y2 + 2) * dest_cx + x];
					}
					else {
						c4[2] = c4[2] = buffor[(y2 + 1) * dest_cx + x];
					}

					if ((y2 + 3) < src_cy) {
						c4[3] = buffor[(y2 + 3) * dest_cx + x];
					}
					else if ((y2 + 2) < src_cy) {
						c4[3] = buffor[(y2 + 2) * dest_cx + x];
					}
					else {
						c4[3] = buffor[(y2 + 1) * dest_cx + x];
					}

					c = ((3.0f) * (CANI_DOUBLE)(c4[1] - c4[0])) + ((1.0f / 3.0f) * (CANI_DOUBLE)(c4[3] - c4[0])) - ((3.0f / 2.0f) * (CANI_DOUBLE)(c4[2] - c4[0]));

					b = (2.0f * (CANI_DOUBLE)(c4[1] - c4[0])) - ((0.25f) * (CANI_DOUBLE)(c4[2] - c4[0])) - (3.0f / 2.0f * c);

					a = (CANI_DOUBLE)c4[1] - (CANI_DOUBLE)c4[0] - b - c;

					wynik = (a * y5 * y5 * y5) + (b * y5 * y5) + (c * y5) + c4[0];

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFF) wynik = 0xFF;

					*dest = (CANI_BYTE)wynik;

				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BYTE));
		}

		CANI_FREE(buffor);// delete[] buffor;

		return TRUE;
	}

	return FALSE;
}


CANI_DOUBLE CANI_LagrangeInterpolationAlpha(
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



CANI_EXPORT CANI_CHAR CANI_LagrangeInterpolationResizeAlpha(CANI_BYTE* src_adress, CANI_LONG src_cx, CANI_LONG src_cy, CANI_BYTE* dest_adress, CANI_LONG dest_cx, CANI_LONG dest_cy) {

	CANI_BYTE* buffor;

	if (src_cx < 1 || src_cy < 1 || dest_cx < 1 || dest_cy < 1) return FALSE;

	buffor = (CANI_BYTE*)CANI_MALLOC(src_cy * dest_cx * sizeof(CANI_BYTE));//new CANI_BYTE[src_cy * dest_cx];

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

					CANI_BYTE* dest = &buffor[y * dest_cx + x];

					x3 = ((CANI_DOUBLE)x) * rx;
					x5 = 1.0f + x3 - floor(x3);
					x2 = ((CANI_LONG)(floor(x3))) - 1;


					if (x2 >= 0) {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2];
					}
					else {
						y4[0] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];
					}

					y4[1] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];

					if ((x2 + 2) < src_cx) {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2];
					}
					else {
						y4[2] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];
					}

					if ((x2 + 3) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 3];
					}
					else if ((x2 + 2) < src_cx) {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 2];
					}
					else {
						y4[3] = (CANI_DOUBLE)src_adress[y * src_cx + x2 + 1];
					}

					wynik = CANI_LagrangeInterpolationAlpha(x4, y4, 4, x5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFF) wynik = 0xFF;

					*dest = (CANI_BYTE)wynik;
				}
			}
		}
		else {
			CANI_MEMCPY(buffor, src_adress, src_cy * dest_cx * sizeof(CANI_BYTE));
		}

		//CANI_LONG y6;

		if (dest_cy != src_cy) {
			x4[0] = 0.0f;
			x4[1] = 1.0f;
			x4[2] = 2.0f;
			x4[3] = 3.0f;

			for (y = 0; y < dest_cy; y++) {
				for (x = 0; x < dest_cx; x++) {
					CANI_BYTE* dest = &dest_adress[y * dest_cx + x];


					y3 = ((CANI_DOUBLE)y) * ry;
					y5 = 1.0f + y3 - floor(y3);
					y2 = ((CANI_LONG)(floor(y3))) - 1;

					//y6 = y2;


					if (y2 >= 0) y4[0] = buffor[y2 * dest_cx + x];
					else y4[0] = buffor[(y2 + 1) * dest_cx + x];

					y4[1] = buffor[(y2 + 1) * dest_cx + x];

					if ((y2 + 2) < src_cy) {
						y4[2] = buffor[(y2 + 2) * dest_cx + x];
					}
					else {
						y4[2] = y4[2] = buffor[(y2 + 1) * dest_cx + x];
					}

					if ((y2 + 3) < src_cy) {
						y4[3] = buffor[(y2 + 3) * dest_cx + x];
					}
					else if ((y2 + 2) < src_cy) {
						y4[3] = buffor[(y2 + 2) * dest_cx + x];
					}
					else {
						y4[3] = buffor[(y2 + 1) * dest_cx + x];
					}

					wynik = CANI_LagrangeInterpolationAlpha(x4, y4, 4, y5);

					if (wynik < 0) wynik = 0;
					else if (wynik > 0xFF) wynik = 0xFF;

					*dest = (CANI_BYTE)wynik;

				}
			}
		}
		else {
			CANI_MEMCPY(dest_adress, buffor, dest_cy * dest_cx * sizeof(CANI_BYTE));
		}

		CANI_FREE(buffor);// delete[] buffor;

		return TRUE;
	}

	return FALSE;
}