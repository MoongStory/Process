/*******************************************************************************
MIT License

Copyright (c) 2023 Moong

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

// https://github.com/MoongStory/Process

#if _MSC_VER > 1000
	#pragma once
#endif

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>
#include <vector>
#include <WTypes.h>

#if _MSC_VER <= 1200
	// Visual Studio 6.0에서만 발생.
	#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the browser information.
#endif

#if _MSC_VER <= 1200
	#ifndef TOKEN_MANDATORY_LABEL
		typedef struct _TOKEN_MANDATORY_LABEL {
			SID_AND_ATTRIBUTES Label;
		} TOKEN_MANDATORY_LABEL, * PTOKEN_MANDATORY_LABEL;
	#endif

	#ifndef SE_GROUP_INTEGRITY
		#define SE_GROUP_INTEGRITY                 (0x00000020L)
	#endif
#endif

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
				static const int VECTOR_IS_EMPTY = 4;
			}
		}

		namespace INTEGRITY_LEVEL
		{
			static const int LOW = 0;
			static const int MEDIUM = 1;
			static const int HIGH = 2;
			static const int SYSTEM = 3;
		}
	}

	class Process
	{
	public:
	protected:
	private:
		static const std::string INTEGRITY_LEVEL_SID_UNTRUSTED;
		static const std::string INTEGRITY_LEVEL_SID_BELOW_LOW;
		static const std::string INTEGRITY_LEVEL_SID_LOW;
		static const std::string INTEGRITY_LEVEL_SID_MEDIUM_LOW;
		static const std::string INTEGRITY_LEVEL_SID_MEDIUM;
		static const std::string INTEGRITY_LEVEL_SID_HIGH;
		static const std::string INTEGRITY_LEVEL_SID_SYSTEM;


	public:
		/********************************************************************************
		* 간단한 설명 :
		*	작업 관리자에 프로세스가 실행중인지 체크.
		* 파라미터 :
		*	std::string 또는 std::vector<std::string>
		*		"작업 관리자 - 세부 정보"에서 확인 가능한 프로세스 이름.
		*	bool
		*		백그라운드에서 실행중인 프로세스 포함 여부.
		* 리턴 값(int) :
		*	0 - 실행중인 프로세스 있음.
		*	1 - 실행중인 프로세스 없음.
		*	2 - CreateToolhelp32Snapshot 호출 실패.
		*	3 - Process32First 호출 실패.
		* 사용 예 :
		*	IsExistProcess("chrome.exe");
		*	IsExistProcess("msedge.exe", false);
		*********************************************************************************/
		static const int is_exist_process(IN const std::string process_name, const bool include_background_process = true);
		static const int is_exist_process(IN const std::vector<std::string> process_name_list, const bool include_background_process = true);

		static const int terminate_process_normal(IN const std::string& process_name);
		static const int terminate_process_normal(IN std::vector<std::string>& process_name_list);
		static const int terminate_process(IN std::vector<std::string>& process_name_list);
		static const int terminate_process(IN const std::string& process_name);
		static const bool terminate_process(IN const DWORD process_id);
		static const bool terminate_process(IN const HWND hwnd);

		static const int create_process_with_integrity_level(IN const int integrity_level, IN const std::string path_process, IN const std::string param = "");

		static const bool check_duplicate_execution();

		static const std::vector<DWORD> get_process_id(IN const std::string process_name = "", IN const bool include_background_process = true);
		static const std::string get_path(IN DWORD process_id = 0);
	private:
		static const int send_close_message_to_process_with_same_pid(IN const DWORD pid);
		static const bool is_background_process(IN const DWORD pid);
	};
}

#endif _PROCESS_H_
