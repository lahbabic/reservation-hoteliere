all: client.o serveur.o mainClient mainS
FLAGS =-DUSECOLORS -Wall -g -DPRODUCTION 

client.o: client/client.c client/client.h
	cd client | gcc $(FLAGS) -c $^ **--outdir=client/**| cd ..

serveur.o: serveur/serveur.c serveur/serveur.h
	cd serveur | gcc $(FLAGS) -c $^ **--outdir=serveur/**| cd .. 

mainClient: client/mainClient.c client/client.o 
	gcc $(FLAGS) $^ -o client/$@

mainS: serveur/mainS.c serveur/serveur.o
	gcc $(FLAGS) $^ -o serveur/$@
	
<<<<<<< HEAD
clean: 
	rm serveur/*.o serveur/mainS; rm client/*.o client/mainClient
=======
clean: serveur/mainServeur.o client/mainClient.o
	rm serveur/serveur.o | rm  client/client.o 
>>>>>>> 26375247e1e95aa3af0d993d9fe5b8dd2a13e67c


