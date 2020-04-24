#include "cani.h"

#define PNG_BYTES_TO_CHECK 4

#ifdef CANI_USE_LIBAPNG

CANI_CHAR readSetup(FILE* image, png_structp* png_ptr_read, png_infop* info_ptr_read)
{
	int rc;
	png_byte pngSig[8];

	rc = (int)fread(pngSig, 1, 8, image);
	if (rc != 8) return FALSE;

	rc = png_check_sig(pngSig, 8);
	if (rc == 0) return FALSE;

	*png_ptr_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (*png_ptr_read == NULL) return FALSE;

	*info_ptr_read = png_create_info_struct(*png_ptr_read);
	if (*info_ptr_read == NULL) return FALSE;

	png_init_io(*png_ptr_read, image);
	png_set_sig_bytes(*png_ptr_read, 8);

	return TRUE;
}

typedef struct _CHUNK { unsigned char* p; unsigned int size; } CHUNK;

typedef struct _APNGFrame { 
	unsigned char* p;
	unsigned char **rows;
	unsigned int w, h, delay_num, delay_den;
} APNGFrame ;

CANI_ANIMATION* CANI_ReadAPNG_FILE(FILE* file, CANI_PROGRESS_FUNC Progress ) {
	if (file) {

		png_structp png_ptr_read;
		png_infop info_ptr_read;
		int count;
		png_uint_32 width;
		png_uint_32 height;
		png_size_t rowbytes;
		png_byte color_type;
		png_byte bit_depth;
		uint32_t first;
		png_uint_32 x, y;
		png_int_32 frames;
		CANI_BYTE ProgressNow = 0;

		//png_structp png_ptr_write;
		//png_infop info_ptr_write;
		//png_bytepp rowPointers;
		//png_uint_32 bytesPerRow;


		if (!readSetup(file, &png_ptr_read, &info_ptr_read)) return 0;

		if (setjmp(png_jmpbuf(png_ptr_read))) return 0;

		png_read_info(png_ptr_read, info_ptr_read);

		if (!png_get_valid(png_ptr_read, info_ptr_read, PNG_INFO_acTL)) return 0;

		width = png_get_image_width(png_ptr_read, info_ptr_read);

		height = png_get_image_height(png_ptr_read, info_ptr_read);

		rowbytes = png_get_rowbytes(png_ptr_read, info_ptr_read);

		color_type = png_get_color_type(png_ptr_read, info_ptr_read);

		bit_depth = png_get_bit_depth(png_ptr_read, info_ptr_read);

		/*rowPointers = png_malloc(png_ptr_read, sizeof(png_bytep) * height);
		bytesPerRow = png_get_rowbytes(png_ptr_read, info_ptr_read);

		for (count = 0; count < info_ptr_read->height; count++)
			rowPointers[count] = png_malloc(png_ptr_read, bytesPerRow);*/

		first = (png_get_first_frame_is_hidden(png_ptr_read, info_ptr_read) != 0) ? 1 : 0;

		if (first) {

			png_bytep* row_pointers;

			row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
			for (y = 0; y < height; y++) {
				row_pointers[y] = (png_byte*)malloc(rowbytes);

			}
			//int dlaY;
			png_read_image(png_ptr_read, row_pointers);

			for (y = 0; y < height; y++) CANI_FREE(row_pointers[y]);
			CANI_FREE(row_pointers);

			//MessageBoxA(0, "png_get_first_frame_is_hidden", "", 0);
		}

		frames = (png_int_32)png_get_num_frames(png_ptr_read, info_ptr_read) - first;

		//char szCaniBuffer[1024];

		//sprintf(szCaniBuffer, "%u", frames);

		//MessageBoxA(0, szCaniBuffer, "", 0);

		if (frames > 0) {

			png_uint_32 nf_width;
			png_uint_32 nf_height;
			png_uint_32 nf_x_offset;
			png_uint_32 nf_y_offset;
			png_uint_16 nf_delay_num;
			png_uint_16 nf_delay_den;
			png_byte nf_dispose_op = 0;
			png_byte nf_blend_op = 0;
			int imagesize;
			CANI_DWORD Size;
			png_bytep* row_pointers;

			CANI_ANIMATION* rv = CANI_MALLOC(sizeof(CANI_ANIMATION));

			if (!rv) return 0;

			CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));

			rv->Width = width;
			rv->Height = height;
			rv->ImagesNR = frames;

			rv->Format.format = CAF_FORMAT_RGB;
			rv->Format.rgb.Red = 8;
			rv->Format.rgb.Green = 8;
			rv->Format.rgb.Blue = 8;
			rv->Format.rgb.Alpha = 8;

			imagesize = width * height * 4;

			rv->Images = CANI_MALLOC(frames*sizeof(CANI_FRAME_DATA));

			if (!rv->Images) {
				png_destroy_read_struct(&png_ptr_read, &info_ptr_read, NULL);
				CANI_FREE(rv);
				return 0;
			}

			CANI_MEMSET(rv->Images, 0x00, frames * sizeof(CANI_FRAME_DATA));

			Size = width * height;

			row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
			for (y = 0; y < height; y++) {
				row_pointers[y] = (png_byte*)malloc(rowbytes);

			}

			for (count = 0; count < frames; count++)
			{

				rv->Images[count].Image = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));
				if (rv->Images[count].Image == 0) {
					CANI_DWORD m;
					for (m = count; m > 0; m--) {
						CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
					}
					CANI_FREE(rv->Images);
					CANI_FREE(rv);
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}

				memset(rv->Images[count].Image, 0x00, rv->Width * rv->Height * sizeof(CANI_BGRAQUAD));
			}

			for (count = 0; count < frames; count++)
			{

				CANI_BGRAQUAD* ImageCurrent;

				CANI_BGRAQUAD* ImagePrevious;

				if (Progress) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)(count * 100 / frames);

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				ImageCurrent = ((CANI_BGRAQUAD*)rv->Images[count].Image);

				ImagePrevious = count > 0 ? (CANI_BGRAQUAD*)rv->Images[count - 1].Image : 0;

				png_read_frame_head(png_ptr_read, info_ptr_read);

				//png_size_t rowbytes = png_get_rowbytes(png_ptr_read, info_ptr_read);

				if (png_get_valid(png_ptr_read, info_ptr_read, PNG_INFO_fcTL))
				{

					static one_bit_mask[8] = {
						0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
					};
					static two_bits_mask[4] = {
						0xC0, 0x30, 0x0C, 0x03
					};
					static two_bits_shift[4] = {
						6, 4, 2, 0
					};
					static four_bits_mask[2] = {
						0xF0, 0x0F
					};
					static four_bits_shift[2] = {
						4, 0
					};

					png_color_16 bg_color;
					png_color_16p bg_color_p = 0;
					
					png_get_next_frame_fcTL(png_ptr_read, info_ptr_read,
						&nf_width, &nf_height,
						&nf_x_offset, &nf_y_offset,
						&nf_delay_num, &nf_delay_den,
						&nf_dispose_op, &nf_blend_op);

					png_get_bKGD(png_ptr_read, info_ptr_read, &bg_color_p );

					if (bg_color_p) {

						bg_color.red = bg_color_p->red;
						bg_color.green = bg_color_p->green;
						bg_color.blue = bg_color_p->blue;
						bg_color.gray = bg_color_p->gray;
						bg_color.index = bg_color_p->index;

						if (color_type == PNG_COLOR_TYPE_GRAY) {

							bg_color.red = bg_color.gray;
							bg_color.green = bg_color.gray;
							bg_color.blue = bg_color.gray;
						}
						else if (color_type == PNG_COLOR_TYPE_PALETTE) {
							png_colorp palette;
							int num_palette = 0;
							png_get_PLTE(png_ptr_read, info_ptr_read, &palette, &num_palette);

							bg_color.red = palette[bg_color.index].red;
							bg_color.green = palette[bg_color.index].green;
							bg_color.blue = palette[bg_color.index].blue;
						}
						else {
						}
					}

					/*if (count == 0) {
						CANI_DWORD n;
						for (n = 0; n < Size; n++) {
							ImageCurrent[n].Red = (CANI_BYTE)( bg_color.red );
							ImageCurrent[n].Green = (CANI_BYTE)(bg_color.green);
							ImageCurrent[n].Blue = (CANI_BYTE)(bg_color.blue);
							ImageCurrent[n].Alpha = 0xFF;
						}
						memset(ImageCurrent, 0x00, rv->Width * rv->Height * sizeof(CANI_BGRAQUAD));
					}*/

					if (nf_dispose_op == PNG_DISPOSE_OP_PREVIOUS && (count < (frames - 1))) {
						CANI_MEMCPY(rv->Images[count + 1].Image, rv->Images[count].Image, Size * sizeof(CANI_BGRAQUAD));
					}

					rv->Images[count].Time = nf_delay_num * 1000 / nf_delay_den;

					/*png_bytep* row_pointers;

					row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
					for (png_uint_32 y = 0; y < height; y++) {
						row_pointers[y] = (png_byte*)malloc(rowbytes);

					}*/

					png_read_image(png_ptr_read, row_pointers);

					if (color_type == PNG_COLOR_TYPE_RGB_ALPHA && bit_depth == 8 ) {

						CANI_BYTE pix_byte = 4;
						for (y = 0; y < nf_height; y++) {
							for (x = 0; x < nf_width; x++) {

								CANI_LONG pos = (height - (nf_y_offset + y) - 1) * width + nf_x_offset + x;
								ImageCurrent[pos].Red = row_pointers[y][x * pix_byte];
								ImageCurrent[pos].Green = row_pointers[y][x * pix_byte + 1];
								ImageCurrent[pos].Blue = row_pointers[y][x * pix_byte + 2];
								ImageCurrent[pos].Alpha = row_pointers[y][x * pix_byte + 3];

							}
						}

						rv->Images[count].Style |= CANI_STYLE_ALPHA;

					}
					else if (color_type == PNG_COLOR_TYPE_RGB && bit_depth == 8 ) {

						png_color_16p bgt = 0;
						CANI_BYTE pix_byte = 3;

						for (y = 0; y < nf_height; y++) {
							for (x = 0; x < nf_width; x++) {

								CANI_LONG pos = (height - (nf_y_offset + y) - 1) * width + nf_x_offset + x;
								ImageCurrent[pos].Red = row_pointers[y][x * pix_byte];
								ImageCurrent[pos].Green = row_pointers[y][x * pix_byte + 1];
								ImageCurrent[pos].Blue = row_pointers[y][x * pix_byte + 2];
								ImageCurrent[pos].Alpha = 0xFF;

							}
						}

						if (png_get_tRNS(png_ptr_read, info_ptr_read, 0, 0, &bgt)) {
							if (bgt) {

								rv->Images[count].bgColorRGB.Red = (CANI_BYTE)bgt->red;
								rv->Images[count].bgColorRGB.Green = (CANI_BYTE)bgt->green;
								rv->Images[count].bgColorRGB.Blue = (CANI_BYTE)bgt->blue;

								CANI_RGBA_2_RGBA16(&rv->Images[count].bgColorRGB16, &rv->Images[count].bgColorRGB, 1);

								rv->Images[count].Style |= CANI_STYLE_TRANSPARENT;
							}
						}
					}
					else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth == 8 ) {

						png_color_16p bgt = 0;

						for (y = 0; y < nf_height; y++) {
							for (x = 0; x < nf_width; x++) {
								CANI_BYTE gray = row_pointers[y][x];

								CANI_LONG pos = (height - (nf_y_offset + y) - 1) * width + nf_x_offset + x;

								ImageCurrent[pos].Red = gray;
								ImageCurrent[pos].Green = gray;
								ImageCurrent[pos].Blue = gray;
								ImageCurrent[pos].Alpha = 0xFF;
							}
						}

						if (png_get_tRNS(png_ptr_read, info_ptr_read, 0, 0, &bgt)) {
							if (bgt) {

								rv->Images[count].bgColorRGB.Red = (CANI_BYTE)bgt->gray;
								rv->Images[count].bgColorRGB.Green = (CANI_BYTE)bgt->gray;
								rv->Images[count].bgColorRGB.Blue = (CANI_BYTE)bgt->gray;

								CANI_RGBA_2_RGBA16(&rv->Images[count].bgColorRGB16, &rv->Images[count].bgColorRGB, 1);

								rv->Images[count].Style |= CANI_STYLE_TRANSPARENT;
							}
						}
					}
					else if (color_type == PNG_COLOR_TYPE_PALETTE) {

						png_colorp palette;
						int num_palette = 0;
						png_bytep trans_alpha = NULL;
						int num_trans = 0;
						png_get_PLTE(png_ptr_read, info_ptr_read, &palette, &num_palette);

						rv->Images[count].Style |= CANI_STYLE_ALPHA;

						if (png_get_tRNS(png_ptr_read, info_ptr_read, &trans_alpha, &num_trans, 0)) {

							for (y = 0; y < nf_height; y++) {
								for (x = 0; x < nf_width; x++) {

									CANI_BYTE Alpha;
									CANI_LONG pos;
									CANI_BYTE index = 0;

									if (bit_depth == 8) {
										index = row_pointers[y][x];
									}
									else if (bit_depth == 4) {
										CANI_DWORD shift = x % 2;
										index = (row_pointers[y][x/2] & four_bits_mask[shift]) >> (four_bits_shift[shift]);
									} 
									else if (bit_depth == 2) {
										CANI_DWORD shift = x % 4;
										index = ( row_pointers[y][x/4] & two_bits_mask[shift] ) >> (two_bits_shift[shift]);
									}
									else if (bit_depth == 1) {
										CANI_DWORD shift = x % 8;
										index = row_pointers[y][x/8] & one_bit_mask[shift] ? 1 : 0;
									}

									Alpha = index < num_trans ? trans_alpha[index] : 255;

									pos = (height - (nf_y_offset + y) - 1) * width + nf_x_offset + x;

									if (nf_blend_op == PNG_BLEND_OP_SOURCE) {

										ImageCurrent[pos].Red = palette[index].red;
										ImageCurrent[pos].Green = palette[index].green;
										ImageCurrent[pos].Blue = palette[index].blue;
										ImageCurrent[pos].Alpha = Alpha;
									}
									else {

										CANI_DWORD r, g, b, a;

										CANI_LONG al = 0xFF * 0xFF - (0xFF - (CANI_LONG)(Alpha)) * (0xFF - (CANI_LONG)(ImageCurrent[pos].Alpha));

										if (al) {
											CANI_LONG u = (CANI_LONG)(Alpha) * 0xFF;

											CANI_LONG v = (0xFF - (CANI_LONG)(Alpha)) * (CANI_LONG)(ImageCurrent[pos].Alpha);

											r = ((CANI_LONG)(palette[index].red) * u + (CANI_LONG)(ImageCurrent[pos].Red) * v) / al;

											g = ((CANI_LONG)(palette[index].green) * u + (CANI_LONG)(ImageCurrent[pos].Green) * v) / al;

											b = ((CANI_LONG)(palette[index].blue) * u + (CANI_LONG)(ImageCurrent[pos].Blue) * v) / al;

											a = al / 0xFF;

											ImageCurrent[pos].Red = (CANI_BYTE)CANI_CLIP(r);
											ImageCurrent[pos].Green = (CANI_BYTE)CANI_CLIP(g);
											ImageCurrent[pos].Blue = (CANI_BYTE)CANI_CLIP(b);
											ImageCurrent[pos].Alpha = (CANI_BYTE)CANI_CLIP(a);
										}
										else {

											ImageCurrent[pos].Red = (CANI_BYTE)palette[index].red;
											ImageCurrent[pos].Green = (CANI_BYTE)palette[index].green;
											ImageCurrent[pos].Blue = (CANI_BYTE)palette[index].blue;
											ImageCurrent[pos].Alpha = (CANI_BYTE)Alpha;
										}

									}
								}
							}

						}
						else {

							for (y = 0; y < nf_height; y++) {
								for (x = 0; x < nf_width; x++) {

									CANI_BYTE index = 0;
									CANI_LONG pos;

									if (bit_depth == 8) {
										index = row_pointers[y][x];
									}
									else if (bit_depth == 4) {
										CANI_DWORD shift = x % 2;
										index = (row_pointers[y][x / 2] & four_bits_mask[shift]) >> (four_bits_shift[shift]);
									}
									else if (bit_depth == 2) {
										CANI_DWORD shift = x % 4;
										index = (row_pointers[y][x / 4] & two_bits_mask[shift]) >> (two_bits_shift[shift]);
									}
									else if (bit_depth == 1) {
										CANI_DWORD shift = x % 8;
										index = row_pointers[y][x / 8] & one_bit_mask[shift] ? 1 : 0;
									}

									pos = (height - (nf_y_offset + y) - 1) * width + nf_x_offset + x;
									ImageCurrent[pos].Red = palette[index].red;
									ImageCurrent[pos].Green = palette[index].green;
									ImageCurrent[pos].Blue = palette[index].blue;
									ImageCurrent[pos].Alpha = 0xFF;
								}
							}
						}

						//MessageBoxA(0, "PNG_COLOR_TYPE_PALETTE", 0, 0);
					}

					if (nf_dispose_op != PNG_DISPOSE_OP_PREVIOUS && (count<(frames-1))) {

						CANI_BGRAQUAD* ImageNext = (CANI_BGRAQUAD*)rv->Images[count + 1].Image;

						CANI_MEMCPY(ImageNext, ImageCurrent, Size * sizeof(CANI_BGRAQUAD));

						if (nf_dispose_op == PNG_DISPOSE_OP_BACKGROUND) {

								for (y = 0; y < nf_height; y++) {
									for (x = 0; x < nf_width; x++) {
										CANI_BYTE index = row_pointers[y][x];

										CANI_LONG pos = (height - (nf_y_offset + y) - 1) * width + nf_x_offset + x;
										ImageNext[pos].Red = 0;// bg_color.red;
										ImageNext[pos].Green = 0;//bg_color.green;
										ImageNext[pos].Blue = 0;//bg_color.blue;
										ImageNext[pos].Alpha = 0x00;
									}
								}
						}
							//for (j = 0; j < h0; j++)
								//memset(frameNext.rows[y0 + j] + x0 * 4, 0, w0 * 4);
					}

					//for (y = 0; y < height; y++) CANI_FREE(row_pointers[y]);
					//CANI_FREE(row_pointers);
				}
				else
				{
					/* the first frame doesn't have an fcTL so it's expected to be hidden,
					* but we'll extract it anyway */
					
					//next_frame_width = png_get_image_width(png_ptr_read, info_ptr_read);
					//next_frame_height = png_get_image_height(png_ptr_read, info_ptr_read);

					//png_read_image(png_ptr_read, row_pointers);

					//MessageBox(0, "gffdgd", "", 0);
				}

				//rv->Images[count].Time = 100;
			}

			for (y = 0; y < height; y++) CANI_FREE(row_pointers[y]);
			CANI_FREE(row_pointers);

			png_destroy_read_struct(&png_ptr_read, &info_ptr_read, NULL);

			return rv;
		}
	}
	return 0;
}

