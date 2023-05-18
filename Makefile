SRCSSERVER=server.c
SRCSCLIENT=client.c
SERVER=server
CLIENT=client
CC=cc
CFLAGS=-Wall -Wextra -Werror
all : $(SERVER) $(CLIENT)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

${SERVER} : ${SRCSSERVER:.c=.o}
	${CC} ${CFLAGS} ${SRCSSERVER:.c=.o} -o ${SERVER}

${CLIENT} : ${SRCSCLIENT:.c=.o}
	${CC} ${CFLAGS} ${SRCSCLIENT:.c=.o} -o ${CLIENT}

clean :
	rm -f ${SRCSSERVER:.c=.o} ${SRCSCLIENT:.c=.o}

fclean : clean
	rm -f ${SERVER} ${CLIENT}

re : fclean all

.PHONY : all clean fclean re
