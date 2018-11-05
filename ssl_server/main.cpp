#include <winsock2.h>					//socket
#include <Ws2tcpip.h>					//inet_pton
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <process.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>

using namespace std;

unsigned WINAPI client_chat(void* a) {
	SOCKET c = *(SOCKET*)a;

	static char c_message[1460] = { "\0", };
	static char *server_hello = "HTTP/1.1 200 OK\nContent-Length: 5\n\nHello\n\0";

	while (recv(c, c_message, sizeof(c_message), 0) > 0) {
		cout << c_message << endl;
		memset(c_message, '\0', sizeof(c_message));
		send(c, server_hello, strlen(server_hello), 0);
	}

	closesocket(c);
	return 0;
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		cout << "ex) " << argv[0] << "<Port>" << endl;
		return -1;
	}

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {										
		cout << "Socket reset error" << endl;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);									
	if (s == SOCKET_ERROR) {
		cout << "Socket create error" << endl;
		WSACleanup();
		return -1;
	}

	sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {	// bind
		cout << "Bind error" << endl;
	}

	if (listen(s, 5) == SOCKET_ERROR) {														// listen
		cout << "Listen error" << endl;
	}

	while (1) {
		int c_addr_size = sizeof(client_addr);
		SOCKET c = accept(s, (struct sockaddr *)&client_addr, &c_addr_size);
		if (c == SOCKET_ERROR) {																// accept
			cout << "Accept error" << endl;
		}

		char ip[16];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
		cout << ip << " connect!" << endl;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, client_chat, (void*)&c, 0, NULL);
	}

	closesocket(s);
	WSACleanup();
	return 0;
}