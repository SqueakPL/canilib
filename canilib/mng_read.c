#include "cani.h"

#ifdef CANI_USE_LIBMNG

typedef struct tagCANI_MNG {
	FILE* fd;
	CANI_BYTE* buffer;
	mng_uint32 width;
	mng_uint32 height;
	CANI_DWORD iteration;
	CANI_DWORD ticks;
	CANI_ANIMATION* animation;

} CANI_MNG;

static mng_ptr MNG_DECL my_alloc(mng_size_t len)
{
	char* ptr = CANI_MALLOC(len);
	CANI_MEMSET(ptr, 0, len);
	return (mng_ptr)ptr;
}

static void MNG_DECL my_free(mng_ptr ptr, mng_size_t len)
{
	CANI_FREE(ptr);
}

static mng_bool MNG_DECL my_openstream(mng_handle handle)
{
	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);
	//mng->fd = fopen(mng->name, "rb");
	if (!mng->fd) {
		return MNG_FALSE;
	}
	return MNG_TRUE;
}

static mng_bool MNG_DECL my_closestream(mng_handle handle)
{
	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);
	if (mng->fd) {
		//fclose(mng->fd);
	}
	return MNG_TRUE;
}

static mng_bool MNG_DECL my_readdata(mng_handle handle, mng_ptr buf, mng_uint32 buflen,
	mng_uint32p read)
{
	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);
	*read = (mng_uint32)fread(buf, 1, buflen, mng->fd);
	return MNG_TRUE;
}

static mng_bool MNG_DECL my_processheader(mng_handle handle, mng_uint32 width, mng_uint32 height)
{
	CANI_MNG *mng = (CANI_MNG*)mng_get_userdata(handle);
	mng->width = width;
	mng->height = height;
	mng->buffer = malloc(width * height * 3);
	if (mng->buffer) memset(mng->buffer, 0x00, width * height * 3);

	return MNG_TRUE;
}


static mng_ptr MNG_DECL my_getcanvasline(mng_handle handle, mng_uint32 linenr)
{
	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);
	return mng->buffer + linenr * mng->width * 3;
}


static mng_bool MNG_DECL my_refresh(mng_handle handle, mng_uint32 x, mng_uint32 y,
	mng_uint32 width, mng_uint32 height)
{
	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);

	if ((mng->animation)&&(mng->buffer)) {

		if (mng->animation->Images && mng->buffer ) {
			CANI_BGRAQUAD *Image = (CANI_BGRAQUAD*)mng->animation->Images[mng->animation->ImagesNow].Image;
			//mng->animation->Images[mng->animation->ImagesNow].Time = msecs;
			if (Image) {
				mng_uint32 x2, y2;
				if (mng->animation->ImagesNow > 0) {
					memcpy(Image, mng->animation->Images[mng->animation->ImagesNow - 1].Image, mng->width * mng->height * sizeof(CANI_BGRAQUAD));
				}
				for (y2 = x; y2 < mng->height; y2++) {
					for (x2 = x; x2 < mng->width; x2++) {

						CANI_BGRAQUAD* Image8 = &Image[(mng->height - (y2+y) - 1 ) * mng->width + x2 + x];
						CANI_BYTE* ImageBuffer = &mng->buffer[(y2 + y) * mng->width * 3 + 3 * ( x2 + x ) ];

						Image8->Red = ImageBuffer[0];
						Image8->Green = ImageBuffer[1];
						Image8->Blue = ImageBuffer[2];
						Image8->Alpha = 0xFF;

					}

				}
			}
		}

	}

	return MNG_TRUE;
}

static mng_uint32 MNG_DECL my_gettickcount(mng_handle handle)
{

#ifdef CANI_WINDOWS
	return GetTickCount();
#else
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long long ms = (long long)tp.tv_sec * 1000L + tp.tv_usec / 1000;
	return (mng_uint32)ms;
#endif

}

static mng_bool MNG_DECL my_settimer(mng_handle handle, mng_uint32 msecs)
{

	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);

#ifdef CANI_WINDOWS
	mng->ticks = GetTickCount() + msecs;// 
#else
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long long ms = (long long)tp.tv_sec * 1000L + tp.tv_usec / 1000;
	mng->ticks = (mng_uint32)ms + msecs;// 
#endif

	if (mng->animation) {

		if (mng->animation->Images) {
			mng->animation->Images[mng->animation->ImagesNow].Time = msecs;
		}

	}

	return MNG_TRUE;
}

static mng_bool MNG_DECL my_processmend(mng_handle handle, mng_uint32 iterationsdone,
	mng_uint32 iterationsleft)
{
	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);
	mng->iteration = iterationsdone;

	return MNG_TRUE;
}

