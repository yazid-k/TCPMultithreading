#include "projet.h"

void	*reception(void *arg) //Thread pour la réception des messages du serveur
{
	char	reponse[1024] = {0};
	int		*sock;

	sock = (int *)arg;
	bzero(reponse, 1024);
	while (1)
	{
		bzero(reponse, 1024);
		recv(*sock, reponse, 1024, MSG_NOSIGNAL); //Réception du message
		if (reponse[0])
			printf("Message recu : %s\n", reponse); //Affichage
	}
	return (NULL);
}

int main()
{

	char				*ip = "127.0.0.1"; //IP localhost
	int					port = 5555;
	int					sock;
	struct sockaddr_in	addr;
	char				message[1024] = {0};
	pthread_t			thread;

	sock = socket(AF_INET, SOCK_STREAM, 0); //Création socket
	if (sock < 0)
		return (1);
	printf("Socket créee\n");
	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = inet_addr(ip);
	while(connect(sock, (struct sockaddr*)&addr, sizeof(addr))); //Connection au serveur
	printf("Connection au serveur réussie\n");
	pthread_create(&thread, NULL, reception, &sock);
	while(message[0] != 'f') //Boucle pour le prompt
	{
		printf("Entrez un message :\n");
		bzero(message, 1024);
		scanf("%s", message);
		send(sock, message, strlen(message), MSG_NOSIGNAL); //Envoi du message au serveur
		sleep(1);
	}
	pthread_join(thread, NULL);
	close(sock);
}
