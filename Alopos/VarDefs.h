#pragma once
// Variable definitions

#ifndef __cplusplus

#define true	1
#define false	0
typedef _Bool Bool;

#else // __cplusplus

#define Bool	bool

#endif /* __cplusplus */

#if defined(__GNUC__)
typedef __UINT8_TYPE__ Uint8;
typedef __UINT8_TYPE__ Byte;

typedef __UINT16_TYPE__ Uint16;
typedef __UINT16_TYPE__ Byte2;

typedef __UINT32_TYPE__ Uint32;
typedef __UINT32_TYPE__ Byte4;

typedef __UINT64_TYPE__ Uint64;
typedef __UINT64_TYPE__ Byte8;

typedef __SIZE_TYPE__ Index;
#else
typedef unsigned __int8 Uint8;
typedef unsigned __int8 Byte;

typedef unsigned __int16 Uint16;
typedef unsigned __int16 Byte2;

typedef unsigned __int32 Uint32;
typedef unsigned __int32 Byte4;

typedef unsigned __int64 Uint64;
typedef unsigned __int64 Byte8;

typedef unsigned int Index;
#endif