static mng_bool MNG_DECL my_errorproc(mng_handle handle, mng_int32 errorcode,
	mng_int8 severity, mng_chunkid chunkname, mng_uint32 chunkseq,
	mng_int32 extra1, mng_int32 extra2, mng_pchar errortext)
{
	CANI_MNG* mng;

	mng = (CANI_MNG*)mng_get_userdata(handle);
	mng->iteration = 0x7fffffff;
	if (errortext) {
		//char szCaniBuffer[128];
		//sprintf(szCaniBuffer, "%i", errorcode);
		//MessageBoxA(0, szCaniBuffer, "my_errorproc", 0);
	}
	return TRUE;
}


CANI_ANIMATION* CANI_ReadMNG_FILE(FILE* file, CANI_PROGRESS_FUNC Progress) {
	if (file) {

		CANI_ANIMATION* rv;
		CANI_MNG mng;
		mng_handle handle;
		mng_retcode myretcode;

		rv = CANI_MALLOC(sizeof(CANI_ANIMATION));

		CANI_MEMSET(rv, 0x00, sizeof(CANI_ANIMATION));

		rv->Format.format = CAF_FORMAT_RGB;
		rv->Format.rgb.Red = 8;
		rv->Format.rgb.Green = 8;
		rv->Format.rgb.Blue = 8;
		rv->Format.rgb.Alpha = 8;

		memset(&mng, 0x00, sizeof(CANI_MNG));
		mng.fd = file;
		mng.animation = rv;

		handle = mng_initialize(&mng, my_alloc, my_free, MNG_NULL);
		if (handle == MNG_NULL) {
			return 0;
		}
		mng_setcb_openstream(handle, my_openstream);
		mng_setcb_closestream(handle, my_closestream);
		mng_setcb_readdata(handle, my_readdata);
		mng_setcb_processheader(handle, my_processheader);
		mng_setcb_processmend(handle, my_processmend);
		mng_setcb_getcanvasline(handle, my_getcanvasline);
		mng_setcb_refresh(handle, my_refresh);
		mng_setcb_gettickcount(handle, my_gettickcount);
		mng_setcb_settimer(handle, my_settimer);
		mng_setcb_errorproc(handle, my_errorproc);

		myretcode = mng_read(handle);

		while (myretcode == MNG_NEEDMOREDATA) {
			/* wait for input-data to arrive */
			myretcode = mng_read_resume(handle);
		}

		if (mng_get_imagetype(handle)==2) {

			CANI_DWORD Size;
			CANI_DWORD n;
			int code;

			CANI_BYTE ProgressNow = 0;

			//char szCaniBuffer[128];
			//sprintf(szCaniBuffer, "layers=%i frames=%i", mng_get_totallayers(handle), mng_get_totalframes(handle) );
			//MessageBoxA(0, szCaniBuffer, "mng_get_totallayers", 0);

			rv->Width = mng.width;
			rv->Height = mng.height;

			Size = rv->Width * rv->Height;

			rv->ImagesNR = mng_get_totallayers(handle) - 1;

			rv->Images = malloc(rv->ImagesNR * sizeof(CANI_FRAME_DATA));

			CANI_MEMSET(rv->Images, 0x00, rv->ImagesNR * sizeof(CANI_FRAME_DATA));

			for (n = 0; n < rv->ImagesNR; n++) {
				rv->Images[n].Image = (CANI_BYTE*)CANI_MALLOC(Size * sizeof(sizeof(CANI_BGRAQUAD)));
				if (rv->Images[n].Image == 0) {
					CANI_DWORD m;
					for (m = n; m > 0; m--) {
						CANI_FREE(rv->Images[m - 1].Image);//delete [] rv->Images[m].Image;
					}
					CANI_FREE(rv->Images);
					CANI_FREE(rv);
					CANI_SetLastError(CANI_ERROR_NO_FREE_MEMORY);
					return 0;
				}
				CANI_MEMSET(rv->Images[n].Image, 0x00, Size * sizeof(sizeof(CANI_BGRAQUAD)));
			}

			code = mng_display(handle);

			rv->ImagesNow++;

			while (rv->ImagesNow < rv->ImagesNR) {

				if (Progress) {

					CANI_BYTE ProgressNow2 = (CANI_BYTE)(rv->ImagesNow * 100 / rv->ImagesNR );

					if (ProgressNow2 != ProgressNow) {
						ProgressNow = ProgressNow2;
						Progress(ProgressNow);
					}

				}

				mng_display_goframe(handle, rv->ImagesNow);
				mng_display_resume(handle);
				rv->ImagesNow++;
			}

			rv->ImagesNow = 0;

			if (mng.iteration == 0x7fffffff) {
				//return -1;
			}

			//CANI_FCLOSE(file);

			return rv;

		}

		CANI_FREE(rv);
	}
	return 0;
}

#endif