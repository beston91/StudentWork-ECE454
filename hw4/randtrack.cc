
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 

#include "defs.h"
#include "hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4



/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "Need for Debug",                  /* Team name */

    "Yuan Feng",                    /* First member full name */
    "999284876",                 /* First member student number */
    "michaelyuan.feng@mail.utoronto.ca",                 /* First member email address */

    "Mingqi Hou",                           /* Second member full name */
    "999767676",                           /* Second member student number */
    "mingqi.hou@mail.utoronto.ca"                            /* Second member email address */
};

unsigned num_threads;
unsigned samples_to_skip;

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};

  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

#ifdef REDUCTION
  void deep_copy(hash<sample,unsigned> *src, hash<sample,unsigned> *dest);
#endif

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned". 

hash<sample,unsigned> h;

struct task_range{
  int begin;
  int end;
  int id;
};

#ifdef E_LOCK
  pthread_mutex_t e_mutex[RAND_NUM_UPPER_BOUND];
#endif


pthread_t * tid;

#ifdef  GLOBAL_LOCK
pthread_mutex_t mutex_lock;
#endif

struct task_range * range;
void* task(void * task_range_split);
hash<sample,unsigned> * hash_list;

#define tid_create(x) {\
tid = (pthread_t *)malloc((x) * sizeof(pthread_t));\
}

#define task_split(x) \
do{\
  if((x)==1 || (x)==0){\
    range = (struct task_range*)malloc(1 * sizeof(struct task_range));\
    range->begin = 0;\
    range->end = NUM_SEED_STREAMS;\
    range->id = 0;\
  }\
  else{\
    int stride = NUM_SEED_STREAMS/(x);\
    range = (struct task_range*)malloc((x) * sizeof(struct task_range));\
    for(int i = 0; i < (x); i++){\
      if(i == 0){\
        (range+i)->begin=i;\
        (range+i)->end=(range+i)->begin+stride;\
        (range+i)->id = i;\
      }\
      else{\
        (range+i)->begin = (range + i - 1)->end;\
        (range+i)->end = (range+i)->begin + stride;\
        (range+i)->id = i;\
      }\
    }\
  }\
}while(0)

#define thread_create(x)  \
  do{\
    for(int i = 0; i < (x); i++){   \
      pthread_create((tid+i), NULL, &task, (void*)(range + i));\
  }\
}while(0)

#define thread_join(x)  \
do{\
  for(int i = 0; i < (x); i++){\
    pthread_join(*(tid+i), NULL);\
  }\
}while(0)

#define hash_create(x)  {\
  hash_list = (hash<sample,unsigned> *)malloc((x) * sizeof(hash<sample,unsigned>));\
  for(int i = 0 ; i < (x); i++){\
    (hash_list+i)->setup(14);\
  }\
}




int  
main (int argc, char* argv[]){

#ifdef  GLOBAL_LOCK
  pthread_mutex_init(&mutex_lock, NULL);
#endif

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );
  printf( "Student 2 Name: %s\n", team.name2 );
  printf( "Student 2 Student Number: %s\n", team.number2 );
  printf( "Student 2 Email: %s\n", team.email2 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);

#ifdef GLOBAL_LOCK
  //printf("This GLOBAL LOCK\n");

  tid_create(num_threads);

  task_split(num_threads);

  thread_create(num_threads);

  thread_join(num_threads);

  pthread_mutex_destroy(&mutex_lock);
#endif

#ifdef T_MEM
  //printf("This Transaction Memory\n");

  tid_create(num_threads);

  task_split(num_threads);

  thread_create(num_threads);

  thread_join(num_threads);
#endif

#ifdef LL_LOCK
 //printf("This  LIST LOCK\n");

  tid_create(num_threads);

  task_split(num_threads);

  thread_create(num_threads);

  thread_join(num_threads);
#endif

#ifdef E_LOCK
 //printf("This ELEMENT LOCK2\n");
int t;
for(t=0; t < RAND_NUM_UPPER_BOUND; t++){

pthread_mutex_init(&e_mutex[t], NULL);
}


  tid_create(num_threads);

  task_split(num_threads);

  thread_create(num_threads);

  thread_join(num_threads);
#endif

#ifdef REDUCTION
  //printf("This REDUCTION LOCK\n");

  tid_create(num_threads);

  task_split(num_threads);

  hash_create(num_threads);

  thread_create(num_threads);

  thread_join(num_threads);

int k;
for(k=0; k < num_threads; k++){
deep_copy(hash_list+k, &h);
}
#endif


#ifdef DEFAULT
  //prf("This DEFAULT\n");

  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  for (i=0; i<NUM_SEED_STREAMS; i++){
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      // if this sample has not been counted before
      if (!(s = h.lookup(key))){
  
        // insert a new element for it into the hash table
        s = new sample(key);
        h.insert(s);
      }

      // increment the count for the sample
      s->count++;
    }
  }
