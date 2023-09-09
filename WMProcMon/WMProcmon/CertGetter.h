#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "crypt32.lib")

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

typedef struct {
    LPWSTR lpszProgramName;
    LPWSTR lpszPublisherLink;
    LPWSTR lpszMoreInfoLink;
} SPROG_PUBLISHERINFO, * PSPROG_PUBLISHERINFO;


int GetCert(wchar_t* exefilename, wchar_t* disallowed_subject);

BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext, WCHAR dis_name[]);


LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);

BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,
    PSPROG_PUBLISHERINFO Info);

BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME* st);

BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO* pCounterSignerInfo);
