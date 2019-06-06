/****************************************************************************
*
*                         The Universal VESA VBE
*
*                   Copyright (C) 1996 SciTech Software.
*                           All rights reserved.
*
* Filename:     $Workfile:   uvbelib.h  $
* Version:      $Revision:   1.2  $
*
* Language:     ANSI C
* Environment:  MS-DOS/WinDirect
*
* Description:  Header file for the UVBELib(tm) functions.
*
* $Date:   19 Jun 1996 16:42:12  $ $Author:   KendallB  $
*
****************************************************************************/

#ifndef __UVBELIB_H
#define __UVBELIB_H

#ifndef	__DEBUG_H
#include "debug.h"
#endif

/*--------------------- Macros and type definitiosn -----------------------*/

typedef enum {
	/*--------------------------------------------------------------------
	 * Non-fatal return codes
	 *------------------------------------------------------------------*/
	UV_ok,				/* UVBELib successfully installed the driver	*/
	UV_noDetect,		/* Driver file is configured for standard VGA	*/
	UV_alreadyVBE20,	/* VBE 2.0 detected and ignoreVBE was false		*/
	UV_laterVersion,	/* A later TSR version of UniVBE is installed	*/

	UV_lastNonFatal,	/* Last non fatal error code					*/

	/*--------------------------------------------------------------------
	 * Fatal return codes
	 *------------------------------------------------------------------*/
	UV_errNotFound,		/* Could not find UNIVBE.DRV driver file		*/
	UV_errNotValid,		/* File loaded not a valid driver file			*/
	UV_errOldVersion,	/* Attempt to load older driver file version	*/
	UV_errNoMemory,		/* Could not allocate memory to load driver		*/
	UV_errNoRealMemory,	/* Could not allocate real mode memory			*/

	/*--------------------------------------------------------------------
	 * Fatal internal return codes. Contact SciTech if you get any of
	 * these as these indicate configuration problems that should not
	 * occur.
	 *------------------------------------------------------------------*/
	UV_errGenerate,		/* Error generating UNIVBE.DRV driver file		*/
	UV_errCheckInstall,	/* Check install function failed				*/
	} UV_installCodes;

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {            			/* Use "C" linkage when in C++ mode */
#endif

/* Routines to install the UVBELib VBE 2.0 device support routines */

int	_ASMAPI UV_install(char *driverDir,bool ignoreVBE,bool analyseModes);
void	_ASMAPI UV_exit(void);
void    _ASMAPI UV_getDriverPath(char *argv0,char *buf);

/* Get error message and copyright strings */

char * 	_ASMAPI UV_getErrorStr(int err);
char * 	_ASMAPI UV_getCopyrightStr(void);
char * 	_ASMAPI UV_getMajorVersion(void);
char * 	_ASMAPI UV_getMinorVersion(void);
char * 	_ASMAPI UV_getReleaseDate(void);

/* Get configuration strings */

char * 	_ASMAPI UV_getSuperVGAStr(void);
char * 	_ASMAPI UV_getDACStr(void);
char * 	_ASMAPI UV_getClockStr(void);

#ifdef  __cplusplus
}                       			/* End of "C" linkage for C++   	*/
#endif

#endif  /* __UVBELIB_H */
