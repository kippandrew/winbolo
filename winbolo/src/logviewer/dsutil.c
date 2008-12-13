/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


/* This file is heavily based upon sample code provided by Microsoft as part of the DirectX SDK */

/*  Modifed 27/3/99 By John Morrison to add background sound toggle and custom buffer desciption settings */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include "..\..\bolo\global.h"
#include "..\winbolo.h"
#include "..\dsutil.h"

static const char c_szWAV[] = "WAV";


#pragma pack(1)
typedef struct
{
    BYTE        RIFF[4];          // "RIFF"
    DWORD       dwSize;           // Size of data to follow
    BYTE        WAVE[4];          // "WAVE"
    BYTE        fmt_[4];          // "fmt "
    DWORD       dw16;             // 16
    WORD        wOne_0;           // 1
    WORD        wChnls;           // Number of Channels
    DWORD       dwSRate;          // Sample Rate
    DWORD       BytesPerSec;      // Sample Rate
    WORD        wBlkAlign;        // 1
    WORD        BitsPerSample;    // Sample size
    BYTE        DATA[4];          // "DATA"
    DWORD       dwDSize;          // Number of Samples
} WaveHeader;
#pragma pack()


IDirectSoundBuffer *DSCreateBufferFromWaveFile(IDirectSound *pDS, char *fileName);

///////////////////////////////////////////////////////////////////////////////
//
// DSLoadSoundBufferFile
//
///////////////////////////////////////////////////////////////////////////////

IDirectSoundBuffer *DSLoadSoundBufferFile(IDirectSound *pDS, LPCTSTR lpName)
{
  return DSCreateBufferFromWaveFile(pDS, (char *) lpName);  
}

///////////////////////////////////////////////////////////////////////////////
//
// DSLoadSoundBufferResource
//
///////////////////////////////////////////////////////////////////////////////

IDirectSoundBuffer *DSLoadSoundBufferResource(IDirectSound *pDS, HMODULE hModule, LPCTSTR lpName)
{
    IDirectSoundBuffer *pDSB = NULL;
    DSBUFFERDESC dsBD = {0};
    BYTE *pbWaveData;
    bool backgroundSound;  /* Should we be using background sound */


    backgroundSound = windowGetBackgroundSound();

    if (DSGetWaveResource(hModule, lpName, &dsBD.lpwfxFormat, &pbWaveData, &dsBD.dwBufferBytes))
    {
        dsBD.dwSize = sizeof(dsBD);
        if (backgroundSound == TRUE)
        {
            dsBD.dwFlags = DSBCAPS_STATIC | DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE; //DSBCAPS_GLOBALFOCUS 
        }
        else 
        {
            dsBD.dwFlags = DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE;
        }

        if (SUCCEEDED(IDirectSound_CreateSoundBuffer(pDS, &dsBD, &pDSB, NULL)))
        {
            if (!DSFillSoundBuffer(pDSB, pbWaveData, dsBD.dwBufferBytes))
            {
                IDirectSoundBuffer_Release(pDSB);
                pDSB = NULL;
            }
        }
        else
        {
            pDSB = NULL;
        }
    }

    return pDSB;
}

///////////////////////////////////////////////////////////////////////////////
//
// DSReloadSoundBuffer
//
///////////////////////////////////////////////////////////////////////////////

