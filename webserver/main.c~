#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "socket.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


int socket_client;
FILE *file;
char *message_recu;


enum http_method{
	HTTP_GET,
	HTTP_UNSUPPORTED,
};


typedef struct{
	enum http_method method;
	int major_version;
	int minor_version;
	char *url;
} http_request;


/*Permet de récuperer un mot dans une phrase*/
char *cut(char *texte, short mot){

	short idx = 0;
	short i = 1;
	char *result = malloc(50);
	while(idx < (signed) strlen(texte) && i != mot){
		if(texte[idx] == ' ')
			i++;
		idx++;
	}
	i = 0;
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


/*Lecture message recut et gestion des erreur liés*/
void fgets_or_exit(char *buffer, int size, FILE *stream){

	if(fgets(buffer, size, stream) == NULL)	//Recupère le message envoyer par le client 
		exit(1);
		
}


/*Ignore les lignes d'entêtes*/
void skip_headers(FILE *client){

	while(strcmp(message_recu, "\n") != 0 && strcmp(message_recu, "\r\n") != 0)	//Tant que ce n'est pas une ligne vide on ignore
		fgets_or_exit(message_recu, 150, client);
		
}


/*Analyse la première ligne envoyer par le client et stocke les infos dans une structure*/
int parse_http_request(char *request_line, http_request *request){

	char *new;			//Chaine contenant le protocole
	short i = 0;		//Indice
	short mot = 1;		//Compte le nombre de mot de la requete de connection
	enum http_method method;
	
	request->major_version = 1;
	request->minor_version = 0;
	
	while(mot < 5 && i < (signed) strlen(request_line)){	//Verifie que le nombre de mot est inférieur à 5
		if(request_line[i] == ' '){							//Vérifie si il y a un nouveau mot
			mot++;
		}
		i++;
	}
	new = cut(request_line, 3);								//Recupère le troisème mot (Normalement "HTTP/1.1" ou "HTTP1.0")
	if(mot == 3){											//Verifie que le nombre de mot est egale à 3
		if(strcmp(cut(request_line, 1), "GET") == 0){		//Verifie que la requete est bien GET
			method = HTTP_GET;
			request->method = method;
			if(strcmp(new, "HTTP/1.1") == 0 || strcmp(new, "HTTP/1.0") == 0){	//Verifie que le protocol est bien HTTP/1.1 ou HTTP/1.0
				request->url = cut(request_line, 2);
				return 1;
			}
		} else {
			method = HTTP_UNSUPPORTED;
			request->method = method;
		}
	}
	return -1;
	
}


/*Envoie la ligne de statu au client*/
void send_statu(FILE *client, int code, const char *reason_phrase){

	if(fprintf(client, "HTTP/1.1 %d %s\n", code, reason_phrase) < 0){
		perror("write message");
		exit(1);
	}
	
}


/*Envoie une réponse au client en fonction de sa requete*/
void send_response(FILE *client, int code, const char *reason_phrase, const char *message_body){

	send_statu(client, code, reason_phrase);
	if(fprintf(client, "Content-length: %d\nmessage-body: %s", (int) strlen(message_body), message_body) < 0){
		perror("write message");
		exit(1);
	}
	
}


/*On recupère l'url sans la partie query*/
char *rewrite_url(char *url){
	char *newURL = malloc(50);
	short i = 0;
	while(i < (signed) strlen(url) && url[i] != '?')
		newURL[i] = url[i];
	newURL = realloc(newURL, i);
	return newURL;
}


/*Tente d'acceder au répertoire demandé dans l'URL*/
int check_and_open(const char *url, const char *document_root){
	int o = -1;
	char filePath[50];
	char docRoot[100];
	
	strcpy(filePath, url);
	strcpy(docRoot, document_root);
	strcat(docRoot, filePath);
	
	struct stat statStruct;
	if(stat(docRoot, &statStruct) == -1)
		return -1;
	if((statStruct.st_mode & S_IFMT) == S_IFREG){
		o = open(docRoot, O_RDONLY);
		if(o == -1)
			return -1;
	}
	return o;
}


/*Gère la connexion au serveur*/
int connexion(int socket_serveur, char *documentRoot) {

	int pid;
	message_recu = malloc(150);
	http_request request;
	
	//const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur, faite vous plaisir :)\n";
	
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

		short con = 0;
		
		while(1){
		
			fgets_or_exit(message_recu, 150, file);
		
			if(con == 0){
				int fd;
				short a = parse_http_request(message_recu, &request);
				skip_headers(file);
				
				/*Envois un message au client suivant sa requète*/
				if(request.method == HTTP_UNSUPPORTED){
					send_response(file, 405, "Method Not Allowed", "Method Not Allowed\r\n");
					exit(1);
				} else if(a == -1){
					send_response(file, 400, "Bad Request", "Bad request\r\n");
					exit(1);
				}
				

				fd = check_and_open(request.url, documentRoot);
				if(fd == -1){
					send_response(file, 404, "Not Found", "Not Found\r\n");
					exit(1);
				}
				
				read(fd, message_recu, 150);
				send_response(file, 200, "OK", message_recu);
				
				con ++;
			} else {
				printf("%s", message_recu);
			}
		}	
	} else
		close(socket_client);

	return socket_client;
	
}


int main(int argc, char *argv[]){

	int socket_serveur;
	
	if(argc != 2){
		printf("Erreur : paramètre(s) incorect\n");
		return -1;
	}
	
	if(opendir(argv[1]) == NULL){
		printf("Erreur : Répértoire invalide\n");
		return -1;
	}
	
	socket_serveur = creer_serveur(8080);
	if(socket_serveur == -1){
		perror("probleme creation serveur\n");
		return -1;
	}
	
	while(1)
		connexion(socket_serveur, argv[1]);
	
	return 0;

}
