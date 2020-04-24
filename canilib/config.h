#pragma once

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <memory.h>

#pragma warning( disable : 4142 )

#ifdef _WINDLL
#define CANI_EXPORT __declspec(dllexport)
#else
#define CANI_EXPORT
#endif

#define CANI_FILES_LIST "*.cani;*.lcaf;*.jpg;*.jpeg;*.gif;*.png;*.apng;*.mng;*.pbm;*.pgm;*.ppm;*.bmp"
#define CANI_FILES_LIST_L L"*.cani;*.lcaf;*.jpg;*.jpeg;*.gif;*.png;*.apng;*.mng;*.pbm;*.pgm;*.ppm;*.bmp"

#define CANI_UNICODE_FILE // define when "_wfopen" and "_wunlink" function is enable

#define CANI_WINDOWS_GDI // define when operating system is WINDOWS

#define CANI_WINDOWS // define when operating system is WINDOWS

#define CANI_USE_GIFLIB // uses LZW compress method

#define CANI_USE_LIBMNG // uses MNG library

#define CANI_USE_LIBAPNG

#define CANI_USE_LIBPNG

#define CANI_INLINE

#define CANI_IMAGE_IS_8_BITS 1
#define CANI_IMAGE_IS_16_BITS 2

typedef unsigned char CANI_BYTE;
typedef unsigned short CANI_WORD;
typedef unsigned long CANI_DWORD;

typedef char CANI_CHAR;
typedef short CANI_SHORT;
typedef long CANI_LONG;

typedef float CANI_FLOAT;
typedef double CANI_DOUBLE;

typedef unsigned long CANI_COLORREF;

typedef unsigned long long CANI_QWORD;

typedef long long CANI_CONVERT_VARIABLE;

#ifdef  _WIN64
typedef unsigned long long CANI_MEMORY_SIZE;
typedef long long CANI_FILE_POS;
#define CANI_FTELL _ftelli64
#define CANI_FSEEK _fseeki64
#else
typedef unsigned long CANI_MEMORY_SIZE;
typedef long CANI_FILE_POS;
#define CANI_FTELL ftell
#define CANI_FSEEK fseek
#endif

#define CANI_FOPEN_ASCII fopen
#define CANI_FOPEN_UTF16 _wfopen
#define CANI_FREAD fread
#define CANI_FWRITE fwrite
#define CANI_FGETC fgetc
#define CANI_FCLOSE fclose
#if defined(WIN32) || defined(_WIN64)
#define CANI_FDELETE_ASCII _unlink
#define CANI_FDELETE_UTF16 _wunlink
#else
#define CANI_FDELETE_ASCII unlink
#define CANI_FDELETE_UTF16 wunlink
#endif

#define CANI_MALLOC malloc
#define CANI_REALLOC realloc
#define CANI_MEMCPY memcpy
#define CANI_MEMSET memset
#define CANI_MEMCMP memcmp
#define CANI_FREE free

#define CANI_MAX_32BIT_SIZE 0x7FFFFFFF

#define TRUE 1
#define FALSE 0

#include "..\zlib\zlib.h"

#include "..\jpeg\jpeglib.h"

#ifdef CANI_USE_GIFLIB
#include "..\giflib\my_giflib.h"
#endif

