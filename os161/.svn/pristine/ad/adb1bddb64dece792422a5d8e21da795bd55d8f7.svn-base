/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>
#include <queue.h>


/*
 *
 * Constants
 *
 */

/*
 * Number of cars created.
 */

#define NCARS 20

// definining directions


/*
 *
 * Function Definitions
 *
 */

// create locks for four different regions

struct lock *NW;
struct lock *NE;
struct lock *SW;
struct lock *SE;
struct lock *N;
struct lock *E;
struct lock *S;
struct lock *W;
struct lock *neLeft;
struct lock *nwLeft;
struct lock *seLeft;
struct lock *swLeft;

// Allowing only three cars inside the intersection
struct lock *carOne;
struct lock *carTwo;
struct lock *carThree;


static void
initialize_locks(){

	NW = lock_create("NW");
	NE = lock_create("NE");
	SW = lock_create("SW");
	SE = lock_create("SE");
	N = lock_create("N");
	E = lock_create("E");
	S = lock_create("S");
	W = lock_create("W");
	neLeft = lock_create("neLeft");
	nwLeft = lock_create("nwLeft");
	seLeft = lock_create("seLeft");
	swLeft = lock_create("swLeft");
	carOne = lock_create("carOne");
	carTwo = lock_create("carTwo");
	carThree = lock_create("carThree");
}



static const char *directions[] = { "N", "E", "S", "W" };

static const char *msgs[] = {
        "approaching:",
        "region1:    ",
        "region2:    ",
        "region3:    ",
        "leaving:    "
};

/* use these constants for the first parameter of message */
enum { APPROACHING, REGION1, REGION2, REGION3, LEAVING };

static void
message(int msg_nr, int carnumber, int cardirection, int destdirection)
{
        kprintf("%s car = %2d, direction = %s, destination = %s\n",
                msgs[msg_nr], carnumber,
                directions[cardirection], directions[destdirection]);
}
 

/*
 * gostraight()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement passing straight through the
 *      intersection from any direction.
 *      Write and comment this function.
 */



static
void
gostraight(unsigned long cardirection,
           unsigned long carnumber)
{
		(void) cardirection;
		(void) carnumber;

}


/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(unsigned long cardirection,
         unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;
}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnright(unsigned long cardirection,
          unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) cardirection;
        (void) carnumber;
}


/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long carnumber: holds car id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createcars().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */
 
static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
        int cardirection;
		int cardestination;
		// Assume worst case from the very beginning
		int sameDirection = 1;
        /*
         * Avoid unused variable and function warnings.
         */
        (void) unusedpointer;
        /*
         * cardirection is set randomly.
		 * Car direction are as follows -
		 * North = 0;
		 * East = 1; 
		 * South = 2;
		 * West = 3; 
         */

        cardirection = random() % 4;
		cardestination = random() % 4;

		if (cardirection == 0 && cardestination == 0) sameDirection = 1;
		else if (cardirection == 2 && cardestination == 2) sameDirection = 1;
		else if (cardirection == 1 && cardestination == 1) sameDirection = 1;
		else if (cardirection == 3 && cardestination == 3) sameDirection = 1;
		else sameDirection = 0;		
	
		while (sameDirection != 0){
		cardestination = random() % 4;
		if (cardirection == 0 && cardestination == 0) sameDirection = 1;
		else if (cardirection == 2 && cardestination == 2) sameDirection = 1;
		else if (cardirection == 1 && cardestination == 1) sameDirection = 1;
		else if (cardirection == 3 && cardestination == 3) sameDirection = 1;
		else sameDirection = 0;		
	}
	 

