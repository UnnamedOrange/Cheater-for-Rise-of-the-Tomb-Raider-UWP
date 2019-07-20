// MIT License
// 
// Copyright (c) 2019 Orange Software
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pch.h"
#include "resource.h"
#include "Main Dialog.h"

#include "main.h"

const std::wstring MainDialog::strButton[2] =
{
	L"当你的游戏运行时，点击我",
	L"Activated / 点我查看关于"
};

INT_PTR CALLBACK MainDialog::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		HANDLE_WM_INITDIALOG(hwnd, wParam, lParam, OnInitDialog);
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, OnCommand);
	default:
	{
		if (false) {}
		else if (message == WM_INITCHEATER)
			InitCheater();
		else if (message == WM_UNLOADCHEATER)
			UnloadCheater();
		else
			return FALSE;
	}
	}
	return TRUE;
}
BOOL MainDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	SetWindowTextW(hwnd, instance.GetApplicationName().c_str());
	SetWindowTextW(GetDlgItem(hwnd, IDC_BUTTON1), strButton[0].c_str());
	return TRUE;
}
VOID MainDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDCANCEL:
	{
		EndDialog(hwnd, 0);
		break;
	}
	case IDC_CHECK1:
	case IDC_CHECK2:
	case IDC_CHECK3:
	{
		int isCheck = Button_GetCheck(hwndCtl);
		if (isCheck)
			helper.Revoke(hProcess, id - IDC_CHECK1);
		else
			helper.Apply(hProcess, id - IDC_CHECK1);
		isCheck = helper.Check(hProcess, id - IDC_CHECK1);
		if (!~isCheck)
			throw std::runtime_error("internal error");
		Button_SetCheck(hwndCtl, isCheck);
		break;
	}
	case IDC_BUTTON1:
	{
		BOOL bFound = FALSE;
		if (hProcess) {}
		else
		{
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (INVALID_HANDLE_VALUE == hSnapshot)
				throw std::runtime_error("fail to create a snapshot.");
			PROCESSENTRY32W pe32{ sizeof(PROCESSENTRY32W) };
			BOOL bRet = Process32FirstW(hSnapshot, &pe32);
			if (!bRet)
				throw std::runtime_error("fail to (Process32FirstW).");
			while (bRet)
			{
				if (std::wstring(pe32.szExeFile) == L"ROTTR_UAP.exe")
				{

					// 获取进程基址
					HANDLE hSnapshot2;
					// 通过 CreateToolhelp32Snapshot 和线程ID，获取进程快照
					hSnapshot2 = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
					if (INVALID_HANDLE_VALUE == hSnapshot)
						throw std::runtime_error("fail to create a snapshot.");
					MODULEENTRY32W me32{ sizeof(MODULEENTRY32W) };
					bRet = Module32FirstW(hSnapshot2, &me32);
					if (!bRet)
						throw std::runtime_error("fail to (Module32FirstW).");
					while (bRet)
					{
						WCHAR szExt[5];
						wcscpy_s(szExt, me32.szExePath + wcslen(me32.szExePath) - 4);
						if (!wcscmp(szExt, L".exe"))
						{
							CloseHandle(hSnapshot2);
							helper.SetBase(me32.hModule);
							break;
						}
						bRet = Module32NextW(hSnapshot2, &me32);
					};

					OpenTheProcess(pe32.th32ProcessID);
					bFound = TRUE;
					break;
				}
				bRet = Process32NextW(hSnapshot, &pe32);
			}
			CloseHandle(hSnapshot);
		}
		SetWindowTextW(GetDlgItem(hwnd, IDC_BUTTON1), strButton[(size_t)(bool)hProcess].c_str());

		if (!bFound)
		{
			about.AccessParam().hWndParent = hwnd;
			about.Create();
		}
		break;
	}
	default:
		break;
	}
}

VOID MainDialog::InitCheater()
{
	if (!hProcess)
		throw std::logic_error("(hProcess) should be nonzero.");

	for (int i = 0; i < NUM_CHECK; i++)
	{
		int ret = helper.Check(hProcess, i);
		if (!~ret)
		{
			MessageBoxW(GetHwnd(), L"您的游戏版本貌似不对", L"错误", MB_ICONERROR);
			throw std::runtime_error("out of except.");
		}
		Button_SetCheck(GetDlgItem(GetHwnd(), IDC_CHECK1 + i), ret);
	}

	for (int i = 0; i < NUM_CHECK; i++)
		EnableWindow(GetDlgItem(GetHwnd(), IDC_CHECK1 + i), TRUE);

	tip.ShowTip(L"ON", L"修改器已启动");
}
VOID MainDialog::UnloadCheater()
{
	CloseHandle(hProcess);
	hProcess = nullptr;
	SetWindowTextW(GetDlgItem(GetHwnd(), IDC_BUTTON1), strButton[0].c_str());

	for (int i = 0; i < NUM_CHECK; i++)
		EnableWindow(GetDlgItem(GetHwnd(), IDC_CHECK1 + i), FALSE);

	tip.ShowTip(L"OFF", L"游戏已退出");
}

VOID MainDialog::OpenTheProcess(DWORD dwPid)
{
	if (hProcess)
		throw std::logic_error("(hProcess) should be (nullptr).");

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
	{
		MessageBoxW(GetHwnd(), L"打开进程失败！", L"错误", MB_ICONERROR);
		return;
	}

	PostMessageW(GetHwnd(), WM_INITCHEATER, NULL, NULL);

	CloseHandle(CreateThread(nullptr, NULL,
		[](LPVOID lpThreadParameter) ->DWORD
		{
			MainDialog& obj = *(MainDialog*)lpThreadParameter;
			WaitForSingleObject(obj.hProcess, INFINITE);
			PostMessageW(obj.GetHwnd(), obj.WM_UNLOADCHEATER, NULL, NULL);
			return 0;
		},
		this, NULL, nullptr));
}