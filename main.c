/* 
 * constrains：
 *
 * 1) Modify the procedure driveRoad such that the following rules are obeyed:
 *
 *   	A) Avoid all collisions. 
 *
 * 	B) Multiple cars should be allowed on the road, as long as they are
 *  	traveling in the same direction.  
 *
 * 	C) If a car arrives and there are already other cars going IN THE
 *  	SAME DIRECTION, the arriving car should be allowed enter as soon as it
 * 	can. Two situations might prevent this car from entering immediately:
 *  	(1) there is a car immediately in front of it (going in the same
 *   	direction), and if it enters it will crash (which breaks rule A);
 * 	(2) one or more cars have arrived at the other end to travel in the
 *  	opposite direction and are waiting for the current cars on the road
 * 	to exit, which is covered by the next rule.  
 *
 *  	D) If a car arrives and there are already other cars traveling in the
 * 	OPPOSITE DIRECTION, the arriving car must wait until all these other
 *  	cars complete their course over the road and exit. It should only wait
 *   	for the cars already on the road to exit; no new cars traveling in the
 * 	same direction as the existing ones should be allowed to enter.  
 *
 *  	E) The previous rule implies that if there are multiple cars at each
 * 	end waiting to enter the road, each side will take turns in allowing
 *  	one car to enter and exit. However, if there are no cars waiting at
 * 	one end, then as cars arrive at the other end, they should be allowed
 *  	to enter the road immediately.  
 *
 *   	F) If the road is free (no cars), then any car attempting to enter
 * 	should not be prevented from doing so. 
 *
 *  	G) All starvation must be avoided.  For example, any car that is
 * 	waiting must eventually be allowed to proceed. 
 *
 * IMPLEMENTATION GUIDELINES
 * 
 * 1) Avoid busy waiting.  In class one of the reasons given for using
 * semaphores was to avoid busy waiting in user code and limit it to
 * minimal use in certain parts of the kernel. This is because busy
 * waiting uses up CPU time, but a blocked process does not.  You have
 * semaphores available to implement the driveRoad function, so you
 * should not use busy waiting anywhere. 
 *
 * 2) Prevent race conditions.  One reason for using semaphores is to
 * enforce mutual exclusion on critical sections to avoid race conditions. 
 * You will be using shared memory in your driveRoad implementation.  
 * Identify the places in your code where there may be race conditions
 * (the result of a computation on shared memory depends on the order
 * that processes execute). Prevent these race conditions from occurring
 * by using semaphores.  
 *
 * 3) Implement semaphores fully and robustly. It is possible for your
 * driveRoad function to work with an incorrect implementation of
 * semaphores because controlling cars may not exercise every use of
 * semaphores.  You will be penalized if your semaphores are not correctly
 * implemented, even if your driveRoad works. 
 *
 * 4) Control cars with semaphores: Semaphores should be the basic
 * mechanism for enforcing the rules on driving cars.  You should not
 * force cars to delay in other ways inside driveRoad such as by calling
 * the Delay function or changing the speed of a car. (You can leave in
 * the delay that is already there that represents the car's speed, just
 * don't add any additional delaying).  Also, you should not be making
 * decisions on what cars do using calculations based on car speed (since
 * there are a number of unpredictable factors that can affect the
 * actual cars' progress). 
 *
 */

#include <stdio.h>
#include "aux.h"
#include "sys.h"
#include "umix.h"

void InitRoad (void);
void driveRoad (int from, int mph);

void Main ()
{
	InitRoad ();

	if (Fork () == 0) {
		Delay (1200);			// car 2
		driveRoad (WEST, 60);
		Exit ();
	}

	if (Fork () == 0) {
		Delay (900);			// car 3
		driveRoad (EAST, 50);
		Exit ();
	}

	if (Fork () == 0) {
		Delay (900);			// car 4
		driveRoad (WEST, 30);
		Exit ();
	}

	driveRoad (EAST, 40);			// car 1

	Exit ();
	
}


struct {
	
	int road[NUMPOS+1];		// structure of variables to be shared
	int cars_on_road;
	int direction;
	int east_stop;
	int west_stop;
	int sem;
	int first_in_front;
	int east_waiting_num;
	int west_waiting_num;
} shm;

