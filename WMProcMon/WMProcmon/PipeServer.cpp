#include "PipeServer.h"

using namespace std;

void StartPipeServer()
{
    cout << "Starting new thread for pipes" << endl;
	std::thread t(PipeServer);
	t.detach();

    cout << "Starting new thread for pipes" << endl;
    std::thread t2(PipeServerQuitter);
    t2.detach();
}

void PipeServer()
{
    HANDLE hPipe;
    DWORD pid = GetCurrentProcessId();
    DWORD sessionID = 0;
    bool success = ProcessIdToSessionId(pid, &sessionID);
    cout << "SesionID = " << sessionID << endl;
    std::wstring pipename = L"\\\\.\\pipe\\PIPE";// << sessionID;
    WCHAR sessionIDText[10];
    ZeroMemory(sessionIDText, 10 * sizeof(WCHAR));
    _itow_s(sessionID, sessionIDText, 10);
    pipename.append(sessionIDText);

    wcout << L"Creating Named Pipe: " << pipename.c_str() << endl;

    hPipe = CreateNamedPipe(pipename.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
        1,
        1024,
        1024,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);
    while (hPipe != INVALID_HANDLE_VALUE)
    {
 //       if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
 //       {
 //           while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
 //           {
 //               /* add terminating zero */
 //               buffer[dwRead] = '\0';
 //
 //               /* do something with data in buffer */
 //               printf("%s", buffer);
 //           }
 //       }
        cout << "SERVER Pipe waiting for client connection" << endl;
        if (ConnectNamedPipe(hPipe, NULL) != FALSE)
        {
            cout << "SERVER Pipe received connection. Now disconnecting." << endl;
            DisconnectNamedPipe(hPipe);
            continue;
        }
        break;
    }
    cout << "Exiting pipe thread" << endl;
}


void PipeServerQuitter()
{
    HANDLE hPipe;
    DWORD pid = GetCurrentProcessId();
    DWORD sessionID = 0;
    bool success = ProcessIdToSessionId(pid, &sessionID);
    cout << "SesionID = " << sessionID << endl;
    std::wstring pipename = L"\\\\.\\pipe\\PIPE";// << sessionID;
    WCHAR sessionIDText[10];
    ZeroMemory(sessionIDText, 10 * sizeof(WCHAR));
    _itow_s(sessionID, sessionIDText, 10);
    pipename.append(sessionIDText);
    pipename.append(L"QUIT");
    wcout << L"Creating Named Pipe: " << pipename.c_str() << endl;

    hPipe = CreateNamedPipe(pipename.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
        1,
        1024,
        1024,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);
    while (hPipe != INVALID_HANDLE_VALUE)
    {
        //       if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
        //       {
        //           while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
        //           {
        //               /* add terminating zero */
        //               buffer[dwRead] = '\0';
        //
        //               /* do something with data in buffer */
        //               printf("%s", buffer);
        //           }
        //       }
        cout << "SERVER Pipe waiting for client connection" << endl;
        if (ConnectNamedPipe(hPipe, NULL) != FALSE)
        {
            cout << "SERVER Pipe received connection. Now disconnecting." << endl;
            DisconnectNamedPipe(hPipe);
            //Exit the process because a connection to the quit pipe means exit app
            exit(0);
            continue;
        }
        break;
    }
    cout << "Exiting pipe thread" << endl;
}