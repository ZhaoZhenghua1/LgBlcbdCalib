////////////////////////////////////////////////////////////////////////////////////
// 

#include "RegisterEx.h"
#include "luboheader.h"


#define RIGISTER_KEY _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\CloudClassroom\\")

CRegisterEx::CRegisterEx(void)
{
}

CRegisterEx::~CRegisterEx(void)
{
}

DWORD WINAPI CRegisterEx::GetRegValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,DWORD *pdwType,DWORD *pdwSize,PVOID *ppValue,bool bNew)
{
	LONG ret = ERROR_SUCCESS;			//函数返回值
	HKEY hOpenKey;

	*pdwSize = 0;

	PVOID    achValue = *ppValue;
	DWORD	 cchValue = MAX_VALUE_NAME; 

	if((!bNew) && (achValue == NULL))
	{
		return 0;
	}

	ret = RegOpenKeyEx(hKey,lpSubKey,REG_OPTION_NON_VOLATILE,KEY_READ,&hOpenKey);
	if(ret == ERROR_SUCCESS)
	{
		if(bNew)
		{
			*ppValue = NULL;
			achValue = (TCHAR*)GlobalAlloc(GMEM_ZEROINIT,MAX_VALUE_NAME);
		}
		cchValue = MAX_VALUE_NAME; 	
		((BYTE *)achValue)[0] = '\0'; 
		ret = ::RegQueryValueEx(hOpenKey,lpValueName,0,pdwType,(BYTE *)achValue,&cchValue);
		if(ret == ERROR_SUCCESS)
		{
			*ppValue = 	achValue;
			*pdwSize = cchValue;
		}
		else if(bNew)
		{
			GlobalFree(achValue);
		}

		CloseHandle(hOpenKey);
	}

	return ret;
}

DWORD WINAPI CRegisterEx::SetRegValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,DWORD dwType,DWORD dwSize,PVOID pValue)
{
	LONG ret = ERROR_SUCCESS;			//函数返回值
	HKEY hOpenKey;
	DWORD dwCreateOROpen;

	ret = RegCreateKeyEx(hKey,lpSubKey,0,_T("Lange"),REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hOpenKey,&dwCreateOROpen);
	if(ret == ERROR_SUCCESS)
	{
		ret = ::RegSetValueEx(hOpenKey,lpValueName,0,dwType,(BYTE *)pValue,dwSize);

		CloseHandle(hOpenKey);
	}

	return ret;	
}

DWORD WINAPI CRegisterEx::RegDeleteKeyValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName)
{
	LONG ret = ERROR_SUCCESS;			//函数返回值
	HKEY hOpenKey;

	ret = RegOpenKeyEx(hKey,lpSubKey,0,KEY_ALL_ACCESS ,&hOpenKey);
	if(ret == ERROR_SUCCESS)
	{
		ret = ::RegDeleteValue(hOpenKey,lpValueName);

		CloseHandle(hOpenKey);
	}

	return ret;	
}

