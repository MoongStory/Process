#include "Process.h"

#include <tlhelp32.h>
#include <algorithm>
#include <strsafe.h>

#include <sddl.h> // ConvertStringSidToSid ȣ�� �� �ʿ�.

// https://github.com/MoongStory/FileInfo
#include "../../FileInfo/FileInfo/FileInfo.h"

const std::string MOONG::Process::INTEGRITY_LEVEL_SID_UNTRUSTED		= "S-1-16-0";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_BELOW_LOW		= "S-1-16-2048";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_LOW			= "S-1-16-4096";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_MEDIUM_LOW	= "S-1-16-6144";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_MEDIUM		= "S-1-16-8192";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_HIGH			= "S-1-16-12288";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_SYSTEM		= "S-1-16-16384";

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
	if (process_id == (DWORD)lParam)
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

const int MOONG::Process::CreateProcessWithIntegrityLevel(const int integrity_level, const std::string path_process, const std::string param/* = ""*/)
{
	// Set integrity SID
	std::string integrity_sid;

	switch (integrity_level)
	{
	case MOONG::PROCESS::INTEGRITY_LEVEL::LOW:
		integrity_sid = INTEGRITY_LEVEL_SID_LOW;
		break;
	case MOONG::PROCESS::INTEGRITY_LEVEL::MEDIUM:
		integrity_sid = INTEGRITY_LEVEL_SID_MEDIUM;
		break;
	case MOONG::PROCESS::INTEGRITY_LEVEL::HIGH:
		integrity_sid = INTEGRITY_LEVEL_SID_HIGH;
		break;
	case MOONG::PROCESS::INTEGRITY_LEVEL::SYSTEM:
		integrity_sid = INTEGRITY_LEVEL_SID_SYSTEM;
		break;
	default:
		integrity_sid = INTEGRITY_LEVEL_SID_LOW;
		break;
	}

	BOOL					fRet = FALSE;
	HANDLE					hToken = NULL;
	HANDLE					hNewToken = NULL;
	PSID					pIntegritySid = NULL;
	TOKEN_MANDATORY_LABEL	TIL = { 0 };
	PROCESS_INFORMATION		ProcInfo = { 0 };
	STARTUPINFOA			StartupInfo = { 0 };

	fRet = OpenProcessToken(GetCurrentProcess(),
		TOKEN_DUPLICATE |
		TOKEN_ADJUST_DEFAULT |
		TOKEN_QUERY |
		TOKEN_ASSIGN_PRIMARY,
		&hToken);
	
	if (!fRet)
	{
		if (hToken != NULL)
		{
			CloseHandle(hToken);
		}
	}

	fRet = DuplicateTokenEx(hToken,
		0,
		NULL,
		SecurityImpersonation,
		TokenPrimary,
		&hNewToken);

	if (!fRet)
	{
		if (hNewToken != NULL)
		{
			CloseHandle(hNewToken);
		}
		
		if (hToken != NULL)
		{
			CloseHandle(hToken);
	}
	}

#if _MSC_VER <= 1200
	HMODULE hModule = LoadLibraryA("Advapi32.dll");
	if(hModule == NULL)
	{
		// ����.
	}
	else
	{
		typedef BOOL (*Type_ConvertStringSidToSidA)(LPCSTR StringSid, PSID *Sid);
		Type_ConvertStringSidToSidA ConvertStringSidToSidA = (Type_ConvertStringSidToSidA)GetProcAddress(hModule, "ConvertStringSidToSidA");

		fRet = ConvertStringSidToSidA(integrity_sid.c_str(), &pIntegritySid);
	}

	if(hModule != NULL)
	{
		FreeLibrary(hModule);
	}
#else
	fRet = ConvertStringSidToSidA(integrity_sid.c_str(), &pIntegritySid);
#endif

	if (!fRet)
	{
		LocalFree(pIntegritySid);
		
		if (hNewToken != NULL)
		{
			CloseHandle(hNewToken);
		}
		
		if (hToken != NULL)
		{
			CloseHandle(hToken);
	}
	}

	TIL.Label.Attributes = SE_GROUP_INTEGRITY;
	TIL.Label.Sid = pIntegritySid;

	//
	// Set the process integrity level
	//

#if _MSC_VER <= 1200
	// TokenIntegrityLevel�� _TOKEN_INFORMATION_CLASS���� 25�� ���ǵǾ� �ִµ� Visual Studio 6.0������ ������ ���� 18�� ���ǵ� ���� �������̴�.
	fRet = SetTokenInformation(hNewToken, (enum _TOKEN_INFORMATION_CLASS)25 /*TokenIntegrityLevel*/, &TIL, sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(pIntegritySid));
#else
	fRet = SetTokenInformation(hNewToken, TokenIntegrityLevel, &TIL, sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(pIntegritySid));
#endif

	if (!fRet)
	{
		LocalFree(pIntegritySid);
		
		if (hNewToken != NULL)
		{
			CloseHandle(hNewToken);
		}
		
		if (hToken != NULL)
		{
			CloseHandle(hToken);
	}
	}

	//
	// Create the new process at integrity level
	//

	if (param.empty() == true)
	{
		fRet = CreateProcessAsUserA(hNewToken,
			path_process.c_str(),
			NULL,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&StartupInfo,
			&ProcInfo);
	}
	else
	{
		std::string path_process_with_param;
		path_process_with_param = path_process;
		path_process_with_param += " ";
		path_process_with_param += param;

		// 2��° �Ķ���� ���� ��� �ѱ��, 3��° �Ķ���� "/s" �Ѱ��� �� �� ��.
		// ���� ��� ���� " /s" ���̰� 2��° �Ķ���ͷ� �Ѱ��� �� �� ��.
		// ���� ��� ���� " /s" ���̰� 3��° �Ķ���ͷ� �Ѱ��� �� ������ Ȯ��.
		fRet = CreateProcessAsUserA(hNewToken,
			path_process_with_param.c_str(),
			NULL,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&StartupInfo,
			&ProcInfo);
	}

	if (ProcInfo.hProcess != NULL)
	{
		CloseHandle(ProcInfo.hProcess);
	}

	if (ProcInfo.hThread != NULL)
	{
		CloseHandle(ProcInfo.hThread);
	}

	LocalFree(pIntegritySid);

	if (hNewToken != NULL)
	{
		CloseHandle(hNewToken);
	}

	if (hToken != NULL)
	{
		CloseHandle(hToken);
	}

	return EXIT_SUCCESS;
}

const bool MOONG::Process::CheckDuplicateExecution()
{
	char event_name[256] = { 0 };
	
	StringCbPrintfA(event_name, sizeof(event_name), "%s_%s", MOONG::FileInfo::GetFolderName().c_str(), MOONG::FileInfo::GetFileNameWithoutFileExtension().c_str());

	HANDLE duplicateCheck = CreateEventA(NULL, FALSE, FALSE, event_name);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return true;
	}

	return false;
}