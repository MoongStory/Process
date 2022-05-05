#include "Process.h"

#include <tlhelp32.h>
#include <algorithm>

int MOONG::Process::IsExistProcess(const std::string process_name)
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

int MOONG::Process::TerminateProcessNormal(const std::string process_name)
{
	std::vector<std::string> process_name_list;
	
	process_name_list.push_back(process_name);

	return MOONG::Process::TerminateProcessNormal(process_name_list);
}

int MOONG::Process::TerminateProcessNormal(std::vector<std::string>& process_name_list)
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

			// TODO: ProcessID�� �ٷ� HWND�� �����ϴ� ��� ã�ƺ���.
			MOONG::Process::SendTerminateMessageToProcessWithSamePID(GetDesktopWindow(), pe32.th32ProcessID);

			//break; // �ּ� ������ ��� ������ �̸��� ���μ����� 2�� �̻� �������� ��� �ϳ��� �����.
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return EXIT_SUCCESS;
}

int MOONG::Process::SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid)
{
	std::vector<HWND> startHWND;

	startHWND.push_back(hWnd);

	return MOONG::Process::SendTerminateMessageToProcessWithSamePID(startHWND, pid);
}

int MOONG::Process::SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, DWORD pid)
{
	HWND hWnd = NULL;
	TCHAR szCaption[1025] = { 0 };
	DWORD dwProcId = 0;
	std::vector<HWND> hWndListNextDept;

	for (size_t i = 0; i < hWndList.size(); i++)
	{
		hWnd = ::GetTopWindow(hWndList[i]);

		do
		{
			::GetWindowText(hWnd, szCaption, 1024);
			::GetWindowThreadProcessId(hWnd, &dwProcId);

			if (szCaption[0] != '\0' && ::IsWindowVisible(hWnd))
			{
				if (dwProcId == pid)
				{
					// ���μ��� ���� �� WM_CLOSE, WM_DESTROY, WM_QUIT �޽����� ������� �߻��Ѵ�.
					PostMessage(hWnd, WM_CLOSE, NULL, NULL);
					PostMessage(hWnd, WM_DESTROY, NULL, NULL);
					PostMessage(hWnd, WM_QUIT, NULL, NULL);

					// ũ�� ���� ��� â�� ������ ���� ����� ��� Ư�� �ڵ��� ������ �ڽ� ������ ������ �ڵ��� �����ϹǷ�
					// PID�� ��ġ�ϴ� ������ �ڵ��� ã�Ҵ��� ���� ���� ���迡 �ִ� ��� �ڵ���� Ž���Ѵ�.
					// �θ� ������ A, �ڽ� ������ B, ���� ������ C��� �ϰ� Target�� �Ǵ� �����츦 T��� �����Ѵٸ�
					// ũ�� �����찡 �������� �� C ������ ���� ����� T�� ������ ����.
					continue;
				}

				// ������ �⺻ ������ ��� ����ȭ�鿡 �����찡 �� ������ ������ ������ ������ UI �����찡 �־� �ڽĳ�� Ž���� �ʿ�.
				// Ư���Ѱ� �ּ�ȭ ��ư�� ���� â�� �ּ�ȭ ��Ű�� UI �����찡 ����ȭ�� �ڽ� ������ �ٲ�.
				// �θ� ������ A, �ڽ� ������ B, ���� ������ C��� �ϰ� Target�� �Ǵ� �����츦 T��� �����Ѵٸ�
				// ���� �����찡 ����ȭ�鿡 ������ ��� A - B - T�̰�, �ּ�ȭ �� ���¿����� A - T ���°� ��.
				if (GetWindow(hWnd, GW_CHILD))
				{
					hWndListNextDept.push_back(hWnd);
				}
			}
		} while (hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT));
	}

	if (hWndListNextDept.size() > 0)
	{
		return SendTerminateMessageToProcessWithSamePID(hWndListNextDept, pid);
	}

	return EXIT_SUCCESS;
}

int MOONG::Process::TerminateProcess(std::vector<std::string>& process_name_list)
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

int MOONG::Process::TerminateProcess(const std::string file_name)
{
	std::vector<std::string> process_name_list;

	process_name_list.push_back(file_name);

	return MOONG::Process::TerminateProcess(process_name_list);
}

bool MOONG::Process::TerminateProcess(HWND hwnd)
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