// EventSink.cpp
/*#include "eventsink.h"
#include "CertGetter.h"

ULONG EventSink::AddRef()
{
    cout << "EventSink::AddRef()" << endl;
    return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release()
{
    cout << "EventSink::Release()" << endl;
    LONG lRef = InterlockedDecrement(&m_lRef);
    if (lRef == 0)
        delete this;
    return lRef;
}


HRESULT EventSink::QueryInterface(REFIID riid, void** ppv)
{
    cout << "EventSink::QueryInterface(REFIID riid, void** ppv)" << endl;
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink*)this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }
    else return E_NOINTERFACE;
}


HRESULT EventSink::Indicate(long lObjectCount,
    IWbemClassObject** apObjArray)
{
    cout << "EventSink::Indicate" << endl;
    HRESULT hres = S_OK;
    HRESULT hres2 = S_OK;
    long unsigned result;
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

        wstring full_path = ws.substr(offset1+1, offset2 - offset1 - 1);
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


        n = ws.find(token2, n+1);
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
        lstrcpynW(k, full_path.c_str(), MAX_PATH);
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

HRESULT EventSink::SetStatus(
    // LONG lFlags,
    // HRESULT hResult,
    // BSTR strParam,
    // IWbemClassObject __RPC_FAR* pObjParam
)
{
    cout << "EventSink::SetStatus" << endl;
    if (lFlags == WBEM_STATUS_COMPLETE)
    {
        printf("Call complete. hResult = 0x%X\n", hResult);
    }
    else if (lFlags == WBEM_STATUS_PROGRESS)
    {
        printf("Call in progress.\n");
    }
    return WBEM_S_NO_ERROR;
}    // end of EventSink.cpp




HRESULT DoFullProcess(long lObjectCount,
    IWbemClassObject** apObjArray)
{
    cout << "EventSink::Indicate" << endl;
    HRESULT hres = S_OK;
    HRESULT hres2 = S_OK;
    long unsigned result;
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
        lstrcpynW(k, full_path.c_str(), MAX_PATH);
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

*/