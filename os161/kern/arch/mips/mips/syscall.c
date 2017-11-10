#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <addrspace.h>

#define KERN_PTR	((void *)0x80000000)	/* addr within kernel */
#define INVAL_PTR	((void *)0x40000000)
/*
 * System call handler.
 *
 * A pointer to the trapframe created during exception entry (in
 * exception.S) is passed in.
 *
 * The calling conventions for syscalls are as follows: Like ordinary
 * function calls, the first 4 32-bit arguments are passed in the 4
 * argument registers a0-a3. In addition, the system call number is
 * passed in the v0 register.
 *
 * On successful return, the return value is passed back in the v0
 * register, like an ordinary function call, and the a3 register is
 * also set to 0 to indicate success.
 *
 * On an error return, the error code is passed back in the v0
 * register, and the a3 register is set to 1 to indicate failure.
 * (Userlevel code takes care of storing the error code in errno and
 * returning the value -1 from the actual userlevel syscall function.
 * See src/lib/libc/syscalls.S and related files.)
 *
 * Upon syscall return the program counter stored in the trapframe
 * must be incremented by one instruction; otherwise the exception
 * return code will restart the "syscall" instruction and the system
 * call will repeat forever.
 *
 * Since none of the OS/161 system calls have more than 4 arguments,
 * there should be no need to fetch additional arguments from the
 * user-level stack.
 *
 * Watch out: if you make system calls that have 64-bit quantities as
 * argusizeof(args)ments, they will get passed in pairs of registers, and not
 * necessarily in the way you expect. We recommend you don't do it.
 * (In fact, we recommend you don't use 64-bit quantities at all. See
 * arch/mips/include/types.h.)
 */

const int NAME_MAX = 4096;
void
mips_syscall(struct trapframe *tf)
{
	int callno;
	int32_t retval;
	int err;

	assert(curspl==0);

	callno = tf->tf_v0;

	/*
	 * Initialize retval to 0. Many of the system calls don't
	 * really return a value, just 0 for success and -1 on
	 * error. Since retval is the value returned on success,
	 * initialize it to 0 by default; thus it's not necessary to
	 * deal with it except for calls that return other values, 
	 * like write.
	 */

	retval = 0;

	//initialize locks


	switch (callno) {
	    case SYS_reboot:
		err = sys_reboot(tf->tf_a0);
		break;
//write system call
	    case SYS_write:
		err = sys_write(tf->tf_a0, (void*) tf->tf_a1, (size_t) tf->tf_a2);
		break;

	    case SYS_read:
			err = sys_read(tf->tf_a0, (void*) tf-> tf_a1, (size_t) tf->tf_a2);
                        retval = tf->tf_a2;
		break;

	    case SYS_fork:
                    err = sys_fork(tf, &retval);		
		break;

	    case SYS_getpid:
                    err = sys_getpid(&retval);
		break;
		
		case SYS_waitpid:
		err = sys_waitpid((pid_t) tf->tf_a0, (int*) tf-> tf_a1, tf->tf_a2, &retval);
		break;

		case SYS__exit:
		sys__exit(tf->tf_a0);
		break;

		case SYS_execv:
		err = sys_execv((char*) tf->tf_a0, (char **) tf->tf_a1);
                
		break;

	    /* Add stuff here */
 
	    default:
		kprintf("Unknown syscall %d\n", callno);
		err = ENOSYS;
		break;
	}


	if (err) {
		/*
		 * Return the error code. This gets converted at
		 * userlevel to a return value of -1 and the error
		 * code in errno.
		 */
		tf->tf_v0 = err;
		tf->tf_a3 = 1;      /* signal an error */
	}
	else {
		/* Success. */
		tf->tf_v0 = retval;
		tf->tf_a3 = 0;      /* signal no error */
	}
	
	/*
	 * Now, advance the program counter, to avoid restarting
	 * the syscall over and over again.
	 */
	
	tf->tf_epc += 4;

	/* Make sure the syscall code didn't forget to lower spl */
	assert(curspl==0);
}

void
md_forkentry(struct trapframe *tf, unsigned long child_addr)
{
	/*
	 * This function is provided as a reminder. You need to write
	 * both it and the code that calls it.
	 *
	 * Thus, you can trash it and do things another way if you prefer.
	 */
    struct trapframe* local = tf;
    struct addrspace* nas = (struct addrspace*) child_addr;


    local->tf_v0 = 0;
    local->tf_a3 = 0;
    local->tf_epc += 4;
    
    curthread->t_vmspace = nas;
	as_activate(nas);
	
	struct trapframe tfnew = *local;
	kfree(tf);
        mips_usermode(&tfnew);

}

