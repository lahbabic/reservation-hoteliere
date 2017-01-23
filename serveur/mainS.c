#include <stdio.h>
#include <stdlib.h>
#include "serveur.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>

typedef struct choix_h *c_h;
struct choix_h{		//structure qui va stocker les hotels ayant les caractéristiques de ce que le client demande
	char **tabs;	// matrices d'hotel avec leurs caractérstiques
	int nb_h_t; // nombres d'hotel trouvée 
};

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
c_h 		mem_store_pointer(c_h t_h, char *tab)//mem and store
{
	int i = t_h->nb_h_t;
	if(i==1){
		t_h->tabs = (char**)malloc(sizeof(char*));
		t_h->tabs[i-1] = (char*)malloc(sizeof(char)*(strlen(tab)+1));
		strncpy(t_h->tabs[i-1],tab,strlen(tab)+1);	
		t_h->tabs[i-1][strlen(tab)]= '\0';
	}
	else if(i>1){	
		char **t1;
		t1 = (char**)realloc(t_h->tabs,i*sizeof(char*)); //reallouer dans la matrice une ligne suplémentaire 
		if(t1==NULL){
			free(t_h->tabs);
		}
		else{
			t_h->tabs = t1;
			t_h->tabs[i-1] = (char*)malloc(sizeof(char)*(strlen(tab)+1));// allouer de l'espace à cette nouvelle ligne
			strncpy(t_h->tabs[i-1],tab,strlen(tab)+1);	// copier le contenu de tmp dans la ligne
			t_h->tabs[i-1][strlen(tab)]= '\0';
		}
	}
	return t_h;
}

void		free_str(c_h t_h)
{

	int i;
	for (i = 0; i < t_h->nb_h_t; ++i)
	{
		free(t_h->tabs[i]);
	}
	free(t_h->tabs);		
	free(t_h);
}

/* fonction permettant d'extraire une requete du message recu par le serveur;
elle s'occupe uniquement d'extraire le premier élément du message et n'en pas de savoir
si la requete est reconnue par le serveur ou pas*/
char 		*extract_str(char *message, int i)
{
	char tmp[100]; char *str = NULL;
	int len = 0; int len_r = 0;
	while(i>=1){
		// récupérer la première sous-chaine qui commence à l'adresse message+len_r et la mettre dans tmp
		sscanf(message+len_r, "%s ",tmp);//possiblité de buffer overflow<!>!!!!	 
		for (len = 0; tmp[len] != '\0' || len > 99; ++len); // compter le nombre de caractères dans tmp

		if(len == 0)
			return NULL;
		else if(str != NULL){
			free(str);
			str = (char*)malloc((len+1) * sizeof(char) );
		}
		else if(str == NULL)
			str = (char*)malloc((len+1) * sizeof(char) );

		strncpy(str,tmp,len+1);
		str[len] = '\0';
		len_r = strlen(str) + len_r+1;
		i--;
	}
	return str;
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
		fclose(fp);	free(requete);
		return 1;
	}	
	fclose(fp);
	free(requete);
	return 0;
}
/* Cette fonction permet de rechercher dans le fichier s'il y a des hotels correspondant 
aux informations demander par le client pour ensuite lui permettre de choisir un hotel afin de reserver
une chambre */
c_h 		search(char* message)
{
	FILE *fp;
	char *s;  char *str; char tmp[100];	char *c;//la catégorie
	int cat =0; //nombre d'hotel trouvé 
	int cat_d = 0; // catégorie demandée
	c_h t_h; //t_h : tableau d'hotels

	t_h = (c_h)malloc(sizeof(struct choix_h));
	t_h->tabs = NULL;
	t_h->nb_h_t = 0;
	str = extract_str(message, 2);	// extraire le deuxième élement : la ville
	c = extract_str(message,3);		
	cat = atoi(c);		free(c);
	fp = fopen("hotel","r");
	if(fp == NULL) //Erreur d'ouverture du fichier
		return NULL;
	while(fgets(tmp, 100, fp))
	{
		s = extract_str(tmp, 2);	//la ville 
		str = tolowercase(str);// mettre le tous en miniscule
		s = tolowercase(s);
		if(strcmp(str,s)==0){	// si la ville est trouvée dans le fichier hotel
			c = extract_str(tmp,3);		
			cat_d = atoi(c);	free(c);
			if(cat == cat_d){
				++t_h->nb_h_t;
				t_h = mem_store_pointer(t_h ,tmp);
				if(t_h->tabs == NULL)
					return NULL;
			}
				
			//free(message);
		}
		free(s);
	}
	free(str);
	fclose(fp); 
   	return t_h;
}


int main ()
{
	int fini, err, i;	
	char *message = NULL;
	char *requete;
	c_h t_h;	
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
					t_h = search(message);
					if(t_h !=NULL){
						i = t_h->nb_h_t;
						while(i>0)
						{
							//printf("%s\n",t_h->tabs[i-1] );
							Emission(t_h->tabs[i-1]);
							i--;
						}
						Emission("200\n");

						free_str(t_h);// free la matrice
						//free(message);  free(requete);
						//exit(1);
					}
					else{ 
						Emission("400\n");
					}
				}
				free(message); free(requete);
			}else
				fini = 1;
		}

		TerminaisonClient();
	}

	return 0;
}