#endif


  // print a list of the frequency of all samples
  h.print();

}

#ifdef GLOBAL_LOCK
void* task(void * task_range_split){
  //printf("This GLOBAL LOCK2\n");
  // process streams starting with different initial numbers
  int i, j, k,rnum;
  unsigned key;
  sample *s;

  struct task_range *local_range = (struct task_range *) task_range_split;

  for (i = local_range->begin ; i < local_range->end; i++){
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      pthread_mutex_lock(&mutex_lock); 

      // if this sample has not been counted before
      if (!(s = h.lookup(key))){
       
      // insert a new element for it into the hash table
      s = new sample(key);
      h.insert(s);
      }
       s->count++;

      pthread_mutex_unlock(&mutex_lock);

    }
  }
      // increment the count for the sample
  pthread_exit(NULL);
}
#endif


#ifdef T_MEM
void* task(void * task_range_split){
  //printf("This Transaction Memory2\n");
  // process streams starting with different initial numbers
  int i, j, k,rnum;
  unsigned key;
  sample *s;

  struct task_range *local_range = (struct task_range *) task_range_split;

  for (i = local_range->begin ; i < local_range->end; i++){
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      __transaction_atomic{
      // if this sample has not been counted before
        if (!(s = h.lookup(key))){
         
        // insert a new element for it into the hash table
        s = new sample(key);
        h.insert(s);
        }
        
        s->count++;
      }
    }
  }
      // increment the count for the sample
  pthread_exit(NULL);
}
#endif

#ifdef LL_LOCK
void* task(void * task_range_split){
	 // printf("This  LIST LOCK2\n");
  // process streams starting with different initial numbers
  int i, j, k,rnum;
  unsigned key;
  sample *s;

  struct task_range *local_range = (struct task_range *) task_range_split;

  for (i = local_range->begin ; i < local_range->end; i++){
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      h.this_list_lock(key);
      // if this sample has not been counted before
      if (!(s = h.lookup(key))){
       
      // insert a new element for it into the hash table
      s = new sample(key);
      h.insert(s);
      }
      
      s->count++;
      h.this_list_unlock(key);
    }
    
  }
      // increment the count for the sample
  pthread_exit(NULL);
}
#endif

#ifdef E_LOCK
void* task(void * task_range_split){
  //printf("This ELEMENT LOCK3\n");

  // process streams starting with different initial numbers
  int i, j, k,rnum;
  unsigned key;
  sample *s;

  struct task_range *local_range = (struct task_range *) task_range_split;

  for (i = local_range->begin ; i < local_range->end; i++){
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

    	pthread_mutex_lock(&(e_mutex[key]));
      // if this sample has not been counted before
      if (!(s = h.lookup(key))){
       
        // insert a new element for it into the hash table
        s = new sample(key);

        h.insert(s);
      }
      
      
      s->count++;
      pthread_mutex_unlock(&(e_mutex[key]));
    }
    
  }
      // increment the count for the sample
  pthread_exit(NULL);
}
#endif

#ifdef REDUCTION
void* task(void * task_range_split){
  //printf("This reduction 2\n");

  // process streams starting with different initial numbers
  int i, j, k,rnum;
  unsigned key;
  sample *s;
  hash<sample,unsigned> * this_hash = &hash_list[((struct task_range*)task_range_split)->id];
  //hash<sample,unsigned> this_hash;//may not safe for stack
  //printf("--%x--", this_hash);
  //printf("--%d--", ((struct task_range*)task_range_split)->begin);
  //printf("--%d--", ((struct task_range*)task_range_split)->end);

  struct task_range *local_range = (struct task_range *) task_range_split;

  for (i = local_range->begin ; i < local_range->end; i++){
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      // if this sample has not been counted before
      if (!(s = this_hash->lookup(key))){
       
        // insert a new element for it into the hash table
        s = new sample(key);
        this_hash->insert(s);
      }
      
      s->count++;
    }
    
  }

 //deep_copy(this_hash, &h);
      // increment the count for the sample
  pthread_exit(NULL);
}
#endif

#ifdef REDUCTION
void deep_copy(hash<sample,unsigned> *src, hash<sample,unsigned> *dest){
  int i;
  sample *src_sample;
  sample *dest_sample;

  //key range is [0...3]
  for(i = 0; i < RAND_NUM_UPPER_BOUND; i++){
    if((src_sample = src->lookup(i)) && !(dest_sample = dest->lookup(i))){
      dest_sample = new sample(i);
      dest->insert(dest_sample);
      dest_sample->count += src_sample->count;
    }
    else if((src_sample = src->lookup(i)) && (dest_sample = dest->lookup(i))){
      dest_sample->count += src_sample->count;
    }
  }

}
#endif



