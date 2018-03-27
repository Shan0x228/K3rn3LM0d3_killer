//---------------------------------------------------------------------------

#include <vcl.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainWindow *MainWindow;
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
void __fastcall TMainWindow::UpdateProcess_ButtonClick(TObject *Sender)
{
	GetProcessList();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormCreate(TObject *Sender)
{
	GetProcessList();
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::KillProcess_ButtonClick(TObject *Sender)
{
	for (int i = 0; i < MainWindow->ProcessContainer_ListBox->Items->Count; i++)
	{
		if (i == MainWindow->ProcessContainer_ListBox->ItemIndex)
		{
			KillProcessById(PidParser(MainWindow->ProcessContainer_ListBox->Items->Strings[MainWindow->ProcessContainer_ListBox->ItemIndex]));
            GetProcessList();
		}
	}
}
//---------------------------------------------------------------------------

