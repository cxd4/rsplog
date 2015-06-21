#ifndef _RCP_H_
#define _RCP_H_

#include <limits.h>

#if (CHAR_BIT < 8) || (UCHAR_MAX < 0xFF)
typedef signed long     s8;
typedef unsigned long   u8;
#else
typedef signed char     s8;
typedef unsigned char   u8;
#endif

#if (UINT_MAX < 0xFFFFFFFFUL) || (INT_MIN > -0x80000000L)
typedef signed long     s32;
typedef unsigned long   u32;
#else
typedef signed int      s32;
typedef unsigned int    u32; /* native ABI to MIPS */
#endif

/*
 * optional name-mangling for external symbols
 */
#if 0
#define DMEM    msp_data
#define IMEM    msp_code
#define DRAM    msp_dram
#endif

#define MAX_SPMEM_ADDR          0x00001FFFUL
#define MAX_RDRAM_ADDR          0x00FFFFFFUL

static u8 DMEM[(MAX_SPMEM_ADDR + 1UL) >> 1];
static u8 IMEM[(MAX_SPMEM_ADDR + 1UL) >> 1];
/* extern u8 DRAM[MAX_RDRAM_ADDR + 1UL]; */

#endif
