#include "program.h"

/*************************************************************************
 *
 * FUN, DEC, LOC: KEY
 * 
 *************************************************************************/

static u32 key_get (s32*, str, s08);

/*************************************************************************
 *
 * FUN, DEC, LOC: SEMAPHORE
 * 
 *************************************************************************/

static u32 sem_new (s32*, s32);
static u32 sem_del (s32*);
static u32 sem_inc (s32*);
static u32 sem_dec (s32*);
static u32 sem_set (s32*, s32*);
static u32 sem_get (s32*, s32*);

/*************************************************************************
 *
 * FUN, DEC, LOC: SHARED MEMORY
 * 
 *************************************************************************/

static u32 shm_get (s32*, s32*);
static u32 shm_del (s32*);
static u32 shm_map (buf*, s32*);
static u32 shm_rem (buf*);

/*************************************************************************
 *
 * FUN, DEF, LOC: WORK
 * 
 *************************************************************************/

static s32 work (s32*, s32*);

/*************************************************************************
 *
 * FUN, DEP, LOC: MAIN
 * 
 *************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

/*************************************************************************
 *
 * FUN, DEF, LOC: MAIN
 * 
 *************************************************************************/

#define MAIN_VERSION 'a'

int main (int argc, char ** argv)
{
	s32 r = EXIT_SUCCESS;
	s32 k = 0;
	s32 s = 0;
	s32 v = FRACTAL_PROCESS;
	s32 m = 0;
	buf b = 0;
	plt t = {0};

	if (FRACTAL_FAILURE == key_get (&k, *argv, MAIN_VERSION))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == sem_new (&s, k))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == sem_set (&s, &v))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == shm_get (&m, &k))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == shm_map (&b, &m))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == shm_rem (&b))
	{
		goto FAILURE;
	}

	for (u32 i = 0; i < FRACTAL_PROCESS; i++)
	{
		if (! (t [i] = fork ()))
		{
			exit (work (&s, &m));
		}
	}

	for (s32 w = 0; w != FRACTAL_PROCESS; sem_get (&s, &w))
	{
		sleep (1); printf ("[DEBUG] %i\n", w); fflush (stdout);
	}

	for (u32 i = 0; i < FRACTAL_PROCESS; i++)
	{
		if (t [i] != waitpid (t [i], 0, 0))
		{
			printf ("[DEBUG] %i failed\n", t [i]);
		}
	}

	if (FRACTAL_FAILURE == sem_del (&s))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == shm_del (&m))
	{
		goto FAILURE;
	}

	goto SUCCESS;

FAILURE:

	r = EXIT_FAILURE;

	if (errno)
	{
		printf ("[ERROR] %s\n", strerror (errno));
	}

	r = sem_del (0) & sem_dec (0) & sem_inc (0);

SUCCESS:

	return r;
}

/*************************************************************************
 *
 * FUN, DEP, LOC: KEYS
 * 
 *************************************************************************/

#include <sys/ipc.h>

/*************************************************************************
 *
 * FUN, DEF, LOC: KEYS
 * 
 *************************************************************************/

