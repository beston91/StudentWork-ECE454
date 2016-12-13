
CC = g++
CFLAGS_default = -O3 -D DEFAULT
CFLAGS_tm = -O3 -D T_MEM 
CFLAGS_global_lock = -O3 -D GLOBAL_LOCK 
CFLAGS_list_lock = -O3 -D LL_LOCK 
CFLAGS_element_lock = -O3 -D E_LOCK 
CFLAGS_reduction = -O3 -D REDUCTION 



all: randtrack_default randtrack_tm randtrack_global_lock randtrack_list_lock randtrack_element_lock randtrack_reduction 

randtrack_default: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS_default) randtrack.cc -o randtrack

randtrack_tm: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS_tm) randtrack.cc -o randtrack_tm -lpthread -fgnu-tm

randtrack_global_lock: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS_global_lock) randtrack.cc -o randtrack_global_lock -lpthread

randtrack_list_lock: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS_list_lock) randtrack.cc -o randtrack_list_lock -lpthread

randtrack_element_lock: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS_element_lock) randtrack.cc -o randtrack_element_lock -lpthread

randtrack_reduction: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS_reduction) randtrack.cc -o randtrack_reduction -lpthread

clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock randtrack_element_lock randtrack_reduction
