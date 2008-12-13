//DirectXAVI.h : AVI code by Tobias Johansson
// Sample code from http://www.flipcode.com/archives/DirectDraw_AVI_Writer.shtml

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DIRECTXAVI_
#define _DIRECTXAVI_

//#include "Core.h"
#include <windows.h>
#include <ddraw.h>
#include <vfw.h> //The VideoForWindows header file, Vfw32.lib must also be linked

////////////////////////////////////////////////
//
// WriteAVIFile - Open AVI streams
//
// Parameters: 
//   char *fname: name of avi file
//   lpSurf: DirectDraw surface to capture video from
//   int FramesPerSecond: The frame frequency of the video (25-30 is normal)
//
// Return 1 is succesful
// Other value are error values
//////////////////////////////////////////////////
#define AVI_ERROR_WRONGSURFACE           55
#define AVI_ERROR_UNSUPPORTEDPIXELFORMAT 66
#define AVI_ERROR_OTHERERROR             77

int WriteAVIFile( char *fname, LPDIRECTDRAWSURFACE lpSurf, int FramesPerSecond );

/////////////////////////////////////////////////
//
// CloseAVI - End video capture and closes avi streams
//
/////////////////////////////////////////////////

void CloseAVIFile();

//////////////////////////////////////////////////
//
// WriteFrame - Write a frame to the avi file
//
// Parameters: 
//  lpSurf: ddraw surface for read from, preferably same surf as in WriteAVIFile
//
// Return 1 if succesful
// Other values are error values
/////////////////////////////////////////

int WriteFrame( LPDIRECTDRAWSURFACE lpSurf );

#endif