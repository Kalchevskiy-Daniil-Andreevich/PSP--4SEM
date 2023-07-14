#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <ctime>
#include "Winsock2.h"  
#pragma comment(lib, "WS2_32.lib")  

using namespace std;

bool GetServerByName(char* name, char* hostName, sockaddr* from, int* flen);
string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

void main()
{
	WSADATA wsaData;

	SOCKADDR_IN from;
	memset(&from, 0, sizeof(from));
	char name[] = "Hello";
	int lfrom = sizeof(from);
	char hostName[] = "DESKTOP-6EJJG7F";
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		if (GetServerByName(name, hostName, (sockaddr*)&from, &lfrom))
		{
			cout << "Connection successful" << endl << endl;

			cout << "Connected server: " << endl;
			cout << "Hostname: " << hostName << endl;
			cout << "IP: " << inet_ntoa(from.sin_addr) << endl;
			cout << "Port: " << htons(from.sin_port) << endl;

		}
		else
		{
			cout << "Wrong call name" << endl;
		}

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << "WSAGetLastError: " << errorMsgText << endl;
	}
}
//После того, как IP-адрес сервера определен, необходимо установить необходимый  номер порта и послать  позывной  в адрес сокета  сервера.
bool GetServerByName(char* name, char* hostName, sockaddr* from, int* flen) //функ испол в том случае, если извест сивол имя комп, на котором запу серв
{
	SOCKET sock;

	hostent* servInfo; //струк, котор исп этой функ для хран результ работ функ
	memset(&servInfo, 0, sizeof(servInfo));
	servInfo = gethostbyname(hostName); //функ для поиска сервера
	in_addr* addr_list;
	addr_list = (in_addr*)*(servInfo->h_addr_list);

	((SOCKADDR_IN*)from)->sin_family = AF_INET;
	((SOCKADDR_IN*)from)->sin_port = htons(2000); 
	((SOCKADDR_IN*)from)->sin_addr = *addr_list;
	int lbuf;
	char ibuf[50];
	int optval = 1;

	if ((sock = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket: ", WSAGetLastError());

	if (lbuf = sendto(sock, name, strlen(name) + 1, NULL, from, *flen) == SOCKET_ERROR)
		throw SetErrorMsgText("sendto: ", WSAGetLastError());

	if (lbuf = recvfrom(sock, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
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

	if (closesocket(sock) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());
}

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case WSAEINTR: msgText = "Function interrupted"; break;
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
	case WSAECONNRESET: break;
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