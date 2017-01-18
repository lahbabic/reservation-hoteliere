#ifndef __SERVEUR_H__
#define __SERVEUR_H__
#include <stddef.h>
/* Initialisation.
 * Creation du serveur.
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int Initialisation();

/* Initialisation.
 * Creation du serveur en pr�cisant le service ou num�ro de port.
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int InitialisationAvecService(char *service);


/* Attends qu'un client se connecte.
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int AttenteClient();

/* Recoit un message envoye par le client.
 * retourne le message ou NULL en cas d'erreur.
 * Note : il faut liberer la memoire apres traitement.
 */
char *Reception();

/* Envoie un message au client.
 * Attention, le message doit etre termine par \n
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int Emission(char *message);

/* Recoit des donnees envoyees par le client.
 * renvoie le nombre d'octets re�us, 0 si la connexion est ferm�e,
 * un nombre n�gatif en cas d'erreur
 */
int ReceptionBinaire(char *donnees, size_t tailleMax);

/* Envoie des donn�es au client en pr�cisant leur taille.
 * renvoie le nombre d'octets envoy�s, 0 si la connexion est ferm�e,
 * un nombre n�gatif en cas d'erreur
 */
int EmissionBinaire(char *donnees, size_t taille);


/* Ferme la connexion avec le client.
 */
void TerminaisonClient();

/* Arrete le serveur.
 */
void Terminaison();

/* Etrait le nom d'un fichier d'une requete */
int extraitFichier(char  *requete,  char *tableauNomFichier, int tailleTableauNomFichier);

/*determine la taille d'un fichier renvoie sa taille , sinon elle renvoie -1 */
size_t longueur_fichier(char *nomFichier) ;

/*Envoie le contenu d'un fichier , -1 s'il y a erreur d'ouverture du fichier*/
int envoyerContenuFichierTexte(char *nomFichier);

int envoyerContenuFichierBinaire(char *nomFichier);

int envoyerReponse200JPG(char *nomFichier);

int envoyerReponse200ICO(char*nomFichier);

int envoyerReponse200HTML(char *nomFichier);

int envoyerReponse404(char *nomFichier);

int envoyerReponse500(char *message);

/*struct hotel{
	char *nom;
	char *ville;
	int cat;
	int prix_ch;
	int nb_ch;
	//int tab[2][nb_ch]
};*/
//typedef struct hotel *hotel_s;
#endif
