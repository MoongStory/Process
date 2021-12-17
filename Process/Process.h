// https://github.com/MoongStory/Process

#pragma once

#include <iostream>
#include <vector>
#include <Windows.h>

namespace Moong_Process
{
	class Process
	{
	public:
		int IsExistProcess(std::string strProcessName);
		int TerminateProcessNormal(const std::string& processName);
		int TerminateProcessNormal(const std::vector<std::string>& processNameList);
		int TerminateProcess(const std::vector<std::string>& processNameList);
		int TerminateProcess(const char* const file_name);
		int TerminateProcess(const std::string file_name);
		BOOL TerminateProcess(HWND hwnd);
	private:
		int SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid);
		int SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, DWORD pid);
	};
}