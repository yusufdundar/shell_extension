#include <Windows.h>
#include <string>
#include "myshellGUID.h"
#include <ShlObj.h>

HINSTANCE g_hInstance;

BOOL __stdcall DllMain(
	HINSTANCE hinstDLL,
	DWORD     fdwReason,
	LPVOID    lpvReserved)
{
	switch (fdwReason)
	{
	case(DLL_PROCESS_ATTACH):
		g_hInstance = hinstDLL;
		return true;
	default:
		break;
	}
	return true;
}

HRESULT __stdcall DllCanUnloadNow()
{
	return E_NOTIMPL;
}

HRESULT __stdcall DllGetClassObject(
	REFCLSID rclsid,
	REFIID   riid,
	LPVOID   *ppv)
{
	return E_NOTIMPL;
}

std::wstring MyStringFromCLSID(IID iid)
{
	wchar_t* tempString;
	StringFromCLSID(iid, &tempString);
	return std::wstring(tempString);
}

std::wstring MyGetModuleFileName()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(g_hInstance, buffer, MAX_PATH);
	return std::wstring(buffer);
}

DWORD MySizeInBytes(std::wstring target)
{
	DWORD size = (target.size() + 1) * 2;
	return size;
}
HRESULT __stdcall DllRegisterServer()
{
	MessageBox(NULL, L"WE ARE IN", L"DllegisterServer()", MB_OK);

	HKEY hKey;
	DWORD lpDisp;
	std::wstring lpSubKey;

	//Create GUID Key	
	lpSubKey = L"SOFTWARE\\Classes\\CLSID\\" + MyStringFromCLSID(CLSID_myshellextension);
	LONG result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpSubKey.c_str() , 0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKey,&lpDisp);
	if (result != ERROR_SUCCESS)
	{
		return E_UNEXPECTED;
	}

	//Create InprocServer32 key
	result = RegCreateKeyEx(hKey, L"InprocServer32", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &lpDisp);
	if (result != ERROR_SUCCESS)
	{
		return E_UNEXPECTED;
	}

	//Set (default) value
	std::wstring lpDllPath = MyGetModuleFileName();
	result = RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)lpDllPath.c_str(), MySizeInBytes(lpDllPath));
	if (result != ERROR_SUCCESS)
	{
		return E_UNEXPECTED;
	}

	//Creating ThreadingModel value and set to Apartment
	std::wstring apartment = L"Apartment";
	result = RegSetValueEx(hKey, L"ThreadingModel", 0, REG_SZ, (BYTE*)apartment.c_str(), MySizeInBytes(apartment));
	if (result != ERROR_SUCCESS)
	{
		return E_UNEXPECTED;
	}
	RegCloseKey(hKey);

	//Alert that there has been a change:
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

	return S_OK;
}

HRESULT __stdcall DllUnregisterServer()
{
	return E_NOTIMPL;
}