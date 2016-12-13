#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"
/*************************************************************************
 * Basic Constants and Macros
 * You are not required to use these macros but may find them helpful.
 *************************************************************************/
#define WSIZE       sizeof(void *)         /* word size (bytes) */
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<7)      /* initial heap size (bytes) */
#define OVERHEAD    DSIZE     /* overhead of header and footer (bytes) */

#define MAX(x,y) ((x) > (y)?(x) :(y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(uintptr_t *)(p))
#define PUT(p,val)      (*(uintptr_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~(DSIZE - 1))
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
#define MIN_SIZE        2 * DSIZE
/* alignment */
#define ALIGNMENT 16
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0xf)


/*********************************************************
 * Team structure for grading
 ********************************************************/
team_t team = {
    /* Team name */
    "haha hehe",
    /* First member's full name */
    "Mingqi Hou",
    /* First member's email address */
    "mingqi.hou@mail.utoronto.ca",
    /* Second member's full name (leave blank if none) */
    "Yuan Feng",
    /* Second member's email address (leave blank if none) */
    "michaelyuan.feng@hotmail.com"
};

/**********************************************************
 * list management Start
 *********************************************************/
struct list_entry {
    struct list_entry *prev, *next;
};

typedef struct list_entry list_entry_t;


// Hyposize largest allocated chunk size is 4096 Bytes
// Then 2^12, 12 free list for different size of chunks
list_entry_t* free_list[12];
//segragate management
//list management functions
static inline void list_init(list_entry_t *elm) __attribute__((always_inline));
static inline void list_add(list_entry_t *elm) __attribute__((always_inline));
static inline void list_add_before(list_entry_t *listelm, list_entry_t *elm) __attribute__((always_inline));
static inline void list_add_after(list_entry_t *listelm, list_entry_t *elm) __attribute__((always_inline));
static inline void list_del(list_entry_t *listelm) __attribute__((always_inline));
static inline void list_del_init(list_entry_t *listelm) __attribute__((always_inline));
//static inline bool list_empty(list_entry_t *list) __attribute__((always_inline));
static inline list_entry_t *list_next(list_entry_t *listelm) __attribute__((always_inline));
static inline list_entry_t *list_prev(list_entry_t *listelm) __attribute__((always_inline));

static inline void __list_add(list_entry_t *elm, list_entry_t *prev, list_entry_t *next) __attribute__((always_inline));
static inline void __list_del(list_entry_t *prev, list_entry_t *next) __attribute__((always_inline));
static inline int list_hash(size_t size) __attribute__((always_inline));
static inline int is_pow_of_2(size_t size);
static inline size_t next_pow_of_2(size_t size);
int mm_check(void);

static inline int is_pow_of_2(size_t size){
    return !(size & (size - 1));
}

static inline size_t next_pow_of_2(size_t size){
    if(is_pow_of_2(size))
        return size;
    else{
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        size++;
        return size;
    }
}

static inline void list_init(list_entry_t *elm) {
    elm->prev = NULL;
    elm->next = elm;
}

// choose corresponding size free list to add based on size of elm
static inline void list_add(list_entry_t *elm) {
    assert(elm != NULL);

    //get chunk size
    size_t chunk_size = GET_SIZE(HDRP(elm));
    //check which free list should add to
    int list_index = list_hash(chunk_size);
    if(free_list[list_index]  != NULL){
        list_add_before(free_list[list_index], elm);
	}
    else{
        // if it is the header
        free_list[list_index] = elm;
        list_init(elm);
    }
}

static inline void list_add_before(list_entry_t *listelm, list_entry_t *elm) {
    __list_add(elm, listelm->prev, listelm);
}

static inline void list_add_after(list_entry_t *listelm, list_entry_t *elm) {
    __list_add(elm, listelm, listelm->next);
}

