#include <stdio.h>
#include <stdlib.h>
#include "serveur.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>


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
char** 		mem_store_pointer(char **tabs, char *tab, int i)
{

	printf("%d\n",i );
	if(i==1){
		tabs = (char**)malloc(sizeof(char*));
		tabs[i-1] = (char*)malloc(sizeof(char)*strlen(tab));
		tabs[i-1] = tab;
		printf("|||| %s\n",tabs[i-1] );
	}
	else if(i>1){
		printf("ouais ouais\n");	
		char **t1;
		t1 = realloc(tabs,i*sizeof(char*));
		if(t1==NULL){
			free(tabs);
		}
		else{
			tabs = t1;
			tabs[i-1] = (char*)malloc(sizeof(char)*strlen(tab));
			tabs[i-1] = tab;
			printf("|||| %s\n",tabs[i-1] );
			printf("|||| %s\n",tabs[i-2] );

		}
	}
	return tabs;
}


/* fonction permettant d'extraire une requete du message recu par le serveur;
elle s'occupe uniquement d'extraire le premier élément du message et n'en pas de savoir
si la requete est reconnue par le serveur ou pas*/
char 		*extract_first_str(char *message)
{
	char tmp[100]; char *req;
	int len;
	sscanf(message, "%s ",tmp);
	for (len = 0; tmp[len] != '\0' || len > 99; ++len);

	if(len == 0)
		return NULL;
	else
		req = (char*)malloc((len+1) * sizeof(char) );
	strncpy(req,tmp,len+1);
	req[len] = '\0';
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
	char *s;  char *str; char tmp[100];	char **tabs = NULL;
	int len; int nb_h_t = 0; //nombre d'hotel trouvé 
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
		str = tolowercase(str);
		s = tolowercase(s);
		if(strcmp(str,s)==0){
			nb_h_t++;
			if(nb_h_t > 0){
				if(Emission("chambre disponible 200	\n") !=1)
					printf("Erreur d'emission.\n");
			}
			tabs = mem_store_pointer(tabs,tmp, nb_h_t);
			printf("tabs = %s\n",tabs[nb_h_t-1] );
			if( Emission(tmp) !=1)// aulieu d'emission, il faudrait stocker tmp dans un **p ainsi on pourra retourner tout les
				// trouver qui correspondent au exigence du client 	
				printf("Erreur d'emission.\n");
			//free(message);
		}
	}
	if(nb_h_t == 0){
		return 1;
	}
	//printf("%s%s\n",tabs[0], tabs[1] );
    //printf("zefdzezf%s\n",str);
    free(tabs);
	return 0;
}
int main ()
{
	int fini, err;	
	char *message = NULL;
	char *requete;
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
					// stocker la recherche dans un pointer de pointer **p, puis envoyer au client pour faire son choix
					// recevoir le choix de l'utilisateur, stocker la reservation dans le fichier de reservation
					// pour une autre reservation il faudra comparer le nombre de chambre de l'hotel choisit par l'utilisateur 
					// et le nombre de chambre déjà reserver à cet hotel pour savoir s'il ya  des chambres de disponible
					if(search(message)==0){
						Emission("200\n");
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

