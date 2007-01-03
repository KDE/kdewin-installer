/****************************************************************************
**
** Copyright (C) 2005-2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <stdio.h>
#include <windll/windll.h>
#define LPSTR char *

	/* Dummy "print" routine that simply outputs what is sent from the dll */
int WINAPI DisplayBuf(LPSTR buf, unsigned long size)
{
printf("%s", (char *)buf);
return (unsigned int) size;
}


/* This is a very stripped down version of what is done in Wiz. Essentially
   what this function is for is to do a listing of an archive contents. It
   is actually never called in this example, but a dummy procedure had to
   be put in, so this was used.
 */
void WINAPI ReceiveDllMessage(unsigned long ucsize, unsigned long csiz,
    unsigned cfactor,
    unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
    char c, LPSTR filename, LPSTR methbuf, unsigned long crc, char fCrypt)
{
char psLBEntry[_MAX_PATH];
char LongHdrStats[] =
          "%7lu  %7lu %4s  %02u-%02u-%02u  %02u:%02u  %c%s";
char CompFactorStr[] = "%c%d%%";
char CompFactor100[] = "100%%";
char szCompFactor[10];
char sgn;

if (csiz > ucsize)
   sgn = '-';
else
   sgn = ' ';
if (cfactor == 100)
   strcpy(szCompFactor, CompFactor100);
else
   sprintf(szCompFactor, CompFactorStr, sgn, cfactor);
   sprintf(psLBEntry, LongHdrStats,
      ucsize, csiz, szCompFactor, mo, dy, yr, hh, mm, c, filename);

printf("%s\n", psLBEntry);
}

/* Password entry routine - see password.c in the wiz directory for how
   this is actually implemented in WiZ. If you have an encrypted file,
   this will probably give you great pain.
 */
int WINAPI password(char *p, int n, const char *m, const char *name)
{
return 1;
}

int unzip(char *fileName, char *rootdir)
{
	DCL dcl;
	DCL *lpDCL = &dcl;
	
	lpDCL->ncflag = 0; /* Write to stdout if true */
	lpDCL->fQuiet = 0; /* We want all messages.
	                      1 = fewer messages,
	                      2 = no messages */
	lpDCL->ntflag = 0; /* test zip file if true */
	lpDCL->nvflag = 0; /* give a verbose listing if true */
	lpDCL->nzflag = 0; /* display a zip file comment if true */
	lpDCL->ndflag = 1; /* Recreate directories != 0, skip "../" if < 2 */
	lpDCL->naflag = 0; /* Do not convert CR to CRLF */
	lpDCL->nfflag = 0; /* Do not freshen existing files only */
	lpDCL->noflag = 1; /* Over-write all files if true */
	lpDCL->ExtractOnlyNewer = 0; /* Do not extract only newer */
	lpDCL->PromptToOverwrite = 0; /* "Overwrite all" selected -> no query mode */
	lpDCL->lpszZipFN = fileName; /* The archive name */
	lpDCL->lpszExtractDir = rootdir; /* The directory to extract to. This is set
	                                 to NULL if you are extracting to the
	                                 current directory.	 */

	USERFUNCTIONS UserFunctions,*lpUserFunctions;
	lpUserFunctions = &UserFunctions;
	lpUserFunctions->password = password;
	lpUserFunctions->print = DisplayBuf;
	lpUserFunctions->sound = NULL;
//	lpUserFunctions->replace = GetReplaceDlgRetVal;
	lpUserFunctions->SendApplicationMessage = ReceiveDllMessage;
	
	return Wiz_SingleEntryUnzip(1, 0, 0, 0, &lpDCL, &lpUserFunctions);
}