BOOL DSReloadSoundBuffer(IDirectSoundBuffer *pDSB, HMODULE hModule, LPCTSTR lpName)
/* NOTE: hModule UNUSED */
{
    BOOL result=FALSE;
    BYTE *pbWaveData;
    DWORD cbWaveSize;

    if (DSGetWaveResource(NULL, lpName, NULL, &pbWaveData, &cbWaveSize))
    {
        if (SUCCEEDED(IDirectSoundBuffer_Restore(pDSB)) &&
            DSFillSoundBuffer(pDSB, pbWaveData, cbWaveSize))
	{
	    result = TRUE;
        }
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////
//
// DSGetWaveResource
//
///////////////////////////////////////////////////////////////////////////////

BOOL DSGetWaveResource(HMODULE hModule, LPCTSTR lpName,
    WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData, DWORD *pcbWaveSize)
{
    HRSRC hResInfo;
    HGLOBAL hResData;
    void *pvRes;

    hResInfo = FindResource(hModule, lpName, c_szWAV);
    hResData = LoadResource(hModule, hResInfo);
    pvRes = LockResource(hResData);
    if ((hResInfo != NULL) && (hResData != NULL) && (pvRes != NULL) && DSParseWaveResource(pvRes, ppWaveHeader, ppbWaveData, pcbWaveSize))
    {
        return TRUE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL DSFillSoundBuffer(IDirectSoundBuffer *pDSB, BYTE *pbWaveData, DWORD cbWaveSize)
{
    if (pDSB && pbWaveData && cbWaveSize)
    {
        LPVOID pMem1, pMem2;
        DWORD dwSize1, dwSize2;

        if (SUCCEEDED(IDirectSoundBuffer_Lock(pDSB, 0, cbWaveSize,
            &pMem1, &dwSize1, &pMem2, &dwSize2, 0)))
        {
            CopyMemory(pMem1, pbWaveData, dwSize1);

            if ( 0 != dwSize2 )
                CopyMemory(pMem2, pbWaveData+dwSize1, dwSize2);

            IDirectSoundBuffer_Unlock(pDSB, pMem1, dwSize1, pMem2, dwSize2);
            return TRUE;
        }
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL DSParseWaveResource(void *pvRes, WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData,DWORD *pcbWaveSize)
{
    DWORD *pdw;
    DWORD *pdwEnd;
    DWORD dwRiff;
    DWORD dwType;
    DWORD dwLength;

    if (ppWaveHeader)
        *ppWaveHeader = NULL;

    if (ppbWaveData)
        *ppbWaveData = NULL;

    if (pcbWaveSize)
        *pcbWaveSize = 0;

    pdw = (DWORD *)pvRes;
    dwRiff = *pdw++;
    dwLength = *pdw++;
    dwType = *pdw++;

    if (dwRiff != mmioFOURCC('R', 'I', 'F', 'F'))
        goto exit;      // not even RIFF

    if (dwType != mmioFOURCC('W', 'A', 'V', 'E'))
        goto exit;      // not a WAV

    pdwEnd = (DWORD *)((BYTE *)pdw + dwLength-4);

    while (pdw < pdwEnd)
    {
        dwType = *pdw++;
        dwLength = *pdw++;

        switch (dwType)
        {
        case mmioFOURCC('f', 'm', 't', ' '):
            if (ppWaveHeader && !*ppWaveHeader)
            {
                if (dwLength < sizeof(WAVEFORMAT))
                    goto exit;      // not a WAV

                *ppWaveHeader = (WAVEFORMATEX *)pdw;

                if ((!ppbWaveData || *ppbWaveData) &&
                    (!pcbWaveSize || *pcbWaveSize))
                {
                    return TRUE;
                }
            }
            break;

        case mmioFOURCC('d', 'a', 't', 'a'):
            if ((ppbWaveData && !*ppbWaveData) ||
                (pcbWaveSize && !*pcbWaveSize))
            {
                if (ppbWaveData)
                    *ppbWaveData = (LPBYTE)pdw;

                if (pcbWaveSize)
                    *pcbWaveSize = dwLength;

                if (!ppWaveHeader || *ppWaveHeader)
                    return TRUE;
            }
            break;
        }

        pdw = (DWORD *)((BYTE *)pdw + ((dwLength+1)&~1));
    }

exit:
    return FALSE;
}


//----------------------------------------------------------------------
// 
// Function     : CreateSoundBuffer()
//
// Purpose      : Creates a DirectSound buffer
//
//----------------------------------------------------------------------

BOOL CreateSoundBuffer(IDirectSound *pDS, IDirectSoundBuffer **buff, DWORD dwBufSize, DWORD dwFreq, DWORD dwBitsPerSample, DWORD dwBlkAlign, BOOL bStereo)
{
    PCMWAVEFORMAT pcmwf;
    DSBUFFERDESC dsbdesc;
    bool backgroundSound;  /* Should we be using background sound */

    backgroundSound = windowGetBackgroundSound();


    
    // Set up wave format structure.
    memset( &pcmwf, 0, sizeof(PCMWAVEFORMAT) );
    pcmwf.wf.wFormatTag         = WAVE_FORMAT_PCM;      
    pcmwf.wf.nChannels          = bStereo ? 2 : 1;
    pcmwf.wf.nSamplesPerSec     = dwFreq;
    pcmwf.wf.nBlockAlign        = (WORD)dwBlkAlign;
    pcmwf.wf.nAvgBytesPerSec    = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
    pcmwf.wBitsPerSample        = (WORD)dwBitsPerSample;

    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));  // Zero it out. 
    dsbdesc.dwSize              = sizeof(DSBUFFERDESC);

    
        if (backgroundSound == TRUE)
        {
            dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE; //DSBCAPS_GLOBALFOCUS 
        }
        else 
        {
            dsbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE;
        }
    
    
//    dsbdesc.dwFlags             = DSBCAPS_CTRL3D;               // Needed creation flag for Direct 3D Sound
        dsbdesc.dwBufferBytes       = dwBufSize; 
    dsbdesc.lpwfxFormat         = (LPWAVEFORMATEX)&pcmwf;


    if (SUCCEEDED(IDirectSound_CreateSoundBuffer(pDS, &dsbdesc, buff, NULL))) {
      return TRUE;
    }
    
    // Yahoo!
    return FALSE;
}

//----------------------------------------------------------------------
// 
// Function     : ReadData()
//
// Purpose      : Reads in data from a wave file
//
//----------------------------------------------------------------------

BOOL ReadData(LPDIRECTSOUNDBUFFER lpDSB, FILE* pFile, DWORD dwSize, DWORD dwPos) 
{
    // Lock data in buffer for writing
    LPVOID pData1;
    DWORD  dwData1Size;
    LPVOID pData2;
    DWORD  dwData2Size;
    HRESULT rval;

    // Seek to correct position in file (if necessary)
    if (dwPos != 0xffffffff) 
    {
        if (fseek(pFile, dwPos, SEEK_SET) != 0) 
        {
            return FALSE;
        }
    }


    rval = lpDSB->lpVtbl->Lock(lpDSB, 0, dwSize, &pData1, &dwData1Size, &pData2, &dwData2Size, DSBLOCK_FROMWRITECURSOR);
    if (rval != DS_OK)
    {
        return FALSE;
    }

    // Read in first chunk of data
    if (dwData1Size > 0) 
    {
        if (fread(pData1, dwData1Size, 1, pFile) != 1) 
        {          
                        char holder[256];
                        wsprintf(holder,"Data1 : %d, dwdata: %d, pFile: %d",pData1,dwData1Size,pFile);
                        OutputDebugString(holder);
            return FALSE;
        }
    }

    // read in second chunk if necessary
    if (dwData2Size > 0) 
    {
        if (fread(pData2, dwData2Size, 1, pFile) != 1) 
        {
            return FALSE;
        }
    }

    // Unlock data in buffer
    rval = lpDSB->lpVtbl->Unlock(lpDSB, pData1, dwData1Size, pData2, dwData2Size);
    if (rval != DS_OK)
    {
        return FALSE;
    }

    // Yahoo!
    return TRUE;
}

//----------------------------------------------------------------------
// 
// Function     : CreateSoundBufferFromWaveFile()
//
// Purpose      : Creates a DirectSound buffer from a wave file
//
//----------------------------------------------------------------------

IDirectSoundBuffer *DSCreateBufferFromWaveFile(IDirectSound *pDS, char *fileName) {
    IDirectSoundBuffer *returnValue = NULL;
    BOOL bStereo;
    DWORD dwSize;
    FILE* pFile;
    WaveHeader wavHdr;
  
    // Open the wave file       
    pFile = fopen(fileName,"rb");
    if (pFile == NULL) return FALSE;

    // Read in the wave header          
    
    if (fread(&wavHdr, sizeof(wavHdr), 1, pFile) != 1) 
    {
        fclose(pFile);
        return NULL;
    }

    // Figure out the size of the data region
    dwSize = wavHdr.dwDSize;

    // Is this a stereo or mono file?
    bStereo = wavHdr.wChnls > 1 ? TRUE : FALSE;

    // Create the sound buffer for the wave file
    
    if (CreateSoundBuffer(pDS, &returnValue, dwSize, wavHdr.dwSRate, wavHdr.BitsPerSample, wavHdr.wBlkAlign, bStereo) == FALSE)
    {
        // Close the file
        fclose(pFile);
        
        return NULL;
    }

    // Read the data for the wave file into the sound buffer
    if (!ReadData(returnValue, pFile, dwSize, sizeof(wavHdr))) 
    {
        fclose(pFile);
        return NULL;
    }

    // Close out the wave file
    fclose(pFile);

    // Yahoo!
    return returnValue;
}

BOOL DSReLoadSoundBufferFile(IDirectSoundBuffer *pDSB, LPCTSTR lpFileName) {
    BOOL bStereo;
    DWORD dwSize;
    FILE* pFile;
    WaveHeader wavHdr;
  
    // Open the wave file       
    pFile = fopen(lpFileName,"rb");
    if (pFile == NULL) return FALSE;

    // Read in the wave header          
    
    if (fread(&wavHdr, sizeof(wavHdr), 1, pFile) != 1) 
    {
        fclose(pFile);
        return FALSE;
    }

    // Figure out the size of the data region
    dwSize = wavHdr.dwDSize;

    // Is this a stereo or mono file?
    bStereo = wavHdr.wChnls > 1 ? TRUE : FALSE;

    // Create the sound buffer for the wave file
    
    // Read the data for the wave file into the sound buffer
    if (!ReadData(pDSB, pFile, dwSize, sizeof(wavHdr))) 
    {
        fclose(pFile);
        return FALSE;
    }

    // Close out the wave file
    fclose(pFile);

    // Yahoo!
    return TRUE;
}
