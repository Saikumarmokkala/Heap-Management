
/*
   Group Members:

   Name : SAI KUMAR REDDY MOKKALA
   Student ID: 1001728207

   Name : NITHISH REDDY MINNUPURI
   Student ID: 1001633261

*/

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)      ((b) + 1)
#define BLOCK_HEADER(ptr)   ((struct _block *)(ptr) - 1)


static int atexit_registered = 0;
static int num_mallocs       = 0;
static int num_frees         = 0;
static int num_reuses        = 0;
static int num_grows         = 0;
static int num_splits        = 0;
static int num_coalesces     = 0;
static int num_blocks        = 0;
static int num_requested     = 0;
static int max_heap          = 0;

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none

 */
void printStatistics( void )
{
  printf("\nheap management statistics\n");
  printf("mallocs:\t%d\n", num_mallocs );
  printf("frees:\t\t%d\n", num_frees );
  printf("reuses:\t\t%d\n", num_reuses );
  printf("grows:\t\t%d\n", num_grows );
  printf("splits:\t\t%d\n", num_splits );
  printf("coalesces:\t%d\n", num_coalesces );
  printf("blocks:\t\t%d\n", num_blocks );
  printf("requested:\t%d\n", num_requested );
  printf("max heap:\t%d\n", max_heap );
}

struct _block
{
   size_t  size;         /* Size of the allocated _block of memory in bytes */
   struct _block *prev;  /* Pointer to the previous _block of allcated memory   */
   struct _block *next;  /* Pointer to the next _block of allcated memory   */
   bool   free;          /* Is this _block free?                     */
   char   padding[3];
};


struct _block *freeList = NULL; /* Free list to track the _blocks available */

/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free _blocks
 * \param size size of the _block needed in bytes
 *
 * \return a _block that fits the request or NULL if no free _block matches
 *
 * \TODO Implement Next Fit
 * \TODO Implement Best Fit
 * \TODO Implement Worst Fit
 */
struct _block *findFreeBlock(struct _block **last, size_t size)
{
   struct _block *curr = freeList;

#if defined FIT && FIT == 0
   /* First fit */
   while (curr && !(curr->free && curr->size >= size))
   {
      *last = curr;
      curr  = curr->next;
   }
#endif

#if defined BEST && BEST == 0
   struct _block *bestfitptr = NULL;   // Declaring the bestfitpointer


// It loops until the current pointer is true
   while(curr)
   {

    /*  If the current block is free and the current size is greater than the process size,
        when there is some unallocated memory left after giving the process memory.

    */

       if (curr->free && curr->size >= size)
       {
           // The bestfit memory allocated is stored.
           bestfitptr = curr;
           //break;
        }

       *last = curr;       // Last pointer is updated to the current pointer
        curr = curr -> next;   // Current pointer is updated to the next

   }


   if (bestfitptr)
   {

        while (curr)

        {
      /* If the unallocated  curent memory is less than the best allocated
                 memory, then update it.
      */

            if (curr->free && curr->size >= size)
            {
                 if (curr-> size  < bestfitptr -> size)
                 {

                     bestfitptr = curr;


                 }


            }

          // Last pointer is updated to the current pointer
            *last = curr;
          // Current pointer is updated to the next
             curr = curr-> next;

        }

   }


curr = bestfitptr;

#endif


#if defined WORST && WORST == 0
 struct _block *bestfitptr = NULL;   // Declaring the bestfitpointer


// It loops until the current pointer is true
   while(curr)
   {

    /*  If the current block is free and the current size is greater than the process size,
        when there is some unallocated memory left after giving the process memory.

    */

       if (curr->free && curr->size >= size)
       {
           // The bestfit memory allocated is stored.
           bestfitptr = curr;
           //break;
        }

       *last = curr;       // Last pointer is updated to the current pointer
        curr = curr -> next;   // Current pointer is updated to the next

   }


   if (bestfitptr)
   {

        while (curr)

        {
      /* If the unallocated  curent memory is less than the best allocated
                 memory, then update it.
      */

            if (curr->free && curr->size >= size)
            {
                 if (curr-> size > bestfitptr -> size)
                 {

                     bestfitptr = curr;


                 }


            }

          // Last pointer is updated to the current pointer
            *last = curr;
          // Current pointer is updated to the next
             curr = curr-> next;

        }

   }


curr = bestfitptr;

#endif



#if defined NEXT && NEXT == 0

struct _block *nextfitptr = NULL;