static u32 key_get (s32* k, str f, s08 v)
{
	u32 r = FRACTAL_SUCCESS;
	s32 u = 0;

	if (! k)
	{
		goto FAILURE;
	}

	*k = 0;

	if (0 > (u = ftok (f, v)))
	{
		goto FAILURE;
	}

	*k = u; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

/*************************************************************************
 *
 * FUN, DEP, LOC: SEMAPHORE
 * 
 *************************************************************************/

#include <sys/sem.h>

#define SEMAPHORE_FLAG 0666 | IPC_CREAT

/*************************************************************************
 *
 * FUN, DEF, LOC: SEMAPHORE
 * 
 *************************************************************************/

static u32 sem_new (s32* s, s32 k)
{
	u32 r = FRACTAL_SUCCESS;
	s32 u = 0;

	if (! s)
	{
		goto FAILURE;
	}

	if (0 > (u = semget (k, 1, 0)))
	{
		if (0 > (u = semget (k, 1, SEMAPHORE_FLAG)))
		{
			goto FAILURE;
		}
	}

	*s = u; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:


	return r;
}

static u32 sem_del (s32* s)
{
	u32 r = FRACTAL_SUCCESS;

	if (! s)
	{
		goto FAILURE;
	}

	if (0 > *s)
	{
		goto FAILURE;
	}

	if (0 > semctl (*s, 1, IPC_RMID, 0))
	{
		goto FAILURE;
	}

	*s = 0; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_inc (s32* s)
{
	u32 					r = FRACTAL_SUCCESS;
	s32 					u = 0;
	struct sembuf v = {0, 0, 0};

	if (! s)
	{
		goto FAILURE;
	}

	if (0 > *s)
	{
		goto FAILURE;
	}

	if (0 > (u = semctl (*s, 0, GETVAL, 0)))
	{
		goto FAILURE;
	}

	if (u >= FRACTAL_PROCESS)
	{
		goto FAILURE;
	}

	v.sem_num = 0;
	v.sem_op	= 1;
	v.sem_flg	= 0;

	if (0 > semop (*s, &v, 1))
	{
		goto FAILURE;
	}

	goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_dec (s32* s)
{
	u32 					r = FRACTAL_SUCCESS;
	s32 					u = 0;
	struct sembuf v = {0, 0, 0};

	if (! s)
	{
		goto FAILURE;
	}

	if (0 > *s)
	{
		goto FAILURE;
	}

	if (0 > (u = semctl (*s, 0, GETVAL, 0)))
	{
		goto FAILURE;
	}

	if (! u)
	{
		goto FAILURE;
	}

	v.sem_num =  0;
	v.sem_op	= -1;
	v.sem_flg	=  0;

	if (0 > semop (*s, &v, 1))
	{
		goto FAILURE;
	}

	goto SUCCESS;


FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_set (s32* s, s32* v)
{
	u32 r = FRACTAL_SUCCESS;
	s32 u = 0;

	if (! s)
	{
		goto FAILURE;
	}

	if (0 > *s)
	{
		goto FAILURE;
	}

	if (! v)
	{
		goto FAILURE;
	}

	u = *v;

	if (0 > semctl (*s, 0, SETVAL, u))
	{
		goto FAILURE;
	}

	*v = 0; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_get (s32* s, s32* v)
{
	u32 r = FRACTAL_SUCCESS;
	s32 u = 0;

	if (! s)
	{
		goto FAILURE;
	}

	if (0 > *s)
	{
		goto FAILURE;
	}

	if (! v)
	{
		goto FAILURE;
	}

	if (0 > (u = semctl (*s, 0, GETVAL, 0)))
	{
		goto FAILURE;
	}

	*v = u; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

/*************************************************************************
 *
 * FUN, DEP, LOC: SHARED MEMORY
 * 
 *************************************************************************/

#include <sys/shm.h>

#define SHAREDMEM_FLAG 0666 | IPC_CREAT

/*************************************************************************
 *
 * FUN, DEF, LOC: SHARED MEMORY
 * 
 *************************************************************************/

static u32 shm_get (s32* m, s32* k)
{
	u08 r = FRACTAL_SUCCESS;
	s32 u = 0;

	if (! m)
	{
		goto FAILURE;
	}

	if (! k)
	{
		goto FAILURE;
	}

	if (0 > *k)
	{
		goto FAILURE;
	}

	if (0 > (u = shmget (*k, FRACTAL_SPACE, 0)))
	{
		if (0 > (u = shmget (*k, FRACTAL_SPACE, SHAREDMEM_FLAG)))
		{
			goto FAILURE;
		}
	}

	*m = u; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 shm_del (s32* s)
{
	u32 r = FRACTAL_SUCCESS;

	if (! s)
	{
		goto FAILURE;
	}

	if (0 > *s)
	{
		goto FAILURE;
	}

	if (0 > shmctl (*s, IPC_RMID, 0))
	{
		goto FAILURE;
	}

	*s = 0; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 shm_map (buf* b, s32* m)
{
	u32 r = FRACTAL_SUCCESS;
	buf u = 0;


	if (! b)
	{
		goto FAILURE;
	}

	if (! m)
	{
		goto FAILURE;
	}

	if (0 > *m)
	{
		goto FAILURE;
	}

	if ((buf)0 > (u = (shmat (*m, 0, 0))))
	{
		goto FAILURE;
	}

	*b = u; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 shm_rem (buf* b)
{
	u32 r = FRACTAL_SUCCESS;

	if (! b)
	{
		goto FAILURE;
	}

	if ((buf)0 > *b)
	{
		goto FAILURE;
	}

	if (0 > shmdt (*b))
	{
		goto FAILURE;
	}

	*b = 0; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;

}

/*************************************************************************
 *
 * FUN, DEF, LOC: WORK
 * 
 *************************************************************************/

static s32 work (s32* s, s32* m)
{
	s32 r = EXIT_SUCCESS;
	s32 u = 0;
	buf v = 0;

	if ((! s) || (0 > *s))
	{
		goto FAILURE;
	}

	if ((! m) || (0 > *m))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == sem_dec (s))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == sem_get (s, &u))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == shm_map (&v, m))
	{
		goto FAILURE;
	}

	sleep (getpid () % FRACTAL_PROCESS + 4);

	if (FRACTAL_FAILURE == sem_inc (s))
	{
		goto FAILURE;
	}

	goto SUCCESS;


FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	if (FRACTAL_FAILURE == shm_rem (&v))
	{
		r = FRACTAL_FAILURE;
	}

	return r;
}
