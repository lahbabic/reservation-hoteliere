#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#endif

#include <errno.h>

#include "serveur.h"

#define TRUE 1
#define FALSE 0
#define LONGUEUR_TAMPON 4096


#ifdef WIN32
#define perror(x) printf("%s : code d'erreur : %d\n", (x), WSAGetLastError())
#define close closesocket
#define socklen_t int
#endif

/* Variables cachees */

/* le socket d'ecoute */
int socketEcoute;
/* longueur de l'adresse */
socklen_t longeurAdr;
/* le socket de service */
int socketService;
/* le tampon de reception */
char tamponClient[LONGUEUR_TAMPON];
int debutTampon;
int finTampon;
int finConnexion = 0;


/* Initialisation.
 * Creation du serveur.
 */
int Initialisation() {
	return InitialisationAvecService("12321");
}

/* Initialisation.
 * Creation du serveur en precisant le service ou numero de port.
 * renvoie 1 si ca c'est bien passe 0 sinon
 */
int InitialisationAvecService(char *service) {
	int n;
	const int on = 1;
	struct addrinfo	hints, *res, *ressave;

	#ifdef WIN32
	WSADATA	wsaData;
	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
	{
		printf("WSAStartup() n'a pas fonctionne, erreur : %d\n", WSAGetLastError()) ;
		WSACleanup();
		exit(1);
	}
	memset(&hints, 0, sizeof(struct addrinfo));
    #else
	bzero(&hints, sizeof(struct addrinfo));
	#endif

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(NULL, service, &hints, &res)) != 0)  {
     		printf("Initialisation, erreur de getaddrinfo : %s", gai_strerror(n));
     		return 0;
	}
	ressave = res;

	do {
		socketEcoute = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (socketEcoute < 0)
			continue;		/* error, try next one */

		setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
#ifdef BSD
		setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
#endif
		if (bind(socketEcoute, res->ai_addr, res->ai_addrlen) == 0)
			break;			/* success */

		close(socketEcoute);	/* bind error, close and try next one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL) {
     		perror("Initialisation, erreur de bind.");
     		return 0;
	}

	/* conserve la longueur de l'addresse */
	longeurAdr = res->ai_addrlen;

	freeaddrinfo(ressave);
	/* attends au max 4 clients */
	listen(socketEcoute, 4);
	printf("Creation du serveur reussie.\n");

	return 1;
}

/* Attends qu'un client se connecte.
 */
int AttenteClient() {
	struct sockaddr *clientAddr;
	char machine[NI_MAXHOST];

	clientAddr = (struct sockaddr*) malloc(longeurAdr);
	socketService = accept(socketEcoute, clientAddr, &longeurAdr);
	if (socketService == -1) {
		perror("AttenteClient, erreur de accept.");
		return 0;
	}
	if(getnameinfo(clientAddr, longeurAdr, machine, NI_MAXHOST, NULL, 0, 0) == 0) {
		printf("Client sur la machine d'adresse %s connecte.\n", machine);
	} else {
		printf("Client anonyme connecte.\n");
	}
	free(clientAddr);
	/*
	 * Reinit buffer
	 */
	debutTampon = 0;
	finTampon = 0;
	finConnexion = FALSE;

	return 1;
}

/* Recoit un message envoye par le serveur.
 */
char *Reception() {
	char message[LONGUEUR_TAMPON];
	int index = 0;
	int fini = FALSE;
	int retour = 0;
	int trouve = FALSE;

	if(finConnexion) {
		return NULL;
	}

	while(!fini) {
		/* on cherche dans le tampon courant */
		while((finTampon > debutTampon) && (!trouve)) {
			//fprintf(stderr, "Boucle recherche char : %c(%x), index %d debut tampon %d.\n",
			//		tamponClient[debutTampon], tamponClient[debutTampon], index, debutTampon);
			if (tamponClient[debutTampon]=='\n')
				trouve = TRUE;
			else
				message[index++] = tamponClient[debutTampon++];
		}
		/* on a trouve ? */
		if (trouve) {
			message[index++] = '\n';
			message[index] = '\0';
			debutTampon++;
			fini = TRUE;
			//fprintf(stderr, "trouve\n");
#ifdef WIN32
			return _strdup(message);
#else
			return strdup(message);
#endif
		} else {
			/* il faut en lire plus */
			debutTampon = 0;
			//fprintf(stderr, "recv\n");
			retour = recv(socketService, tamponClient, LONGUEUR_TAMPON, 0);
			//fprintf(stderr, "retour : %d\n", retour);
			if (retour < 0) {
				perror("Reception, erreur de recv.");
				return NULL;
			} else if(retour == 0) {
				fprintf(stderr, "Reception, le client a ferme la connexion.\n");
				finConnexion = TRUE;
				// on n'en recevra pas plus, on renvoie ce qu'on avait ou null sinon
				if(index > 0) {
					message[index++] = '\n';
					message[index] = '\0';
#ifdef WIN32
					return _strdup(message);
#else
					return strdup(message);
#endif
				} else {
					return NULL;
				}
			} else {
				/*
				 * on a recu "retour" octets
				 */
				finTampon = retour;
			}
		}
	}
	return NULL;
}

