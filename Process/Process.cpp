#pragma region ���μ��� ų �� �� ����
// TODO: ������ ������ ã�� ������� ����...
//		PID���� HWND�� ��ų�
//		HANDLE���� HWND�� ��ų�
//		(HANDLE hGetFromPID = OpenProcess(MAXIMUM_ALLOWED, FALSE, pe32.th32ProcessID)
//
//		EnumWindows()
//		https://stackoverflow.com/questions/11711417/get-hwnd-by-process-id-c
#pragma endregion ���μ��� ų �� �� ����


#include "Process.h"

#include <tlhelp32.h>
#include <tchar.h>
#include <algorithm>

BOOL MOONG::PROCESS::Process::IsExistProcess(const CStringA process_name) const
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return MOONG::PROCESS::RETURN_CODE::ERROR_CREATE_TOOLHELP32_SNAPSHOT;
	}

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return MOONG::PROCESS::RETURN_CODE::ERROR_PROCESS32_FIRST;
	}

	do {
		const CStringA exe_file(pe32.szExeFile);

		if(process_name.CompareNoCase(exe_file) == 0)
		{
			CloseHandle(hProcessSnap);

			return MOONG::PROCESS::RETURN_CODE::FIND_PROCESS;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return MOONG::PROCESS::RETURN_CODE::CAN_NOT_FIND_PROCESS;
}

int MOONG::PROCESS::Process::TerminateProcessNormal(const CStringA process_name) const
{
	std::vector<CStringA> process_name_list;
	
	process_name_list.push_back(process_name);

	return this->TerminateProcessNormal(process_name_list);
}

int MOONG::PROCESS::Process::TerminateProcessNormal(std::vector<CStringA>& process_name_list) const
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return MOONG::PROCESS::RETURN_CODE::ERROR_CREATE_TOOLHELP32_SNAPSHOT;
	}

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return MOONG::PROCESS::RETURN_CODE::ERROR_PROCESS32_FIRST;
	}

	bool is_process_name_same = false;

	do {
		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			const CStringA exe_file(pe32.szExeFile);

			if(process_name_list[i].CompareNoCase(exe_file) == 0)
			{
				is_process_name_same = true;

				break;
			}
		}

		if (is_process_name_same)
		{
			is_process_name_same = false;

			// TODO: ProcessID�� �ٷ� HWND�� �����ϴ� ��� ã�ƺ���.
			this->SendTerminateMessageToProcessWithSamePID(GetDesktopWindow(), pe32.th32ProcessID);

			break;

			//break; // �ּ� ������ ��� ������ �̸��� ���μ����� 2�� �̻� �������� ��� �ϳ��� �����.
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return EXIT_SUCCESS;
}

int MOONG::PROCESS::Process::SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid) const
{
	std::vector<HWND> startHWND;

	startHWND.push_back(hWnd);

	return this->SendTerminateMessageToProcessWithSamePID(startHWND, pid);
}

int MOONG::PROCESS::Process::SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, DWORD pid) const
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

int MOONG::PROCESS::Process::TerminateProcess(std::vector<CStringA>& process_name_list) const
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return MOONG::PROCESS::RETURN_CODE::ERROR_CREATE_TOOLHELP32_SNAPSHOT;
	}

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return MOONG::PROCESS::RETURN_CODE::ERROR_PROCESS32_FIRST;
	}

	bool is_process_name_same = false;

	do {
		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			const CStringA exe_file(pe32.szExeFile);

			if(process_name_list[i].CompareNoCase(exe_file) == 0)
			{
				is_process_name_same = true;

				break;
			}
		}

		if (is_process_name_same)
		{
			is_process_name_same = false;

			HANDLE hGetFromPID = OpenProcess(MAXIMUM_ALLOWED, FALSE, pe32.th32ProcessID);

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

int MOONG::PROCESS::Process::TerminateProcess(const CStringA file_name) const
{
	std::vector<CStringA> process_name_list;

	process_name_list.push_back(file_name);

	return this->TerminateProcess(process_name_list);
}

BOOL MOONG::PROCESS::Process::TerminateProcess(HWND hwnd) const
{
	if (hwnd == NULL)
	{
		return FALSE;
	}

	DWORD dwPid = 0;

	::GetWindowThreadProcessId(hwnd, &dwPid);

	HANDLE hGetFromPID = ::OpenProcess(MAXIMUM_ALLOWED, FALSE, dwPid);

	if (hGetFromPID)
	{
		// FIXME: TerminatePrcess�� �������� �ʴ� ���, ������ ���� �Լ��� �ٲ� ��
		::TerminateProcess(hGetFromPID, 0); // TerminateProcess ����ó�� ����

		CloseHandle(hGetFromPID);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}