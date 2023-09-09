#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <psapi.h>
#include "CertGetter.h"

using namespace std;

void CheckProcessIDsAll(wchar_t *blocked_app_publisher)
{

	HANDLE processHandle = NULL;
	TCHAR filename[MAX_PATH];
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };

	Process32First(snapshot, &entry);
	do
	{

		processHandle = NULL;
		processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
		if (processHandle != NULL) {
			if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0) {
				CloseHandle(processHandle);
				continue;
			}
			else {
				CloseHandle(processHandle);
				int err = GetCert(filename, blocked_app_publisher);
				if (err == 0) //success
				{
					//cout << "MATCH CONFIRMED" << endl;
					int pid = entry.th32ProcessID;

					HANDLE h = OpenProcess(PROCESS_TERMINATE, false, pid);
					if (!h)
					{
						//cout << "Failed to open process handle with error: " << GetLastError() << " PID: " << pid << endl;
						continue;
					}
					//cout << "Opened handle to process with PROCESS_TERMINATE permission" << endl;
					int t = TerminateProcess(h, 1);
					if (t == 0)
					{
						//cout << "Failed to TerminateProcess with error: " << GetLastError() << endl;
						CloseHandle(h);
						continue;
					}
					CloseHandle(h);
					//cout << "Terminated process with pid: " << pid << endl;
				}
				else //fail
				{
					//cout << "MISMATCH CONFIRMED" << endl;
					continue;
				}
			}
		}
		else {
			//Failed to open process
		}
	} while (Process32Next(snapshot, &entry));
}