#include <winsock2.h>					//socket
#include <Ws2tcpip.h>					//inet_pton
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <process.h>
#include <openssl/ssl.h>

using namespace std;

int ssl_client_chat(SSL *ssl){
	static char c_message[1460] = { "\0", };
	static char *server_hello = "HTTP/1.1 200 OK\nContent-Length: 5\n\nHello\n\0";

	while (SSL_read(ssl, c_message, sizeof(c_message)) > 0) {
		cout << c_message << endl;
		memset(c_message, '\0', sizeof(c_message));
		SSL_write(ssl, server_hello, strlen(server_hello));
	}
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
	
	SSL_library_init();				//OpenSSL init
	SSL_CTX* ctx = SSL_CTX_new(TLSv1_2_server_method());				//TLSv1.2 context create

	SSL_CTX_use_certificate_file(ctx, "C:\\Program Files\\SnoopSpy\\certificate\\test.com.crt", SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(ctx, "C:\\Program Files\\SnoopSpy\\certificate\\test.com.key", SSL_FILETYPE_PEM);

	if (bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {	// bind
		cout << "Bind error" << endl;
	}

	if (listen(s, 5) == SOCKET_ERROR) {														// listen
		cout << "Listen error" << endl;
	}

	while (1) {
		int c_addr_size = sizeof(client_addr);
		SOCKET c = accept(s, (struct sockaddr *)&client_addr, &c_addr_size);
		if (c == SOCKET_ERROR) {															// accept
			cout << "Accept error" << endl;
		}

		SSL *ssl = SSL_new(ctx);
		SSL_set_fd(ssl, c);
		SSL_accept(ssl);

		ssl_client_chat(ssl);
		SSL_free(ssl);
	}

	SSL_CTX_free(ctx);
	closesocket(s);
	WSACleanup();
	return 0;
}