// https://github.com/MoongStory/Process

#if _MSC_VER > 1000
	#pragma once
#endif

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>
#include <vector>
#include <Windows.h>

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
		* ������ ���� :
		*	�۾� �����ڿ� ���μ����� ���������� üũ.
		* �Ķ���� :
		*	std::string �Ǵ� std::vector<std::string>
		*		"�۾� ������ - ���� ����"���� Ȯ�� ������ ���μ��� �̸�.
		*	bool
		*		��׶��忡�� �������� ���μ��� ���� ����.
		* ���� ��(int) :
		*	0 - �������� ���μ��� ����.
		*	1 - �������� ���μ��� ����.
		*	2 - CreateToolhelp32Snapshot ȣ�� ����.
		*	3 - Process32First ȣ�� ����.
		* ��� �� :
		*	IsExistProcess("chrome.exe");
		*	IsExistProcess("msedge.exe", false);
		*********************************************************************************/
		static const int IsExistProcess(IN const std::string process_name, const bool include_background_process = true);
		static const int IsExistProcess(IN const std::vector<std::string> process_name_list, const bool include_background_process = true);

		static const int TerminateProcessNormal(IN const std::string process_name);
		static const int TerminateProcessNormal(IN std::vector<std::string>& process_name_list);
		static const int TerminateProcess(IN std::vector<std::string>& process_name_list);
		static const int TerminateProcess(IN const std::string file_name);
		static const bool TerminateProcess(IN HWND hwnd);

		static const int CreateProcessWithIntegrityLevel(IN const int integrity_level, IN const std::string path_process, IN const std::string param = "");

		static const bool CheckDuplicateExecution();
	private:
		static const int SendCloseMessageToProcessWithSamePID(IN const DWORD pid);
		static const bool IsBackgroundProcess(IN const DWORD pid);
	};
}

#endif _PROCESS_H_