int 
sys_write(int fd, const void *buf, size_t nbytes){

// may need to check size using for loop
// may need to use a lock if test fails
			
	if(fd != STDOUT_FILENO && fd!= STDERR_FILENO)
		return EBADF;
        
        if (buf == NULL)
            return EFAULT;
        
	char* a = kmalloc(sizeof(char)*nbytes);
	int error = copyin(buf, a, sizeof(char)*nbytes);
        if (error == EFAULT) return EFAULT;

        int i;
        for(i = 0; i < nbytes; i++){
            putch(a[i]);
        }
        kfree(a);

        return 0;
}

int 
sys_read(int fd, void *buf, size_t buflen){
        
	if(fd != STDIN_FILENO)
		return EBADF;
        char* a = kmalloc(sizeof(char)*buflen);
        int s;
        for(s = 0; s < buflen; s++){
            a[s] = getch(); 
        }
        int error = copyout(a, buf, sizeof(char)*buflen);
        if (error) return EFAULT;
        kfree(a);
        return 0;
        
        // CHECK UIO
        // check vnode
        // Ginny the ECE god spent 2 days on it GREATTTTTTTTTTTTTTTTTTTTTTT!!!!!!!!!!!!!1
}


int
sys_fork(struct trapframe *tf, int* retval){
	//initialize return result to 0
	int result = 0;
        int parent_process_id = curthread->pid_thread; //current thread just became a DAD/MOM!
        
	struct addrspace *newaddr;
	//new thread!!!
	struct thread *child;
        struct trapframe *newtf = (struct trapframe*) kmalloc(sizeof(struct trapframe));
	if(newtf == NULL){
		*retval = ENOMEM;
		return ENOMEM;
	}
	//copy address space
	//returns either 0 -> all well or ENOMEM (4) if not enough mem
	result = as_copy(curthread->t_vmspace, &newaddr);
	if(result){
		*retval = result;
		return -1; //result
	}
        as_activate(curthread->t_vmspace);
	// new address space
	//alocate mem for new trapframe
	*newtf = *tf;
	//fork the thread, if result is not 0 error occured
        //copy address space
	//returns either 0 -> all well or ENOMEM (4) if not enough mem
	result = thread_fork(curthread->t_name, (struct trapframe*) newtf, (unsigned long) newaddr, md_forkentry, &child);
	if(result){
		*retval = result;
		return -1; //result
	}	
	//if all is well return pID when done
	*retval = child->pid_thread;
        total_processes[child->pid_thread].ppid = parent_process_id;
        child->ppid = total_processes[child->pid_thread].ppid;
         //kfree(newtf);
	return 0;
}

int sys_getpid(int* retval){
    *retval = curthread->pid_thread;
    return 0;
}

int sys_waitpid(pid_t pid, int *status, int options, int* retval){
	
        if(options != 0){
		 return EINVAL;
	}
        
	if(status == NULL){
		return EFAULT;
	}
        
        int* errorCheck = kmalloc(sizeof(int));
        int error = copyin(status, errorCheck, sizeof(int));
        if (error != 0){
            kfree(errorCheck);
            *retval = -1;
            return error;
        }

	if (pid == curthread->ppid) 
  	    return EINVAL; 	
        if (pid == curthread->pid_thread)
            return EINVAL;
        
        if ((int) status % 4 != 0)
            return EINVAL;
        
        
        if (total_processes[(int)pid].process_thread == NULL)
            return EINVAL;
        
        if (pid < 0)
            return EINVAL;
        
        if (pid == 0)
        {
            *status = 0;
            return EINVAL;
        }
        
        if (total_processes[(int)pid].wait == 1)
            return EINVAL;
        
// if the process already exited         
        if (total_processes[(int)pid].exit == 1){
            *retval = pid;
            *status = total_processes[(int)pid].exitcode;
            return 0;
        }
        

// if not then acquire the lock and put it in wait under the process.pid it is calling
        *retval = pid;
        lock_acquire(total_processes[(int)pid].process_exit_lock);
        cv_wait(total_processes[(int)pid].process_wait_cv, total_processes[(int)pid].process_exit_lock);
        lock_release(total_processes[(int)pid].process_exit_lock);    
        *status = total_processes[(int)pid].exitcode;
        *retval = pid;
        total_processes[(int)pid].wait = 1;
        return 0;
}

void sys__exit(int exitcode){

// acquire the lock and release everything that's sleeping under the parent = current thread

    lock_acquire(total_processes[curthread->pid_thread].process_exit_lock);
    cv_broadcast(total_processes[curthread->pid_thread].process_wait_cv, total_processes[curthread->pid_thread].process_exit_lock);
    lock_release(total_processes[curthread->pid_thread].process_exit_lock);
    
// change exit code
    total_processes[curthread->pid_thread].exit = 1;
    total_processes[curthread->pid_thread].exitcode = exitcode;
    user_threads --;
    thread_exit(); 
}

