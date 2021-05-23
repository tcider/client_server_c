#include "header.h"

int main(int argc, char** argv)
{
	int	socket_fd; // Дескриптор открытия сокета
	int	res; // Общая переменная для получения результатов функций
	struct 	sockaddr_in client_addr = {0}; //Структура данных сокета клиента
	char	buf[BUF_SIZE]; // Буфер на стеке для текстовых строк
	char	*server_ip = NULL; // Адрес сервера
	int		server_port = 0; // Порт сервера
	char 	*command = NULL; // Команда серверу

	//  Проверяем сколько аргументов передано при запуске программы
	if (argc == 1)
	{
		printf("Client: Нет аргументов (client -i 127.0.0.1 -p 10000 -c pwd)\n");
		return(1);
	}

	// Считываем аргументы запуска программы
	while ((res = getopt(argc, argv, "i:p:c:")) != -1)
	{
		if (res == 'i')
			server_ip = optarg;
		else if (res == 'p')
			server_port = atoi(optarg);
		else if (res == 'c')
			command = optarg;
		else if (res == ':')
		{
			printf("Client: Пустой аргумент\n");
			return(1);
		}
		else if (res == '?')
		{
			printf("Client: Неизвестный аргумент (client -i 127.0.0.1 -p 10000 -c pwd)\n");
			return(1);
		}
	}
	if (!server_ip || !server_port || !command)
	{
		printf("Client: Отсутствует аргумент (client -i 127.0.0.1 -p 10000 -c pwd)\n");
		return(1);
	}

	// Создаем сокет, используем IPv4 поэтому AF_INT
	// протокол TCP поэтму SOCK_STREAM, 0 - это прикладной протокол по умолчанию
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
        printf("Client: Ошибка открытия сокета\n");
        return(1);
    }

	//  Инициализируем структуру сокета
	client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(server_port);

	// Преобразуем адрес из строки в .sin_addr
    res = inet_pton(AF_INET, server_ip, &client_addr.sin_addr);
    if (!res)
	{
        printf("Client: IP адрес указан неверно\n");
        return(1);
    }
    if (res == -1)
	{
		printf("Client: Ошибка inet_pton\n");
        return(1);
    }

	// Подключаемся к серверу
	res = connect(socket_fd, (struct sockaddr *) &client_addr, sizeof(client_addr));
    if (res == -1)
	{
		printf("Client: Ошибка соединения - сервер не найден\n");
		return(1);
    }

	// Готовим команду серверу (контеинизируем в формат execute|команда)
	if (strcmp(command, "exit"))
		strcpy(buf, "execute|");
	else
		strcpy(buf, "exit|");
	strcat(buf, command);

	// Отправляем серверу команду
    write(socket_fd, buf, strlen(buf));
    res = read(socket_fd, buf, BUF_SIZE);
    if (res == -1)
	{
		printf("Client: Ошибка чтения ответа сервера\n");
		return(1);
    }
    else if (!res)
        printf("Client: Пустой ответ\n");
	else
	{
		// Выводим ответ сервера
		buf[res] = '\0';
		printf("Client: Ответ сервера -\n%s", buf);
    }
    close(socket_fd);
    return 0;
}

