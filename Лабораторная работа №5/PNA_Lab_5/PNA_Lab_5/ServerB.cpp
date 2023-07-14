#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <ctime>
#include "Winsock2.h"  
#pragma comment(lib, "WS2_32.lib")   // экспорт  WS2_32.dll

using namespace std;

bool GetRequestFromClient(char*, short, struct sockaddr*, int*);
bool PutAnswerToClient(char*, short, struct sockaddr*, int*);
void SearchServer(char*);
string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

int main()
{
	WSADATA wsaData;
	SOCKET sS;
	SOCKADDR_IN serv;
	SOCKADDR_IN from;

	char name[50] = "Hello";
	serv.sin_family = AF_INET;           // используется IP-адресация  
	serv.sin_port = htons(2000);          // порт 2000
	serv.sin_addr.s_addr = INADDR_ANY;   // любой собственный IP-адрес 

	memset(&from, 0, sizeof(from));   // обнулить память
	int lfrom = sizeof(from);

	char hostname[25];
	hostent* clntInfo;
	memset(&clntInfo, 0, sizeof(clntInfo));

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());

		cout << "Search servers..." << endl;
		SearchServer(name);

		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("Socket:", WSAGetLastError());
		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
			throw  SetErrorMsgText("Bind:", WSAGetLastError());
		if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
			throw SetErrorMsgText("gethostname: ", WSAGetLastError());
		cout << "Server hostname: " << hostname << endl;
		do
		{
			if (GetRequestFromClient(name, sS, (sockaddr*)&from, &lfrom))
			{
				clntInfo = gethostbyaddr((const char*)&from.sin_addr, 4, AF_INET); //получ символ имени компью по сетев адрес

				cout << endl;
				cout << "Client socket:" << endl;
				cout << "Hostname: " << clntInfo->h_name << endl;
				cout << "IP: " << inet_ntoa(from.sin_addr) << endl;
				cout << "Port: " << htons(from.sin_port) << endl;
				cout << endl;
				if (PutAnswerToClient(name, sS, (sockaddr*)&from, &lfrom))
				{
					cout << "Success" << endl;
				}
			}
			else
				cout << "Wrong call name" << endl;
		} while (true);

		if (closesocket(sS) == SOCKET_ERROR)
			throw  SetErrorMsgText("CloseSocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << "\n" << errorMsgText << endl;
	}
}