#endif

#ifdef CANI_USE_LIBPNG

CANI_ANIMATION* CANI_ReadPNG_FILE(FILE* file) {
	if (file) {

		png_infop info;
		png_uint_32 width;
		png_uint_32 height;
		png_byte color_type;
		png_byte bit_depth;
		png_size_t rowbytes;
		CANI_ANIMATION* rv;
		CANI_MEMORY_SIZE Size;
		png_uint_32 n,x,y;
		png_bytep *row_pointers;

		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!png) return 0;

		info = png_create_info_struct(png);

		if (!info) return 0;

		if (setjmp(png_jmpbuf(png))) return 0;

		png_init_io(png, file);

		png_read_info(png, info);

		width = png_get_image_width(png, info);

		height = png_get_image_height(png, info);

		color_type = png_get_color_type(png, info);

		bit_depth = png_get_bit_depth(png, info);

		rowbytes = png_get_rowbytes(png, info);

		rv = CANI_MALLOC(sizeof(CANI_ANIMATION));

		if (!rv) {
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return 0;
		}

		CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));

		rv->Images = CANI_MALLOC(sizeof(CANI_FRAME_DATA));

		if (!rv->Images) {
			png_destroy_read_struct(&png, &info, NULL);
			CANI_FREE(rv);
			CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
			return 0;
		}

		CANI_MEMSET(rv->Images, 0x00, sizeof(CANI_FRAME_DATA));

		rv->ImagesNR = 1;
		rv->Width = width;
		rv->Height = height;

		rv->Format.format = CAF_FORMAT_RGB;
		rv->Format.rgb.Red = 8;
		rv->Format.rgb.Green = 8;
		rv->Format.rgb.Blue = 8;
		rv->Format.rgb.Alpha = 8;

		Size = width * height;

		//if (bit_depth == 16) png_set_strip_16(png);

		if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);

		// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.

		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);

		//if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);

		// These color_type don't have an alpha channel then fill it with 0xff.

		//if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) 
		//	png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

		//if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);

		png_read_update_info(png, info);

		color_type = png_get_color_type(png, info);

		bit_depth = png_get_bit_depth(png, info);

		rowbytes = png_get_rowbytes(png, info);

		row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for (y = 0; y < height; y++) {
			row_pointers[y] = (png_byte*)malloc(rowbytes);

		}
		//int dlaY;
		png_read_image(png, row_pointers);

		if (bit_depth == 16 ) {

			CANI_BGRA16QUAD* Image = ((CANI_BGRA16QUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRA16QUAD));

			if (!Image) {
				CANI_FREE(rv->Images);
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			rv->Images->Style |= CANI_STYLE_16BITS_IMAGE;

			if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
				CANI_BYTE pix_byte = 8;
				for (n = 0; n < height; n++) {
					for (x = 0; x < width; x++) {

						Image[n * width + x].Red =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 1]);

						Image[n * width + x].Green =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 2]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 3]);

						Image[n * width + x].Blue =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 4]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 5]);

						Image[n * width + x].Alpha =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 6]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 7]);

					}
				}

				rv->Images->Style |= CANI_STYLE_ALPHA;

			}
			else if (color_type == PNG_COLOR_TYPE_RGB) {

				CANI_BYTE pix_byte = 6;
				png_color_16p bgt = 0;

				for (n = 0; n < height; n++) {
					for (x = 0; x < width; x++) {

						Image[n * width + x].Red = 
							( ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 1] );

						Image[n * width + x].Green =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 2]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 3]);

						Image[n * width + x].Blue =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 4]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 5]);

						Image[n * width + x].Alpha = 0xFFFF;

					}
				}

				if (png_get_tRNS(png, info, 0, 0, &bgt)) {
					if (bgt) {

						rv->Images->bgColorRGB16.Red = bgt->red;
						rv->Images->bgColorRGB16.Green = bgt->green;
						rv->Images->bgColorRGB16.Blue = bgt->blue;

						CANI_RGBA16_2_RGBA(&rv->Images->bgColorRGB, &rv->Images->bgColorRGB16, 1);

						rv->Images->Style |= CANI_STYLE_TRANSPARENT;
					}
				}
			}
			else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
				CANI_BYTE pix_byte = 4;

				for (n = 0; n < height; n++) {
					//for (m=0;m<info_ptr->rowbytes;m++) {
					for (x = 0; x < width; x++) {
						CANI_WORD gray =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 1]);

						Image[n * width + x].Red = gray;

						Image[n * width + x].Green = gray;

						Image[n * width + x].Blue = gray;

						Image[n * width + x].Alpha =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 2]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 3]);
					}
				}

				rv->Images->Style |= CANI_STYLE_ALPHA;
			}
			else if (color_type == PNG_COLOR_TYPE_GRAY) {
				CANI_BYTE pix_byte = 2;
				png_color_16p bgt = 0;

				for (n = 0; n < height; n++) {
					//for (m=0;m<info_ptr->rowbytes;m++) {
					for (x = 0; x < width; x++) {
						CANI_WORD gray =
							(((CANI_WORD)row_pointers[height - n - 1][x * pix_byte]) << 8) | ((CANI_WORD)row_pointers[height - n - 1][x * pix_byte + 1]);

						Image[n * width + x].Red = gray;

						Image[n * width + x].Green = gray;

						Image[n * width + x].Blue = gray;

						Image[n * width + x].Alpha = 0xFFFF;
					}
				}

				if (png_get_tRNS(png, info, 0, 0, &bgt)) {
					if (bgt) {

						rv->Images->bgColorRGB16.Red = bgt->gray;
						rv->Images->bgColorRGB16.Green = bgt->gray;
						rv->Images->bgColorRGB16.Blue = bgt->gray;

						CANI_RGBA16_2_RGBA(&rv->Images->bgColorRGB, &rv->Images->bgColorRGB16, 1);

						rv->Images->Style |= CANI_STYLE_TRANSPARENT;
					}
				}
			}


		} else if (bit_depth == 8) {

			CANI_BGRAQUAD* Image = ((CANI_BGRAQUAD*)rv->Images->Image) = CANI_MALLOC(Size * sizeof(CANI_BGRAQUAD));

			if (!Image) {
				CANI_FREE(rv->Images);
				CANI_FREE(rv);
				CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
				return 0;
			}

			//char szCaniBuffer[1024];
			//sprintf(szCaniBuffer, "%u", color_type );
			//MessageBoxA(0, szCaniBuffer, "color_type", 0);

			if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
				CANI_BYTE pix_byte = 4;
				for (n = 0; n < height; n++) {
					for (x = 0; x < width; x++) {

						Image[n * width + x].Red =
							row_pointers[height - n - 1][x * pix_byte];

						Image[n * width + x].Green =
							row_pointers[height - n - 1][x * pix_byte + 1];

						Image[n * width + x].Blue =
							row_pointers[height - n - 1][x * pix_byte + 2];

						Image[n * width + x].Alpha =
							row_pointers[height - n - 1][x * pix_byte + 3];

					}
				}

				rv->Images->Style |= CANI_STYLE_ALPHA;

			} else if (color_type == PNG_COLOR_TYPE_RGB) {
				CANI_BYTE pix_byte = 3;
				png_color_16p bgt = 0;
				for (n = 0; n < height; n++) {
					for (x = 0; x < width; x++) {

						Image[n * width + x].Red =
							row_pointers[height - n - 1][x * pix_byte];

						Image[n * width + x].Green =
							row_pointers[height - n - 1][x * pix_byte + 1];

						Image[n * width + x].Blue =
							row_pointers[height - n - 1][x * pix_byte + 2];

						Image[n * width + x].Alpha = 0xFF;

					}
				}

				if (png_get_tRNS(png, info, 0, 0, &bgt ) ) {
					if (bgt) {

						rv->Images->bgColorRGB.Red = (CANI_BYTE)bgt->red;
						rv->Images->bgColorRGB.Green = (CANI_BYTE)bgt->green;
						rv->Images->bgColorRGB.Blue = (CANI_BYTE)bgt->blue;

						CANI_RGBA_2_RGBA16( &rv->Images->bgColorRGB16, &rv->Images->bgColorRGB, 1);

						rv->Images->Style |= CANI_STYLE_TRANSPARENT;
					}
				}

			} else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {

				CANI_BYTE pix_byte = 2;
				for (n = 0; n < height; n++) {
					for (x = 0; x < width; x++) {
						CANI_BYTE gray = (CANI_BYTE)row_pointers[height - n - 1][x * pix_byte];

						Image[n * width + x].Red = gray;

						Image[n * width + x].Green = gray;

						Image[n * width + x].Blue = gray;

						Image[n * width + x].Alpha =
							row_pointers[height - n - 1][x * pix_byte + 1];

					}
				}

				rv->Images->Style |= CANI_STYLE_ALPHA;

			}
			else if (color_type == PNG_COLOR_TYPE_GRAY) {

				png_color_16p bgt = 0;

				for (n = 0; n < height; n++) {
					//for (m=0;m<info_ptr->rowbytes;m++) {
					for (x = 0; x < width; x++) {
						CANI_BYTE gray = (CANI_BYTE)row_pointers[height - n - 1][x];

						Image[n * width + x].Red = gray;

						Image[n * width + x].Green = gray;

						Image[n * width + x].Blue = gray;

						Image[n * width + x].Alpha = 0xFF;
					}
				}

				if (png_get_tRNS(png, info, 0, 0, &bgt)) {
					if (bgt) {

						rv->Images->bgColorRGB.Red = (CANI_BYTE)bgt->gray;
						rv->Images->bgColorRGB.Green = (CANI_BYTE)bgt->gray;
						rv->Images->bgColorRGB.Blue = (CANI_BYTE)bgt->gray;

						CANI_RGBA_2_RGBA16(&rv->Images->bgColorRGB16, &rv->Images->bgColorRGB, 1);

						rv->Images->Style |= CANI_STYLE_TRANSPARENT;
					}
				}
			}
		}

		for (n = 0; n < height; n++) CANI_FREE(row_pointers[n]);
		CANI_FREE(row_pointers);

		png_destroy_read_struct(&png, &info, NULL);

		return rv;
	}
	return 0;
}

#endif