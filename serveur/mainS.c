#include <stdio.h>
#include <stdlib.h>
#include "serveur.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char 		*extract_str(char *message, int i);
char *tolowercase(char *tmp)//met tous les caractères en minuscule pour une bonne comparaison
{

	if(!tmp)
		return NULL;
	int i;
	for (i = 0; tmp[i] != '\0' ; ++i)
	{
		tmp[i] = (char)tolower(tmp[i]);
	}
	tmp[i] = '\0';
	return tmp;
}



//fonction permettant de gérer la mémoire et de stocker le pointer donner en argument  
char** 		mem_store_pointer(char **tabs, char *tab, int i)//mem and store
{

	if(i==1){
		tabs = (char**)malloc(sizeof(char*));
		tabs[i-1] = (char*)malloc(sizeof(char)*strlen(tab)+1);
		strncpy(tabs[i-1],tab,strlen(tab));	
		tabs[i-1][strlen(tab)]= '\0';
	}
	else if(i>1){	
		char **t1;
		t1 = (char**)realloc(tabs,i*sizeof(char*)); //reallouer dans la matrice une ligne suplémentaire 
		if(t1==NULL){
			free(tabs);
		}
		else{
			tabs = t1;
			tabs[i-1] = (char*)malloc(sizeof(char)*strlen(tab)+1);// allouer de l'espace à cette nouvelle ligne
			strncpy(tabs[i-1],tab,strlen(tab));	// copier le contenu de tmp dans la ligne
			tabs[i-1][strlen(tab)]= '\0';
		}
	}
	return tabs;
}


/* fonction permettant d'extraire une requete du message recu par le serveur;
elle s'occupe uniquement d'extraire le premier élément du message et n'en pas de savoir
si la requete est reconnue par le serveur ou pas*/
char 		*extract_str(char *message, int i)
{
	char tmp[100]; char *req;
	int len = 0; int len_r = 0;
	while(i>=1){
		sscanf(message+len_r, "%s ",tmp);
		for (len = 0; tmp[len] != '\0' || len > 99; ++len);

		if(len == 0)
			return NULL;
		else
			req = (char*)malloc((len+1) * sizeof(char) );
		strncpy(req,tmp,len+1);
		req[len] = '\0';
		len_r = strlen(req) + len_r+1;
		i--;
	}
	return req;
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
	requete = extract_str(message,1);
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
char** 		search(char* message)
{
	FILE *fp;
	char *s;  char *str; char tmp[100];	char **tabs = NULL;
	int cat =0,nb_h_t = 0; //nombre d'hotel trouvé 
	int cat_d = 0; // catégorie demandée
	str = extract_str(message, 2);	// extraire le deuxième élement : la ville
	cat = atoi(extract_str(message,3));
	fp = fopen("hotel","r");
	if(fp == NULL) //Erreur d'ouverture du fichier
		return NULL;
	while(fgets(tmp, 100, fp))
	{
		s = extract_str(tmp, 2);	//la ville 
		str = tolowercase(str);// mettre le tous en miniscule
		s = tolowercase(s);
		if(strcmp(str,s)==0){// si la ville est trouvée dans le fichier hotel
			++nb_h_t;
			cat_d = atoi(extract_str(tmp,3));
			if(cat == cat_d){
				tabs = mem_store_pointer(tabs,tmp, nb_h_t);
				if(tabs == NULL)
					return NULL;
				if( Emission(tmp) !=1)// aulieu d'emission, il faudrait stocker tmp dans un **p ainsi on pourra retourner tout les
					printf("Erreur d'emission.\n");	// trouver qui correspondent au exigence du client 	
			}
				
			//free(message);
		}
		free(s);
	}
	if(nb_h_t == 0){
		return NULL;
	}
	free(str); 
   	return tabs;
}


int main ()
{
	int fini, err;	
	char *message = NULL;
	char *requete;
	char **tabs;

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
				requete = extract_str(message,1);
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
					// stocker la recherche dans un pointer de pointer **p, puis envoyer au client pour faire son choix
					// recevoir le choix de l'utilisateur, stocker la reservation dans le fichier de reservation
					// pour une autre reservation il faudra comparer le nombre de chambre de l'hotel choisit par l'utilisateur 
					// et le nombre de chambre déjà reserver à cet hotel pour savoir s'il ya  des chambres de disponible
					if((tabs = search(message))!=NULL){
						Emission("200\n");
						printf("laaa taile de tabs est %zu\n",sizeof(tabs) );
					}
					else{ 
						Emission("400\n");
					}
				}
				free(message);
			}else
				fini = 1;
		}

		TerminaisonClient();
	}

	return 0;
}

