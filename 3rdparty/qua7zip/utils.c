/*
 * utils.c - small wrapper to lzma sdk
 * Copyright (C) 2007  Igor Mironchick <imironchick@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */


#include <stdio.h>

#include "utils.h"


#ifdef _LZMA_IN_CB

Byte g_Buffer[kBufferSize];

SZ_RESULT SzFileReadImp(void *object, void **buffer, size_t maxRequiredSize, size_t *processedSize)
{
  CFileInStream *s = (CFileInStream *)object;
  size_t processedSizeLoc;
  if (maxRequiredSize > kBufferSize)
    maxRequiredSize = kBufferSize;
  processedSizeLoc = fread(g_Buffer, 1, maxRequiredSize, s->File);
  *buffer = g_Buffer;
  if (processedSize != 0)
    *processedSize = processedSizeLoc;
  return SZ_OK;
}

#else

SZ_RESULT SzFileReadImp(void *object, void *buffer, size_t size, size_t *processedSize)
{
  CFileInStream *s = (CFileInStream *)object;
  size_t processedSizeLoc = fread(buffer, 1, size, s->File);
  if (processedSize != 0)
    *processedSize = processedSizeLoc;
  return SZ_OK;
}

#endif

void PrintError(char *sz)
{
  printf("\nERROR: %s\n", sz);
}

SZ_RESULT SzFileSeekImp(void *object, CFileSize pos)
{
  CFileInStream *s = (CFileInStream *)object;
  int res = fseek(s->File, (long)pos, SEEK_SET);
  if (res == 0)
    return SZ_OK;
  return SZE_FAIL;
}

SZ_RESULT SzInit(
    CFileInStream *archiveStream, 
    CArchiveDatabaseEx *db,
    ISzAlloc *allocMain, 
    ISzAlloc *allocTemp,
	const wchar_t * fileName )
{
	archiveStream->File = _wfopen(fileName, L"rb");
	if (archiveStream->File == 0)
	{
		PrintError("can not open input file");
		return SZE_FAIL;
	}

	archiveStream->InStream.Read = SzFileReadImp;
	archiveStream->InStream.Seek = SzFileSeekImp;

	allocMain->Alloc = SzAlloc;
	allocMain->Free = SzFree;

	allocTemp->Alloc = SzAllocTemp;
	allocTemp->Free = SzFreeTemp;

	InitCrcTable();
	SzArDbExInit(db);

	return SzArchiveOpen(&archiveStream->InStream, db, allocMain, allocTemp);
}

void SzClean(
	Byte *outBuffer,
	CArchiveDatabaseEx *db,
	ISzAlloc *allocImp,
	CFileInStream *archiveStream )
{
	allocImp->Free( outBuffer );
	SzArDbExFree( db, allocImp->Free );
	fclose( archiveStream->File );
}

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
	ISzAlloc *allocTemp )
{
	return SzExtract( inStream, db, fileIndex, 
		blockIndex, outBuffer, outBufferSize, 
		offset, outSizeProcessed, 
		allocMain, allocTemp );
}
