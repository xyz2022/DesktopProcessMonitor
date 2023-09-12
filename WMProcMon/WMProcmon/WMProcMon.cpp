//#include "eventsink.h"
#include "PipeServer.h"
#include "CertGetter.h"
#include "ProcessScanner.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include <iomanip>
#include <string>
#include <future>
#include <csignal>
#include <processthreadsapi.h>

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

#pragma comment(lib, "wbemuuid.lib")

BOOL WINAPI ConsolelHandlerRoutine(_In_ DWORD dwCtrlType);
HRESULT DoFullProcess(long lObjectCount, IWbemClassObject** apObjArray);

static IWbemLocator* pLoc;
static IWbemServices* pSvc;
static BOOL UnloadEverything();
static IEnumWbemClassObject* pEnum;

HRESULT ProcessEnum(IEnumWbemClassObject* pEnum);
wchar_t evil_app_publisher[MAX_PATH] = L"Psiphon Inc.";
int main(int iArgCnt, char** argv)
{
    //Run check on all existing processes for this computer
    //This is a one time check. Run this in a background thread
    //to ensure mainthread is catching new created processes 

    thread t1(CheckProcessIDsAll, evil_app_publisher);

    DWORD pid = GetCurrentProcessId();
    cout << "MAIN PID: " << pid << endl;

    HRESULT hres;

    SetConsoleCtrlHandler(ConsolelHandlerRoutine, true);

    StartPipeServer();

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library. Error code = 0x"
            << hex << hres << endl;
        return 1;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM negotiates service
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );


    if (FAILED(hres))
    {
        cout << "Failed to initialize security. Error code = 0x"
            << hex << hres << endl;
        CoUninitialize();
        return 1;                      // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object. "
            << "Err code = 0x"
            << hex << hres << endl;
        CoUninitialize();
        return 1;                 // Program has failed.
    }

    // Step 4: ---------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    pSvc = NULL;

    // Connect to the local root\cimv2 namespace
    // and obtain pointer pSvc to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc
    );

    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x"
            << hex << hres << endl;
        pLoc->Release();
        CoUninitialize();
        return 1;                // Program has failed.
    }

    cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;



    ///////////////MY ADED
    // 
    
    //IWbemClassObject* apObj[10];

    hres = pSvc->ExecNotificationQuery(
        _bstr_t("WQL"),
        _bstr_t("SELECT * "
            "FROM __InstanceCreationEvent WITHIN 1 "
            "WHERE TargetInstance ISA 'Win32_Process'"),
        WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY,
        NULL,
        &pEnum);
        
    // Check for errors.
    if (FAILED(hres))
    {
        printf("ExecNotificationQueryAsync failed "
            "with = 0x%X\n", hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    cout << "Starting Main Loop" << endl;
    
    ProcessEnum(pEnum);
    
    cout << "WBEM_S_NO_ERROR != hRes) Exited main loop" << endl;

    // Cleanup
    // ========
    UnloadEverything();

    return 0;   // Program successfully completed.
}



HRESULT ProcessEnum(IEnumWbemClassObject* pEnum)
{
    HRESULT    hRes = WBEM_S_NO_ERROR;

    // Final Next will return WBEM_S_FALSE
    while (WBEM_S_NO_ERROR == hRes)
    {
        ULONG            uReturned;
        IWbemClassObject* apObj[10];

        hRes = pEnum->Next(WBEM_INFINITE, 1, apObj, &uReturned);

        if (SUCCEEDED(hRes))
        {
            // Do something with the objects.
            //ProcessObjects( uReturned,  apObj );
            DoFullProcess(uReturned, apObj);
            //for (ULONG n = 0; n < uReturned; n++)
            //{
            //    apObj[n]->Release();
           // }

        }    // If Enum succeeded...
    }    // While Enum is returning objects...

    return hRes;
}

BOOL WINAPI ConsolelHandlerRoutine(_In_ DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        UnloadEverything();
        return FALSE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        UnloadEverything();
        return FALSE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        UnloadEverything();
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        UnloadEverything();
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        UnloadEverything();
        return FALSE;

    default:

        return FALSE;
    }
}