static inline void list_del(list_entry_t *listelm) {

    assert(listelm != NULL);

    //get chunk size
    size_t chunk_size = GET_SIZE(HDRP(listelm));

    int list_index = list_hash(chunk_size);

     if(listelm->prev == NULL &&  listelm == listelm->next){   
     		// this is the only element left in this list      
				free_list[list_index] = NULL;
			}
			else if(listelm == listelm->next){
				listelm->prev->next =  listelm->prev;
				free_list[list_index] = listelm->prev;
			}else{

        __list_del(listelm->prev, listelm->next);
   	 }

}

static inline void list_del_init(list_entry_t *listelm) {
    list_del(listelm);
    list_init(listelm);
}

/*
static inline bool
list_empty(list_entry_t *list) {
    return list->next == list;
}*/


static inline list_entry_t * list_next(list_entry_t *listelm) {
    return listelm->next;
}


static inline list_entry_t * list_prev(list_entry_t *listelm) {
    return listelm->prev;
}


static inline void __list_add(list_entry_t *elm, list_entry_t *prev, list_entry_t *next) {
    if(prev != NULL){
	prev->next = next->prev = elm;
    elm->prev = prev;

	}
	else{ 
	next->prev = elm;
	elm->prev = NULL;
	}
    elm->next = next;
}

static inline void __list_del(list_entry_t *prev, list_entry_t *next) {
	if(prev != NULL)
    		prev->next = next;
    next->prev = prev;
}

extern team_t team;

//depending on allocated size, return corresponding free list size index
int list_hash(size_t size){
   
   int counter = 0;

   assert(size != 0);

   while(size){
    if(size > 32){
        size = size >> 3;
        counter ++;
    }
    else{
        size = size >> 1;
        counter ++;
    }
   }

   if (counter >= 12)
     return 11;
   else 
     return counter - 1;
   
}

/**************************************************************************
 * list management Done
 ***************************************************************************/


void* heap_listp = NULL;

/**********************************************************
 * mm_init
 * Get first chunk of heap and initialize it
 * To intialize segrage list
 * By observing the test benches molloc size,
 * there are 12 lists from 1 byte to 4096 bytes
 * all size > 4096 bytes, put in last one
 * This is done to prevent errors when a call to mm_init
 * is made during a trace
 **********************************************************/
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                                 // alignment padding
    PUT(heap_listp + (1 * WSIZE), PACK(OVERHEAD, 1));   // prologue header
    PUT(heap_listp + (2 * WSIZE), PACK(OVERHEAD, 1));   // prologue footer
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));          // epilogue header
    heap_listp += DSIZE;
    
    // Initialize the free lists to NULL
    int i;
    for (i=0; i < 12; i++)
        free_list[i] = NULL;

    return 0;
}

/**********************************************************
 * coalesce
 * The function is used to coalesce blocks
 * when free a block, the blocks surronding the block could be free
 * then should be coalesced together to increase ultilization for future
 * malloc calls
 * Consider fource cases discussed in the class
 * 1. if prev and next are all allocated
 * 2. if prev is free and next is allocated
 * 3. if prev is allocated and next is free
 * 4. if prev and next are both free
 * Function return the pointer after coalsecing
 **********************************************************/
void *coalesce(void *bp)
{
//printf("coalesce\n");
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {   /* Case 1 */

        /*
         prev block is allocated and next block is allocated
         and do nothing        
        */
        return bp;
    }

    else if (prev_alloc && !next_alloc)
    {   /* Case 2 */
        /*
         prev block is allocated and next block is free
         should coalesce next block with the current free block
         and remove the next from the free list which used to belong to         
        */
        list_entry_t* next = (list_entry_t*)NEXT_BLKP(bp);
        list_del(next);

        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));

        //update block information

        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        
        return (bp);
    }

    else if (!prev_alloc && next_alloc)
    {   /* Case 3 */
        /* 
         prev block is free and next block is allocated
         should coalesce prev block with the current free block
         Remove the prev free block from corresponding free_list
        */
        list_entry_t* prev = (list_entry_t*)PREV_BLKP(bp);
        list_del(prev);

        //update block information

        size += GET_SIZE(HDRP(PREV_BLKP(bp)));

        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));

        return (PREV_BLKP(bp));
    }

    else
    {   /* Case 4 */
        /*
         prev block is free and next block is free
         should coalesce prev block and next with the current free block
         Remove both the next and prev free blocks from corresponding free list
        */
        list_entry_t* prev = (list_entry_t*)PREV_BLKP(bp);
        list_entry_t* next = (list_entry_t*)NEXT_BLKP(bp);
        list_del(prev);
        list_del(next);

        //update block information

        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));

        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));

  
        return (PREV_BLKP(bp));
    }
}

