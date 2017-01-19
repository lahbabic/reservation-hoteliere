#include <stdio.h>
#include <stdlib.h>
#include "serveur.h"
#include <unistd.h>
#include <string.h>

/* fonction permettant d'extraire une requete du message recu par le serveur;
elle s'occupe uniquement d'extraire le premier élément du message et n'en pas de savoir
si la requete est reconnue par le serveur ou pas*/
char 		*extract_first_str(char *message)
{
	char tmp[20]; char *requete;
	int len;
	sscanf(message, "%s ",tmp);
	len = strlen(tmp);
	requete = (char*)malloc(len * sizeof(char) );
	while(len >= 0){
		requete[len] = tmp[len];
		len--;
	}
	return requete;
}

/* cette fonction permet d'inserer les informations relatives à un hotel 
dans la base de donnée(ici un fichier "hotel")*/
int 		insert(char* message)
{
	char *requete;
	int len;
	FILE *fp;
	fp = fopen("hotel","a");
	if(fp == NULL) //Erreur d'ouverture du fichier
		return 1;	
	requete = extract_first_str(message);
	len = strlen(requete);
	/*Si le nombre d'éléments à écrire dans le fichier ne correspond pas aux nombres d'éléments écrit*/
	if(fprintf(fp,"%s",(message+len+1)) != strlen(message+len+1))
	{
		fclose(fp);
		return 1;
	}	
	fclose(fp);
	return 0;
}
/* Cette fonction permet de rechercher dans le fichier s'il y a des hotels correspondant 
aux informations demander par le client pour ensuite lui permettre de choisir un hotel afin de reserver
une chambre */
int 		search(char* message)
{
	FILE *fp;
	char *s;  char *str; char tmp[100];
	int len;
	str = extract_first_str(message);	// extraire la requete afin de savoir sa taille
	len = strlen(str);
	str = extract_first_str(message+len+1);// extraire le premier élément après la requete donc la ville
	fp = fopen("hotel","r");
	if(fp == NULL) //Erreur d'ouverture du fichier
		return 1;
	while(fgets(tmp, 100, fp))
	{
		s = extract_first_str(tmp);	
		len = strlen(s);
		s = extract_first_str(tmp+len+1);
		if(strcmp(str,s)==0){
			if( Emission(tmp) !=1)
				printf("Erreur d'emission.\n");
			//free(message);
		}
	}
	
    printf("zefdzezf%s\n",str);
	return 0;
}
int main ()
{
	int fini, err;	
	char *message = NULL;
	char *requete = NULL;
	//char nomFichier[30];
	//int extr_fich = 5;  // retour de la fonction extraitFichier
	//char *extensionFichier; char *tmp;
	Initialisation();
	while(1) {
		fini = 0;
		
		AttenteClient();
	
		while(!fini) {
			message = Reception();
			printf("J'ai recu: %s\n", message);
			if(message!=NULL){
				requete = extract_first_str(message);
				if(strcmp(requete,"INSERT")==0){
					err = insert(message);
					if(err == 0){
						if(Emission("202 hotel enregistre avec succes\n")!=1)
							printf("Erreur d'emission\n");
					}
					else if(err == 1)
					{
						if(Emission("402 Erreur lors de l'enregistrement\n")!=1)
							printf("Erreur d'emission\n");
					}
				}
				else if(strcmp(requete,"SEARCH")==0)
				{
					search(message);
				}
				free(message);
			}else
				fini = 1;
		}

		TerminaisonClient();
	}

	return 0;
}

