// https://github.com/MoongStory/Process

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>
#include <vector>
#include <Windows.h>

namespace MOONG
{
	namespace PROCESS
	{
		namespace RETURN
		{
			static const int SUCCESS = 0;
			static const int FIND_PROCESS = 0;
			
			namespace FAILURE
			{
				enum FAILURE
				{
					CAN_NOT_FIND_PROCESS = 1,
					ERROR_CREATE_TOOLHELP32_SNAPSHOT = 2,
					ERROR_PROCESS32_FIRST = 3
				};
			}
		}

		class Process
		{
		public:
			static int IsExistProcess(const std::string process_name);
			static int TerminateProcessNormal(const std::string process_name);
			static int TerminateProcessNormal(std::vector<std::string>& process_name_list);
			static int TerminateProcess(std::vector<std::string>& process_name_list);
			static int TerminateProcess(const std::string file_name);
			static BOOL TerminateProcess(HWND hwnd);
		private:
			static int SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid);
			static int SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, DWORD pid);
		};
	}
}

#endif _PROCESS_H_