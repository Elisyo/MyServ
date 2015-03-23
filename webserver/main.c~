#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "socket.h"
#include <unistd.h>
#include <stdlib.h>


/*Gère la connexion au serveur*/
int connexion(int socket_serveur) {

	int socket_client;
	int pid;
	char *message_recu = malloc(150);
	int nbCaratere;
	const char *message_bienvenue = "Bonjour , bienvenue sur mon serveur, je  			vais vous chanter une petite chanson : coucou c'est moi moumou la 			reine des moueeeeeeeeettes qui s'en va tout droit vers euralille, 			fait attention, fait attention, la chanson recommence... .\nAh 			tchou tchou pouet pouet la voila, la totomobile, ah tchou thou 			pouet pouet la voila, que fait-elle donc la ! Jour mémorable de sa 			première sortie, touloute touloute, lorsqu'elle entra dans une 			confiserie, dans une confiserie ! hummmmmmmmmmm !!!!!!\nQue 			viens-tu faire ici?\nTu veux de l'argent ? bah t'en aura pas !\nTu 			veux une bonne note ? Fini ce TP !!!\nlolilol!\n#hashtag\nUn mur en 			brique tombe et casse ca donne quoi ? un mur cassé;\nVoilà la 			fin de mon long et très long message pour juste vous dire 			bienvenue. La main est à vous.\n";

	/*Tentative de connexion au client*/
	socket_client = accept(socket_serveur, NULL, NULL);
	if(socket_client == -1){
		perror("accept");
		return -1;
	}

	/*Crée un nouveau processus pour gérer le multi utilisateur*/
	pid = fork();
	if(pid == -1)
		perror("pid");
	else if(pid == 0){
	
		/*Affiche le message de bienvenue au client*/
		write(socket_client, message_bienvenue, strlen(message_bienvenue));

		/*Renvoie tout les messages recut du client au client*/
		while(1){
			nbCaratere = read(socket_client, message_recu, 150);
			write(socket_client, message_recu, nbCaratere);
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
