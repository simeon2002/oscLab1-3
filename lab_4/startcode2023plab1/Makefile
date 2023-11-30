
all: main.c datamgr.c datamgr.h config.h lib/dplist.c lib/dplist.h
	gcc -g main.c datamgr.c lib/dplist.c lib/dplist.h -o main.out

gen: config.h file_creator.c 
	gcc -g file_creator.c config.h -o gen.out

gendebug: config.h file_creator.c
	gcc -g file_creator.c config.h -o gendebug.out -DDEBUG

all_with_temps: main.c datamgr.c datamgr.h config.h lib/dplist.c lib/dplist.h
	 gcc -g -Wall main.c lib/dplist.h lib/dplist.c datamgr.c -o main.out -DRUN_AVG_LENGTH=10 -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=12