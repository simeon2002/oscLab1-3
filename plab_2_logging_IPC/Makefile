all: main.c sensor_db.h sensor_db.c logger.h logger.c
	gcc -g -Wall main.c sensor_db.c logger.c -o main.out

run: all
	./main.out

clean_output:
	rm main.out sensor_db.csv gateway.log

zip:
	zip milestone2.zip *.c *.h
