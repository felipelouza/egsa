CC = gcc
CWARNING =  -Wall -Wno-unused-result -Wno-implicit-function-declaration -Wno-comment -Wno-format 
#VLIB= -g -O0
MY_CXX_OPT_FLAGS= -O3 -ffast-math -funroll-loops -m64 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64

LFLAGS = -lm -ldl
LIBOBJ = external/malloc_count/malloc_count.o\
		heap.o\
		utils.o\
		file.o\
		esa.o\
		lcp.o\
		external/sais-lite-LCP.o\
		external/sa-is.o\
##

BWT = 0
DEBUG = 0
MODE = 2 #DNA = 1; READ = 2


DEFINES = -DDEBUG=$(DEBUG) -DMODE=$(MODE) -DBWT=$(BWT)
CFLAGS = $(MY_CXX_OPT_FLAGS) $(CWARNING) $(LFLAGS) $(VLIB) $(DEFINES)

########################################################################

CHECK = 1
PRE = 1

#DIR = example/fastq/
DIR = example/fasta/

#INPUT = reads-10000.fastq
INPUT = all.in

K =  5000
#K =  5

#1024MB
MEMLIMIT = 1 

########################################################################

all: egsa

flush:
	sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
	
clean:

	rm *.o
	rm external/*.o
	rm egsa

erase:
	rm $(DIR)*.gesa
	rm $(DIR)*.bin
	rm -Rf $(DIR)*.esa
	rm -Rf $(DIR)partition/*
	rm -Rf $(DIR)tmp/*
	
egsa: egsa.c ${LIBOBJ} defines.h utils.h
	$(CC) $(LIBOBJ) $(CFLAGS) egsa.c -o egsa 

##
LIBH = defines.h utils.h
esa.o 	: $(LIBH)
heap.o	: $(LIBH)
file.o	: $(LIBH)
utils.o	: $(LIBH)
lcp.o	: $(LIBH)

##

run: egsa
	./egsa $(PRE) $(DIR) $(INPUT) $(K) $(MEMLIMIT) $(CHECK)

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./egsa2 $(PRE) $(DIR) $(INPUT) $(K) $(MEMLIMIT) $(CHECK)