DWORD WINAPI CRegisterEx::RegReplaceKeyValue(HKEY hKey,LPCTSTR lpSubKeyNew,LPCTSTR lpSubKeyOld)
{
	HKEY hOpenKeyNew,hOpenKeyOld;			//注册表键的句柄
	LONG ret;			//函数返回值
	CString csValue;

	TCHAR    *achKey = NULL;   // buffer for subkey name
	DWORD    cbName = MAX_KEY_LENGTH;  // size of name string 
	TCHAR    *achValue = NULL;
	DWORD	 cchValue = MAX_VALUE_NAME; 
	DWORD    dwType = REG_SZ, dwRET = 0;
	int i = 0;

	ret=RegOpenKeyEx(hKey,lpSubKeyNew,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,&hOpenKeyNew);
	if(ret==ERROR_SUCCESS)
	{
		ret=RegOpenKeyEx(hKey,lpSubKeyOld,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,&hOpenKeyOld);
		if(ret==ERROR_SUCCESS)
		{
			achValue = (TCHAR*)GlobalAlloc(GMEM_ZEROINIT,MAX_VALUE_NAME);; 
			dwType = REG_SZ;
			i = 0;

			while(1)
			{
				cchValue = MAX_VALUE_NAME; 	
				achValue[0] = '\0'; 
				ret = RegEnumValue(hOpenKeyNew, i, 
					achValue, 
					&cchValue, 
					NULL, 
					NULL,
					NULL,
					NULL);
				if(ret != ERROR_SUCCESS) break;

				csValue = achValue;

				cchValue = MAX_VALUE_NAME; 	
				achValue[0] = '\0'; 
				ret = ::RegQueryValueEx(hOpenKeyNew,csValue,0,&dwType,(BYTE *)achValue,&cchValue);
				if(ret == ERROR_SUCCESS)
				{
					ret = ::RegSetValueEx(hOpenKeyOld,csValue,0,dwType,(BYTE *)achValue,cchValue);
				}

				i++;
			}

			GlobalFree(achValue);
			CloseHandle(hOpenKeyOld);
		}


		CloseHandle(hOpenKeyNew);
	}

	return 0;
}

DWORD WINAPI CRegisterEx::RegDeleteAllKeyValue(HKEY hKey,LPCTSTR lpSubKey)
{
	HKEY hOpenKey;			//注册表键的句柄
	LONG ret;			//函数返回值
	CString csValue;

	TCHAR    *achKey = NULL;   // buffer for subkey name
	DWORD    cbName = MAX_KEY_LENGTH;  // size of name string 
	TCHAR    *achValue = NULL;
	DWORD	 cchValue = MAX_VALUE_NAME; 
	DWORD    dwType = REG_SZ, dwRET = 0;
	int i = 0;

	ret=RegOpenKeyEx(hKey,lpSubKey,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,&hOpenKey);
	if(ret==ERROR_SUCCESS)
	{
		achValue = (TCHAR*)GlobalAlloc(GMEM_ZEROINIT,MAX_VALUE_NAME);; 
		dwType = REG_SZ;
		i = 0;

		while(1)
		{
			cchValue = MAX_VALUE_NAME; 	
			achValue[0] = '\0'; 
			ret = RegEnumValue(hOpenKey, i, 
				achValue, 
				&cchValue, 
				NULL, 
				NULL,
				NULL,
				NULL);
			if(ret != ERROR_SUCCESS) break;

			//csValue = achValue;
			//csValue.Trim();
			dwRET = ::RegDeleteValue(hOpenKey,achValue);
			if(dwRET == 0) continue; //删除成功

			i++;
		}

		GlobalFree(achValue);
		CloseHandle(hOpenKey);
	}

	return 0;
}

DWORD WINAPI CRegisterEx::RegDeleteAllSubKey(HKEY hKey,LPCTSTR lpSubKey)
{
	HKEY hOpenKey;			//注册表键的句柄
	LONG ret;			//函数返回值
	CString csSubKey,csValue,csKey;

	TCHAR    *achKey = NULL;   // buffer for subkey name
	DWORD    cbName = MAX_KEY_LENGTH;  // size of name string 
	TCHAR    *achValue = NULL;
	DWORD	 cchValue = MAX_VALUE_NAME; 
	DWORD    dwType = REG_SZ, dwRET = 0;;

	int i = 0;

	achKey = (TCHAR*)GlobalAlloc(GMEM_ZEROINIT,MAX_KEY_LENGTH);   	
	ret=RegOpenKeyEx(hKey,lpSubKey,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,&hOpenKey);
	if(ret==ERROR_SUCCESS)
	{		
		dwType = REG_SZ;
		i = 0;

		while(1)
		{
			cbName = MAX_KEY_LENGTH; 
			ret = RegEnumKeyEx(hOpenKey, i, achKey, &cbName, NULL, NULL, NULL, NULL);
			if(ret != ERROR_SUCCESS) break;

			csKey = achKey;

			ret = SHDeleteKey(hOpenKey,csKey);
			if(ret == ERROR_SUCCESS)
			{
				continue;
			}

			i++;
		}

		CloseHandle(hOpenKey);
	}

	return 0;
}

