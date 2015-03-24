#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "socket.h"


/*Gère la connexion au serveur*/
int connexion(int socket_serveur) {

	int socket_client;
	int pid;
	FILE *file;
	char *message_recu = malloc(150);
	
	const char *httpOk = "HTTP/1.1 200 Ok\n";
	const char *httpKo = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\n400 Bad request\r\n";
	
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
		int con  = 0;
		
		while(1){
		
			tmp = fgets(message_recu, 150, file);
		
			if(con == 0){
				char new[4];		//Tableau contenant la requete (GET)
				char new2[9];		//Tableau contenant le protocole
				short i = 0;		//Indice
				short mot = 1;		//Compte le nombre de mot de la requete de connection
				
		
				while(mot < 5 && i < (signed) strlen(message_recu)){	//Verifie que le nombre de mot est inférieur à 5
					if(i < 3)											//Recupère le premier mot (Normalement "GET")
						new[i] = message_recu[i];
					else if(i > 5 && i < 15)							//Recupère le troisème mot (Normalement "HTTP/1.1" ou "HTTP1.0")
						new2[i-6] = message_recu[i];
					if(message_recu[i] == ' '){							//Vérifie si il y a un nouveau mot
						mot++;
					}
					i++;
				}
				if(mot == 3){								//Verifie que le nombre de mot est egale à 3
					new[3]='\0';
					new2[8]='\0';
					if(strcmp(new, "GET") != 0){			//Verifie que la requete est bien GET
						if(write(socket_client, httpKo, strlen(httpKo)) == -1){
							perror("write message");
							exit(1);
						}
						exit(1);
					} else if(strcmp(new2, "HTTP/1.1") == 0 || strcmp(new2, "HTTP/1.0") == 0){	//Verifie que le protocol est bien HTTP/1.1 ou HTTP/1.0
						if(write(socket_client, httpOk, strlen(httpOk)) == -1){					//Indique que la requete est bonne
							perror("write message");
							exit(1);
						}
						con++;
					} else {
						if(write(socket_client, httpKo, strlen(httpKo)) == -1){					//Indique que la requete est mauvaise
							perror("write message");
							exit(1);
						}
						exit(1);
					}
					while(strcmp(message_recu, "\n") != 0 && strcmp(message_recu, "\r\n") != 0){
						tmp = fgets(message_recu, 150, file);
					}
					if(write(socket_client, message_bienvenue, strlen(message_bienvenue)) == -1){
						perror("write message");
						exit(1);
					}
				} else {
					if(write(socket_client, httpKo, strlen(httpKo)) == -1){
						perror("write message");
						exit(1);
					}
					exit(1);
				}
				printf("fini\n");
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
