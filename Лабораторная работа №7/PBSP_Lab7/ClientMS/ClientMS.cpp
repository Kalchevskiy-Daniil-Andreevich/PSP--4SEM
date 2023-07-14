#include <iostream>
#include <clocale>
#include <ctime>
#include "ErrorMessage.h"
#include "Windows.h"
#define NAME L"\\\\*\\mailslot\\Box"
#define STOP "STOP"
using namespace std;


int main()
{
    setlocale(LC_ALL, "rus");
    HANDLE cM;
    DWORD rb;
    clock_t start, end;
    char wbuf[300] = "Hello from Maislot-client";

    try 
    {
        if ((cM = CreateFile(NAME, GENERIC_WRITE, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE) 
                throw SetPipeError("CreateFile: ", GetLastError());

        int countMessage;
        cout << "Enter number of messages: ";
        cin >> countMessage;

        for (int i = 1; i <= countMessage; i++) 
        {
            if (i) 
                start = clock();
            
            if (!WriteFile(cM, wbuf, sizeof(wbuf), &rb, NULL)) 
                throw SetPipeError("WriteFile: ", GetLastError());

            cout << wbuf << endl;
        }

        if (!WriteFile(cM, STOP, sizeof(STOP), &rb, NULL)) 
            throw SetPipeError("WriteFile: ", GetLastError());

        end = clock();
        cout << "\n[INFO] Time elapsed: " << ((double)(end - start) / CLK_TCK) << " seconds" << endl;

        if (!CloseHandle(cM)) 
            throw SetPipeError("CloseHandle: ", GetLastError());
    }
    catch (string ErrorPipeText) 
    {
        cout << endl << ErrorPipeText;
    }
}