//---------------------------------------------------------------------------

#include <vcl.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "winioctl.h"
#include <string>
#include <cstring>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainWindow *MainWindow;
//---------------------------------------------------------------------------

#define DEVICE_SEND CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)
// #define DEVICE_REC  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_DATA)

// Driver handle
HANDLE hDriver = NULL;

const wchar_t cpDriverName[] = L"keM0de_killer";
const wchar_t cpDriverPath[] = L"C:\\keM0de_killer.sys";

SC_HANDLE hSCService;
SC_HANDLE hSCManager;
//---------------------------------------------------------------------------
// Getting process list into ListBox
void GetProcessList()
{
	MainWindow->ProcessContainer_ListBox->Clear();

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
    int nErrorCode = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!hSnap)
	{
		MessageBox(MainWindow->Handle, TEXT("Process snapshot error"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		nErrorCode = GetLastError();
		return;
	}

	if (!Process32First(hSnap, &pe32))
	{
		MessageBox(MainWindow->Handle, TEXT("ProcessFirst error"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
        nErrorCode = GetLastError();
		return;
	}
	do
	{
		MainWindow->ProcessContainer_ListBox->Items->Add("["+UnicodeString(pe32.th32ProcessID) + "] " + UnicodeString(pe32.szExeFile));
	}   while(Process32Next(hSnap, &pe32));

    CloseHandle(hSnap);
}

// Killing process by id
bool KillProcessById(int pID)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, pID);
	if (!hProcess)
	{
    	return false;
	}
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	return true;
}

// 'Govnocode' section || parse pID from listbox
int PidParser(UnicodeString processInfo)
{
	UnicodeString pID;

	for (int i = 1; processInfo.Length(); i++)
	{
		pID.Insert(processInfo.begin()[i], i);
		if (processInfo.begin()[i+1] == ']')
		{
			break;
		}
	}
	return pID.ToInt();
}

//---------------------------------------------------------------------------
__fastcall TMainWindow::TMainWindow(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
BOOL LoadDriver()
{
	/*HKEY hKey = NULL;
	DWORD dwPos = 0;
	DWORD dwErrorControl = 0x1;
	DWORD dwStart = 0x3;
	DWORD dwType = 0x1;

	const wchar_t displayName[] = L"keM0de_killer";
	const char imagePath[MAX_PATH]   = "\\??\\C:\\keM0de_killer.sys";
	/*TCHAR buffer[MAX_PATH];

	GetFullPathName(TEXT("keM0de_killer.sys"), MAX_PATH, (TCHAR*)buffer, NULL);
	wcscat((WCHAR*)imagePath, (WCHAR*)buffer);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\ControlSet001\\Services\\keM0de_killer"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwPos);

	RegSetValueEx(hKey, TEXT("DisplayName"), 0, REG_SZ, (const BYTE*)displayName, sizeof(displayName));
	RegSetValueEx(hKey, TEXT("ErrorControl"), 0, REG_DWORD, (const BYTE*)&dwErrorControl, sizeof(dwErrorControl));
	RegSetValueEx(hKey, TEXT("ImagePath"), 0, REG_EXPAND_SZ, (const BYTE*)imagePath, MAX_PATH);
	RegSetValueEx(hKey, TEXT("Start"), 0, REG_DWORD, (const BYTE*)&dwStart, sizeof(dwStart));
	RegSetValueEx(hKey, TEXT("Type"), 0, REG_DWORD, (const BYTE*)&dwType, sizeof(dwType));

	RegCloseKey(hKey); */

	hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if (!hSCManager)
	{
        MessageBox(MainWindow->Handle, TEXT("Error while driver loading"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
        return FALSE;
	}

    hSCService = CreateService( hSCManager, cpDriverName, cpDriverName,
                                SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
                                SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
								cpDriverPath, NULL, NULL, NULL, NULL, NULL );

	if( hSCService == NULL && GetLastError() == ERROR_SERVICE_EXISTS )
	{
		hSCService = OpenService( hSCManager, cpDriverName, SERVICE_ALL_ACCESS );
	}

	if( hSCService == NULL )
	{
        return FALSE;
	}

	if(!StartService(hSCService, 0, NULL))
	{
		if(GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)
		{
			MessageBox(MainWindow->Handle, UnicodeString(GetLastError()).c_str(), TEXT("ERROR"), MB_OK | MB_ICONERROR);
			return TRUE;
		}
		else if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
		{
			//MessageBox(MainWindow->Handle, TEXT("SERVICE ALLREADY RUNNING error"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
			return TRUE;
		}
		return FALSE;
	}
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL UnloadDriver()
{
	SERVICE_STATUS sS;

	if (!ControlService(hSCService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&sS))
	{
		UnicodeString err_code = UnicodeString(GetLastError());
		MessageBox(MainWindow->Handle, err_code.c_str(), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		//DeleteService(hSCService);
		CloseServiceHandle( hSCManager );
		CloseServiceHandle( hSCService );
		return FALSE;
	}
	//DeleteService(hSCService);
	CloseServiceHandle( hSCManager );
	CloseServiceHandle( hSCService );
    CloseHandle(hDriver);
	return TRUE;
}
//---------------------------------------------------------------------------
BOOL GetDriverHandle()
{
	hDriver = CreateFile(TEXT("\\\\.\\keMode_killer_link"), GENERIC_ALL, 0, 0, OPEN_EXISTING,
								FILE_ATTRIBUTE_SYSTEM, 0);
	if (hDriver == INVALID_HANDLE_VALUE)
	{
		MessageBox(MainWindow->Handle, TEXT("Can't get driver handle"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		MainWindow->ProcessContainer_ListBox->Enabled = false;
		MainWindow->KillProcess_Button->Enabled = false;
        MainWindow->UpdateProcess_Button->Enabled = false;
        MainWindow->StatusBar1->Panels->Items[0]->Text = L"Driver status: FAILED";
		return FALSE;
	}
	MainWindow->StatusBar1->Panels->Items[0]->Text = L"Driver status: SUCCESS";
	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::UpdateProcess_ButtonClick(TObject *Sender)
{
	GetProcessList();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormCreate(TObject *Sender)
{
	GetProcessList();
	if (LoadDriver())
	{
		GetDriverHandle();
	}
	else
	{
        MessageBox(MainWindow->Handle, TEXT("Error while LoadDriver()"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::KillProcess_ButtonClick(TObject *Sender)
{
	for (int i = 0; i < MainWindow->ProcessContainer_ListBox->Items->Count; i++)
	{
		if (i == MainWindow->ProcessContainer_ListBox->ItemIndex)
		{
            int pID = PidParser(MainWindow->ProcessContainer_ListBox->Items->Strings[MainWindow->ProcessContainer_ListBox->ItemIndex]);
			DWORD read;
			int outputData;

			if (!DeviceIoControl(hDriver, DEVICE_SEND, &pID, sizeof(pID), &outputData, sizeof(outputData), &read, NULL))
			{
				MessageBox(MainWindow->Handle, TEXT("Send error"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
			}
            GetProcessList();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    UnloadDriver();
}
//---------------------------------------------------------------------------
