// https://github.com/MoongStory/Process

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>
#include <atlstr.h>
#include <vector>
#include <Windows.h>

namespace MOONG
{
	namespace PROCESS
	{
		// TODO: return (숫자) enum 추가해서 명시적으로 수정.
		namespace RETURN_CODE
		{
			enum RETURN_CODE
			{
				FIND_PROCESS = 0,
				CAN_NOT_FIND_PROCESS = 1,
				ERROR_CREATE_TOOLHELP32_SNAPSHOT = 2,
				ERROR_PROCESS32_FIRST = 3
			};
		}

		class Process
		{
		public:
			int IsExistProcess(CStringA process_name);
			int TerminateProcessNormal(CStringA process_name);
			int TerminateProcessNormal(std::vector<CStringA>& process_name_list);
			int TerminateProcess(std::vector<CStringA>& process_name_list);
			int TerminateProcess(CStringA file_name);
			BOOL TerminateProcess(HWND hwnd);
		private:
			int SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid);
			int SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, DWORD pid);
		};
	}
}

#endif _PROCESS_H_