// lock *one, *two, *three, *four;

	void* number = carnumber;
	// car direction == car destination -> go straight
	if ((cardirection == cardestination + 2) || (cardirection == cardestination - 2) ){
			if ( cardirection  == 0) // from north to south
		{ 	
			lock_acquire(N);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carOne);
			lock_acquire(NW);
			message(1, carnumber, cardirection, cardestination);
			lock_release(N);
			lock_acquire(SW);
			message(2, carnumber, cardirection, cardestination);
			lock_release(NW);
			lock_acquire(swLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(SW);
			lock_release(carOne);
			lock_release(swLeft);			
		}	
			else if ( cardirection  == 1) // from East to West
		{
			lock_acquire(E);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carTwo);
			lock_acquire(NE);
			message(1, carnumber, cardirection, cardestination);
			lock_release(E);
			lock_acquire(NW);
			message(2, carnumber, cardirection, cardestination);
			lock_release(NE);
			lock_acquire(nwLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(NW);
			lock_release(carTwo);
			lock_release(nwLeft);
					
		}		
			else if ( cardirection  == 2) // from South to North
		{ 		
			lock_acquire(S);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carThree);
			lock_acquire(SE);
			message(1, carnumber, cardirection, cardestination);
			lock_release(S);
			lock_acquire(NE);
			message(2, carnumber, cardirection, cardestination);
			lock_release(SE);
			lock_acquire(neLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(NE);
			lock_release(carThree);
			lock_release(neLeft);
		}
			else if ( cardirection  == 3) // from West to East
		{
			lock_acquire(W);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carOne);
			lock_acquire(SW);
			message(1, carnumber, cardirection, cardestination);
			lock_release(W);
			lock_acquire(SE);
			message(2, carnumber, cardirection, cardestination);
			lock_release(SW);
			lock_acquire(seLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(SE);
			lock_release(carOne);
			lock_release(seLeft);
		}
	}


	// car direction = 0 and car destination = 1 -> from North to East
	else if (cardirection == 0 && cardestination == 1){
			lock_acquire(N);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carTwo);
			lock_acquire(NW);
			message(1, carnumber, cardirection, cardestination);
			lock_release(N);
			lock_acquire(SW);
			message(2, carnumber, cardirection, cardestination);
			lock_release(NW);
			lock_acquire(SE);
			message(3, carnumber, cardirection, cardestination);
			lock_release(SW);
			lock_acquire(seLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(SE);
			lock_release(carTwo);
			lock_release(seLeft);
	}
	// car direction = 0 and car destination = 3 -> from North to West
	else if (cardirection == 0 && cardestination == 3){
			lock_acquire(N);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carThree);
			lock_acquire(NW);
			message(1, carnumber, cardirection, cardestination);
			lock_release(N);
			lock_acquire(nwLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(NW);
			lock_release(carThree);
			lock_release(nwLeft);			
	}

	// car direction = 2 and car destination = 1 -> from South to East 
	else if (cardirection == 2 && cardestination == 1 ){
			lock_acquire(S);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carOne);
			lock_acquire(SE);
			message(1, carnumber, cardirection, cardestination);
			lock_release(S);
			lock_acquire(seLeft);			
			message(4, carnumber, cardirection, cardestination);
			lock_release(SE);
			lock_release(carOne);
			lock_release(seLeft);
			
	}
	// car direction = 2 and car destination = 3 -> from South to West
	else if (cardestination == 2 && cardestination == 3){
			lock_acquire(S);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carTwo);
			lock_acquire(SE);	
			message(1, carnumber, cardirection, cardestination);
			lock_release(S);
			lock_acquire(NE);
			message(2, carnumber, cardirection, cardestination);
			lock_release(SE);
			lock_acquire(NW);
			message(3, carnumber, cardirection, cardestination);
			lock_release(NE);
			lock_acquire(nwLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(NW);
			lock_release(carTwo);
			lock_release(nwLeft);		
  	}

	// car direction = 1 and car destination = 0 -> from East to North 
	else if (cardirection == 1 && cardestination == 0){
			lock_acquire(E);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carThree);
			lock_acquire(NE);
			message(1, carnumber, cardirection, cardestination);
			lock_release(E);
			lock_acquire(neLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(NE);
			lock_release(carThree);
			lock_release(neLeft);
	}
	// car direction = 1 and car destination = 2 -> from East to South
	else if (cardirection == 1 && cardestination == 2){
			lock_acquire(E);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carOne);
			lock_acquire(NE);
			message(1, carnumber, cardirection, cardestination);
			lock_release(E);
			lock_acquire(NW);
			message(2, carnumber, cardirection, cardestination);
			lock_release(NE);
			lock_acquire(SW);
			message(3, carnumber, cardirection, cardestination);
			lock_release(NW);
			lock_acquire(swLeft);
			message(4, carnumber, cardirection, cardestination);
			lock_release(SW);
			lock_release(carOne);
			lock_release(swLeft);	
	}

	// car direction = 3 and car destination = 0 -> From West to North
	else if (cardirection == 3 && cardestination == 0){
			lock_acquire(W);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carTwo);
			lock_acquire(SW);
			message(1, carnumber, cardirection, cardestination);
			lock_release(W);
			lock_acquire(SE);
			message(2, carnumber, cardirection, cardestination);
			lock_release(SW);
			lock_acquire(NE);
			message(3, carnumber, cardirection, cardestination);
			lock_release(SE);
			lock_acquire(neLeft);			
			message(4, carnumber, cardirection, cardestination);			
			lock_release(NE);
			lock_release(carTwo);		
			lock_release(neLeft);
	}
	// car direction = 3 and car destination = 2 -> From West to South
	else if (cardirection == 3 && cardestination == 2){
			lock_acquire(W);
			message(0, carnumber, cardirection, cardestination);
			lock_acquire(carThree);
			lock_acquire(SW);
			message(1, carnumber, cardirection, cardestination);
			lock_release(W);
			lock_acquire(swLeft);			
			message(4, carnumber, cardirection, cardestination);
			lock_release(SW);
			lock_release(carThree);
			lock_release(swLeft);
	}

			
}



/*
 * createcars()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createcars(int nargs,
           char ** args)
{
        int index, error;

        /*
         * Start NCARS approachintersection() threads.
         */
 		initialize_locks();
        for (index = 0; index < NCARS; index++) {

                error = thread_fork("approachintersection thread",
                                    NULL,
                                    index,
                                    approachintersection,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {
                        
                        panic("approachintersection: thread_fork failed: %s\n",
                              strerror(error));
		}
	}

}

