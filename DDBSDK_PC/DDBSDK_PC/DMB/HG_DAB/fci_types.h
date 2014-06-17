/*****************************************************************************
 * Copyright(c) 2009 FCI Inc. All Rights Reserved
 *
 * File name : fci_types.h
 *
 * Description :
 *
 * History :
 * ----------------------------------------------------------------------
 * 2009/08/31  jason		initial
 *******************************************************************************/

#ifndef __FCI_TYPES_H__
#define __FCI_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HANDLE
typedef void * HANDLE;
#endif

#ifndef u8

#define s8         char
#define s16        short int
#define s32        int

#define u8         unsigned char
#define u16        unsigned short
#define u32        unsigned int

#define ulong      unsigned long

#define UINT32     unsigned int
#define UINT16     unsigned short
#define UINT8      unsigned char

#define LWORD      unsigned int
#define WORD       unsigned short
#define BYTE       unsigned char

#define UCLWORD    unsigned int                         // Copy form SM342 SDK
#define UCWORD     unsigned short                       // If the system is SM342, it need to modify
#define UCBYTE     unsigned char

#endif


#ifndef TRUE
 #define TRUE       1
 #define FALSE      0
#endif

#ifndef NULL
 #define NULL      0
#endif

#define BBM_OK     0
#define BBM_NOK    1

#ifdef __cplusplus
}
#endif

#endif /* __FCI_TYPES_H__ */
