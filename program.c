#include "program.h"

/*************************************************************************
 *
 * FUN, DEC, LOC: JMP
 * 
 *************************************************************************/

static nil jmp_sig (s32);

/*************************************************************************
 *
 * DEP, DEC, LOC: JMP
 * 
 *************************************************************************/

#include <setjmp.h>

/*************************************************************************
 *
 * VAR, DEC, LOC: JMP
 * 
 *************************************************************************/

static jmp_buf f_jmp;

/*************************************************************************
 *
 * FUN, DEC, LOC: KEY
 * 
 *************************************************************************/

static u32 key_get (nil*, str, s08);

/*************************************************************************
 *
 * FUN, DEC, LOC: SEMAPHORE
 * 
 *************************************************************************/

static u32 sem_new (nil*);
static u32 sem_del (nil*);
static u32 sem_inc (nil*, u32);
static u32 sem_dec (nil*, u32);
static u32 sem_set (nil*, u32);
static u32 sem_get (nil*, u32);

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

static s32 work (ctx*, s32*, s32*, u32);

static s32 loop (ctx*, s32*, s32*);

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
	plt t = {0};
	ctx c;

	c.key.get = key_get;
	c.sem.new = sem_new;
	c.sem.del = sem_del;
	c.sem.inc = sem_inc;
	c.sem.dec = sem_dec;
	c.sem.get = sem_get;
	c.sem.set = sem_set;
	c.sem.n		= FRACTAL_PROCESS + 1;
	c.shm.get = shm_get;
	c.shm.del = shm_del;
	c.shm.map = shm_map;
	c.shm.rem = shm_rem;
	c.shm.d		= 0;
	c.shm.b		= 0;

	if (FRACTAL_FAILURE == c.key.get (&c, *argv, MAIN_VERSION))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == c.sem.new (&c))
	{
		goto FAILURE;
	}

	for (int i = 0; i < FRACTAL_PROCESS; i++)
	{
		c.sem.v = 1;

		if (FRACTAL_FAILURE == c.sem.set (&c, i))
		{
			goto FAILURE;
		}
	}

	c.sem.v = FRACTAL_PROCESS;

	if (FRACTAL_FAILURE == c.sem.set (&c, FRACTAL_PROCESS))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == c.shm.get (&(c.shm.d), &(c.key.d)))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == c.shm.map (&(c.shm.b), &(c.shm.d)))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == c.shm.rem (&(c.shm.b)))
	{
		goto FAILURE;
	}

	for (u32 i = 0; i < FRACTAL_PROCESS; i++)
	{
		if (! (t [i] = fork ()))
		{
			exit (work (&c, &(c.sem.d), &(c.shm.d), i));
		}
	}

	if (FRACTAL_FAILURE == loop (&c, &(c.sem.d), &(c.shm.d)))
	{
		printf ("[DEBUG] loop ended\n");
	}

	for (u32 i = 0; i < FRACTAL_PROCESS; i++)
	{
		if (t [i] != waitpid (t [i], 0, 0))
		{
			printf ("[DEBUG] %i failed\n", t [i]);
		}
	}

	if (FRACTAL_FAILURE == c.sem.del (&c))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == shm_del (&(c.shm.d)))
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

