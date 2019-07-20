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

class MemoryHelper
{
public:
	struct Table
	{
		PVOID pAddress = nullptr;
		DWORD64 origin = NULL;
		DWORD64 cheat = NULL;
		Table() = default;
		Table(PVOID pAddress, DWORD64 origin, DWORD64 cheat) :
			pAddress(pAddress), origin(origin), cheat(cheat) {}
	};
private:
	std::vector<Table> table;

public:
	VOID append(const Table& t)
	{
		table.push_back(t);
	}
	VOID append(const std::initializer_list<Table>& list)
	{
		for (const auto& t : list)
			table.push_back(t);
	}
	// -1 - invalid, 0 - origin, 1 - cheat
	int Check(HANDLE hProcess, int idx)
	{
		const auto& t = table[idx];
		DWORD64 val = NULL;
		SIZE_T read = NULL;
		ReadProcessMemory(hProcess, t.pAddress, &val,
			sizeof(DWORD64), &read);
		if (read != sizeof(DWORD64))
			throw std::runtime_error("fail to read memory!");

		if (val == t.origin)
			return 0;
		else if (val == t.cheat)
			return 1;
		else
			return -1;
	}
private:
	BOOL WriteMemory(HANDLE hProcess, PVOID pWrite, DWORD64 val)
	{
		SIZE_T written = NULL;
		WriteProcessMemory(hProcess, pWrite, &val, sizeof(val), &written);
		if (written != sizeof(val))
			throw std::runtime_error("fail to write memory!");
		return TRUE;
	}
public:
	VOID Apply(HANDLE hProcess, int idx)
	{
		const auto& t = table[idx];
		WriteMemory(hProcess, t.pAddress, t.cheat);
	}
	VOID Revoke(HANDLE hProcess, int idx)
	{
		const auto& t = table[idx];
		WriteMemory(hProcess, t.pAddress, t.origin);
	}

public:
	MemoryHelper() = default;
	MemoryHelper(const std::initializer_list<Table>& list)
	{
		append(list);
	}
};