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
#include "About Window.h"

#include "main.h"

INT_PTR CALLBACK AboutWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);

	default:
		return FALSE;
	}
	return TRUE;
}
BOOL AboutWindow::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	SetWindowTextW(GetDlgItem(hwnd, IDC_STATIC1),
		instance.GetApplicationName().c_str());
	WCHAR szBuffer[100];
	const auto& t = instance.GetFileInfo();
	swprintf(szBuffer, 100, L"Ver: %d.%d.%d.%d", t.GetVer1(), t.GetVer2(), t.GetVer3(), t.GetVer4());
	SetWindowTextW(GetDlgItem(hwnd, IDC_STATIC2), szBuffer);

	SetWindowTextW(GetDlgItem(hwnd, IDC_EDIT1),
		L"MIT License\r\n\r\n"
		"Copyright (c) 2019 Orange Software\r\n\r\n"
		"Permission is hereby granted, free of charge, to any person obtaining a copy\r\n"
		"of this software and associated documentation files (the \"Software\"), to deal\r\n"
		"in the Software without restriction, including without limitation the rights\r\n"
		"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\r\n"
		"copies of the Software, and to permit persons to whom the Software is\r\n"
		"furnished to do so, subject to the following conditions:\r\n\r\n"
		"The above copyright notice and this permission notice shall be included in all\r\n"
		"copies or substantial portions of the Software.\r\n\r\n"
		"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\r\n"
		"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\r\n"
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\r\n"
		"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\r\n"
		"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\r\n"
		"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\r\n"
		"SOFTWARE.");
	return TRUE;
}
VOID AboutWindow::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	case IDCANCEL:
	{
		EndDialog(hwnd, 0);
		break;
	}
	default:
		break;
	}
}