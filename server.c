#include "header.h"

// Функция получающая тип команды из контейнера
static int	get_command_type(char *conteiner)
{
	if (!strncmp(conteiner, "execute|", 8)) // strncmp сравнивает только первые n символов
		return(1);
	else if (!strncmp(conteiner, "exit|", 5))
		return(0);
	return(-1);
}

// Функция получает указатель на строку команды из контейнера
static char	*get_command(char *conteiner)
{
	char	*res;

	res = strchr(conteiner, '|');
	if (res)
		res++;
	return(res);
}

// Функция пишущая ответ в открытый дескриптор клиента
static void	response(int client_fd, char *response)
{
	int	len;

	len = strlen(response);
	write(client_fd, response, len);
	sleep(1);
    close(client_fd);
}

int main()
{
	int	socket_fd; // Дескриптор открытия сокета
	int	bind_fd; // Дескриптор привязки сокета
	int	client_fd; // Дескриптор для accept()
	int	res; // Переменныя для общего получения результатов функций
	struct	sockaddr_in server_addr = {0}; // Струркутра для хранения данных сокета сервера
	struct 	sockaddr_in client_addr = {0}; // .. клиента
	socklen_t	client_addr_len; // Переменная на сткеке для хранения размера структуры адреса клиента
	char	buf[BUF_SIZE]; // Буфер на стеке для чтения сообщения клиента
	FILE *pipe; // Структура для запуска команды в bash
	

	// Создаем сокет, используем IPv4 поэтому AF_INT
	// протокол TCP поэтму SOCK_STREAM, 0 - это прикладной протокол по умолчанию
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
        printf("Server: Ошибка открытия сокета\n");
        return(1);
    }

	// Инициализируем структуру для сокета, задаем порт опредделенный в header.h
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

	// Привязка сокета, тк bind() принимает структуру struct sockaddr мы приводим к этому типу
	bind_fd = bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_fd == -1)
	{
        printf("Server: Ошибка привязки\n");
        return(1);
    }
	
	// Слушаем сокет для одного клиента
	res = listen(socket_fd, 1);
    if (res == -1)
	{
        printf("Server: Ошибка listen()\n");
        return(1);
    }
    
	// Запускаем бесконечный цикл для непрерывной работы сервера
	while (1)
	{
		// Принимаем запрос клиента
		client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (client_fd == -1)
		{
  	    	printf("Server: Ошибка приема\n");
  	    	return(1);
		}

    	// Читаем сообщение клиента
		res = read(client_fd, buf, BUF_SIZE);
    	if (res == -1)
		{
        	printf("Server: Ошибка чтения\n");
        	continue;
    	}
    	else if (!res)
        	printf("Server: Пустая команда\n");
	
		// Нультерминируем приянтую от клиента строку и выводим принятую команду
		buf[res] = '\0';
		res = get_command_type(buf);
		if (res == -1)
		{
			response(client_fd, "Неверный формат контейнера");
			continue;
		}
		else if (!res)
		{
			response(client_fd, "OK\n");
			break;
		}
		pipe = popen(get_command(buf), "r");
		res = fread(buf, 1, BUF_SIZE, pipe);
		if (res == -1)
		{
			response(client_fd, "Ошибка pipe на сервере");
			continue;
		}
      	buf[res] = '\0';
		pclose(pipe);
		response(client_fd, buf);
	}
    close(socket_fd);
    return 0;
}