/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Free the former epilogue block
 * and reallocate its new header
 **********************************************************/
void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignments */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ( (bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));                // free block header
    PUT(FTRP(bp), PACK(size, 0));                // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));        // new epilogue header

    return bp;
}

/**********************************************************
 * find_fit
 * the solution used is first fit which has advantage of fast
 * search, find it check the size of asize and go to corresponding
 * size of free list to looking for an apporarite chunk and
 * return the address of that chunk of memory
 **********************************************************/
void * find_fit(size_t asize)
{
//printf("find fit\n");
    if(asize == 0)
        return NULL;

    // check the size and return the corresponding size free list index
    int free_list_index = list_hash(asize);
    int index;
    
    list_entry_t * list_elm_ptr;
    list_entry_t * list_ptr;
    list_entry_t * traverse_ptr;

    char * allocated_ptr;
    
    size_t block_size;
    size_t left_size;

    //solution 1 = first fit
    for(index = free_list_index; index < 12; index ++){
        if(free_list[index] != NULL){

            //get corresponding size list index
            list_elm_ptr = free_list[index];

        //get block information
        block_size = GET_SIZE(HDRP(list_elm_ptr));
        
        /*
         * if size fits, delete it from the list and return
         * 2 situations:
         * * after split asize, the left chunck size still > 2*Disze
         *   the left can still be used for future
         * * after split asize, the left chunck size still < 2*Disze
         *   the left cannot be used for future
         */

         //situation 1
        if(block_size >= asize && (block_size - asize) >= MIN_SIZE){
            list_del(list_elm_ptr);
            left_size = block_size - asize;
            
            //move to new fit segragate list
            //left_size_round_up = next_pow_of_2(left_size);
            allocated_ptr = (char*)list_elm_ptr + left_size;

            //mark the allocated one
            PUT(HDRP(allocated_ptr), PACK(asize, 1));
            PUT(FTRP(allocated_ptr), PACK(asize, 1));

            //add info to the new chhuck
            PUT(HDRP(list_elm_ptr), PACK(left_size, 0));
            PUT(FTRP(list_elm_ptr), PACK(left_size, 0));

            /*
             * update the list, size is changed, also should change 
             * corresponding size free list
            */
            if(left_size != 0){

                index = list_hash(left_size);
                list_ptr = free_list[index];
            
                if(list_ptr != NULL)
                    list_add_before(list_ptr, list_elm_ptr);
                else if(list_ptr == NULL){
                    free_list[index] = list_elm_ptr;
                    list_init(list_elm_ptr);
                }
                else
                    list_add(list_elm_ptr);

                return (void*)allocated_ptr;
            }
        }
        //situation 2, just do not do split, return all the chunk
        else if(block_size >= asize && (block_size - asize) < MIN_SIZE){
            list_del(list_elm_ptr);
            return (void*)list_elm_ptr;
        }

        list_elm_ptr = list_elm_ptr->prev;

        //if still has free blocks to check, then traverse
        for(; list_elm_ptr != NULL; list_elm_ptr = list_elm_ptr->prev){
             block_size = GET_SIZE(HDRP(list_elm_ptr));
            
            /*
             * if size fits, delete it from the list and return
             * 2 situations:
             * * after split asize, the left chunck size still > 2*Disze
             *   the left can still be used for future
             * * after split asize, the left chunck size still < 2*Disze
             *   the left cannot be used for future
             */

             //situation 1
            if(block_size >= asize && (block_size - asize) >= MIN_SIZE){                
                list_del(list_elm_ptr);
                left_size = block_size - asize;
                
                //move to new fit segragate list
                //left_size_round_up = next_pow_of_2(left_size);
                allocated_ptr = (char*)list_elm_ptr + left_size;

                //mark the allocated one
                PUT(HDRP(allocated_ptr), PACK(asize, 1));
                PUT(FTRP(allocated_ptr), PACK(asize, 1));

                //add info to the new chhuck
                PUT(HDRP(list_elm_ptr), PACK(left_size, 0));
                PUT(FTRP(list_elm_ptr), PACK(left_size, 0));

                /*
                 * update the list, size is changed, also should change 
                 * corresponding size free list
                */
                if(left_size != 0){

                    index = list_hash(left_size);
                    list_ptr = free_list[index];
                
                    if(list_ptr != NULL)
                        list_add_before(list_ptr, list_elm_ptr);
                    else if(list_ptr == NULL){
                        free_list[index] = list_elm_ptr;
                        list_init(list_elm_ptr);
                    }
                    else
                        list_add(list_elm_ptr);

                    return (void*)allocated_ptr;
                }
            }
            //situation 2, just do not do split, return all the chunk
            else if(block_size >= asize && (block_size - asize) < MIN_SIZE){
                list_del(list_elm_ptr);
                return (void*)list_elm_ptr;
            }
        }
      }  
    }

    return NULL;
}

