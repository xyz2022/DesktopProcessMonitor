/*
// EventSink.h
#ifndef EVENTSINK_H
#define EVENTSINK_H

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

HRESULT DoFullProcess(long lObjectCount,IWbemClassObject** apObjArray);

class EventSink : public IWbemObjectSink
{
    LONG m_lRef;
    bool bDone;

public:
    EventSink() { m_lRef = 0; }
    ~EventSink() { bDone = true; }


    bool IsActive() const {
        // Check if the sink has been active recently (you can define your own criteria)
        //return (std::chrono::steady_clock::now() - lastActivityTime) < std::chrono::seconds(10);
        return TRUE;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT
        STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate(
        LONG lObjectCount,
        IWbemClassObject __RPC_FAR* __RPC_FAR* apObjArray
    );

    virtual HRESULT STDMETHODCALLTYPE SetStatus(   
        // LONG lFlags,
        // HRESULT hResult,
        // BSTR strParam,
        // IWbemClassObject __RPC_FAR* pObjParam
    );

private:
    std::chrono::steady_clock::time_point lastActivityTime;
};

#endif    // end of EventSink.h
*/