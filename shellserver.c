#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>

#define PORT "5000"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {

	WSADATA WsaData;

	WSAStartup(MAKEWORD(2, 2), &WsaData);

	struct addrinfo hints, * p, * servinfo;

	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PORT, &hints, &servinfo);

	int sockfd;

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			printf("Trying socket again");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			closesocket((SOCKET)sockfd);
			printf("Trying bind again.");
			continue;
		}

		break;
	}

	DWORD timeout = 1000;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);

	freeaddrinfo(servinfo);

	if (listen(sockfd, 10) == -1) {
		printf("Failed to listen.");
		return 1;
	}

	int remoteDesk;

	struct sockaddr_storage remoteaddr;

	int sin_size = sizeof remoteaddr;


	remoteDesk = accept(sockfd, (struct sockaddr*)&remoteaddr, &sin_size);


	char buf[10000];
	char message[4096];

	int bytesRecv = -1;

	memset(buf, 0, sizeof buf);
	memset(message, 0, sizeof message);

	do {
		memset(buf, 0, sizeof buf);
		memset(message, 0, sizeof message);
		bytesRecv = recv(remoteDesk, buf, sizeof buf, 0);
		printf("%s", buf);
	} while (bytesRecv >= 0);


	while (1) {
		memset(buf, 0, sizeof buf);
		memset(message, 0, sizeof message);

		gets(message);
		strcat(message, "\r\n");

		send(remoteDesk, message, strlen(message), 0);
		
		do {
			memset(buf, 0, sizeof buf);
			memset(message, 0, sizeof message);
			bytesRecv = recv(remoteDesk, buf, sizeof buf, 0);
			printf("%s", buf);
		
		} while (bytesRecv >= 0);

	}

	WSACleanup();
	return 0;
}
