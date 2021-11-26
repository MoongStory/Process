#pragma once

#include <iostream>
#include <vector>
#include <Windows.h>

namespace Moong_TerminateProcess
{
	class TerminateProcess
	{
	public:
		int TerminateProcessNormal(const std::string& processName);
		int TerminateProcessNormal(const std::vector<std::string>& processNameList);
	private:
		int SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid);
		int SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, DWORD pid);
	};
}