int sys_execv(char* program, char **args){


    if (program == NULL ||args == NULL) return EFAULT;
   
     int* errorCheck = kmalloc(sizeof(int));
        int error = copyin(program, errorCheck, sizeof(int));
        if (error != 0){
            kfree(errorCheck);
            return error;
        }

       
         error = copyin(args, errorCheck, sizeof(int));
        if (error != 0){
            kfree(errorCheck);
            return error;
        }
   
       if (strcmp(program, "") == 0) return EINVAL; 
   
    if (strlen(program) <= 1) return EFAULT;
         
    
        struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result, arg_end, spl;
        int i = 0;
        size_t bufferlen;
          spl = splhigh(); // Block interrupts
        
        //kprintf("mallocing some stuff");
        //copy args to kernel space
         //account for null
        char* a = (char*) kmalloc(NAME_MAX);
        if(a == NULL){ 
            splx(spl);
           return ENOMEM;   
        }      //error check
        
        //copy from program(user_level) to a(kernel_level)

        result = copyinstr((userptr_t)program, a, NAME_MAX, &bufferlen); 
        
        if (result == EFAULT){
            splx(spl);//error check
            return EFAULT;
        }
        //end of the array args by finding NULL
        while(args[i] != NULL) {  
           
              int error = copyin(args[i], errorCheck, sizeof(int));
                   if (error != 0){
                                  kfree(errorCheck);
                                  splx(spl);//
                                      return error;
                                 }
            i++;
        }

        arg_end = i;     //assign end index to arg_end because z will be reused
        char **arg_new;  //make new array to copy args                
        arg_new = (char **) kmalloc(arg_end*sizeof(char*)); //memory allocation
        if( arg_new == NULL ){      //error check
            kfree(a);
            kfree(errorCheck);
            splx(spl);//
            return ENOMEM;
        }

        int q;  //new variable for array
        
        //copying all of arg to arg_new
        for(i = 0; i <= arg_end; i++){

            if(i < arg_end){
                
                int length = (strlen(args[i])) + 1; //add 1 to length to accommodate NULL
                arg_new[i] = (char*)kmalloc(length); //mem alloc
                
                if(arg_new[i] == NULL){     //error check and deallocation
                    
                    for(q = 0; q < i; q++){
                        
                        kfree(arg_new[q]);
                    }
                    kfree(errorCheck);
                    kfree(arg_new);
                    kfree(a);
                    splx(spl);//
                    return ENOMEM;
                }

               result = copyinstr((userptr_t)args[i], arg_new[i], length, &bufferlen);
                  //copy in stuff
                if (result == EFAULT) return EFAULT;   
               
                //error check
            }
            
            else arg_new[i] = NULL;    //put NULL at the end of args_new
        }
	/* Open the file. */

	result = vfs_open(a, O_RDONLY, &v);
        //error check
	if (result) {
            kfree(errorCheck);
            kfree(a);
            splx(spl);//
            return result;
	}
        //kprintf("some stuff");

        //make current thread's address space to NULL
        if(curthread->t_vmspace) curthread->t_vmspace = NULL;
        
	/* We should be a new thread. */
	assert(curthread->t_vmspace == NULL);

	/* Create a new address space. */
	curthread->t_vmspace = as_create();
        //error check
	if (curthread->t_vmspace==NULL) {
		vfs_close(v);
		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_vmspace);

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
        //error check
	if (result) {
            splx(spl);//
                kfree(a);
                kfree(arg_new);
                 kfree(errorCheck);
		/* thread_exit destroys curthread->t_vmspace */
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);
        //error check
	if (result) {
            splx(spl);//
                kfree(a);
                kfree(arg_new);
                 kfree(errorCheck);
		/* thread_exit destroys curthread->t_vmspace */
		return result;
	}
        
        //push back on stack -> reverse!
        int offset = 0;
        int argptr[arg_end];
        for(i = arg_end-1; i >= 0 ; i--){
            int length = strlen(arg_new[i]) + 1;
          
            //check if length of string is a multiple of 4
            //if not, then find the offset to next multiple of 4
            //then decrease the stackptr by that amount
            if(length%4){
                //int remainder = length%4;
                offset = (length + (4 - (length%4)));
                stackptr = stackptr -(offset);
            }
            else{
                //length is already a multiple of 4
                stackptr = stackptr - length;
            }
            //copy out from the new array arg_new(kernel space) to stackptr
            copyoutstr(arg_new[i], (userptr_t)stackptr, length, &bufferlen);
            argptr[i] = stackptr;

        }
        // Beginning of stack
        arg_new[arg_end] = 0;
        int j;
        for(j = arg_end; j >=0; j--){
           stackptr-=4;
           copyout(&argptr[j], (userptr_t)stackptr, sizeof(argptr[j])); 
        }
                
        //null pointer
        kfree(arg_new);
        kfree(a);
        splx(spl);//
         kfree(errorCheck);
	/* Warp to user mode. */
	md_usermode(arg_end/*argc*/, (userptr_t)stackptr/*userspace addr of stackptr*/,
		    stackptr, entrypoint);
	
        
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}



