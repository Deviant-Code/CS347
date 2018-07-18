#include <stdio.h>
#include <unistd.h>
#include "heap347.h"
#include "HeapTestEngine.h"
#define NALLOC 1024

typedef long Align; 

union header {
	struct {
		union header *ptr; // Points to next avail. block
		unsigned size; // Total size of this block
	} s;
	Align x; // force allignment of blocks
}

// this is the free-list, initially empty
static Header *free_list = NULL;

int main(){

	//Initialize free_list
	if (init_freelist() != 0) {
    	printf("cannot initialize free-list, run aborted\n");
    return 1;
    }
    

	//void init_heap_test();
	//void heap_test();

}

/* function init_freelist()
 * initializes the free list
 */
int init_freelist() {
    
    void *mem;                  // address of heap-space from sbrk() call
    Header *block;

    mem = sbrk(MINALLOC * sizeof(Header));
    if (mem == (void *)(-1)) {
        printf("sbrk() failed\n");
        return -1;
    }
    
    // set up a size 0 Header for the start of the free_list
    block = (Header *)mem;
    block->data.size = 0;
    block->data.next = block + 1;
    free_list = block;
    
    // set up the next block, containing the usable memory
    // size is nunits-2 since one block is used for the free-list
    // header and another for the header of this first block
    block = (Header *)(block + 1);
    block->data.size = MINALLOC - 2;
    block->data.next = free_list; 
    
    dump_freelist();
    
    return 0;
}

//requests nbytes bytes of heap-space. If successful, malloc347() returns the address of the allocated space; if
//unsuccessful (not able to allocate the space), malloc347() returns NULL.

/* function do_malloc()
 * the real heap allocation function
 * allocates (at least) nbytes of space on the heap
 * parameter: nbytes (int), size of space requested
 * return: address of space allocated on the heap
 */

static void *do_malloc (int nbytes) {

    unsigned int nunits;        // the number of Header-size units required
    Header *curr, *prev;        // used in free-list traversal

    // from nbytes, calculate the number of Header block units
    nunits = (nbytes - 1) / sizeof(Header) + 1;

}

/* function malloc347()
 * a wrapper for do_malloc, with log output
 * allocates (at least) nbytes of space on the heap
 * parameter: nbytes (int), size of space requested
 * return: address of space allocated on the heap
 */
void *malloc347 (int nbytes) {
    
    int units = (int)((nbytes - 1)/sizeof(Header) + 2);
    printf("malloc %d bytes %d units ", 
           nbytes, units);  
    
    void *allocated = do_malloc(nbytes);
    unsigned long address = (unsigned long) allocated;
    
    // log the outcome of the allocation
    // units total includes header
    printf("allocated at 0x%x\n", (unsigned) address);
    dump_freelist();
    
    return allocated;
}

void free347(void *ptr){


}



//Request more memory from kernel
static Header *morecore(unsigned nu){
	char *cp *sbrk(int);
	Header *up;

	if(nu < NALLOC)
		nu = NALLOC;
	cp = sbrk(nu * sizeof(Header));
	if(cp == (char *) -1) // no space left
		return NULL;
	up = (Header *) cp;
	up->s.size = nu;
	free((void *)(up+1));
	return freep;


}