static BOOL UnloadEverything()
{
    cout << "unloading everything" << endl;
    //pSvc->CancelAsyncCall(pStubSink);
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return TRUE;
}


HRESULT DoFullProcess(long lObjectCount,
    IWbemClassObject** apObjArray)
{
    HRESULT hres = S_OK;
    HRESULT hres2 = S_OK;
    BSTR  bstrDriveObj;
    bool success = false;

    wstring token1(L"ExecutablePath");
    wstring::size_type token1Len = token1.length();

    wstring token2(L"ProcessId = ");
    wstring::size_type token2Len = token2.length();

    wstring token3(L"ProcessId = ");
    wstring::size_type token3Len = token3.length();

    for (int i = 0; i < lObjectCount; i++)
    {
        success = true;
        cout << "Event occurred" << endl;

        hres2 = apObjArray[i]->GetObjectText(WBEM_FLAG_NO_FLAVORS, &bstrDriveObj);
        apObjArray[i]->Release();
        std::wstring ws(bstrDriveObj, SysStringLen(bstrDriveObj));


        wcout << L"GetObjectText:" << ws << endl << endl;
        cout << "Looking for ExecutablePath" << endl;
        //
        // Find Executable PATH
        //

        std::wstring::size_type n;
        n = ws.find(token1);
        if (n == wstring::npos)
        {
            success = false;
            cout << "ExecutablePath not found" << endl;
            continue;
        }
        else
            cout << "ExecutablePath found at pos: " << to_string(n) << endl;

        wstring::size_type offset1 = ws.find(L'"', n + token1Len);
        if (offset1 == wstring::npos)
        {
            success = false;
            continue;
        }
        wstring::size_type offset2 = ws.find(L'"', offset1 + 1);
        if (offset2 == wstring::npos)
        {
            success = false;
            continue;
        }

        wstring full_path = ws.substr(offset1 + 1, offset2 - offset1 - 1);
        wcout << "FOUND executablepath: " << "FULL PATH " << full_path << endl;

        //
        // Find ParentProcess ID first
        // (because it contains "ProcessID" it is found before the real "ProcessID"
        //

        n = ws.find(token2);
        if (n == wstring::npos)
        {
            success = false;
            cout << "ParentProcessID not found" << endl;
            continue;
        }
        else
            cout << "ParentProcessID found at pos: " << to_string(n) << endl;


        n = ws.find(token2, n + 1);
        if (n == wstring::npos)
        {
            success = false;
            cout << "ProcessID not found" << endl;
            continue;
        }
        else
            cout << "ProcessID found at pos: " << to_string(n) << endl;
        offset1 = ws.find(L';', n + token2Len);
        if (offset1 == wstring::npos)
        {
            success = false;
            continue;
        }
       

        wstring process_id = ws.substr(n + token2Len, offset1 - (n + token2Len));
        //cout << n << " minus " << offset1 - n << " then offset = " << offset1 << endl;
        //wcout << "FOUND process_id: " << process_id << endl;




        SysFreeString(bstrDriveObj);
        bstrDriveObj = NULL;


        wchar_t k[MAX_PATH];
        LPWSTR c = lstrcpynW(k, full_path.c_str(), MAX_PATH);
        wchar_t j[MAX_PATH] = L"Psiphon Inc.";
        int err = GetCert(k, j);
        if (err == 0) //success
        {
            cout << "MATCH CONFIRMED" << endl;
            int pid = _wtoi(process_id.c_str());

            cout << "Terminate process with ID: " << pid << endl;

            HANDLE h = OpenProcess(PROCESS_TERMINATE, false, pid);
            if (!h)
            {
                cout << "Failed to open process handle with error: " << GetLastError() << " PID: " << pid << endl;
                continue;
            }
            cout << "Opened handle to process with PROCESS_TERMINATE permission" << endl;
            int t = TerminateProcess(h, 1);
            if (t == 0)
            {
                cout << "Failed to TerminateProcess with error: " << GetLastError() << endl;
                CloseHandle(h);
                continue;
            }
            CloseHandle(h);
            cout << "Terminated process with pid: " << pid << endl;
        }
        else //fail
        {
            cout << "MISMATCH CONFIRMED" << endl;
            continue;
        }
    }
    return WBEM_S_NO_ERROR;
}