#ifdef CANI_USE_LIBMNG
#include "..\libmng\libmng_types.h"
#define MNG_EXT
#define MNG_DECL
#define MNG_NEEDMOREDATA     (mng_retcode)14   /* I'm hungry, give me more    */
MNG_EXT mng_handle  MNG_DECL mng_initialize(mng_ptr       pUserdata, mng_memalloc  fMemalloc, mng_memfree   fMemfree, mng_traceproc fTraceproc);
MNG_EXT mng_ptr     MNG_DECL mng_get_userdata(mng_handle        hHandle);
MNG_EXT mng_retcode MNG_DECL mng_setcb_openstream(mng_handle        hHandle,
    mng_openstream    fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_closestream(mng_handle        hHandle,
    mng_closestream   fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_readdata(mng_handle        hHandle,
    mng_readdata      fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_processheader(mng_handle        hHandle,
    mng_processheader fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_readdata(mng_handle        hHandle,
    mng_readdata      fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_processmend(mng_handle        hHandle,
    mng_processmend   fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_getcanvasline(mng_handle        hHandle,
    mng_getcanvasline fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_refresh(mng_handle        hHandle,
    mng_refresh       fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_gettickcount(mng_handle        hHandle,
    mng_gettickcount  fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_settimer(mng_handle        hHandle,
    mng_settimer      fProc);
MNG_EXT mng_retcode MNG_DECL mng_setcb_errorproc(mng_handle        hHandle,
    mng_errorproc     fProc);
MNG_EXT mng_retcode MNG_DECL mng_read(mng_handle    hHandle);
MNG_EXT mng_retcode MNG_DECL mng_read_resume(mng_handle    hHandle);
MNG_EXT mng_imgtype MNG_DECL mng_get_imagetype(mng_handle        hHandle);
MNG_EXT mng_uint32  MNG_DECL mng_get_totallayers(mng_handle        hHandle);
MNG_EXT mng_retcode MNG_DECL mng_display(mng_handle    hHandle);
MNG_EXT mng_retcode MNG_DECL mng_display_resume(mng_handle    hHandle);
MNG_EXT mng_retcode MNG_DECL mng_display_goframe(mng_handle    hHandle,
    mng_uint32    iFramenr);
#endif

#if defined(CANI_USE_LIBPNG) || defined(CANI_USE_LIBAPNG)
//#define NO_DUMMY_DECL
#include "..\libpng\pnglibconf.h"
#include "..\libpng\png.h"
#endif

#ifdef _X64
#ifdef _DEBUG
#pragma comment( lib, "..\\zlib\\x64\\debug\\zlib.lib" )
#pragma comment( lib, "..\\jpeg\\x64\\debug\\jpeg.lib" )
#ifdef CANI_USE_GIFLIB
#pragma comment( lib, "..\\giflib\\x64\\debug\\giflib.lib" )
#endif
#ifdef CANI_USE_LIBMNG
#pragma comment( lib, "..\\libmng\\x64\\debug\\libmng.lib" )
#endif
#ifdef CANI_USE_LIBPNG
#pragma comment( lib, "..\\libpng\\x64\\debug\\libpng.lib" )
#endif
#else
#pragma comment( lib, "..\\zlib\\x64\\release\\zlib.lib" )
#pragma comment( lib, "..\\jpeg\\x64\\release\\jpeg.lib" )
#ifdef CANI_USE_GIFLIB
#pragma comment( lib, "..\\giflib\\x64\\release\\giflib.lib" )
#endif
#ifdef CANI_USE_LIBMNG
#pragma comment( lib, "..\\libmng\\x64\\release\\libmng.lib" )
#endif
#ifdef CANI_USE_LIBPNG
#pragma comment( lib, "..\\libpng\\x64\\release\\libpng.lib" )
#endif
#endif
#else
#ifdef _DEBUG
#pragma comment( lib, "..\\zlib\\debug\\zlib.lib" )
#pragma comment( lib, "..\\jpeg\\debug\\jpeg.lib" )
#ifdef CANI_USE_GIFLIB
#pragma comment( lib, "..\\giflib\\debug\\giflib.lib" )
#endif
#ifdef CANI_USE_LIBMNG
#pragma comment( lib, "..\\libmng\\debug\\libmng.lib" )
#endif
#ifdef CANI_USE_LIBPNG
#pragma comment( lib, "..\\libpng\\debug\\libpng.lib" )
#endif
#else
#pragma comment( lib, "..\\zlib\\release\\zlib.lib" )
#pragma comment( lib, "..\\jpeg\\release\\jpeg.lib" )
#ifdef CANI_USE_GIFLIB
#pragma comment( lib, "..\\giflib\\release\\giflib.lib" )
#endif
#ifdef CANI_USE_LIBMNG
#pragma comment( lib, "..\\libmng\\release\\libmng.lib" )
#endif
#ifdef CANI_USE_LIBPNG
#pragma comment( lib, "..\\libpng\\release\\libpng.lib" )
#endif
#endif
#endif

#define CANI_CLIP(X) ( (X) > 0xFF ? 0xFF : (X) < 0 ? 0 : X)
#define CANI_CLIP16(X) ( (X) > 0xFFFF ? 0xFFFF : (X) < 0 ? 0 : X)