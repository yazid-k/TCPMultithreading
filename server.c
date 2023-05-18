#include "projet.h"
char				***mat;
pthread_mutex_t		mutex;
int					client_sock[1000] = {0};
pthread_t			threads[1000] = {0};
typedef struct s_struct
{
	int	id;
	int	socket;
} t_struct;

int	matrix_size(char ***mat) //Taille liste de listes de chaines de caractères
{
	int	i;

	i = -1;
	while (mat[++i]);
	return (i);
}

int	array_size(char **array) //Taille liste de chaines de caracteres
{
	int	i;

	i = -1;
	while (array[++i]);
	return (i);
}

void	free_arr(char **arr) //Fonction pour free une liste de chaines de caracteres
{
	int	i;

	i = -1;
	while (arr[++i])
		free(arr[i]);
	free(arr);
}

void	free_matrix(char ***mat) //Fonction pour free la matrice
{
	int	i;
	int	j;

	i = -1;
	while (mat[++i])
	{
		j = -1;
		while (mat[i][j++])
			free(mat[i][j]);
		free(mat[i]);
	}
	free(mat);
}

void	print_matrix(char ***mat) //Affichage de la matrice
{
	int	i;
	int	j;

	i = -1;
	while (mat[++i])
	{
		j = -1;
		printf("User %d :\n", i + 1);
		while (mat[i][++j])
			printf("\t\t%s\n", mat[i][j]);
		printf("\n");
	}
}
char	**copy_messages(char **to_copy)
{
	char	**ret;
	int		i;

	ret = malloc(sizeof(char *) * (array_size(to_copy) + 1));
	if (!ret)
		return (NULL);
	i = -1;
	while (to_copy[++i])
		ret[i] = strdup(to_copy[i]);
	ret[i] = 0;
	return (ret);
}

char	***init_mat() //Initialisation de la matrice
{
	char	***ret;

	ret = malloc(sizeof(char **) * 2);
	if (!ret)
		return (NULL);
	ret[0] = malloc(sizeof(char *) * 2);
	return (ret);
}

char	***add_user(char ***mat) //Fonction pour ajouter un utilisateur dans la matrice
{
	char	***ret;
	int		i;

	ret = malloc(sizeof(char ***) * (matrix_size(mat) + 2));
	if (!ret)
		return (NULL);
	i = -1;
	while(mat[++i])
		ret[i] = copy_messages(mat[i]);
	ret[i] = malloc(sizeof(char **) * 2);
	free_matrix(mat);
	return(ret);
}

void	add_message(char ***mat, char *msg, int user) //Ajout d'un message dans la matrice pour un utilisateur spécifique
{
	char	**arr;
	int		i;

	arr = malloc(sizeof(char *) * (array_size(mat[user - 1]) + 2 ));
	if (!arr)
		return ;
	i = -1;
	while (mat[user - 1][++i])
		arr[i] = strdup(mat[user - 1][i]);
	arr[i] = strdup(msg);
	free_arr(mat[user - 1]);
	mat[user - 1] = arr;
}

void	*client_handler(void *arg) //Fonction des threads pour envoyer un message recu par le serveur à tous les clients
{
	t_struct	*i;
	char		buffer[1024];
	int			r;

	i = (t_struct *)arg;
	pthread_mutex_lock(&mutex);
	mat = add_user(mat);
	pthread_mutex_unlock(&mutex);
	printf("Thread crée pour le client %d\n", i->id);
	while ((r = recv(i->socket , buffer , 1024 , 0)) > 0) //Réception du message
	{
		for(int j = 0; client_sock[j]; j++)//Boucle pour envoyer le message à tous les clients connus et ajout du message à la matrice
		{
			send(client_sock[j], buffer, strlen(buffer), MSG_NOSIGNAL);
			add_message(mat, buffer, i->id + 1);
		}
		bzero(buffer, 1024);
	}
	free(i);
	return (NULL);
}

int main()
{
	char				*ip = "127.0.0.1"; //IP localhost
	int					port = 5555;
	int					server_sock;
	struct sockaddr_in	server_addr, client_addr;
	socklen_t			addr_size;
	int					n;
	int					i;

	server_sock = socket(AF_INET, SOCK_STREAM, 0); //Création socket
	if (server_sock < 0)
		return (1);
	printf("Socket créee\n");
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)); //Bind du socket
	if (n < 0)
		return (1);
	printf("Socket liée\n");
	listen(server_sock, 5); //Ecoute du socket
	printf("Ecoute\n");
	mat = init_mat();
	pthread_mutex_init(&mutex, NULL);
	i = 0;
	while(1)
	{
		addr_size = sizeof(client_addr);
		client_sock[i] = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size); //Accepter la connection du client au serveur
		printf("Connecté au client %d\n", i);
		t_struct	*arg;
		arg = malloc(sizeof(t_struct));
		if (!arg)
			return (1);
		arg->id = i;
		arg->socket = client_sock[i];
		pthread_create(&threads[i], NULL, client_handler, (void *)arg); //Création du thread
		i++;
	}
	for (int j = 0; threads[j]; j++)
		pthread_join(threads[j], NULL);
	for (int k = 0; client_sock[k]; k++)
		close(client_sock[k]);
	free_matrix(mat);
	return 0;
}