/* Envoie un message au client.
 * Attention, le message doit etre termine par \n
 */
int Emission(char *message) {
	int taille;
	if(strstr(message, "\n") == NULL) {
		fprintf(stderr, "Emission, Le message n'est pas termine par \\n.\n");
	}
	taille = strlen(message);
	if (send(socketService, message, taille,0) == -1) {
        perror("Emission, probleme lors du send.");
        return 0;
	}
	printf("Emission de %d caracteres.\n", taille+1);
	return 1;
}


/* Recoit des donnees envoyees par le client.
 */
int ReceptionBinaire(char *donnees, size_t tailleMax) {
	size_t dejaRecu = 0;
	int retour = 0;
	/* on commence par recopier tout ce qui reste dans le tampon
	 */
	while((finTampon > debutTampon) && (dejaRecu < tailleMax)) {
		donnees[dejaRecu] = tamponClient[debutTampon];
		dejaRecu++;
		debutTampon++;
	}
	/* si on n'est pas arrive au max
	 * on essaie de recevoir plus de donnees
	 */
	if(dejaRecu < tailleMax) {
		retour = recv(socketService, donnees + dejaRecu, tailleMax - dejaRecu, 0);
		if(retour < 0) {
			perror("ReceptionBinaire, erreur de recv.");
			return -1;
		} else if(retour == 0) {
			fprintf(stderr, "ReceptionBinaire, le client a ferme la connexion.\n");
			return 0;
		} else {
			/*
			 * on a recu "retour" octets en plus
			 */
			return dejaRecu + retour;
		}
	} else {
		return dejaRecu;
	}
}

/* Envoie des donnees au client en precisant leur taille.
 */
int EmissionBinaire(char *donnees, size_t taille) {
	int retour = 0;
	retour = send(socketService, donnees, taille, 0);
	if(retour == -1) {
		perror("Emission, probleme lors du send.");
		return -1;
	} else {
		return retour;
	}
}

/* determine la longueur d'un fichier: renvoie sa taille sinon -1 */
size_t longueur_fichier(char *nomFichier){
	FILE *fp;
	int long size;
	
	fp = fopen(nomFichier ,"r");
	if(fp == NULL)
		return -1;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);

	return size;	
}

/* Envoie le contenu d'un fichier , s'il y a une erreur d'ouverture du fichier 
	renvoie -1 */
int envoyerContenuFichierTexte(char *nomFichier){
		
	FILE *fp;
	char ligne[70];
	fp = fopen( nomFichier, "r");	
	if(fp == 0)
		return -1;
	while( !(feof(fp)) )	
	{
		if(fgets(ligne , 70, fp) != NULL)//récuperer une ligne du fichier
		{
			strcat(ligne,"\n");//mettre un retour à la ligne à la fin 
			if(Emission(ligne)!=1)//Emettre la ligne lu
					printf("Erreur d'emission\n");
		}
	}
	Emission("\n\n");
	fclose(fp);
	return 0;	
}
// formate une reponse 200 et l'envoie 
int envoyerReponse200HTML(char *nomFichier)
{
	char req[500];
	char len[2];  
	int lenght = 0;
	lenght = longueur_fichier(nomFichier);
	sprintf(len, "%d", lenght);
	strcpy(req,"\nHTTP/1.1 200 OK\nContent-type: text/html\nContent-length: ");
	strcat(req,len);
	strcat(req,"\n\n");
	if(Emission(req)!=1)
					printf("Erreur d'emission\n");
	return 0;
}

int envoyerReponse404(char *nomFichier)
{
	char req[500];
	char len[2];  
	int lenght = 0;
	FILE *fp;
	fp = fopen("page404.html", "w+");	
	if(fp == 0)
		return -1;
	fputs("<HTML>\n",fp);
	fputs("<HEAD><title>404 Error page</title></HEAD>\n",fp);
	fprintf(fp,"<BODY> <h1 >Error 404</h1> <p><br>Page %s Not Found</p>\n",nomFichier);

	fputs("</BODY></HTML>\n\n",fp);
	fclose(fp);
	lenght = longueur_fichier("page404.html");
	sprintf(len, "%d", lenght);
	strcpy(req,"\nHTTP/1.1 404 Not Found\nContent-type: text/html\nContent-length: ");
	strcat(req,len);
	strcat(req,"\n\n");
	if(Emission(req)!=1)
					printf("Erreur d'emission\n");
	envoyerContenuFichierTexte("page404.html");
	return 0;
}

