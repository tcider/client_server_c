#ifndef HEADER_H
# define HEADER_H

# include <sys/types.h> //  Константы AF_INET и тп
# include <sys/socket.h> // Здесь функции socket(), listen(), bind() ...
# include <netinet/in.h> // Структуры struct sockaddr  и sockaddr_in
# include <stdio.h> // printf()
# include <unistd.h> // read() write() getopt()
# include <stdlib.h> // atoi()
# include <arpa/inet.h> // inet_pton()
# include <string.h> // strlen()

# define SERVER_PORT 10000 // Константа для номера порта
# define BUF_SIZE 1000 // Константа для размера буфера

#endif
