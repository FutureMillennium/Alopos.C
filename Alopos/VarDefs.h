#pragma once

#ifndef __cplusplus

#define bool	_Bool
#define true	1
#define false	0

#else // __cplusplus

#define _Bool	bool
#define bool	bool
#define false	false
#define true	true

#endif /* __cplusplus */


typedef __UINT8_TYPE__ Uint8;
typedef __UINT8_TYPE__ Byte;

typedef __UINT16_TYPE__ Uint16;
typedef __UINT16_TYPE__ Byte2;

typedef __SIZE_TYPE__ Index;
