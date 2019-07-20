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

#pragma once
#include "pch.h"

class TipWindow : protected TWindowPopup
{
	virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
	{
		switch (message)
		{
			HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
			HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		default:
			break;
		}
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
	{
		SetLayeredWindowAttributes(GetHwnd(), NULL, 0, ULW_ALPHA);
		ShowWindow(hwnd, SW_SHOW);
		return TRUE;
	}
	VOID OnPaint(HWND hwnd)
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		using namespace Gdiplus;
		Graphics g(ps.hdc);
		g.SetCompositingQuality(CompositingQuality::CompositingQualityHighQuality);
		g.SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAlias);

		SolidBrush brushBlack(Color::Black);
		g.FillRectangle(&brushBlack, Rect(-1, -1, iWidth + 1, iHeight + 1));

		FontFamily ff(L"Î¢ÈíÑÅºÚ");
		Font fTitle(&ff, TDPI::dpi(60.0), 0, UnitPixel);
		Font fCaption(&ff, TDPI::dpi(15.0), 0, UnitPixel);
		SolidBrush brushWhite(Color::White);
		StringFormat sf;
		sf.SetAlignment(StringAlignment::StringAlignmentCenter);
		sf.SetLineAlignment(StringAlignment::StringAlignmentCenter);
		g.DrawString(title.c_str(), title.length(),
			&fTitle, RectF(-1, -1, iWidth + 1, iHeight * 0.9 + 1), &sf, &brushWhite);
		g.DrawString(caption.c_str(), caption.length(),
			&fCaption, RectF(-1, TDPI::dpi(90), iWidth + 1, TDPI::dpi(50)), &sf, &brushWhite);

		EndPaint(hwnd, &ps);
	}

private:
	TGdiplus __unused;

private:
	TTimer timers[3];

	static const int DEGREE = (200);

	std::wstring title, caption;
	int degree{};

public:
	TipWindow()
	{
		auto& param = AccessParam();
		param.dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_SYSMENU;
		param.dwExStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;

		timers[0] = [this](DWORD dwTime)
		{
			timers[0].kill();

			SetLayeredWindowAttributes(GetHwnd(), NULL, 0, ULW_ALPHA);
			SetWindowPos(GetHwnd(), HWND_TOPMOST, 0, 0, TDPI::dpi(150), TDPI::dpi(150), SWP_NOACTIVATE);
			InvalidateRect(GetHwnd(), NULL, TRUE);
			CenterizeWindow();
			SetLayeredWindowAttributes(GetHwnd(), NULL, DEGREE, ULW_ALPHA);
			timers[1].set(1000, FALSE);
		};
		timers[1] = [this](DWORD dwTime)
		{
			timers[1].kill();

			degree = DEGREE;
			timers[2].set(20, FALSE);
		};
		timers[2] = [this](DWORD dwTime)
		{
			SetWindowPos(GetHwnd(), HWND_TOPMOST, NULL, NULL, TDPI::dpi(150), TDPI::dpi(150), SWP_NOMOVE | SWP_NOACTIVATE);
			SetLayeredWindowAttributes(GetHwnd(), NULL, degree, ULW_ALPHA);
			if (degree <= 0)
				timers[2].kill();
			else
			{
				degree = (int)((double)degree * degree / (DEGREE + 1));
				degree = max(degree, 0);
			}
		};
	}
	VOID ShowTip(const std::wstring& title, const std::wstring& caption)
	{
		if (!GetHwnd())
			Create();

		this->title = title;
		this->caption = caption;
		for (auto& t : timers)
			t.kill();

		timers[0].set(0, TRUE);
	}
};