 /* Next fit */

  while (curr)
  {
      // If the current block is free and the current
      // block size is greater than the process size,
      // point to the next fit pointer is updated to the next one
        if (curr->free && curr->size >= size)
       {
            nextfitptr = curr;
       }

       // Last pointer is updated to the current pointer
      *last = curr;
       // Current pointer is updated to the next
       curr  = curr->next;

  }

  curr = nextfitptr;

#endif
  num_reuses++; // Incrementing the number of reuses each time memory is reused
  return curr;

}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if fa  iled
 */
struct _block *growHeap(struct _block *last, size_t size)
{
   /* Request more space from OS */
   struct _block *curr = (struct _block *)sbrk(0);
   struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct _block *)-1)
   {
      return NULL;
   }

   /* Update freeList if not set */
   if (freeList == NULL)
   {
      freeList = curr;
   }

   /* Attach new _block to prev _block */
   if (last)
   {
      last->next = curr;
   }

   /* Update _block metadata */
   curr->size = size;
   curr->next = NULL;
   // It shows the block is in use
   curr->free = false;
   // Increment of grows vaariable
   num_grows++;
   // Growth of maxheap
   max_heap = max_heap +size;

   // Increment of number of blocks
   num_blocks++;
   return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process
 * or NULL if failed
 */
void *malloc(size_t size)
{

   if( atexit_registered == 0 )
   {
      atexit_registered = 1;
      atexit( printStatistics );
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0)
   {
      return NULL;
   }

   /* Look for free _block */
   struct _block *last = freeList;

   struct _block *next = findFreeBlock(&last, size);

   /* TODO: Split free _block if possible */
/*
    if (curr-> size > 2*size) {

            size_t newsplitsize;
            newsplitsize = curr -> size - size - sizeof(struct _block);
            curr-> size = newsplitsize;

              num_splits++;


    }

    */

   /* Could not find free _block, so grow heap */
   if (next == NULL)
   {
      next = growHeap(last, size);
      num_reuses--;

   }

   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL)
   {
      return NULL;
   }

   /* Mark _block as in use */
   next->free = false;
   // Increment the number of mallocs
   // allocated
   num_mallocs++;
   // Increase the requested size
   num_requested = num_requested + size;


   /* Return data address associated with _block */
   return BLOCK_DATA(next);
}

/* The implementation of calloc is done by
   allocating contiguous block of memory
   which is sufficient of n elements with size
   bytes individually.

*/

void *calloc(size_t nmemb, size_t size)
{

   size_t byte_count = nmemb * size;

   void *mem_ptr = malloc(byte_count);

    // Zeroing of all memory bytes by memset
   if (mem_ptr)

   {
    //Initialization of memory to zero
    memset(mem_ptr,0, byte_count);

   }

   return mem_ptr;


}


// In this we are using the malloc for resizing the memory
// so that we can reallocate the memory
void *realloc(void *ptr, size_t size)
{

    void *datablock = NULL;

    if(size)
    {
       // returning after the default allocation of size
       if(!ptr)

       {
           return malloc(size);

       }

       // Assigning the default memory size
       datablock = malloc(size);

       if(datablock)

       {
           //Copying the memory bytes to datablock

           memcpy(datablock,ptr, size);
           free(ptr);

       }

    }

    return datablock;

}



/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr)
{
   if (ptr == NULL)
   {
      return;
   }

   /* Make _block as free */
   struct _block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;

   // It will give the count for frees whenever free are completed
   num_frees++;


   struct _block *next = curr -> next;  // Taking the next pointer to point to the next block of current block

   if (next)

   {
       // Checking if the current and next blocks are free or not
       if (curr -> free&&  next -> free )

       {

        size_t sizenew;

        // Assigning the newsize to the block size in addition to next size block and size of structure block in header
        sizenew = curr-> size + next-> size + sizeof(struct _block);
        // Stting the newsize to the block size
        curr  -> size = sizenew;

        curr -> next = next-> next;
        // Incrementing the coalesces
        num_coalesces++;

        num_blocks--; // Decrementing the number of blocks


        }

    }
/* TODO: Coalesce free _blocks if needed */
}