static u32 key_get (nil* p, str f, s08 v)
{
	u32 	r = FRACTAL_SUCCESS;
	ctx*	c = 0;

	if (! p)
	{
		goto FAILURE;
	}

	c = p;

	if (0 > (c->key.d = ftok (f, v)))
	{
		goto FAILURE;
	}

	goto SUCCESS;

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

static u32 sem_new (nil* p)
{
	u32 	r = FRACTAL_SUCCESS;
	ctx* 	c = 0;

	if (! p)
	{
		goto FAILURE;
	}

	c = p;

	if (0 > (c->sem.d = semget (c->key.d, c->sem.n, 0)))
	{
		if (0 > (c->sem.d = semget (c->key.d, c->sem.n, SEMAPHORE_FLAG)))
		{
			goto FAILURE;
		}
	}

	goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:


	return r;
}

static u32 sem_del (nil* p)
{
	u32 	r = FRACTAL_SUCCESS;
	ctx* 	c = 0;

	if (! p)
	{
		goto FAILURE;
	}

	c = p;

	if (0 > c->sem.d)
	{
		goto FAILURE;
	}

	if (0 > semctl (c->sem.d, 1, IPC_RMID, 0))
	{
		goto FAILURE;
	}

	c->sem.d = 0; goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_inc (nil* p, u32 n)
{
					u32 		r = FRACTAL_SUCCESS;
	struct 	sembuf 	v = {0, 0, 0};
					ctx*		c = 0;

	if (! p)
	{
		goto FAILURE;
	}

	c = p;

	if (0 > c->sem.d)
	{
		goto FAILURE;
	}

	v.sem_num = n;
	v.sem_op	= 1;
	v.sem_flg	= 0;

	if (0 > semop (c->sem.d, &v, 1))
	{
		goto FAILURE;
	}

	goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_dec (nil* p, u32 n)
{
					u32 		r = FRACTAL_SUCCESS;
	struct 	sembuf	v = {0, 0, 0};
					ctx*		c = 0;

	if (! p)
	{
		goto FAILURE;
	}

	c = p;

	if (0 > c->sem.d)
	{
		goto FAILURE;
	}

	v.sem_num =  n;
	v.sem_op	= -1;
	v.sem_flg	=  0;

	if (0 > semop (c->sem.d, &v, 1))
	{
		goto FAILURE;
	}

	goto SUCCESS;


FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_set (nil* p, u32 n)
{
	u32 	r = FRACTAL_SUCCESS;
	ctx*	c = 0;

	if (! p)
	{
		goto FAILURE;
	}

	c = p;

	if (0 > c->sem.d)
	{
		goto FAILURE;
	}


	if (0 > semctl (c->sem.d, n, SETVAL, c->sem.v))
	{
		goto FAILURE;
	}

	goto SUCCESS;

FAILURE:

	r = FRACTAL_FAILURE;

SUCCESS:

	return r;
}

static u32 sem_get (nil* p, u32 n)
{
	u32 	r = FRACTAL_SUCCESS;
	ctx* 	c = 0;

	if (! p)
	{
		goto FAILURE;
	}

	c = p;

	if (0 > (c->sem.d))
	{
		goto FAILURE;
	}

	if (0 > (c->sem.v = semctl (c->sem.d, n, GETVAL, 0)))
	{
		goto FAILURE;
	}

	goto SUCCESS;

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

#include <string.h>
#include <errno.h>

static s32 work (ctx* c, s32* s, s32* m, u32 p)
{
	s32 r = EXIT_SUCCESS;
	buf v = 0;

	if ((! s) || (0 > *s))
	{
		goto FAILURE;
	}

	if ((! m) || (0 > *m))
	{
		goto FAILURE;
	}

	signal (SIGSEGV	, jmp_sig);
	signal (SIGINT	, jmp_sig);

	if (setjmp (f_jmp))
	{
		goto FAILURE;
	}

	if (FRACTAL_FAILURE == c->shm.map (&v, m))
	{
		goto FAILURE;
	}

	for (;;)
	{
		if (FRACTAL_FAILURE == c->sem.dec (c, p))
		{
			goto FAILURE;
		}

		if (FRACTAL_FAILURE == c->sem.get (c, p))
		{
			goto FAILURE;
		}

		sleep (1);

		if (FRACTAL_FAILURE == c->sem.dec (c, FRACTAL_PROCESS))
		{
			goto FAILURE;
		}

		if (FRACTAL_FAILURE == c->sem.get (c, FRACTAL_PROCESS))
		{
			goto FAILURE;
		}

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

/*************************************************************************
 *
 * DEP, DEF, LOC: WORK
 * 
 *************************************************************************/

#include <signal.h>

/*************************************************************************
 *
 * FUN, DEF, LOC: WORK
 * 
 *************************************************************************/

static s32 loop (ctx* c, s32* s, s32* m)
{
	s32 r = FRACTAL_SUCCESS;
	s32 u = 0;

	if (! s)
	{
		goto FAILURE;
	}

	if (! m)
	{
		goto FAILURE;
	}

	signal (SIGSEGV	, jmp_sig);
	signal (SIGINT	, jmp_sig);

	if (setjmp (f_jmp))
	{
		goto FAILURE;
	}

	for (;;)
	{
		if (FRACTAL_FAILURE == c->sem.get (c, FRACTAL_PROCESS))
		{
			goto FAILURE;
		}

		printf ("u: %i\n", u); sleep (1);

		if (u)
		{
			continue;
		}

		for (s32 i = 0; i < FRACTAL_PROCESS; i++)
		{

			if (FRACTAL_FAILURE == c->sem.get (c, i))
			{
				goto FAILURE;
			}
			
			printf ("p: %i %i\n", i, u);
			
			if (u)
			{
				continue;
			}

			if (FRACTAL_FAILURE == c->sem.inc (c, i))
			{
				goto FAILURE;
			}

			if (FRACTAL_FAILURE == c->sem.inc (c, FRACTAL_PROCESS))
			{
				goto FAILURE;
			}
		}

	}
	
	goto SUCCESS;

	FAILURE:

		r = FRACTAL_FAILURE;

	SUCCESS:

	return r;
}

/*************************************************************************
 *
 * FUN, DEF, LOC: JMP
 * 
 *************************************************************************/

static nil jmp_sig (s32 sig)
{
	longjmp (f_jmp, 0x1234);
}
