#ifndef BIT_DEPTH_DEFINES_INCLUDED
#define BIT_DEPTH_DEFINES_INCLUDED
#include <cstdint>

// change this to alternate between 32 and 64 bits of operation.
/////////////////////////////
#define EFVIEMOM_BIT_DEPTH 32
/////////////////////////////

#if EFVIEMOM_BIT_DEPTH == 32

#define SIZEOF_T		4

#define FLOAT_T			float
#define INT_T			int32_t
#define UINT_T			uint32_t

#define AF_COMPLEX_T	c32
#define AF_FLOAT_T		f32
#define AF_INT_T		s32
#define AF_UINT_T		u32

#elif EFVIEMOM_BIT_DEPTH == 64

#define SIZEOF_T		8

#define FLOAT_T			double
#define INT_T			int64_t
#define UINT_T			uint64_t

#define AF_COMPLEX_T	c64
#define AF_FLOAT_T		f64
#define AF_INT_T		s64
#define AF_UINT_T		u64

#endif

#endif