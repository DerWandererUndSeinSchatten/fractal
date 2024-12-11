#ifndef PROGRAM_H
#define PROGRAM_H

typedef unsigned char 			u08;
typedef unsigned short 			u16;
typedef unsigned int 				u32;
typedef unsigned long long 	u64;
typedef char 								s08;
typedef short 							s16;
typedef int 								s32;
typedef long long int 			s64;
typedef float								f32;
typedef double							f64;
typedef void								nil;
typedef char*								str;
typedef unsigned char*			buf;


#define FRACTAL_SUCCESS			0x1
#define FRACTAL_FAILURE			0x0
#define FRACTAL_PROCESS			0x4
#define FRACTAL_PIXEL				0x200
#define FRACTAL_SPACE				FRACTAL_PIXEL * FRACTAL_PIXEL * sizeof (u32)

typedef int  								plt [FRACTAL_PROCESS];		

#endif
