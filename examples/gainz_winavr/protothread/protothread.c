
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>

#include "D:/dev/openwsn/source/node/common/thirdpaty/protothreads/pt.h"
#include "D:/dev/openwsn/source/node/common/thirdpaty/protothreads/pt-sem.h"
//#include "../../common/thirdparty/protothreads/pt.h"
//#include "../../common/thirdparty/protothreads/pt-sem.h"
 
#define NUM_ITEMS 32
#define BUFSIZE 8

static int buffer[BUFSIZE];
static int bufptr;

static void
add_to_buffer(int item)
{
  printf("Item %d added to buffer at place %d\n", item, bufptr);  
  buffer[bufptr] = item;
  bufptr = (bufptr + 1) % BUFSIZE;
}
static int
get_from_buffer(void)
{
  int item;
  item = buffer[bufptr];
  printf("Item %d retrieved from buffer at place %d\n",
	 item, bufptr);
  bufptr = (bufptr + 1) % BUFSIZE;
  return item;
}

static int
produce_item(void)
{
  static int item = 0;
  printf("Item %d produced\n", item);
  return item++;
}

static void
consume_item(int item)
{
  printf("Item %d consumed\n", item);
}

static struct pt_sem full, empty;
 
static 
PT_THREAD(producer(struct pt *pt))
{
  static int produced;
  
  PT_BEGIN(pt);
  
  for(produced = 0; produced < NUM_ITEMS; ++produced) {
  
    PT_SEM_WAIT(pt, &full);
    
    add_to_buffer(produce_item());
    
    PT_SEM_SIGNAL(pt, &empty);
  }

  PT_END(pt);
}
 
static 
PT_THREAD(consumer(struct pt *pt))
{
  static int consumed;
  
  PT_BEGIN(pt);
 
  for(consumed = 0; consumed < NUM_ITEMS; ++consumed) {
    
    PT_SEM_WAIT(pt, &empty);
    
    consume_item(get_from_buffer());    
    
    PT_SEM_SIGNAL(pt, &full);
  }
 
  PT_END(pt);
}
 
static 
PT_THREAD(driver_thread(struct pt *pt))
{
  static struct pt pt_producer, pt_consumer;
 
  PT_BEGIN(pt);
  
  PT_SEM_INIT(&empty, 0);
  PT_SEM_INIT(&full, BUFSIZE);
 
  PT_INIT(&pt_producer);
  PT_INIT(&pt_consumer);
 
  PT_WAIT_THREAD(pt, producer(&pt_producer) &
		     consumer(&pt_consumer));
 
  PT_END(pt);
}


int
main(void)
{
  struct pt driver_pt;

  PT_INIT(&driver_pt);

  while(PT_SCHEDULE(driver_thread(&driver_pt))) {

    /*
     * When running this example on a multitasking system, we must
     * give other processes a chance to run too and therefore we call
     * usleep() resp. Sleep() here. On a dedicated embedded system,
     * we usually do not need to do this.
     */
#ifdef _WIN32
    Sleep(0);
#else
    //usleep(10);
#endif
  }
  return 0;
}