bool CRegisterEx::GetRegDwordValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName, DWORD &dwInitValue)
{
	LONG ret;
	PVOID    achValue = NULL;
	DWORD	 dwValueSize = MAX_VALUE_NAME; 
	DWORD    dwType = REG_SZ;

	ret = GetRegValue(hKey,
		lpSubKey,
		lpValueName,
		&dwType,&dwValueSize,&achValue);
	if(ret==ERROR_SUCCESS)
	{
		dwInitValue = ((DWORD*)achValue)[0];

		GlobalFree(achValue);

		return true;
	}

	return false;
}
//
bool CRegisterEx::GetRegSzValue(HKEY hKey,LPCTSTR lpSubKey,LPCTSTR lpValueName, CString &csInitValue)
{
	LONG ret;
	PVOID    achValue = NULL;
	DWORD	 dwValueSize = MAX_VALUE_NAME; 
	DWORD    dwType = REG_SZ;

	ret = GetRegValue(hKey,
		lpSubKey,
		lpValueName,
		&dwType,&dwValueSize,&achValue);
	if(ret == ERROR_SUCCESS)
	{
		csInitValue = (PWCHAR)achValue;

		GlobalFree(achValue);

		return true;
	}

	return false;
}

CString getstrValueFromRegedit(LPCTSTR lpSubKey, LPCTSTR lpValueName)
{
	CRegisterEx Regedit;

	CString strTemp = TEXT("");
	DWORD TypeOfKey;
	DWORD SizeOfKey;
	PVOID pValueOfKey;

	int ret;

	ret = Regedit.GetRegValue(HKEY_LOCAL_MACHINE, lpSubKey, lpValueName, &TypeOfKey, &SizeOfKey, &pValueOfKey);

	if (ret == ERROR_SUCCESS)
	{
		strTemp = (PWCHAR)pValueOfKey;
	}
	return strTemp;
}

