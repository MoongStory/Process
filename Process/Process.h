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
				static const int CAN_NOT_FIND_PROCESS = 1;
				static const int ERROR_CREATE_TOOLHELP32_SNAPSHOT = 2;
				static const int ERROR_PROCESS32_FIRST = 3;
			}
		}
	}
	
	class Process
	{
	public:
		static const int IsExistProcess(const std::string process_name);
		static const int TerminateProcessNormal(const std::string process_name);
		static const int TerminateProcessNormal(std::vector<std::string>& process_name_list);
		static const int TerminateProcess(std::vector<std::string>& process_name_list);
		static const int TerminateProcess(const std::string file_name);
		static const bool TerminateProcess(HWND hwnd);
	private:
		static const int SendCloseMessageToProcessWithSamePID(const DWORD pid);
	};
}

#endif _PROCESS_H_