void InitRoad ()
{
	Regshm ((char *) &shm, sizeof (shm));
	int i;
	for(i = 0; i <= NUMPOS; i++){
		shm.road[i] = Seminit(1);
	}
	shm.cars_on_road = 0;
	shm.direction = -1;
	shm.east_stop = Seminit(0);
	shm.west_stop = Seminit(0);
	shm.sem = Seminit(1);
	shm.first_in_front = 0;
	shm.east_waiting_num = 0;
	shm.west_waiting_num = 0;
}

#define IPOS (FROM)	(((FROM) == WEST) ? 1 : NUMPOS)

void driveRoad (int from, int mph)
	// from: coming from which direction
	// mph: the speed of the car
{
	int p;
	DPrintf("car pid %d has created, plan to enter from %d\n", Getpid(), from);
	Wait(shm.sem);

	if(from == EAST){
		if((shm.cars_on_road > 0 && shm.first_in_front == 1) || 
				(shm.cars_on_road > 0 && shm.direction == EAST && shm.west_waiting_num > 0) ||
				(shm.cars_on_road > 0 && shm.direction == WEST)){
			shm.east_waiting_num += 1;
			DPrintf("east gate waiting %d cars\n", shm.east_waiting_num);
			Signal(shm.sem);
			Wait(shm.east_stop);
		}else{
			DPrintf("EAST CARS ON ROAD is %d, direction is %d, west_waiting_num is %d\n", shm.cars_on_road, shm.direction, shm.west_waiting_num);

			Signal(shm.sem);
		}
	} else{
		if((shm.cars_on_road > 0 && shm.first_in_front == 1) || 
				(shm.cars_on_road > 0 && shm.direction == WEST && shm.east_waiting_num > 0) ||
				(shm.cars_on_road > 0 && shm.direction == EAST)){
			shm.west_waiting_num += 1;
			DPrintf("west gate waiting %d cars\n", shm.west_waiting_num);
			Signal(shm.sem);
			Wait(shm.west_stop);
		} else{
			DPrintf("WEST CARS ON ROAD is %d, direction is %d, east_waiting_num is %d\n", shm.cars_on_road, shm.direction, shm.east_waiting_num);
			Signal(shm.sem);
		}
	}

	if(from == EAST){
		Wait(shm.road[NUMPOS]);
	}else{
		Wait(shm.road[1]);
	}

	EnterRoad (from);

	Wait(shm.sem);
	shm.first_in_front = 1;
	shm.cars_on_road += 1;
	shm.direction = from;
	Signal(shm.sem);

	int next;
	int pre;
	for (p = 1; p <= NUMPOS; p++) {

		if(shm.direction == EAST){
			pre = NUMPOS - p + 1;
			next = NUMPOS - p;
		} else{
			pre = p;
			next = p + 1;
		}

		Delay (3600/mph);		
		if(p != NUMPOS){
			Wait(shm.road[next]);
			ProceedRoad ();
			Signal(shm.road[pre]);
		} 

		if(p == NUMPOS){
			ProceedRoad ();
			Signal(shm.road[pre]);
		}

		if(p == 1){
			Wait(shm.sem);
			shm.first_in_front = 0;
			Signal(shm.sem);

			if(shm.direction == EAST){
				if(shm.east_waiting_num > 0 && shm.west_waiting_num == 0){
					shm.east_waiting_num -= 1;
					Signal(shm.east_stop);
				}
			} else{
				if(shm.west_waiting_num > 0 && shm.east_waiting_num == 0){
					shm.west_waiting_num -= 1;
					Signal(shm.west_stop);
				}
			}
		}

	}

	Wait(shm.sem);
	shm.cars_on_road -= 1;
	if(shm.cars_on_road == 0){
		shm.direction = -1;
	}

	if(from == EAST){
		/* exit from east, open west entry if all out */
		if(shm.cars_on_road == 0 && shm.west_waiting_num > 0){
			DPrintf("exit from east with west gate open\n");
			shm.west_waiting_num -= 1;
			Signal(shm.west_stop);
			Signal(shm.sem);
		} else{
			Signal(shm.sem);
		}
	} else{
		if(shm.cars_on_road == 0 && shm.east_waiting_num > 0){
			DPrintf("exit from west with east gate open\n");
			shm.east_waiting_num -= 1;
			Signal(shm.east_stop);
			Signal(shm.sem);
		} else{
			Signal(shm.sem);
		}
	}
}     
