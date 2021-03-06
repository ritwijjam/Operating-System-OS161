#ifndef _VM_H_
#define _VM_H_

#include <addrspace.h>
#include <machine/vm.h>

/*
 * VM system-related definitions.
 *
 * You'll probably want to add stuff here.
 */

struct addrspace;


/* Fault-type arguments to vm_fault() */
#define VM_FAULT_READ        0    /* A read was attempted */
#define VM_FAULT_WRITE       1    /* A write was attempted */
#define VM_FAULT_READONLY    2    /* A write to a readonly page was attempted*/
#define SIZE_OF_CORE_MAP     100  /* Assume there are only 100 processes to work with */



// core map entry struct
struct page_structure {
    // just need physical address
    paddr_t physAdd;
    // actually jk to free k_pages mans gotta use v_addr doods
    vaddr_t virAdd;  
    // count how many processes are using it
    int refcount;

};



/* Initialization function */
void vm_bootstrap(void);

/* Fault handling function called by trap code */
int vm_fault(int faulttype, vaddr_t faultaddress);

/* Allocate/free kernel heap pages (called by kmalloc/kfree) */
vaddr_t alloc_kpages(int npages);
paddr_t alloc_user_pages(int npages);
void free_kpages(vaddr_t addr);
void free_user_pages(vaddr_t addr);
paddr_t getppages(unsigned long npages);


void p_pagetable(struct addrspace *as);
paddr_t checkTable(struct addrspace *as, vaddr_t faultaddress);
#endif /* _VM_H_ */
