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

			//break; // 주석 해제할 경우 동일한 이름의 프로세스가 2개 이상 실행중일 경우 하나만 종료됨.
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
	// 윈도우의 상태나 스타일에 따라 처리도 가능.
	// 샘플 코드로 남겨놓음.
	//BOOL isVisible = IsWindowVisible(hwnd);
	//DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	//BOOL isAppWindow = (exStyle & WS_EX_APPWINDOW);
	//BOOL isToolWindow = (exStyle & WS_EX_TOOLWINDOW);
	// 계산기 윈도우 떠있을때랑 최소화 되있을때랑 리턴 결과가 다름. 부모 윈도우가 있다 없다 함.
	// 파라미터는 GW_OWNER인데 실제로는 parent 값을 얻어옴.
	//BOOL isOwned = GetWindow(hwnd, GW_OWNER) ? TRUE : FALSE;

	DWORD process_id = 0;

	GetWindowThreadProcessId(hwnd, &process_id);

	// GetParent() 함수를 통해 부모 윈도우를 체크하면 계산기의 경우 윈도우 떠있을때랑 최소화 되있을때랑 리턴 결과가 다름.
	// 트리 구조가 바뀌면서 부모 윈도우가 있다 없다 함.
	// 따라서 "GetParent(hwnd) == NULL" 이런식으로 체크할 경우 계산기 윈도우가 떠있는지 최소화 상태인지에 따라 결과가 다름.
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
				// FIXME: TerminatePrcess는 권장하지 않는 방법, 안전한 종료 함수로 바꿀 것
				::TerminateProcess(hGetFromPID, 0); // TerminateProcess 예외처리 생략

				CloseHandle(hGetFromPID);
			}

			//break; // 주석 해제할 경우 동일한 이름의 프로세스가 2개 이상 실행중일 경우 하나만 종료됨.
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
		// FIXME: TerminatePrcess는 권장하지 않는 방법, 안전한 종료 함수로 바꿀 것
		::TerminateProcess(hGetFromPID, 0); // TerminateProcess 예외처리 생략

		CloseHandle(hGetFromPID);
	}
	else
	{
		return false;
	}

	return true;
}