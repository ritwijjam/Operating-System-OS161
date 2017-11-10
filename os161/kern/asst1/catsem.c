/*
 * catsem.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use SEMAPHORES to solve the cat syncronization problem in 
 * this file.
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
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2

	
	struct semaphore* catbowl1;
	struct semaphore* catbowl2;
	struct semaphore* mousebowl1;
	struct semaphore* mousebowl2;
	struct semaphore* bowl1;
	struct semaphore* bowl2;
	struct semaphore* threads;

 /* 
 * Function Definitions
 * 
 */

/* who should be "cat" or "mouse" */
static void
sem_eat(const char *who, int num, int bowl, int iteration)
{
        kprintf("%s: %d starts eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
        clocksleep(1);
        kprintf("%s: %d ends eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
}

/*
 * catsem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
catsem(void * unusedpointer, 
       unsigned long catnumber)
{
unsigned int i;

for(i = 0; i < 4; i++){
	int multi = random()%2;
	if(multi == 0){
        P(catbowl1);
		P(bowl1);
		sem_eat("cat", catnumber, 1, i);
		V(bowl1);
		V(catbowl1);
	}
	else{
		P(catbowl2);
		P(bowl2);
		sem_eat("cat", catnumber, 2, i);
		V(bowl2);
		V(catbowl2);
	}

}

  V(threads);      //(void) unusedpointer;
        //(void) catnumber;
}
        

/*
 * mousesem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
mousesem(void * unusedpointer, 
         unsigned long mousenumber)
{
	unsigned int i;

	for(i = 0; i < 4; i++){
		int multi = random()%2;
		if(multi == 0){
		P(mousebowl1);
		P(catbowl2);
		P(bowl1);
		sem_eat("mouse", mousenumber, 1, i);
		V(mousebowl1);
		V(catbowl2);
		V(bowl1);
		}
		else{
		P(mousebowl2);
		P(catbowl1);
		P(bowl2);
		sem_eat("mouse", mousenumber, 2, i);
		V(mousebowl2);
		V(catbowl1);
		V(bowl2);
		}
	}
V(threads);
       //(void) unusedpointer;
        //(void) mousenumber;
}


/*
 * catmousesem()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catsem() and mousesem() threads.  Change this 
 *      code as necessary for your solution.
 */

int
catmousesem(int nargs,
            char ** args)
{
        int index, error;
		catbowl1 = sem_create("catbowl1", 1);
		catbowl2 = sem_create("catbowl2", 1);
		mousebowl1 = sem_create("mousebowl1", 1);
		mousebowl2 = sem_create("mousebowl2", 1);
		bowl1 = sem_create("bowl1", 1);
		bowl2 = sem_create("bowl2", 1);
		threads = sem_create("bowl2", 0);
         /* Avoid unused variable warnings.
         */

        //(void) nargs;
      //  (void) args;
   
        /*
         * Start NCATS catsem() threads.
         */

        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catsem Thread", 
                                    NULL, 
                                    index, 
                                    catsem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catsem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /*
         * Start NMICE mousesem() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mousesem Thread", 
                                    NULL, 
                                    index, 
                                    mousesem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mousesem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

	int  i = 8;
		for(i = 0; i < 8; i++){
			
			P(threads);

		}	


        return 0;
}



/*
 * End of catsem.c
 */
