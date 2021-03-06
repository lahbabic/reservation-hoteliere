all: client.o serveur.o mainClient mainS
FLAGS =-DUSECOLORS -Wall -g -DPRODUCTION 

client.o: client/client.c client/client.h
	cd client | gcc $(FLAGS) -c $^ **--outdir=client/**| cd ..

serveur.o: serveur/serveur.c serveur/serveur.h
	cd serveur | gcc $(FLAGS) -c $^ **--outdir=serveur/**| cd .. 

mainClient: client/mainClient.c client/client.o 
	gcc $(FLAGS) $^ -o client/$@

mainServeur: serveur/mainServeur.c serveur/serveur.o
	gcc $(FLAGS) $^ -o serveur/$@

mainS: serveur/mainS.c serveur/serveur.o
	gcc $(FLAGS) $^ -o serveur/$@

clean: 
	rm serveur/*.o serveur/mainS serveur/mainServeur; rm client/*.o client/mainClient



