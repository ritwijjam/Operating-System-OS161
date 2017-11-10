/*
 * Synchronization primitives.
 * See synch.h for specifications of the functions.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *namearg, int initial_count)
{
	struct semaphore *sem;

	assert(initial_count >= 0);

	sem = kmalloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->name = kstrdup(namearg);
	if (sem->name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->count = initial_count;
	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	spl = splhigh();
	assert(thread_hassleepers(sem)==0);
	splx(spl);

	/*
	 * Note: while someone could theoretically start sleeping on
	 * the semaphore after the above test but before we free it,
	 * if they're going to do that, they can just as easily wait
	 * a bit and start sleeping on the semaphore after it's been
	 * freed. Consequently, there's not a whole lot of point in 
	 * including the kfrees in the splhigh block, so we don't.
	 */

	kfree(sem->name);
	kfree(sem);
}

void 
P(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh();
	while (sem->count==0) {
		thread_sleep(sem);
	}
	assert(sem->count>0);
	sem->count--;
	splx(spl);
}

void
V(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);
	spl = splhigh();
	sem->count++;
	assert(sem->count>0);
	thread_wakeup(sem);
	splx(spl);
}

/*
 * Simple lock for mutual exclusion.
 * Operations:
 *    lock_acquire - Get the lock. Only one thread can hold the lock at the
 *                   same time.
 *    lock_release - Free the lock. Only the thread holding the lock may do
 *                   this.
 *    lock_do_i_hold - Return true if the current thread holds the lock; 
 *                   false otherwise.
 *
 * These operations must be atomic. You get to write them.
 *
 * When the lock is created, no thread should be holding it. Likewise,
 * when the lock is destroyed, no thread should be holding it.
 *
 * The name field is for easier debugging. A copy of the name is made
 * internally.
 */

struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->name = kstrdup(name);
	if (lock->name == NULL) {
		kfree(lock);
		return NULL;
	}
	lock->lock_thread = NULL;
	return lock;
}

void
lock_destroy(struct lock *lock)
{
	// if lock is NULL exit the program
	assert(lock != NULL);
	kfree(lock->name);
	kfree(lock);
}


/*

 *    lock_acquire - Get the lock. Only one thread can hold the lock at the
 *                   same time.
 *    lock_release - Free the lock. Only the thread holding the lock may do
 *                   this.
 *    lock_do_i_hold - Return true if the current thread holds the lock; 
 *                   false otherwise.

*/

void
lock_acquire(struct lock *lock)
{
	int spl = splhigh();
	// if lock is NULL exit the program
	assert(lock != NULL);
	while (lock->lock_thread != NULL)
	{
		thread_sleep(lock); 
	}
	lock->lock_thread = curthread;	
	splx(spl);	
	//(void)lock;  // suppress warning until code gets written
}

void
lock_release(struct lock *lock)
{
	int spl = splhigh();
	//if lock is NULL exit the program
	assert(lock != NULL);
	lock->lock_thread = NULL;
	thread_wakeup(lock);
	splx(spl);	
	//(void)lock;  // suppress warning until code gets written
}

int
lock_do_i_hold(struct lock *lock)
{
	assert(lock != NULL);
	if(lock->lock_thread == curthread)
		return 1;

	return 0;
}

////////////////////////////////////////////////////////////
//
// CV
/*
 * Condition variable.
 *
 * Note that the "variable" is a bit of a misnomer: a CV is normally used
 * to wait until a variable meets a particular condition, but there's no
 * actual variable, as such, in the CV.
 *
 * Operations:
 *    cv_wait      - Release the supplied lock, go to sleep, and, after
 *                   waking up again, re-acquire the lock.
 *    cv_signal    - Wake up one thread that's sleeping on this CV.
 *    cv_broadcast - Wake up all threads sleeping on this CV.
 *
 * For all three operations, the current thread must hold the lock passed 
 * in. Note that under normal circumstances the same lock should be used
 * on all operations with any particular CV.
 *
 * These operations must be atomic. You get to write them.
 *
 * These CVs are expected to support Mesa semantics, that is, no
 * guarantees are made about scheduling.
 *
 * The name field is for easier debugging. A copy of the name is made
 * internally.
 */

struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(struct cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->name = kstrdup(name);
	if (cv->name==NULL) {
		kfree(cv);
		return NULL;
	}
	
	// add stuff here as needed
	
	return cv;
}
void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	// add stuff here as needed
	
	kfree(cv->name);
	kfree(cv);
}

/*    cv_wait      - Release the supplied lock, go to sleep, and, after
 *                   waking up again, re-acquire the lock.
 */
void
cv_wait(struct cv *cv, struct lock *lock)
{
	int spl = splhigh();
	assert(lock != NULL);
	assert(cv != NULL);
	lock_release(lock);
	thread_sleep(cv);
	lock_acquire(lock);
	splx(spl);
}
// *    cv_signal    - Wake up one thread that's sleeping on this CV.
void
cv_signal(struct cv *cv, struct lock *lock)
{
	int spl = splhigh();
	assert(lock != NULL);
	assert(cv != NULL);
	thread_random_wakeup(cv);	
	splx(spl);
}

//  *    cv_broadcast - Wake up all threads sleeping on this CV.

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	int spl = splhigh();
	assert(lock != NULL);
	assert(cv != NULL);
	thread_wakeup(cv);	
	splx(spl);

}
