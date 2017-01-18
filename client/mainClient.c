#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include <string.h>

void 			vider_buffer();

/*fonction permettant de sécuriser la saisie de l'utilisateur, pour éviter des buffers overflow,
ainsi que si l'utilisateur ne saisie rien on lui demandera de resaisir*/
char            *getStr(){

	int len; 
	char tmp[1000];
	char *input;
	printf("\n>>  ");
	if(fgets(tmp, 1000, stdin)==NULL || tmp[0] == '\n'){
		printf("Vous n'avez rien saisie, veuillez reesayer:\n");
		return NULL;				//aucune saisie
	}

	if(tmp[strlen(tmp)-1] != '\n'){ //trop de caractère pour le pointer tmp
		vider_buffer();
		printf("Vous avez saisie trop de caractere, veuillez reesayer:\n");
		return NULL;
	}
	tmp[strlen(tmp)-1] = '\0'; // enlever \n de la chaine
	len = strlen(tmp);
	input = (char *)malloc(len*sizeof(char));
	while(len >= 0){
		input[len] = tmp[len];
		len--;
	}
	return input;
}

void 			menu_reservation(){

	printf("\nTrouvez la chambre qui vous convient!\n");
	char *ville;
	char *nom;
	char *prenom;
	int cat; //catégorie
	char buff[1024];
	int len;
	//date 
	//char *message;
	printf("\nQuel est votre Nom:\n");
	while((nom = getStr()) == NULL);
	printf("\nQuel est votre Prenom:\n");
	while((prenom = getStr()) == NULL);
	printf("\nLa Ville de votre sejour:\n");
	while((ville = getStr()) == NULL);
	printf("\nLa categorie de l'hotel souhaiter:\n\n>>  ");
	do{
		scanf("%d",&cat);
		vider_buffer();
		if(cat <= 0 || cat > 5)
			printf("\nLa categorie doit etre comprise entre 1 et 5\n\n>>  ");	
	}while(cat <= 0 || cat > 5);
	snprintf(buff,1023,"INSERT %s %s %d %d %d\n",nom ,ville ,cat ,prix_ch ,nb_ch);
	len = strlen(buff);
	message = (char*)malloc(len*sizeof(char));
	while(len >= 0){
		message[len] = buff[len];
		len--;
	}
	// envoyer la requete au serveur 
	if( Emission(message) !=1)
		printf("Erreur d'emission.\n");
	free(message);
	free(nom); free(ville); free(prenom);
}

void 			menu_enregistrement(){

	//ch	: Tableau[2][nb_ch] 
	char *ville;	char *nom;	char *message;
	char buff[1024];
	int len, cat, prix_ch, nb_ch;

	//Saisie des informations
	printf("\nQuel est Le nom de l'hotel:\n");
	while((nom = getStr()) == NULL);
	printf("\nDans quel ville se trouve t-il:\n");
	while((ville = getStr()) == NULL);
	printf("Quel est sa categorie:\n\n>>  ");
	do{
		scanf("%d",&cat);
		vider_buffer();
		if(cat <= 0 || cat > 5)
			printf("\nLa categorie doit etre comprise entre 1 et 5\n\n>>  ");	
	}while(cat <= 0 || cat > 5);
	printf("Quel est le nombre de chambres disponibles dans cette hotel:\n\n>>  ");
	do{
		scanf("%d",&nb_ch);
		vider_buffer();
		if(nb_ch <= 0)
			printf("\nVeuillez saisir un nombre positif\n\n>>  ");	
	}while(nb_ch <= 0);
	printf("Quel est le prix des chambres:\n\n>>  ");
	do{
		scanf("%d",&prix_ch);
		vider_buffer();
		if(prix_ch <= 0)
			printf("\nVeuillez saisir un nombre positif\n\n>>  ");	
	}while(prix_ch <= 0);
	//Formater la requete à envoyer au serveur
	// [Nom d’hôtel] [Ville] [Catégorie] [Prix des chambres] [ Nombre de chambre]

	snprintf(buff,1023,"INSERT %s %s %d %d %d\n",nom ,ville ,cat ,prix_ch ,nb_ch);
	//transformer le tableau en pointer pour pouvoir l'utiliser hors de cette fonction
	len = strlen(buff);
	message = (char*)malloc(len*sizeof(char));
	while(len >= 0){
		message[len] = buff[len];
		len--;
	}
	// envoyer la requete au serveur 
	if( Emission(message) !=1)
		printf("Erreur d'emission.\n");
	free(message);
	//recevoir la réponse du serveur et l'afficher 
	message = Reception();
	printf("\n\n###  %s\n\n\n",message);
	free(message); free(ville); free(nom);
	
}

int main() {
	//char *message;
	int choix;
	int fini = 1;
	if(InitialisationAvecService("127.0.0.1", "12321") != 1) {
			printf("Erreur d'initialisation\n");
			exit(0);
		}
	while(fini){
		//affichage du menu et traitement du choix de l'utilisateur
		printf("\n\n\n\n\t\t**** MENU ****");
		printf("\n\t------------------------------");
		printf("\n\t1. Reserver une chambre \n\t2. S'enregistrer en tant qu'hotel\n\t3. Quitter\n");
		printf("\n\n Entrez votre choix: ");
		scanf("%1d",&choix);
		vider_buffer();
		switch(choix)
		{
			case 1:
			    menu_reservation();
			    break;
			case 2:
			    menu_enregistrement();
			    break;
			case 3:
				fini = 0;
				break;
			default:
			    printf("\nSelection invalide ... veuillez reesayer.\n");
		}
		choix = 99;
	}

	Terminaison();
	/*if(InitialisationAvecService("127.0.0.1", "12321") != 1) {
		printf("Erreur d'initialisation\n");
		return 1;
	}

	if(Emission("GET /Makefile HTTP/1.1\nHost : 127.0.0.1\n\
Accept : text/html\nAccept : text/plain\nUser-Agent : Lynx/2.4 libwww/2.1.4\n\n")!=1) {
		printf("Erreur d'Žmission\n");
		return 1;
	}
	if((message = Reception()) !=  NULL)
		printf("J'ai recu:\n");
	while((message = Reception()) !=  NULL)
	{
		printf("%s\n", message);
		free(message);	
	}*/
	/*if(Emission("Host : www.stri.ups-tlse.fr\n")!=1) {
		printf("Erreur d'Žmission\n");
		return 1;
	}
	if(Emission("Accept : text/html\n")!=1) {
		printf("Erreur d'Žmission\n");
		return 1;
	}
	if(Emission("Accept : text/plain\n")!=1) {
		printf("Erreur d'Žmission\n");
		return 1;
	}
	if(Emission("User-Agent : Lynx/2.4 libwww/2.1.4\n\n")!=1) {
		printf("Erreu	r d'Žmission\n");
		return 1;
	}*/

	/*message = Reception();
	if(message != NULL) {
		printf("J'ai recu: %s\n", message);
		free(message);
	} else {
		printf("Erreur de rŽception\n");
		return 1;
	}*/
	

	

	return 0;
}

void vider_buffer(){

	int buf;
	do
	{
		buf = getchar();
	}while(buf != EOF && buf != '\n');
}