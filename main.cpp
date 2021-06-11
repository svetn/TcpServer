#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment (lib,"ws2_32.lib")

using namespace std;
string ipAddress = "127.0.0.1";
int port = 54000;
int main()
{
	// Initialize winsock / making sure we have a connection to winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return 1;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << WSAGetLastError() <<endl;
		WSACleanup();
		return 1;
	}

	// Bind an ip adress and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port); //host_to_network_short
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Couldn't establish a connection to the listenning socket! Quitting";
		return 1;
	}
	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXHOST]; // Service(i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST); //same as memset(host,0,NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}
	// Close listening socket
	closesocket(listening);

	// While loop: accept and echo message back to client
	char buff[4096];
	
	while (true)
	{
		ZeroMemory(buff, 4096);
		
		//Wait for client to send data
		int bytesReceive = recv(clientSocket, buff, 4096, 0);
		if (bytesReceive == SOCKET_ERROR)
		{
			cerr << "Error in revc(). Quitting" << endl;
		}

		if (bytesReceive == 0)
		{
			cout << "Client disconnected" << endl;
			break;
		}

		//Echo message back to client
		send(clientSocket, buff, bytesReceive + 1, 0);

	}
	// Close the sock
	closesocket(clientSocket);

	// Clean up winsock
	WSACleanup();
}
