all: test_server sensor_node

run: runserver

clean:
	rm test_server
	rm sensor_node

test_server: test_server.c lib/tcpsock.c
	gcc -g -pthread test_server.c lib/tcpsock.c -o test_server

sensor_node: sensor_node.c lib/tcpsock.c
	gcc -DLOOPS=5 -g sensor_node.c lib/tcpsock.c -o sensor_node

runserver: test_server
	./test_server 5678 4

runclient1: sensor_node
	./sensor_node 1 1 127.0.0.1 5678

runclient2: sensor_node
	./sensor_node 2 1 127.0.0.1 5678

runclient3: sensor_node
	./sensor_node 3 1 127.0.0.1 5678

runclient4: sensor_node
	./sensor_node 4 1 127.0.0.1 5678

runclient5: sensor_node
	./sensor_node 5 .1 127.0.0.1 5678
