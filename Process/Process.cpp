#include "Process.h"

#include <tlhelp32.h>
#include <algorithm>

const int MOONG::Process::IsExistProcess(const std::string process_name)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return MOONG::PROCESS::RETURN::FAILURE::ERROR_CREATE_TOOLHELP32_SNAPSHOT;
	}

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return MOONG::PROCESS::RETURN::FAILURE::ERROR_PROCESS32_FIRST;
	}

#if _MSC_VER > 1200
	char exe_file[MAX_PATH] = { 0 };
	size_t convertedChars = 0;

	do {
		wcstombs_s(&convertedChars, exe_file, MAX_PATH, pe32.szExeFile, _TRUNCATE);

		if (_stricmp(process_name.c_str(), exe_file) == 0)
		{
#else
	do {
		if (_stricmp(process_name.c_str(), pe32.szExeFile) == 0)
		{
#endif
			CloseHandle(hProcessSnap);

			return MOONG::PROCESS::RETURN::FIND_PROCESS;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return MOONG::PROCESS::RETURN::FAILURE::CAN_NOT_FIND_PROCESS;
}

const int MOONG::Process::TerminateProcessNormal(const std::string process_name)
{
	std::vector<std::string> process_name_list;
	
	process_name_list.push_back(process_name);

	return MOONG::Process::TerminateProcessNormal(process_name_list);
}

const int MOONG::Process::TerminateProcessNormal(std::vector<std::string>& process_name_list)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return MOONG::PROCESS::RETURN::FAILURE::ERROR_CREATE_TOOLHELP32_SNAPSHOT;
	}

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return MOONG::PROCESS::RETURN::FAILURE::ERROR_PROCESS32_FIRST;
	}

	bool is_process_name_same = false;

#if _MSC_VER > 1200
	char exe_file[MAX_PATH] = { 0 };
	size_t convertedChars = 0;

	do {
		wcstombs_s(&convertedChars, exe_file, MAX_PATH, pe32.szExeFile, _TRUNCATE);

		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			if (_stricmp(process_name_list[i].c_str(), exe_file) == 0)
			{
#else
	do {
		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			if (_stricmp(process_name_list[i].c_str(), pe32.szExeFile) == 0)
			{
#endif
				is_process_name_same = true;

				break;
			}
		}

		if (is_process_name_same)
		{
			is_process_name_same = false;

			MOONG::Process::SendCloseMessageToProcessWithSamePID(pe32.th32ProcessID);

			//break; // �ּ� ������ ��� ������ �̸��� ���μ����� 2�� �̻� �������� ��� �ϳ��� �����.
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return EXIT_SUCCESS;
}

BOOL CALLBACK FindProcessToReceiveCloseMessage(HWND hwnd, LPARAM lParam);

const int MOONG::Process::SendCloseMessageToProcessWithSamePID(const DWORD pid)
{
	EnumWindows(FindProcessToReceiveCloseMessage, (LPARAM)pid);
	
	return EXIT_SUCCESS;
}

BOOL CALLBACK FindProcessToReceiveCloseMessage(HWND hwnd, LPARAM lParam)
{
	// �������� ���³� ��Ÿ�Ͽ� ���� ó���� ����.
	// ���� �ڵ�� ���ܳ���.
	//BOOL isVisible = IsWindowVisible(hwnd);
	//DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	//BOOL isAppWindow = (exStyle & WS_EX_APPWINDOW);
	//BOOL isToolWindow = (exStyle & WS_EX_TOOLWINDOW);
	// ���� ������ ���������� �ּ�ȭ ���������� ���� ����� �ٸ�. �θ� �����찡 �ִ� ���� ��.
	// �Ķ���ʹ� GW_OWNER�ε� �����δ� parent ���� ����.
	//BOOL isOwned = GetWindow(hwnd, GW_OWNER) ? TRUE : FALSE;

	DWORD process_id = 0;

	GetWindowThreadProcessId(hwnd, &process_id);

	// GetParent() �Լ��� ���� �θ� �����츦 üũ�ϸ� ������ ��� ������ ���������� �ּ�ȭ ���������� ���� ����� �ٸ�.
	// Ʈ�� ������ �ٲ�鼭 �θ� �����찡 �ִ� ���� ��.
	// ���� "GetParent(hwnd) == NULL" �̷������� üũ�� ��� ���� �����찡 ���ִ��� �ּ�ȭ ���������� ���� ����� �ٸ�.
	if (process_id == lParam)
	{
		PostMessage(hwnd, WM_CLOSE, NULL, NULL);
		PostMessage(hwnd, WM_DESTROY, NULL, NULL);
		PostMessage(hwnd, WM_QUIT, NULL, NULL);
	}

	return TRUE;
}

const int MOONG::Process::TerminateProcess(std::vector<std::string>& process_name_list)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return MOONG::PROCESS::RETURN::FAILURE::ERROR_CREATE_TOOLHELP32_SNAPSHOT;
	}

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return MOONG::PROCESS::RETURN::FAILURE::ERROR_PROCESS32_FIRST;
	}

	bool is_process_name_same = false;
	
#if _MSC_VER > 1200
	char exe_file[MAX_PATH] = { 0 };
	size_t convertedChars = 0;

	do {
		wcstombs_s(&convertedChars, exe_file, MAX_PATH, pe32.szExeFile, _TRUNCATE);

		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			if (_stricmp(process_name_list[i].c_str(), exe_file) == 0)
#else
	do {
		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			if (_stricmp(process_name_list[i].c_str(), pe32.szExeFile) == 0)
#endif
			{
				is_process_name_same = true;

				break;
			}
		}

		if (is_process_name_same)
		{
			is_process_name_same = false;

			HANDLE hGetFromPID = OpenProcess(MAXIMUM_ALLOWED, false, pe32.th32ProcessID);

			if (hGetFromPID)
			{
				// FIXME: TerminatePrcess�� �������� �ʴ� ���, ������ ���� �Լ��� �ٲ� ��
				::TerminateProcess(hGetFromPID, 0); // TerminateProcess ����ó�� ����

				CloseHandle(hGetFromPID);
			}

			//break; // �ּ� ������ ��� ������ �̸��� ���μ����� 2�� �̻� �������� ��� �ϳ��� �����.
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return EXIT_SUCCESS;
}

const int MOONG::Process::TerminateProcess(const std::string file_name)
{
	std::vector<std::string> process_name_list;

	process_name_list.push_back(file_name);

	return MOONG::Process::TerminateProcess(process_name_list);
}

const bool MOONG::Process::TerminateProcess(HWND hwnd)
{
	if (hwnd == NULL)
	{
		return false;
	}

	DWORD dwPid = 0;

	::GetWindowThreadProcessId(hwnd, &dwPid);

	HANDLE hGetFromPID = ::OpenProcess(MAXIMUM_ALLOWED, false, dwPid);

	if (hGetFromPID)
	{
		// FIXME: TerminatePrcess�� �������� �ʴ� ���, ������ ���� �Լ��� �ٲ� ��
		::TerminateProcess(hGetFromPID, 0); // TerminateProcess ����ó�� ����

		CloseHandle(hGetFromPID);
	}
	else
	{
		return false;
	}

	return true;
}