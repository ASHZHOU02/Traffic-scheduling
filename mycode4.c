
#include "aux.h"
#include "sys.h"
#include "mycode4.h"

#define FALSE 0
#define TRUE 1



static struct {
	int valid;	
	int value;	// value of semaphore
	int procList[MAXPROCS];
	int topIndex;
	int backIndex;
} semtab[MAXSEMS];



/* 	InitSem () is called when the kernel starts running.  Initialize data
 *  	structures (such as the semaphore table) and call any initialization
 *   	functions here. 
 */

void InitSem ()
{
	int s;
	int i;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {		// mark all sems free
		semtab[s].valid = FALSE;
		semtab[s].topIndex = 0;
		semtab[s].backIndex = 0;
		for(i = 0; i < MAXPROCS; i++){
			semtab[s].procList[i] = -1;
		}
	}

}

/* 	MySeminit (v) is called by the kernel whenever the system call
 *  	Seminit (v) is called.  The kernel passes the initial value v. 
 * 	MySeminit should allocate a semaphore (find a free entry in
 *  	semtab and allocate), initialize that semaphore's value to v,
 * 	and then return the ID (i.e., index of the allocated entry).  
 *  	Should return -1 if it fails (e.g., no free semaphores). 
 */

int MySeminit (int v)
	// v: initial value of semaphore
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {
		if (semtab[s].valid == FALSE) {
			break;
		}
	}
	if (s == MAXSEMS) {
		DPrintf ("No free semaphores\n");
		return (-1);
	}

	semtab[s].valid = TRUE;
	semtab[s].value = v;

	return (s);
}

/*   	MyWait (s) is called by the kernel whenever the system call
 * 	Wait (s) is called.  Return 0 if successful, else -1 if failed. 
 */

int MyWait (int s)
	// s: semaphore ID
{

	semtab[s].value--;

	if(semtab[s].value < 0) {
		int currProc = GetCurProc();
		int end = semtab[s].backIndex;
		semtab[s].procList[end] = currProc;
		semtab[s].backIndex = (semtab[s].backIndex + 1) % MAXPROCS;
		Block();
		return 0;
	}


	return (-1);
}

/*  	MySignal (s) is called by the kernel whenever the system call
 * 	Signal (s) is called.  Return 0 if successful, else -1 if failed. 
 */

int MySignal (int s)
	// s: semaphore ID
{

	semtab[s].value++;
	int start = semtab[s].topIndex;

	if(semtab[s].procList[start] != -1){
		int p = semtab[s].procList[start];
		Unblock(p);
		semtab[s].topIndex = (semtab[s].topIndex + 1) % MAXPROCS;
		return 0;
	}

	return (-1);
}