int envoyerReponse500(char *message)
{
	char req[500];
	char len[2];  
	int lenght = 0;	
	FILE *fp;
	fp = fopen("page500.html", "w+");	
	if(fp == 0)
		return -1;
	fputs("<HTML>\n",fp);
	fputs("<HEAD><title>500 Error page</title></HEAD>\n",fp);
	fprintf(fp,"<BODY> <h1 >Oups erreur du serveur!</h1> <p><br> %s </p>\n",message);

	fputs("</BODY></HTML>\n\n",fp);
	fclose(fp);

	lenght = longueur_fichier("page500.html");
	sprintf(len, "%d", lenght);
	strcpy(req,"\nHTTP/1.1 500 Server Error\nContent-type: text/html\nContent-length: ");
	strcat(req,len);
	strcat(req,"\n\n");
	if(Emission(req)!=1)
					printf("Erreur d'emission\n");
	envoyerContenuFichierTexte("page500.html");
	return 0;
}

int extraitFichier(char  *requete,  char *tableauNomFichier, int tailleTableauNomFichier)
{
	int i;
	/* copier le nombre 'tailleTableauNomFichier' caractères après 'GET /'
		si dans la requete il y a plus de 5 éléments donc plus de 'GET /'*/
	if((strlen(requete) > 5)&&(strlen(requete)>tailleTableauNomFichier))
		memcpy(tableauNomFichier, &requete[5], tailleTableauNomFichier);
	else if((strlen(requete) > 5)&&(strlen(requete)<tailleTableauNomFichier))
		memcpy(tableauNomFichier, &requete[5], strlen(requete)-5);
	else
		return -2;
	/*Dans ces caractères copier précedement prendre le premier mot jusqu'à espace;
	cette boucle compte le nombre de caractère jusqu'au prochain espace */
	for (i = 0; tableauNomFichier[i] != ' ' ; ++i);
	/*remplacer l'espace trouvé dans la chaine par '\0' 
		ainsi dans le tableauNomFichier on retrouvera que le nom du fichier
		si le nom du fichier contient un espace, il y aura une erreur
	*/
	tableauNomFichier[i] = '\0';

	
	return 0;
}

int envoyerReponse200ICO(char*nomFichier){

	char req[500];
	char len[2];  
	int lenght = 0;
	lenght = longueur_fichier(nomFichier);
	sprintf(len, "%d", lenght);		// convertir la longueur en chaine de caractère
	//formater la reponse
	strcpy(req,"\nHTTP/1.1 200 OK\r\nContent-type :image/vnd.microsoft.icon\r\nContent-length: ");
	strcat(req,len);
	strcat(req,"\n\n");
	if(Emission(req)!=1){
		return -1;
	}
	return 0;

}
int envoyerReponse200JPG(char *nomFichier){
	
	char req[500];
	char len[2];  
	int lenght = 0;
	lenght = longueur_fichier(nomFichier);
	sprintf(len, "%d", lenght);
	strcpy(req,"\nHTTP/1.1 200 OK\r\nContent-type: image/jpeg\r\nContent-length: ");
	strcat(req,len);
	strcat(req,"\n\n");
	if(Emission(req)!=1){
		return -1;
	}
	return 0;
}

int envoyerContenuFichierBinaire(char *nomFichier)
{
	FILE *fp;
	fp = fopen(nomFichier, "rb");
	if(fp == NULL){
		fclose(fp);
		return -2;
	}
	int nb;
	char *donnees;		donnees = (char*)malloc(20000*sizeof(char));
	while((nb=fread(donnees, 1,20000,fp))>0){	//nb prend le nombre de caractère lu, ainsi Emission binaire aura la bonne taille de la donnee
		if(EmissionBinaire(donnees, nb )==-1)	//S'il y a une erreur d'emission on arrête
			return -1;
		Emission("\n\n");
	}

	free(donnees);
	fclose(fp);
	return 0;
}


/* Ferme la connexion avec le client.
 */
void TerminaisonClient() {
	close(socketService);
}

/* Arrete le serveur.
 */
void Terminaison() {
	close(socketEcoute);
}