bool GetTchParam(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	getstr = getstrValueFromRegedit(strSubKey, _T("TPROCWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.procwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TPROCHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.procheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SHOWRATE"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.showrate = _ttof(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("ISHCHANGE"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.isHchange = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TROIX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.roix = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TROIY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.roiy = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TROIWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.roiwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TROIHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.roiheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TCALIBWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.calibwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TCALIBHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.calibheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDX1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[0].x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDX2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[1].x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDX3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[2].x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDY1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[0].y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDY2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[1].y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDY3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[2].y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDWIDTH1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[0].width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDWIDTH2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[1].width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDWIDTH3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[2].width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDHEIGHT1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[0].height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDHEIGHT2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[1].height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TSHIELDHEIGHT3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.shield[2].height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLACKBOARDX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.blackboardx = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLACKBOARDY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.blackboardy = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLACKBOARDWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.blackboardwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLACKBOARDHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.blackboardheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TEACHERDESKX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.teacherdeskx = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TEACHERDESKY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.teacherdesky = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TEACHERDESKWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.teacherdeskwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TEACHERDESKHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.teacherdeskheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("ZOOMAREAX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.zoomareax = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("ZOOMAREAY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.zoomareay = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("ZOOMAREAWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.zoomareawidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("ZOOMAREAHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.tparam.zoomareaheight = _ttoi(getstr);

	return true;
}

bool GetCalibheight(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	getstr = getstrValueFromRegedit(strSubKey, _T("TLHEIGHTX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.tlheight.x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TLHEIGHTY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.tlheight.y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TLHEIGHTWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.tlheight.width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TLHEIGHTHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.tlheight.height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TRHEIGHTX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.trheight.x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TRHEIGHTY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.trheight.y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TRHEIGHTWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.trheight.width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("TRHEIGHTHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.trheight.height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLHEIGHTX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.blheight.x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLHEIGHTY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.blheight.y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLHEIGHTWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.blheight.width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BLHEIGHTHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.blheight.height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BRHEIGHTX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.brheight.x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BRHEIGHTY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.brheight.y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BRHEIGHTWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.brheight.width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("BRHEIGHTHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.brheight.height = _ttoi(getstr);

	return true;
}

bool GetStuParam(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	getstr = getstrValueFromRegedit(strSubKey, _T("SPROCWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.procwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SPROCHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.procheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SROIX"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.roix = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SROIY"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.roiy = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SROIWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.roiwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SROIHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.roiheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SCALIBWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.calibwidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SCALIBHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.calibheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDX1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[0].x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDX2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[1].x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDX3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[2].x = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDY1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[0].y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDY2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[1].y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDY3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[2].y = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDWIDTH1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[0].width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDWIDTH2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[1].width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDWIDTH3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[2].width = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDHEIGHT1"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[0].height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDHEIGHT2"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[1].height = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SSHIELDHEIGHT3"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.sparam.shield[2].height = _ttoi(getstr);

	if (!GetCalibheight(lubopm))
	{
		return false;
	}

	return true;
}

bool GetInitFromDoc(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	getstr = getstrValueFromRegedit(strSubKey, _T("SAVEWIDTH"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.savewidth = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SAVEHEIGHT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.saveheight = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("SCREANCHANGEPORT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.screanchangeport = _ttoi(getstr);

	getstr = getstrValueFromRegedit(strSubKey, _T("STARTPORT"));
	if (getstr.IsEmpty())
	{
		return false;
	}
	lubopm.startport = _ttoi(getstr);

	if (!GetStuParam(lubopm))
	{
		return false;
	}

	if (!GetTchParam(lubopm))
	{
		return false;
	}

	return true;
}

bool SetstrValueToRegedit(CString strSubKey, CString name, CString value)
{
	CRegisterEx Regedit;

	DWORD TypeOfKey;
	DWORD SizeOfKey;
	PVOID pValueOfKey;
	HKEY hOpenKey;
	DWORD dwCreateOROpen;

	//int ret = Regedit.SetRegValue(HKEY_LOCAL_MACHINE, strSubKey, name, REG_SZ, (value.GetLength()+1)*sizeof(TCHAR), value)

	int ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, strSubKey, 0, _T("Lange"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hOpenKey, &dwCreateOROpen);
	if (ret == ERROR_SUCCESS)
	{
		ret = ::RegSetValueEx(hOpenKey, name, 0, REG_SZ, (BYTE *)value.GetBuffer(), (value.GetLength() + 1) * sizeof(TCHAR));
		CloseHandle(hOpenKey);
		return true;
	}
	return false;
}

bool SetCalibHeightToDoc(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	if (lubopm.sparam.tlheight.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.sparam.tlheight.x);
		if (!SetstrValueToRegedit(strSubKey, _T("TLHEIGHTX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.tlheight.y);
		if (!SetstrValueToRegedit(strSubKey, _T("TLHEIGHTY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.tlheight.width);
		if (!SetstrValueToRegedit(strSubKey, _T("TLHEIGHTWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.tlheight.height);
		if (!SetstrValueToRegedit(strSubKey, _T("TLHEIGHTHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.sparam.trheight.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.sparam.trheight.x);
		if (!SetstrValueToRegedit(strSubKey, _T("TRHEIGHTX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.trheight.y);
		if (!SetstrValueToRegedit(strSubKey, _T("TRHEIGHTY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.trheight.width);
		if (!SetstrValueToRegedit(strSubKey, _T("TRHEIGHTWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.trheight.height);
		if (!SetstrValueToRegedit(strSubKey, _T("TRHEIGHTHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.sparam.blheight.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.sparam.blheight.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BLHEIGHTX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.blheight.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BLHEIGHTY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.blheight.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BLHEIGHTWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.blheight.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BLHEIGHTHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.sparam.brheight.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.sparam.brheight.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BRHEIGHTX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.brheight.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BRHEIGHTY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.brheight.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BRHEIGHTWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.brheight.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BRHEIGHTHEIGHT"), strtemp))
		{
			return false;
		}
	}

	return true;
}

bool SetStuParamToDoc(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	CString strtemp;
	strtemp.Format(_T("%d"), lubopm.sparam.calibwidth);
	if (!SetstrValueToRegedit(strSubKey, _T("SCALIBWIDTH"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.calibheight);
	if (!SetstrValueToRegedit(strSubKey, _T("SCALIBHEIGHT"), strtemp))
	{
		return false;
	}

	/*strtemp.Format(_T("%d"), lubopm.sparam.procwidth);
	if (!SetstrValueToRegedit(strSubKey, _T("SPROCWIDTH"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.procheight);
	if (!SetstrValueToRegedit(strSubKey, _T("SPROCHEIGHT"), strtemp))
	{
		return false;
	}*/

	if (lubopm.sparam.roiwidth != 0)
	{
		strtemp.Format(_T("%d"), lubopm.sparam.roix);
		if (!SetstrValueToRegedit(strSubKey, _T("SROIX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.roiy);
		if (!SetstrValueToRegedit(strSubKey, _T("SROIY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.roiwidth);
		if (!SetstrValueToRegedit(strSubKey, _T("SROIWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.sparam.roiheight);
		if (!SetstrValueToRegedit(strSubKey, _T("SROIHEIGHT"), strtemp))
		{
			return false;
		}
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[0].x);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDX1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[0].y);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDY1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[0].width);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDWIDTH1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[0].height);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDHEIGHT1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[1].x);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDX2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[1].y);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDY2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[1].width);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDWIDTH2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[1].height);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDHEIGHT2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[2].x);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDX3"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[2].y);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDY3"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[2].width);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDWIDTH3"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.sparam.shield[2].height);
	if (!SetstrValueToRegedit(strSubKey, _T("SSHIELDHEIGHT3"), strtemp))
	{
		return false;
	}

	if (!SetCalibHeightToDoc(lubopm))
	{
		return false;
	}

	return true;
}

bool SetBboardParamToDoc(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	if (lubopm.bparam.blackboardLT.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLT.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTX"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLT.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTY"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLT.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTWIDTH"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLT.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.bparam.blackboardRT.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRT.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTX"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRT.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTY"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRT.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTWIDTH"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRT.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.bparam.blackboardLB.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLB.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBX"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLB.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBY"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLB.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBWIDTH"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardLB.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.bparam.blackboardRB.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRB.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBX"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRB.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBY"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRB.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBWIDTH"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam.blackboardRB.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBHEIGHT"), strtemp))
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//对右侧黑板写入注册表
	if (lubopm.bparam2.blackboardLT.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLT.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTX2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLT.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTY2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLT.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTWIDTH2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLT.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLTHEIGHT2"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.bparam2.blackboardRT.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRT.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTX2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRT.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTY2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRT.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTWIDTH2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRT.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRTHEIGHT2"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.bparam2.blackboardLB.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLB.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBX2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLB.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBY2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLB.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBWIDTH2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardLB.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDLBHEIGHT2"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.bparam2.blackboardRB.width > 0)
	{
		CString strtemp;
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRB.x);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBX2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRB.y);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBY2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRB.width);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBWIDTH2"), strtemp))
		{
			return false;
		}
		strtemp.Format(_T("%d"), lubopm.bparam2.blackboardRB.height);
		if (!SetstrValueToRegedit(strSubKey, _T("BBOARDRBHEIGHT2"), strtemp))
		{
			return false;
		}
	}
	return true;
}

bool SetTchParamToDoc(luboparam &lubopm)
{
	CString strSubKey;
	CString getstr;
	strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));

	CString strtemp;
	strtemp.Format(_T("%d"), lubopm.tparam.calibwidth);
	if (!SetstrValueToRegedit(strSubKey, _T("TCALIBWIDTH"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.calibheight);
	if (!SetstrValueToRegedit(strSubKey, _T("TCALIBHEIGHT"), strtemp))
	{
		return false;
	}

	/*strtemp.Format(_T("%d"), lubopm.tparam.procwidth);
	if (!SetstrValueToRegedit(strSubKey, _T("SPROCWIDTH"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.procheight);
	if (!SetstrValueToRegedit(strSubKey, _T("SPROCHEIGHT"), strtemp))
	{
		return false;
	}*/

	if (lubopm.tparam.roiwidth != 0)
	{
		strtemp.Format(_T("%d"), lubopm.tparam.roix);
		if (!SetstrValueToRegedit(strSubKey, _T("TROIX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.roiy);
		if (!SetstrValueToRegedit(strSubKey, _T("TROIY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.roiwidth);
		if (!SetstrValueToRegedit(strSubKey, _T("TROIWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.roiheight);
		if (!SetstrValueToRegedit(strSubKey, _T("TROIHEIGHT"), strtemp))
		{
			return false;
		}
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[0].x);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDX1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[0].y);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDY1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[0].width);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDWIDTH1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[0].height);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDHEIGHT1"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[1].x);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDX2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[1].y);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDY2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[1].width);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDWIDTH2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[1].height);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDHEIGHT2"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[2].x);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDX3"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[2].y);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDY3"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[2].width);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDWIDTH3"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.shield[2].height);
	if (!SetstrValueToRegedit(strSubKey, _T("TSHIELDHEIGHT3"), strtemp))
	{
		return false;
	}


	if (lubopm.tparam.blackboardwidth > 0)
	{
		strtemp.Format(_T("%d"), lubopm.tparam.blackboardx);
		if (!SetstrValueToRegedit(strSubKey, _T("BLACKBOARDX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.blackboardy);
		if (!SetstrValueToRegedit(strSubKey, _T("BLACKBOARDY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.blackboardwidth);
		if (!SetstrValueToRegedit(strSubKey, _T("BLACKBOARDWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.blackboardheight);
		if (!SetstrValueToRegedit(strSubKey, _T("BLACKBOARDHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.tparam.teacherdeskwidth > 0)
	{
		strtemp.Format(_T("%d"), lubopm.tparam.teacherdeskx);
		if (!SetstrValueToRegedit(strSubKey, _T("TEACHERDESKX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.teacherdesky);
		if (!SetstrValueToRegedit(strSubKey, _T("TEACHERDESKY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.teacherdeskwidth);
		if (!SetstrValueToRegedit(strSubKey, _T("TEACHERDESKWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.teacherdeskheight);
		if (!SetstrValueToRegedit(strSubKey, _T("TEACHERDESKHEIGHT"), strtemp))
		{
			return false;
		}
	}

	if (lubopm.tparam.zoomareawidth > 0)
	{
		strtemp.Format(_T("%d"), lubopm.tparam.zoomareax);
		if (!SetstrValueToRegedit(strSubKey, _T("ZOOMAREAX"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.zoomareay);
		if (!SetstrValueToRegedit(strSubKey, _T("ZOOMAREAY"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.zoomareawidth);
		if (!SetstrValueToRegedit(strSubKey, _T("ZOOMAREAWIDTH"), strtemp))
		{
			return false;
		}

		strtemp.Format(_T("%d"), lubopm.tparam.zoomareaheight);
		if (!SetstrValueToRegedit(strSubKey, _T("ZOOMAREAHEIGHT"), strtemp))
		{
			return false;
		}
	}

	strtemp.Format(_T("%3f"), lubopm.tparam.showrate);
	if (!SetstrValueToRegedit(strSubKey, _T("SHOWRATE"), strtemp))
	{
		return false;
	}

	strtemp.Format(_T("%d"), lubopm.tparam.speed);
	if (!SetstrValueToRegedit(strSubKey, _T("SPEED"), strtemp))
	{
		return false;
	}


	if (!SetBboardParamToDoc(lubopm))
	{
		return false;
	}
	
	return true;
}

bool SetInfoToDoc(luboparam &lubopm)
{
	SetStuParamToDoc(lubopm);
	return SetTchParamToDoc(lubopm);
}