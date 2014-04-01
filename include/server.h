#define _WIN32_WINNT 0x0501
#define DEFAULT_PORT "8080"
#define DEFAULT_BUFLEN 512
//encryption
#include <cryptopp/base64.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
//for dynamic lists
#include <vector>
//for null
#include <stddef.h>
//for sockets
#include <winsock2.h>
#include <ws2tcpip.h>
//standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

string encode(string);
string decode(string);
char min(char, char);

class client {
private:
	bool closed;
	string headers;
	bool handshaken;
	void handshake();
	SOCKET socket;
public:
	void * associated = NULL;		
	string getHeader(string);
	client(SOCKET);
	void sendMessage(string);
	bool receiveMessage(string *);	 
	bool isClosed();
	void close();
};

class server {
private:
	bool closed;
	vector<client> clients;
	void (* f_message)(client, string);
	void (* f_connect)(client);
	void (* f_close)(client);
	void (* f_error)(client, std::exception);
    WSADATA wsaData;
    SOCKET ListenSocket;
    struct addrinfo * result;
    struct addrinfo hints;
public:
	void start();
	void close();
	bool connect();	
	void onMessage(void (*)(client, string));
	void onConnect(void (*)(client));
	void onClose(void (*)(client));
	void onError(void (*)(client, std::exception));
};

void client::close() {
	closed = 1;
}

client::client(SOCKET s) {
	socket = s;
	closed = handshaken = false;
}

void client::sendMessage(string message) {
	message = encode(message);	
	int result = send(socket, message.c_str(), message.length(), 0);	
}

bool client::isClosed() {
	return closed;
}

bool client::receiveMessage(string * message) {
	char buffer[DEFAULT_BUFLEN];
	int length = recv(socket, buffer, DEFAULT_BUFLEN, 0);
	if(length > 0) {			
		if(handshaken) {	
			string s = decode(string(buffer, length));
			*message = s;
			return true;	
		} else {			
			headers = string(buffer);
			handshake();
		}
	} else if(length == 0) {
		closed = 1;
	}
	return false;
}

string client::getHeader(string name) {		
	int start = headers.find(name) + name.length() + 2;
	int end = headers.find("\r\n", start);
	return string(headers, start, end - start);
}

void client::handshake() {
	string key = getHeader("Sec-WebSocket-Key");
	string version = getHeader("Sec-WebSocket-Version");
	if(!memcmp(version.c_str(), "13", 2)) {
		key = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";		
		CryptoPP::SHA1 sha;
		string hash = "";
		CryptoPP::StringSource(key, true, new CryptoPP::HashFilter(sha, new CryptoPP::StringSink(hash)));		
		key = hash;
		hash = "";
		CryptoPP::StringSource(key, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(hash)));		
		string talkback = 
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: " + hash + "\r\n";
		send(socket, talkback.c_str(), talkback.length(), 0);
		handshaken = true;
	} else {
		string talkback = 
			"HTTP/1.1 400 Bad Request\r\n"
			"Sec-WebSocket-Version: 13\r\n";
		send(socket, talkback.c_str(), talkback.length(), 0);
	}
}

bool server::connect() {
	close();
	ListenSocket = INVALID_SOCKET;
	result = NULL;
	//create the socket
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) { //Initialize Winsock
        return false;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;    
    if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result)) { //Resolve the server address and port
        WSACleanup();
        return false;
    }    
    if ((ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET) { //Create a SOCKET for connecting to server
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }   
	//bind the socket
    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) { //Setup the TCP listening socket
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }
    freeaddrinfo(result);
	//tell the socket to listen
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }    
	u_long iMode = 1;
	if(ioctlsocket(ListenSocket, FIONBIO, &iMode)) {
		return false;
	}
	closed = false;
	return true;
}

void server::onMessage(void (* message)(client, string)) {
	f_message = message;
}

void server::onConnect(void (* connect)(client)) {
	f_connect = connect;
}

void server::onClose(void (* close)(client)) {
	f_close = close;
}

void server::onError(void (* error)(client, std::exception)) {
	f_error = error;
}


void server::start() {
	SOCKET clientSocket = INVALID_SOCKET;
	int i;
	string s;
	while(!closed) {
		clientSocket = accept(ListenSocket, NULL, NULL);
		if(clientSocket != INVALID_SOCKET) {
			client c = client(clientSocket);
			clients.push_back(c);
			f_connect(c);
		}
		for(i = 0; i < clients.size(); i++) {
			client c = clients[i];			
			try {			
				if(c.receiveMessage(&s)) {
					f_message(c, s);
				}
				if(c.isClosed()) {
					f_close(c);
					clients.erase(clients.begin() + i);
					--i;
				} else {					
					clients[i] = c;
				}
			} catch(std::exception e) {
				f_error(c, e);
			}
		}
	}
	while(clients.size()) {
		f_close(clients[0]);
		clients.erase(clients.begin());
	}
}

void server::close() {	
	if(!closed) {
		closesocket(ListenSocket);
		closed = 1;
	}
}

string decode(string s) {
	int length = s[1] & 127, i = 0;
	string masks, data, message;
	if(length == 126) {
		masks = string(s, 4, 4);
		data = string(s, 8);
	} else if(length == 127) {
		masks = string(s, 10, 4);
		data = string(s, 14);
	} else {
		masks = string(s, 2, 4);
		data = string(s, 6);
	}
	message = "";
	for(; i < data.length(); ++i) {
		message += data[i] ^ masks[i % 4];
	}
	return message;
}

char min(char a, char b) {
	return a < b ? a : b;
}

string encode(string s) {
	string first = string(1, 0x01), 
		cont = string(1, 0x00), 
		first_last = string(1, 0x81), 
		last = string(1, 0x80), 
		encoded = "";
	char jump = 2, i = 0, length;	
	for(; i < s.length(); i += jump) {
		length = min(i + jump, s.length()) - i;		
		if(jump >= s.length()) {
			encoded += first_last + length + string(s, i, length);
		} else if(i + jump >= s.length()) {
			encoded += last + length + string(s, i, length);			
		} else if(encoded.length()) {
			encoded += cont + length + string(s, i, length);						
		} else {
			encoded += first + length + string(s, i, length);						
		}
	}
	return encoded;
}
