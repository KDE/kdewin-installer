/*
 * utils.c - small wrapper to lzma sdk
 * Copyright (C) 2007  Igor Mironchick <imironchick@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>

#include "lzma_sdk/7zip/Archive/7z_C/7zIn.h"
#include "lzma_sdk/7zip/Archive/7z_C/7zCrc.h"
#include "lzma_sdk/7zip/Archive/7z_C/7zExtract.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CFileInStream
{
  ISzInStream InStream;
  FILE *File;
} CFileInStream;

#ifdef _LZMA_IN_CB

#define kBufferSize (1 << 12)
SZ_RESULT SzFileReadImp(void *object, void **buffer, size_t maxRequiredSize, size_t *processedSize);
#else
SZ_RESULT SzFileReadImp(void *object, void *buffer, size_t size, size_t *processedSize);
#endif

SZ_RESULT SzFileSeekImp(void *object, CFileSize pos);

SZ_RESULT SzInit(
    CFileInStream *archiveStream, 
    CArchiveDatabaseEx *db,
    ISzAlloc *allocMain, 
    ISzAlloc *allocTemp,
	const wchar_t * fileName );

void SzClean(
	Byte *outBuffer,
	CArchiveDatabaseEx *db,
	ISzAlloc *allocImp,
	CFileInStream *archiveStream );

SZ_RESULT SzReadFile(
	ISzInStream *inStream, 
	CArchiveDatabaseEx *db,
	UInt32 fileIndex,
	UInt32 *blockIndex,
	Byte **outBuffer,
	size_t *outBufferSize,
	size_t *offset,
	size_t *outSizeProcessed,
	ISzAlloc *allocMain,
	ISzAlloc *allocTemp );

#ifdef __cplusplus
}
#endif

#endif // _UTILS_H_
