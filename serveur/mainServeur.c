#include <stdio.h>
#include <stdlib.h>
#include "serveur.h"
#include <unistd.h>
#include <string.h>


int main ()
{
	int fini;
	char *message = NULL;
	char nomFichier[30];
	int extr_fich = 5;  // retour de la fonction extraitFichier
	char *extensionFichier; char *tmp;
	Initialisation();
	while(1) {
		fini = 0;
	
		AttenteClient();
	
		while(!fini) {
			message = Reception();
			printf("J'ai recu: %s\n", message);
			if(message!=NULL){
				if(message[0] == 'G' && message[1] == 'E' && message[2] == 'T')
				{
					extr_fich = extraitFichier(message, nomFichier, 30);
					if((extr_fich == 0)&&(strcmp(nomFichier,"")==0)){	//si aucun fichier n'est demandé retourner "index.html" 
						if(access("index.html",F_OK) != -1){
							if(envoyerReponse200HTML("index.html")==0)
								envoyerContenuFichierTexte("index.html");
						}
						else if(access("index.php",F_OK) != -1){
							if(envoyerReponse200HTML("index.php")==0)
								envoyerContenuFichierTexte("index.php");
						}
						else{
							envoyerReponse500("Fichier manquant :index.html; aucune page n'a était demandé.");
							extr_fich = 1;
						}
					}else if((extr_fich == 0)&&(access(nomFichier,F_OK) == -1)){ 
						envoyerReponse404(nomFichier);
						extr_fich = 1;
					}else{
						tmp = strdup(nomFichier);
						extensionFichier = strtok(tmp,".");
						extensionFichier = strtok(NULL,".");
						if((extr_fich == 0)&&(strcmp(extensionFichier,"html")==0)){
							if(envoyerReponse200HTML(nomFichier)==0)
								envoyerContenuFichierTexte(nomFichier);
						}
						else if((extr_fich == 0)&&(strcmp(extensionFichier,"php")==0)){
							
							if(envoyerReponse200JPG(nomFichier)==0)
								envoyerContenuFichierBinaire(nomFichier);
						}
						else if((extr_fich == 0)&&(strcmp(extensionFichier,"jpg")==0)){
							
							if(envoyerReponse200JPG(nomFichier)==0)
								envoyerContenuFichierBinaire(nomFichier);
						}
						else if((extr_fich == 0)&&(strcmp(extensionFichier,"ico")==0)){
							
							if(envoyerReponse200ICO(nomFichier)==0)
								envoyerContenuFichierBinaire(nomFichier);
						}
						else if(extr_fich == -2){
							printf("La requete n'est pas formaté correctement.\n");
							break;
						}
						free(tmp);
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

