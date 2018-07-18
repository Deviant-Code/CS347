#include heap347.h
#include stdio.h
#define NALLOC 1024

typedef long Align; 

union header {
	struct {
		union header *ptr; // Points to next avail. block
		unsigned size; // Total size of this block
	} s;
	Align x; // force allignment of blocks
}

int main(){

	//Initialize free_list

	//void init_heap_test();
	//void heap_test();

}

//requests nbytes bytes of heap-space. If successful, malloc347() returns the address of the allocated space; if
//unsuccessful (not able to allocate the space), malloc347() returns NULL.
void *malloc347(int nbytes){


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