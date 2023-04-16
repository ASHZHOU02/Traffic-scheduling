/* Copyright 1989-2023, Joseph Pasquale, University of California, San Diego
 *
 *	UMIX kernel-related major constants and function interfaces
 *
 *	Relevant only to kernel code (i.e., NOT to user processes).
 *
 *	The constants should NOT to be changed, unless explicitly stated
 *	otherwise, as other already-compiled parts of the kernel rely on
 *	these constants.
 *
 *	v2.1
 */

#include <setjmp.h>

/*	Processes
 */
#define MAXPROCS	10	// max processes: DO NOT CHANGE

/*	Shared Memory
 */
#define MAXSHM		1024	// max shared memory size (bytes): DO NOT CHANGE

/*	Semaphores
 */
#define MAXSEMS		100	// max semaphores: DO NOT CHANGE

/*	Scheduling
 */
#define NOSCHEDPOLICY	0
#define ARBITRARY	1	// choose arbitrarily (any process is OK)
#define FIFO		2
#define LIFO		3
#define ROUNDROBIN	4
#define PROPORTIONAL	5

/*	Kernel procedures: can only be called from within the kernel
 */

/*	PA1: Context switching
 */

typedef struct {
        int pid;		// process ID
        jmp_buf env;		// jmp_buf
} CONTEXT;

int SwitchContext(int p);	// switch context to process p
int GetCurProc();		// returns current process's ID
void SetCurProc(int p);		// DO NOT USE: HERE ONLY FOR RestoreContext

				// SaveContext/RestoreContext must be MACROs
#define SaveContext(C)		setjmp ((C)->env)
#define RestoreContext(C)	longjmp ((C)->env, 1)

/*	PA2: Thread System
 */

/*	PA3: Scheduling
 */
void SetTimer(int t);		// timer to interrupt after t ticks
int GetTimer();			// returns the current value of timer
int SetSchedPolicy(int p);	// set the scheduling policy to p
int GetSchedPolicy();		// return the scheduling policy
void DoSched();			// make sched decision at next opportune time

/*	PA4: Synchronization
 */
int Block();			// make curr process ineligible for scheduling
int Unblock(int p);		// make process p eligible for scheduling

/*	Other
 */
void Panic(char *s);		// causes abnormal exit from kernel, print s