/**********************************************************
 * place
 * mark the block from free to allocated
 **********************************************************/
void place(void* bp, size_t asize)
{
//printf("place\n");
    /* Get the current block size */
    size_t bsize = GET_SIZE(HDRP(bp));

    //update allocation information

    PUT(HDRP(bp), PACK(bsize, 1));
    PUT(FTRP(bp), PACK(bsize, 1));
}

/**********************************************************
 * mm_free
 * this function is used for free a dynamic allocated block
 * the main function is to just add the block into correponding size free list
 * that the pointers are stored in the payload
 **********************************************************/
void mm_free(void *bp)
{
 if(bp == NULL){
        return;
    }


    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));

    //first need to check the probality of coalescing and do coalesce if there is
    // then add to corresponding size list
//  

    list_entry_t * temp = (list_entry_t*)coalesce(bp);
//printf("free\n"); 

    list_add(temp);

//	mm_check();
}

/**********************************************************
 * mm_malloc
 * 
 **********************************************************/

void *mm_malloc(size_t size)
{
//printf("malloc\n");

    size_t asize; /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char * bp;

    if (size <= 0)
        return NULL;

    /*
     * As discussed in the class,  especially 512 bytes
     * should go next power of 2, > 512 memory is not enough, so just do only 512
    */
    if(size <= 512)
        size = next_pow_of_2(size);

    /* 
     * Adjust block size to include overhead and alignment requirements. 
    */
    if (size <= DSIZE)
        asize = DSIZE + OVERHEAD;
    else
        asize = DSIZE * ((size + (OVERHEAD) + (DSIZE-1))/ DSIZE);

    /*
     * After size adjustment, need to search from the free block tracking
     * lists and place it which is marked as allocated
    */
    if ((bp = find_fit(asize)) != NULL) {

        place(bp, asize);
        return bp;
    }

    /* 
     * if current left heap size is not enough or there is no fit
     * then ask for a new chunk from system to heap 
    */
    extendsize = asize;
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);

    return bp;

}

