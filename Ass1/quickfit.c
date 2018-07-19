/*
    CSCI 347 Assignment 1
    Heap Management Using Quick-Fit

    Jesse Ericksen, Summer 2018
*/

#include <stdio.h>
#include <unistd.h>
#include "heap347.h"
#include "HeapTestEngine.h"

#define MINALLOC    1024     // minimum request to sbrk(), Header blocks
#define TOTALQUICKLISTS  9 // NUMBER OF QUICK LISTS -- INCREMENTED BY 16 BYTES
#define SIZE_OF_BLOCK  16
#define MAX_SIZE_BLOCK 9

typedef double Align[2];     // to force alignment of free-list blocks
                        // with worst-case data boundaries

// header for free-list entry
union header {
    struct {
        union header *next; // pointer to next block in free-list
        unsigned int size;  // size, in header units, of this block
    } data;

    Align alignment;        // not used, just forces boundary alignment
};

typedef union header Header;

// this is the free-list, initially empty
static Header *free_list = NULL;

static Header *quick_list[TOTALQUICKLISTS];

static void do_free (void *ptr);


/*  function display_block()
 *  displays one block of the free list
 *  parameter: curr, pointer to the block
 *  return: none
 */
static void display_block (Header *curr) {

    // address of this block
    unsigned long curr_addr = (unsigned long)curr;

    // address of next block in the free-list
    unsigned long next_addr = (unsigned long)curr->data.next;

    // address of next block in the heap, possibly an allocated block
    unsigned long next_mem  = curr_addr + (curr->data.size + 1) * sizeof(Header);

    printf("free block:0x%x, %4d units, next free:0x%x next block:0x%x\n",
           (int)curr_addr, curr->data.size + 1, (int)next_addr, (int)next_mem);
}


/*  function dump_freelist()
 *  logs the blocks of the free-list
 *  parameters: none
 *  return: none/* function init_freelist()
 * initializes the free list
 */

static void dump_freelist () {

    Header *curr = free_list;

    while (1) {
        display_block(curr);
        curr = curr->data.next;
        if (curr == free_list) break;
    }
    printf("\n");
}


/*  function more_heap()
 *  uses sbrk() system call to get more heap space
 *  parameter: nblocks, int, the number of units requested
 *  return: pointer to the new allocated heap-space
 */
static void *more_heap (unsigned nunits) {

    void *mem;
    Header *block;
    unsigned long mem_addr;

    // apply minimum request level for sbrk()
    if (nunits < MINALLOC) nunits = MINALLOC;

    // get sbrk() to provide more heap space
    mem = sbrk(nunits * sizeof(Header));
    mem_addr = (unsigned long) mem;
    printf("(sbrk %d 0x%x) ",
           (unsigned)(nunits * sizeof(Header)), (unsigned)mem_addr);
    if (mem == (void *)(-1)) return NULL;

    // set up a free-list block with the new space
    block = (Header *)mem;
    block->data.size = nunits -1;

    // call free347() to add this block to the free-list
    // initially, add it to the allocated count
    // the call to free347() will add it to the free-list
    do_free((void *)(block + 1));

    return mem;
}

/*  function coalesce()
 *  joins two adjacent free-list blocks into one larger block
 *  parameter: curr, Header *, address of the first (lower address) block
 *  return: int, 1 if the blocks were joined, 0 otherwise
 */
static int coalesce (Header *curr) {
    Header *pos = curr;
    Header *next_block = pos->data.next;
    unsigned long pos_addr = (unsigned long)pos;
    unsigned long next_addr = (unsigned long)next_block;

    // try to coalesce with the next block
    if (pos + pos->data.size + 1 == next_block) {
        printf("coalesce 0x%x (%d) with 0x%x (%d)\n",
               (int)pos_addr, pos->data.size+1,
               (int)next_addr, next_block->data.size+1);
        pos->data.size += next_block->data.size + 1;
        pos->data.next = next_block->data.next;
        return 1;
    }
    // the blocks were not adjacent, so coalesce() failed
    return 0;
}

//Initialize freeList for irregular sized blocks
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

	//RETURN nothing if # of bytes requested is 0
	if(nunits == 0){
		return NULL;
	} 

	if(nunits <= TOTALQUICKLISTS){
		int indexOf = (nunits - 1); // Provides the index of QuickList

		//CHECK if any blocks have been allocated to quick list
		if(quick_list[indexOf]->data.next != NULL){
            prev = quick_list[indexOf]->data.next;
            curr = prev->data.next;
            //Check Quick_list until wrapping around
            while(curr != quick_list[indexOf]){
                
                if(nunits == curr->data.size){
                    prev->data.next = curr->data.next;
                    return (void *)(curr + 1);
                }

                prev = curr;
                curr = curr->data.next;
            
		}
	}

	//If no space in Quick List is available, utlilize free list

	prev = free_list;
    curr = prev->data.next;

    while (curr != free_list)  {

        // exact fit
        if (nunits == curr->data.size)
        {
            prev->data.next = curr->data.next;
            return (void *)(curr + 1);
        }

        // larger space than needed
        if (nunits < curr->data.size) {
            curr->data.size -= (nunits + 1);
            curr += curr->data.size + 1;
            curr->data.size = nunits;
            return (void *)(curr + 1);
        }

        // move along to next block
        prev = curr;
        curr = curr->data.next;
    }

    // sufficient space not found in any free-list block,
    // request more heap space and try the allocation request again
    if (more_heap(nunits))
        return do_malloc(nbytes);
    else
        return NULL;


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

/*  function do_free()
 *  adds a previously-allocated space to the free-list
 *  parameter: ptr, void*, address of area being freed
 */
static void do_free (void *ptr) {

    Header *block, *curr;
    int coalesced;

    // block points to the header of the freed space
    block = (Header *)ptr - 1;

    // traverse the free-list, place the block in the correct
    // place, to preserve ascending address order
    curr = free_list;
    while (block > curr) {
        if (block < curr->data.next || curr->data.next == free_list) {

            // need to place block between curr and curr->data.next
            block->data.next = curr->data.next;
            curr->data.next = block;

            // attempt to coalesce with the previous block
            coalesced = 1;
            if (curr != free_list)
                // attempt to coalesce with next neighbor
                coalesced = coalesce(curr);

            if (!coalesced || curr == free_list)
                curr = curr->data.next;

            coalesce(curr);
            break;
        }

        // move along the free-list
        curr = curr->data.next;
    }

}

/*  function free347()
 *  wrapper for a call to do_free()
 *  adds a previously-allocated space to the free-list
 *  parameter: ptr, void*, address of area being freed
 */
void free347 (void *ptr) {

    unsigned long ptr_address = (unsigned long) ptr;
    printf("free 0x%x\n", (unsigned)ptr_address);

    do_free(ptr);

    dump_freelist();

}

int main (int argc, char *argv[]) {

    init_heap_test();

    // initialize the free-list
    if (init_freelist() != 0) {
        printf("cannot initialize free-list, run aborted\n");
        return 1;
    }

    // run the heap test
    heap_test();

    return 0;

}
