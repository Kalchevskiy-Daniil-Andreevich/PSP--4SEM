#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Winsock2.h"//заголовочный файл 
#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#pragma comment(lib, "WS2_32.lib")//библиотека экспорта WS2_32.lib

int main()
{
    setlocale(LC_ALL, "rus");

    WSADATA wsaData;
    SOCKET  sS;
    SOCKADDR_IN serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(2000);
    serv.sin_addr.s_addr = INADDR_ANY;

    try 
    {
        //seterrormsgtxt предназначенный для обработки стандартных ошибок библиотеки WS2_32.LIB
        //WSAGetLastError код ошибки
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) //инициализации библиотеки, [версия сокета, указатель на структуру]
        {  
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)//создаем сокет, [формат адреса, тип сокета, протокол]
        { 
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }
        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)//связывает сокет с параметрами, [сокет, указатель, длина в байтах ]
        {
            throw  SetErrorMsgText("bind: ", WSAGetLastError());
        }
        if (listen(sS, SOMAXCONN) == SOCKET_ERROR) //сокет в режим ожидания(доступным для подключений и устанавливает макс длину очереди), [Дескриптор сокета, длина очереди]
        { 
            throw SetErrorMsgText("listen: ", WSAGetLastError());
        }
        //Установка  параметров сокета осуществляется с помощью структуры SOCKADDR_IN.

        //подключился клиент
        SOCKET cS;
        SOCKADDR_IN clnt;
        memset(&clnt, 0, sizeof(clnt));
        int lclnt = sizeof(clnt);

        clock_t start, end;
        char ibuf[50], obuf[50] = "server: принято ";
        int libuf = 0, lobuf = 0;
        bool flag = true;

        while (true)
        {
            cout << "ServerT\n\n";
            if ((cS = accept(sS, (sockaddr*)&clnt, &lclnt)) == INVALID_SOCKET) //разрешает подключение к сокету
            { 
                throw SetErrorMsgText("accept: ", WSAGetLastError());
            }
            cout << "Client connect: " << endl; //клиент подключился 
            cout << "ADDRES client :          " << inet_ntoa(clnt.sin_addr) << " : " << htons(clnt.sin_port) << endl << endl << endl << endl << endl;
            while (true) 
            {
                if ((libuf = recv(cS, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR) 
                { //принять данные от клиента 
                    cout << "Client disconnect: " << GetErrorMsgText(WSAGetLastError()) << endl;
                    flag = true;
                    break;
                }

                if (flag)
                {
                    flag = false;
                    start = clock();//считаем 
                }

                string obuf = ibuf;
                if ((lobuf = send(cS, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR)
                {
                    throw SetErrorMsgText("send: ", WSAGetLastError());//отправляем клиенту
                }

                if (strcmp(ibuf, "") == 0) 
                {
                    flag = true;
                    end = clock();
                    cout << "Time for send and recv: " << ((double)(end - start) / CLK_TCK) << " c\n\n"; //время 
                    break;
                }
                else 
                {
                    cout << ibuf << endl;
                }
            }
        }
       

        if (closesocket(sS) == SOCKET_ERROR)//закрываем сокеты(освобождаем ресурс)
        {
            throw  SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
        if (closesocket(cS) == SOCKET_ERROR) 
        {
            throw  SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
        if (WSACleanup() == SOCKET_ERROR) //Завершаем использование библиотеки
        {
            throw  SetErrorMsgText("Cleanup: ", WSAGetLastError());
        }
    }
    catch (string errorMsgText)
    {
        cout << endl << errorMsgText;
    }

    system("pause");
    return 0;
}