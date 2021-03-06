#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <addrspace.h>
#include <vm.h>
#include <machine/spl.h>
#include <machine/tlb.h>

/*
 * Dumb MIPS-only "VM system" that is intended to only be just barely
 * enough to struggle off the ground. You should replace all of this
 * code while doing the VM assignment. In fact, starting in that
 * assignment, this file is not included in your kernel!
 */

/* under dumbvm, always have 48k of user stack */
#define DUMBVM_STACKPAGES    24
#define DEFAULT_STACK_LIMIT  0x800000
#define DEFAULT_SBRK_LIMIT 0x1000000


struct addrspace *
as_create(void)
{
	struct addrspace *as = kmalloc(sizeof(struct addrspace));
	if (as==NULL) {
		return NULL;
	}

	as->heap = kmalloc(sizeof(struct region));
	if (as->heap == NULL) {
		kfree(as);
		return NULL;
	}
	as->data = kmalloc(sizeof(struct region));
	if (as->data == NULL) {
		kfree(as->heap);
		kfree(as);
		return NULL;
	}
	as->text = kmalloc(sizeof(struct region));
	if (as->text == NULL) {
		kfree(as->data);
		kfree(as->heap);
		kfree(as);
		return NULL;
	}
	as->stack = kmalloc(sizeof(struct region));
	if (as->text == NULL) {
		kfree(as->stack);
		kfree(as->data);
		kfree(as->heap);
		kfree(as);
		return NULL;
	}

	as->heap->vaddrB = 0;
	as->heap->npages = 0;
	
	as->data->vaddrB = 0;
	as->data->npages = 0;	

	as->text->vaddrB = 0;
	as->text->npages = 0;

	as->stack->vaddrB = 0;
	as->stack->npages = 0;

	as->maxstack = USERSTACK;
	as->maxheap  = 0;
    as->heap_end = 0;

	as->page_table = array_create();
	if (as->page_table == NULL) {
		kfree(as->text);
		kfree(as->stack);
		kfree(as->data);
		kfree(as->heap);
		kfree(as);
		return NULL;
	}

	return as;
}

void
as_destroy(struct addrspace *as)
{
		if (as == NULL)
		return;
	int i;
	for(i = 0; i <  array_getnum(as->page_table); i++){
		struct page_table_entry *t = array_getguy(as->page_table, i);
		array_remove(as->page_table, i);
		free_user_pages(t->paddr);
		kfree(t);
	}
	
	array_destroy(as->page_table);
	kfree(as->heap);
	kfree(as->data);
	kfree(as->stack);
	kfree(as->text);
	kfree(as);
	as = NULL;
}

void
as_activate(struct addrspace *as)
{
	int i, spl;

	(void)as;

	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		TLB_Write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t sz,
		 int readable, int writeable, int executable)
{
	assert(as!=NULL);
	size_t npages; 

	/* Align the region. First, the base... */
	sz += vaddr & ~(vaddr_t)PAGE_FRAME;
	vaddr &= PAGE_FRAME;

	/* ...and now the length. */
	sz = (sz + PAGE_SIZE - 1) & PAGE_FRAME;

	npages = sz / PAGE_SIZE;

	/* We don't use these - all pages are read-write */
	(void)readable;
	(void)writeable;
	(void)executable;


	if (as->text->vaddrB == 0) {
		as->text->vaddrB = vaddr;
		as->text->npages = npages;
		return 0;
	}


	if (as->data->vaddrB == 0) {
		as->data->vaddrB= vaddr;
		as->data->npages = npages;
		return 0;
	}

	/*
	 * Support for more than two regions is not available.
	 */
	kprintf("dumbvm: Warning: too many regions\n");
	return EUNIMP;
}

int
as_prepare_load(struct addrspace *as)
{

	if(as->text->vaddrB > as->data->vaddrB) {
		as->heap->vaddrB = ROUNDUP(as->text->vaddrB + (as->text->npages+1)*PAGE_SIZE, PAGE_SIZE);
                as->heap_end = as->heap->vaddrB;
	} else {
		as->heap->vaddrB = ROUNDUP(as->data->vaddrB + (as->data->npages+1)*PAGE_SIZE , PAGE_SIZE);
                as->heap_end = as->heap->vaddrB;
	}

	as->maxheap = (as->heap->vaddrB) + DEFAULT_SBRK_LIMIT;
	as->stack->vaddrB = as->maxstack - DEFAULT_STACK_LIMIT;
	int s_page = (USERSTACK - as->stack->vaddrB)/PAGE_SIZE;

	as->stack->npages = s_page;
	/*
	kprintf("stack base vaddr, %p\n", as->stack->vaddrB);
	kprintf("stack page size %d\n", as->stack->npages);

	kprintf("heap base vaddr, %p\n", as->heap->vaddrB);
	kprintf("heap page size %d\n", as->heap->npages);
	kprintf("heap top vaddr %p\n", as->heap_end);    */    
	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	/*
	kprintf("data vaddr, %p\n", as->data->vaddrB);
	kprintf("data page size %d\n", as->data->npages);

	kprintf("text vaddr, %p\n", as->text->vaddrB);
	kprintf("text page size %d\n", as->text->npages);

	kprintf("stack vaddr, %p\n", USERSTACK);
	kprintf("stack page size %d\n", as->stack->npages);

	kprintf("heap vaddr, %p\n", as->heap->vaddrB);
	kprintf("heap page size %d\n", as->heap->npages);
	*/

	(void)as;
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	//assert(as->stack->vaddrB != 0);
	
	*stackptr = USERSTACK;
	//as->stack->vaddrB = USERSTACK;


	return 0;
}

int
as_copy(struct addrspace *old, struct addrspace **ret)
{
	//print everything for old

	struct addrspace *new;
	

	new = as_create();

	if (new==NULL) {
		return ENOMEM;
	}

    new->data->vaddrB =  old->data->vaddrB;
  
	new->data->npages =  old->data->npages;	
    new->text->vaddrB =  old->text->vaddrB;

	new->text->npages =  old->text->npages;	
    new->stack->vaddrB =  old->stack->vaddrB;

	new->stack->npages =  old->stack->npages;
    new->heap->vaddrB =  old->heap->vaddrB;
    
	new->heap->npages =  old->heap->npages;  // this should be zero anyway

	new->heap_end = old->heap;
	new->maxstack = old->maxstack;
	new->maxheap = old->maxheap;
        
   if (as_prepare_load(new)) {
		return ENOMEM;
	}

	// my dood we don't assign any physical address to the base region
	// only physical address was needed in page_table
        

        int i, result;
        paddr_t paddr;
        struct page_table_entry *t;
        for(i = 0; i <  array_getnum(old->page_table); i++){
        	t = array_getguy(old->page_table, i);
        	struct page_table_entry *newPg = (struct page_table_entry *)kmalloc(sizeof(struct page_table_entry));
        	
			paddr = alloc_user_pages(1); // allocating 1 page
			if (paddr == 0) return ENOMEM;
			
			newPg->paddr = paddr;
			newPg->vaddr = t->vaddr;

			result = array_add(new->page_table, t);
			assert(result == 0);
		memmove((void *)PADDR_TO_KVADDR(newPg->paddr),
		(const void *)PADDR_TO_KVADDR(t->paddr),
		PAGE_SIZE);
        }
        //kprintf("left loop \n");
        //print and compare
        //old
	/* all pages need to be cleared to 0 */
	/* Put stuff here for your VM system */




        *ret = new;

	return 0;
}
