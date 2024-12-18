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
typedef unsigned int*				img;


#define FRACTAL_SUCCESS			0x1
#define FRACTAL_FAILURE			0x0
#define FRACTAL_PROCESS			0x4
#define FRACTAL_PIXEL				0x200
#define FRACTAL_SPACE				FRACTAL_PIXEL * FRACTAL_PIXEL * sizeof (u32)
#define FRACTAL_CHANNELS		2
#define FRACTAL_SEMAPHORES	5

typedef int  								plt [FRACTAL_PROCESS];		

typedef struct {
	struct {
		u32 (*get) (nil*, str, s08);
		s32 d;
	} key;
	struct {
		u32 (*new) (nil*);
		u32 (*del) (nil*);
		u32 (*inc) (nil*, u32);
		u32 (*dec) (nil*, u32);
		u32 (*set) (nil*, u32);
		u32 (*get) (nil*, u32);
		s32 d;
		u32 n;
		s32 v;
	} sem;
	struct {
		u32 (*get) (s32*, s32*);
		u32 (*del) (s32*);
		u32 (*map) (buf*, s32*);
		u32 (*rem) (buf*);
		s32 d;
		buf b;
	} shm;
	struct {
		u64 pro; // producer
		u64 con; // consumer
	} pip;
} ctx;

#endif
