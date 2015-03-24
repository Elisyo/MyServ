#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "socket.h"


int socket_client;


char *cut(char *texte, int mot){
	short idx = 0;
	short m = 1;
	short i = 0;
	char *result = malloc(50);
	while(idx < (signed) strlen(texte) && m != mot){
		if(texte[idx] == ' ')
			m++;
		idx++;
	}
	if(idx != (signed) strlen(texte)){
		while(texte[idx] != '\n' && idx < (signed) strlen(texte) && texte[idx] != ' '){
			result[i] = texte[idx];
			i++;
			idx++;
		}
		result = realloc(result, i);
		return result;
	}
	return NULL;
}


void erreur(short nb){
	const char *httpKo = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\n400 Bad request\r\n";
	if(nb == 400){
		if(write(socket_client, httpKo, strlen(httpKo)) == -1){
			perror("write message");
			exit(1);
		}
	} else {
		if(write(socket_client, "Erreur 404 : Page Not Found\n", 28) == -1){
			perror("write message");
			exit(1);
		}	
	}
	exit(1);
}


/*Gère la connexion au serveur*/
int connexion(int socket_serveur) {

	int pid;
	FILE *file;
	char *message_recu = malloc(150);
	
	const char *httpOk = "HTTP/1.1 200 Ok\n";
	
	const char *message_bienvenue = "Bonjour ,bienvenue sur mon serveur, je vais vous chanter une petite chanson : coucou c'est moi moumou la reine des moueeeeeeeeettes 				qui s'en va tout droit vers euralille, fait attention, fait attention, la chanson recommence... .\nAh tchou tchou pouet pouet la voila, la totomobile, ah tchou 			thou pouet pouet la voila, que fait-elle donc la ! Jour mémorable de sa première sortie, touloute touloute, lorsqu'elle entra dans une confiserie, dans une 			confiserie ! hummmmmmmmmmm !!!!!!\nQue viens-tu faire ici?\nTu veux de l'argent ? bah t'en aura pas !\nTu veux une bonne note ? Fini ce TP !!!\nlolilol!	 				\n#hashtag\nUn mur en brique tombe et casse ca donne quoi ? un mur cassé;\n";
 
	/*Tentative de connexion au client*/
	socket_client = accept(socket_serveur, NULL, NULL);
	if(socket_client == -1){
		perror("accept\n");
		return -1;
	}
	
	/*Obtenir pointeur vers structure file*/
	file = fdopen(socket_client, "w+");
	
	/*Crée un nouveau processus pour gérer le multi utilisateur*/
	pid = fork();
	if(pid == -1)
		perror("pid");
	else if(pid == 0){

		char *tmp;
		int con = 0;
		
		while(1){
		
			tmp = fgets(message_recu, 150, file);	//Recupère le message envoyer par le client
		
			if(con == 0){
				char *new;			//Chaine contenant le protocole
				short i = 0;		//Indice
				short mot = 1;		//Compte le nombre de mot de la requete de connection
				
				while(mot < 5 && i < (signed) strlen(message_recu)){	//Verifie que le nombre de mot est inférieur à 5
					if(message_recu[i] == ' '){							//Vérifie si il y a un nouveau mot
						mot++;
					}
					i++;
				}
				new = cut(message_recu, 3);								//Recupère le troisème mot (Normalement "HTTP/1.1" ou "HTTP1.0")
				if(mot == 3){											//Verifie que le nombre de mot est egale à 3
					if(strcmp(cut(message_recu, 1), "GET") != 0)		//Verifie que la requete est bien GET
						erreur(400);
					else if(strcmp(new, "HTTP/1.1") == 0 || strcmp(new, "HTTP/1.0") == 0){	//Verifie que le protocol est bien HTTP/1.1 ou HTTP/1.0
						if(write(socket_client, httpOk, strlen(httpOk)) == -1){				//Indique que la requete est bonne
							perror("write message");
							exit(1);
						}
						if(strcmp(cut(message_recu, 2), "/") != 0){							//Verifie que l'url est bien "/"
							erreur(404);
						}
						con++;
					} else
						erreur(400);
					while(strcmp(message_recu, "\n") != 0 && strcmp(message_recu, "\r\n") != 0){
						tmp = fgets(message_recu, 150, file);
					}
					if(write(socket_client, message_bienvenue, strlen(message_bienvenue)) == -1){
						perror("write message");
						exit(1);
					}
				} else {
					erreur(400);
				}
				con ++;
			} else {
				if(tmp != NULL) 
					printf("%s", message_recu);
			}
		}	
	} else
		close(socket_client);

	return socket_client;
	
}


int main(){

	int socket_serveur;
	socket_serveur = creer_serveur(8080);
	if(socket_serveur == -1){
		perror("probleme creation serveur\n");
		return -1;
	}
	while(1)
		connexion(socket_serveur);
		
	return 0;

}