/**********************************************************
 * mm_realloc
 * As required, if the size == 0, this function equals to free
 * if ptr is NULL, this function eqauls to malloc
 * for the other, there are two situations
 * 1. realloc required size > current size
 * 2. realloc required size <= current size
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
//printf("realloc\n");
    //as mentioned in lab manual = free
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    //as mentioned in lab manual = malloc
    if (ptr == NULL)
        return (mm_malloc(size));

    list_entry_t * list_ptr;
    int index;
    char *old_block_ptr = ptr;
    char *new_block_ptr;

    // get current block size
    size_t old_block_size = GET_SIZE(HDRP(old_block_ptr));
    size_t padded_size;
    size_t new_coalesced_size;
    size_t left_size;
    size_t payload_size;

    /*
     * to check if required size is > current padded block size
     * -> do adjustment first for required size
    */  
    // do adjustment as malloc functions
    if (size <= DSIZE)
        padded_size = DSIZE + OVERHEAD;
    else
        padded_size = DSIZE * ((size + (OVERHEAD) + (DSIZE-1))/ DSIZE);

    /*
     * if required adjusted size < current block
     * use current block to do reallocate
     * but if excess > 2* Dsize or excess < 2 * Dsize
    */
    if (padded_size <= old_block_size){
        size_t excess = old_block_size - padded_size;

        // if excess > 2* Dsize, excess can be used as a new block,
        // then make excess as new block and return the current_size - excess == realloc size
        if (excess > MIN_SIZE){
            //update size information
            new_block_ptr = (void *)old_block_ptr + padded_size;    
            PUT(HDRP(new_block_ptr), PACK(excess,0));      
            PUT(FTRP(new_block_ptr), PACK(excess,0));      
            
            // add it to corresponding size free list
            left_size = GET_SIZE(HDRP(new_block_ptr));
            index = list_hash(left_size);

            list_ptr = free_list[index];
                    
            if(list_ptr != NULL)
                list_add_before(list_ptr, (list_entry_t*) new_block_ptr);
            else if(list_ptr == NULL){
                free_list[index] = (list_entry_t*)new_block_ptr;
                list_init((list_entry_t*)new_block_ptr);
            }    
                 
            if(padded_size <= 512)
               padded_size = next_pow_of_2(padded_size);

            //mark the current without excess part is allocated
            PUT(HDRP(old_block_ptr), PACK(padded_size,1));     
            PUT(FTRP(old_block_ptr), PACK(padded_size,1));     

            return old_block_ptr;  
        }
        else //if not enough for making a block under alignment requirement, then just return current block
            return old_block_ptr;
    }
     /*
     * if required adjusted size > current block
     * Two situations 
     * 1. first check if there are surronding blocks around current block
     *     if coalesced size is enough, coalesced them and return current block pointer
     * 2. if there are not
     *     new malloc one
    */
    else{ 
        //To see if there is a block can be coalesced 
        new_block_ptr = coalesce(old_block_ptr);
        new_coalesced_size = GET_SIZE(HDRP(new_block_ptr));
        
        //check size
        if (new_coalesced_size >= padded_size){
            
            payload_size = old_block_size-OVERHEAD;

            //memmove is quick than memcopy, no need memcopy still same block
            memmove(new_block_ptr, old_block_ptr, payload_size); 
            //memcpy(new_block_ptr, old_block_ptr, payload_size);

            PUT(HDRP(new_block_ptr), PACK(new_coalesced_size,1));
            PUT(FTRP(new_block_ptr), PACK(new_coalesced_size,1));
            return new_block_ptr;
        }


       //if there is no coalesced block, need new malloc
        new_block_ptr = mm_malloc(size);

        if (new_block_ptr == NULL)
            return NULL;

        payload_size = old_block_size - OVERHEAD;
        //need memcopy, two different blocks
        memcpy(new_block_ptr, old_block_ptr, payload_size);

        //after new malloc, previous should be add to free list
        PUT(HDRP(old_block_ptr), PACK(old_block_size,0));
        PUT(FTRP(old_block_ptr), PACK(old_block_size,0));
        //list_add function will add old_block_ptr to corresponding size free list
        list_add((list_entry_t*)old_block_ptr);

        return new_block_ptr;
    }

    return NULL;
}


