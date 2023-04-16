/* Copyright 1989-2023, Joseph Pasquale, University of California, San Diego
 *
 *	mycode interface for pa4
 */

void InitSem();				// initialize semaphores
int MySeminit(int v);			// alloc sem, init to v, return semid
int MyWait(int s);			// wait using sem s */
int MySignal(int s);			// signal using sem s */