bool GetRequestFromClient(char* name, short port, struct sockaddr* from, int* flen)
{
	int lbuf;
	char ibuf[50] = "";
	cout << "Wait message..." << endl;
	while (true)
	{
		if (lbuf = recvfrom(port, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
				return false;
			else
				throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
		}
		if (strcmp(ibuf, name) == 0)
			return true;
		else
			return false;
	}
}

bool PutAnswerToClient(char* name, short port, struct sockaddr* to, int* lto)
{
	int lbuf;
	if (lbuf = sendto(port, name, strlen(name) + 1, NULL, to, *lto) == SOCKET_ERROR)
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
	return true;
}

void SearchServer(char* name)
{
	SOCKADDR_IN from;
	memset(&from, 0, sizeof(from));
	int lfrom = sizeof(from);
	SOCKET sock;
	int lbuf;
	int optval = 3000;
	if ((sock = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
	{
		throw SetErrorMsgText("socket: ", WSAGetLastError());
	}
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval)) == SOCKET_ERROR)
		throw SetErrorMsgText("opt: ", WSAGetLastError());
	SOCKADDR_IN all;
	all.sin_family = AF_INET;
	all.sin_port = htons(2000);
	all.sin_addr.s_addr = INADDR_BROADCAST;
	char ibuf[50];
	if (lbuf = sendto(sock, name, strlen(name) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
	}
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&optval, sizeof(optval)) == SOCKET_ERROR)
		throw SetErrorMsgText("opt: ", WSAGetLastError());
	if (lbuf = recvfrom(sock, ibuf, sizeof(ibuf), NULL, (sockaddr*)&from, &lfrom) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAETIMEDOUT)
			throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
	}
	if (lbuf == 0)
	{
		cout << "Server socket: " << endl;
		cout << "IP: " << inet_ntoa(from.sin_addr) << endl;
		cout << "Port: " << htons(from.sin_port) << endl;
	}
	if (closesocket(sock) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());
	}
}

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case WSAEINTR: msgText = "Function interrupted";  break;
	case WSAEACCES: msgText = "Permission denied"; break;
	case WSAEFAULT: msgText = "Wrong address"; break;
	case WSAEINVAL: msgText = "Wrong args"; break;
	case WSAEMFILE: msgText = "Too many files have opened"; break;
	case WSAEWOULDBLOCK: msgText = "Resource temporarily unavailable"; break;
	case WSAEINPROGRESS: msgText = "Operation in progress"; break;
	case WSAEALREADY: msgText = "Operation already in progress"; break;
	case WSAENOTSOCK: msgText = "Wrong socket"; break;
	case WSAEDESTADDRREQ: msgText = "Required adress location"; break;
	case WSAEMSGSIZE: msgText = "Message is too long "; break;
	case WSAEPROTOTYPE: msgText = "Wrong protocol type to socket"; break;
	case WSAENOPROTOOPT: msgText = "Error in protocol option"; break;
	case WSAEPROTONOSUPPORT: msgText = "Protocol is not supported"; break;
	case WSAESOCKTNOSUPPORT: msgText = "Socket type is not supported"; break;
	case WSAEOPNOTSUPP: msgText = "Operation is not supported"; break;
	case WSAEPFNOSUPPORT: msgText = "Protocol type is not supported"; break;
	case WSAEAFNOSUPPORT: msgText = "Addresses type is not supported by protocol"; break;
	case WSAEADDRINUSE: msgText = "Address is already used"; break;
	case WSAEADDRNOTAVAIL: msgText = "Requested address cannot be used"; break;
	case WSAENETDOWN: msgText = "Network disconnected"; break;
	case WSAENETUNREACH: msgText = "Network is unttainable"; break;
	case WSAENETRESET: msgText = "Network dropped the connection"; break;
	case WSAECONNABORTED: msgText = "Connection aborted"; break;
	case WSAECONNRESET: msgText = "Connection restored"; break;
	case WSAENOBUFS: msgText = "Not enough memory for buffers"; break;
	case WSAEISCONN: msgText = "Socket has already connected"; break;
	case WSAENOTCONN: msgText = "Socket has not connected"; break;
	case WSAESHUTDOWN: msgText = "Send cannot be performed: socket was shutdowned"; break;
	case WSAETIMEDOUT: msgText = "Alloted time interval has ended"; break;
	case WSAECONNREFUSED: msgText = "Connection was rejected"; break;
	case WSAEHOSTDOWN: msgText = "Host was shotdowned"; break;
	case WSAEHOSTUNREACH: msgText = "Has not way for host"; break;
	case WSAEPROCLIM: msgText = "Too many processes"; break;
	case WSASYSNOTREADY: msgText = "Network is unavailable"; break;
	case WSAVERNOTSUPPORTED: msgText = "Version is not supported"; break;
	case WSANOTINITIALISED: msgText = "WS2_32.dll is not initialised"; break;
	case WSAEDISCON: msgText = "Connection in progress"; break;
	case WSATYPE_NOT_FOUND: msgText = "Type is not found"; break;
	case WSAHOST_NOT_FOUND: msgText = "Host is not found"; break;
	case WSATRY_AGAIN: msgText = "Try again"; break;
	case WSANO_RECOVERY: msgText = "Unknown error"; break;
	case WSANO_DATA: msgText = "Has not data type"; break;
	case WSA_INVALID_HANDLE: msgText = "Invalid handle"; break;
	case WSA_INVALID_PARAMETER: msgText = "Invalid one or many parameters"; break;
	case WSA_IO_INCOMPLETE: msgText = "IO object is incomplete"; break;
	case WSA_IO_PENDING: msgText = "Operation will end later"; break;
	case WSA_NOT_ENOUGH_MEMORY: msgText = "Not enough memory"; break;
	case WSA_OPERATION_ABORTED: msgText = "Operation was aborted"; break;
	case WSAEINVALIDPROCTABLE: msgText = "Invalid provider"; break;
	case WSAEINVALIDPROVIDER: msgText = "Error in provider version"; break;
	case WSAEPROVIDERFAILEDINIT: msgText = "Failed provider initialization"; break;
	case WSASYSCALLFAILURE: msgText = "Abnormal termination of a system call"; break;
	default: msgText = "Unknown error"; break;
	}
	return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
}