#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int NUM_CLIENTS = 10;
const int PORT = 8101;

void* clientThreadFunction(void* arg)
{
	(void) arg;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char hello[] = "Hello from client";
	char buffer[1024] = {0};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "\n Socket creation error \n";
		return NULL;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		std::cerr << "\nInvalid address/ Address not supported \n";
		return NULL;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "\nConnection Failed \n";
		return NULL;
	}

	send(sock , hello , strlen(hello) , 0 );
	std::cout << "Hello message sent\n";
	valread = read(sock , buffer, sizeof(buffer)-1);
	buffer[valread] = '\0';
	std::cout << buffer << std::endl;

	close(sock);
	return NULL;
}

int main() {
	std::vector<pthread_t> clientThreads;

	for (int i = 0; i < NUM_CLIENTS; ++i)
	{
		pthread_t tid;
		pthread_create(&tid, NULL, clientThreadFunction, NULL);
		clientThreads.push_back(tid);
	}
	for (int i = 0; i < NUM_CLIENTS; ++i)
	{
		pthread_join(clientThreads[i], NULL);
	}
	return 0;
}
