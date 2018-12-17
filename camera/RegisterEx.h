////////////////////////////////////////////////////////////////////////////////////
//
#pragma once
#include <afxwin.h>
#include <shlwapi.h>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

class __declspec(dllexport) CRegisterEx
{
public:
	CRegisterEx(void);
	virtual ~CRegisterEx(void);

	DWORD WINAPI GetRegValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,DWORD *pdwType,DWORD *pdwSize,PVOID *ppValue,bool bNew = true);

	DWORD WINAPI SetRegValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,DWORD dwType,DWORD dwSize,PVOID pValue);

	DWORD WINAPI RegDeleteKeyValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName);

	DWORD WINAPI RegReplaceKeyValue(HKEY hKey,LPCTSTR lpSubKeyNew,LPCTSTR lpSubKeyOld);

	DWORD WINAPI RegDeleteAllKeyValue(HKEY hKey,LPCTSTR lpSubKey);

	DWORD WINAPI RegDeleteAllSubKey(HKEY hKey,LPCTSTR lpSubKey);

	bool GetRegDwordValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName, DWORD &dwInitValue);

	bool GetRegSzValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName, CString &csInitValue);

private:
};

struct luboparam;

CString getstrValueFromRegedit(LPCTSTR lpSubKey, LPCTSTR lpValueName);
bool GetInitFromDoc(luboparam &lubopm);
bool SetInfoToDoc(luboparam &lubopm);
bool SetTchParamToDoc(luboparam &lubopm);
bool SetStuParamToDoc(luboparam &lubopm);

bool GetTchParam(luboparam &lubopm);
bool GetStuParam(luboparam &lubopm);