/*
	mm_check can be called anytime
	Upon completion, it will return 0 is no error is found,
	Otherwise, it will return 1
	All errors are displayed in stdout
*/
int mm_check(void) {

	void *heap_lo_addr = mem_heap_lo();
	void *heap_hi_addr = mem_heap_hi();
	int i;
	int free_block_count = 0;
			int error_count = 0;
					int complete = 0;

	for (i=0; i < 12; i++){	
		//printf("Executing validation for free mem list class %d\n", i);
		list_entry_t *free_mem;
		free_mem = free_list[i];
		while(free_mem!=NULL && complete==0){
			//Is every block in the free list marked as free?
			if(GET_ALLOC(HDRP(free_mem)) != 0){
				printf("Error detected - Block in free memory list %p is not marked as free\n", (void *)free_mem);
				error_count++;
			}
			//Are there any contiguous free blocks that somehow escaped coalescing?
			if(GET_ALLOC(HDRP(PREV_BLKP(free_mem))) == 0 || GET_ALLOC(HDRP(NEXT_BLKP(free_mem))) == 0){	
				printf("Error detected - Block in free memory list %p escaped coalescing\n", (void *)free_mem);
				error_count++;
			}
			//Do the pointers in a heap block point to valid heap addresses?
			if((void *)free_mem < heap_lo_addr||(void *)free_mem > heap_hi_addr){
				printf("Error detected - Block in free memory list %p point to valid heap addresses\n", (void *)free_mem);
				error_count++;
			}		
			if((void *)FTRP(free_mem) > heap_hi_addr){
				printf("Error detected - Block in free memory list %p runs over the heap block\n", free_mem);
				printf("Address of last byte of heap %p - Address of free memory footer %p", (void *)free_mem, (void *)FTRP(free_mem));
				error_count++;
			}
			free_block_count ++;
			if(free_mem->prev!= free_mem && free_mem->prev != free_list[i])
				free_mem = free_mem->prev;
			else
				complete = 1;
		}
	}

	//printf("Executing validation for heap list\n");
	void* heap_pt = heap_listp;
	heap_pt= NEXT_BLKP(heap_pt); //skip the prologue block
	int free_block_count2 = 0;
	complete = 0;
	while(heap_pt != NULL && GET_SIZE(HDRP(heap_pt)) != 0 && (void *)heap_pt < heap_hi_addr){

		//Minimum block size check
    if (GET_SIZE(HDRP(heap_pt)) < OVERHEAD){
      printf("Error detected - Block %p is less than the minimum size\n", (void *)heap_pt);
	
			error_count++;
    }
    //Alignment check
    if (ALIGN(GET_SIZE(HDRP(heap_pt)) != GET_SIZE(HDRP(heap_pt)) || GET_SIZE(HDRP(heap_pt)) % DSIZE != 0)){
      printf("Error detected - Block %p contains a block that is not aligned\n", (void *)heap_pt);
			error_count++;
    }
    //Header footer mismatch check
    if (GET_SIZE(HDRP(heap_pt)) != GET_SIZE(FTRP(heap_pt)) || GET_ALLOC(HDRP(heap_pt)) != GET_ALLOC(FTRP(heap_pt))){
      printf("Error detected - Block %p has header & footer mismatch\n", (void *)heap_pt);
      printf("Header size %lu alloc %d; Footer size %lu alloc %d\n", GET_SIZE(HDRP(heap_pt)), (int)GET_ALLOC(HDRP(heap_pt)), GET_SIZE(FTRP(heap_pt)), (int)GET_ALLOC(FTRP(heap_pt)));
			error_count++;
    }
    //Overlap check
    if (FTRP(heap_pt) > HDRP(NEXT_BLKP(heap_pt))) {
      printf("Error detected - Block %p overlaps with %p\n", (void *)heap_pt, (void *)NEXT_BLKP(heap_pt));
			error_count++;
    }
    if(GET_ALLOC(HDRP(heap_pt)) == 0){
    	free_block_count2 ++;
    } 
			heap_pt = NEXT_BLKP(heap_pt);

	}
	if(free_block_count2 > free_block_count){
		printf("Error detected - Not all free blocks are included in free list\n");
		printf("Free Block has %d, Heap has %d\n", free_block_count, free_block_count2 );
		error_count++;
	}

	if(error_count == 0){
		//printf("NO error detected\n");
		return 0;
	}else{
		printf("%d error(s) detected\n", error_count);
		printf("high heap addr: %p, low heap addr %p\n", heap_hi_addr, heap_lo_addr);
		return 1;
	}

}

