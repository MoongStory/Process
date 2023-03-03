#include "Process.h"

#include "../../ConvertDataType/ConvertDataType/ConvertDataType.h"
#include "../../StringTool/StringTool/StringTool.h"

#include <tlhelp32.h>
#include <algorithm>
#include <strsafe.h>

#include <sddl.h> // ConvertStringSidToSid ȣ�� �� �ʿ�.

#include "../../FileInformation/FileInformation/FileInformation.h"

#include <psapi.h>

#if _MSC_VER <= 1200
	#pragma comment(lib, "User32.lib")
	#pragma comment(lib, "Advapi32.lib")
#endif

const std::string MOONG::Process::INTEGRITY_LEVEL_SID_UNTRUSTED		= "S-1-16-0";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_BELOW_LOW		= "S-1-16-2048";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_LOW			= "S-1-16-4096";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_MEDIUM_LOW	= "S-1-16-6144";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_MEDIUM		= "S-1-16-8192";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_HIGH			= "S-1-16-12288";
const std::string MOONG::Process::INTEGRITY_LEVEL_SID_SYSTEM		= "S-1-16-16384";

BOOL CALLBACK FindProcessToReceiveCloseMessage(HWND hwnd, LPARAM lParam);
BOOL CALLBACK CheckBackgroundProcess(HWND hwnd, LPARAM lParam);

const int MOONG::Process::IsExistProcess(IN const std::string process_name, const bool include_background_process/* = true*/)
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
	
	do {
#if _MSC_VER > 1200
		if (MOONG::StringTool::compare(process_name, MOONG::ConvertDataType::wstring_to_string(pe32.szExeFile), true) == 0)
#else
		if (MOONG::StringTool::compare(process_name, pe32.szExeFile, true) == 0)
#endif
		{
			if(false == include_background_process)
			{
				if(true == IsBackgroundProcess(pe32.th32ProcessID))
				{
					continue;
				}
				else
				{
					CloseHandle(hProcessSnap);
					
					return MOONG::PROCESS::RETURN::FIND_PROCESS;
				}
			}
			else
			{
				CloseHandle(hProcessSnap);
				
				return MOONG::PROCESS::RETURN::FIND_PROCESS;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return MOONG::PROCESS::RETURN::FAILURE::CAN_NOT_FIND_PROCESS;
}

const int MOONG::Process::IsExistProcess(IN const std::vector<std::string> process_name_list, const bool include_background_process/* = true*/)
{
	if(process_name_list.size() <= 0)
	{
		return MOONG::PROCESS::RETURN::FAILURE::VECTOR_IS_EMPTY;
	}

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
	
	do {
		for(size_t i = 0; i < process_name_list.size(); i++)
		{
#if _MSC_VER > 1200
			if (MOONG::StringTool::compare(process_name_list[i], MOONG::ConvertDataType::wstring_to_string(pe32.szExeFile), true) == 0)
#else
			if (MOONG::StringTool::compare(process_name_list[i], pe32.szExeFile, true) == 0)
#endif
			{
				if(false == include_background_process)
				{
					if(true == IsBackgroundProcess(pe32.th32ProcessID))
					{
						continue;
					}
					else
					{
						CloseHandle(hProcessSnap);
						
						return MOONG::PROCESS::RETURN::FIND_PROCESS;
					}
				}
				else
				{
					CloseHandle(hProcessSnap);
					
					return MOONG::PROCESS::RETURN::FIND_PROCESS;
				}
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));
	
	CloseHandle(hProcessSnap);
			
	return MOONG::PROCESS::RETURN::FAILURE::CAN_NOT_FIND_PROCESS;
}

const int MOONG::Process::TerminateProcessNormal(IN const std::string process_name)
{
	std::vector<std::string> process_name_list;
	
	process_name_list.push_back(process_name);

	return MOONG::Process::TerminateProcessNormal(process_name_list);
}

const int MOONG::Process::TerminateProcessNormal(IN std::vector<std::string>& process_name_list)
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
	do {
		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			if (_stricmp(process_name_list[i].c_str(), MOONG::ConvertDataType::wstring_to_string(pe32.szExeFile).c_str()) == 0)
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

const int MOONG::Process::SendCloseMessageToProcessWithSamePID(IN const DWORD pid)
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

const int MOONG::Process::TerminateProcess(IN std::vector<std::string>& process_name_list)
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
	do {
		for (size_t i = 0; i < process_name_list.size(); i++)
		{
			if (_stricmp(process_name_list[i].c_str(), MOONG::ConvertDataType::wstring_to_string(pe32.szExeFile).c_str()) == 0)
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

const int MOONG::Process::TerminateProcess(IN const std::string file_name)
{
	std::vector<std::string> process_name_list;

	process_name_list.push_back(file_name);

	return MOONG::Process::TerminateProcess(process_name_list);
}

const bool MOONG::Process::TerminateProcess(IN HWND hwnd)
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

const int MOONG::Process::CreateProcessWithIntegrityLevel(IN const int integrity_level, IN const std::string path_process, IN const std::string param/* = ""*/)
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
	
	StringCbPrintfA(event_name, sizeof(event_name), "%s_%s", MOONG::FileInformation::GetFolderName().c_str(), MOONG::FileInformation::GetNameWithoutFileExtension().c_str());

	HANDLE duplicateCheck = CreateEventA(NULL, FALSE, FALSE, event_name);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return true;
	}

	return false;
}

const HANDLE MOONG::Process::GetProcessHandle(const std::string process_name/* = ""*/, const bool include_background_process/* = true*/)
{
	if(process_name.length() <= 0)
	{
		return OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
	}

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return INVALID_HANDLE_VALUE;
	}

	HANDLE output = NULL;

	do {
#ifdef _UNICODE
		if (MOONG::StringTool::compare(process_name, MOONG::ConvertDataType::wstring_to_string(pe32.szExeFile), true) == 0)
#else
		if (MOONG::StringTool::compare(process_name, pe32.szExeFile, true) == 0)
#endif
		{
			if(false == include_background_process)
			{
				if(true == IsBackgroundProcess(pe32.th32ProcessID))
				{
					continue;
				}
				else
				{
					CloseHandle(hProcessSnap);

					output = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);

					return output == NULL ? INVALID_HANDLE_VALUE : output;
				}
			}
			else
			{
				CloseHandle(hProcessSnap);

				output = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);

				return output == NULL ? INVALID_HANDLE_VALUE : output;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return INVALID_HANDLE_VALUE;
}

const std::string MOONG::Process::GetPath(const HANDLE param_process_handle/* = NULL*/)
{
	if(param_process_handle == INVALID_HANDLE_VALUE)
	{
		return "INVALID_HANDLE_VALUE";
	}

	HANDLE process_handle = NULL;

	if (param_process_handle == NULL)
	{
		process_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
	}
	else
	{
		process_handle = param_process_handle;
	}

	char file_path[MAX_PATH] = { 0 };
	DWORD buffer_size = sizeof(file_path);

	if (NULL != process_handle)
	{
		if(NULL != GetModuleFileNameExA(process_handle, NULL, file_path, buffer_size))
		{
			// ����.
		}

		CloseHandle(process_handle);
	}

	return file_path;
}

const bool MOONG::Process::IsBackgroundProcess(IN const DWORD pid)
{
	return EnumWindows(CheckBackgroundProcess, (LPARAM)pid) == TRUE ? true : false;
}

BOOL CALLBACK CheckBackgroundProcess(HWND hwnd, LPARAM lParam)
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

	if (process_id == (DWORD)lParam)
	{
		if(TRUE == IsWindowVisible(hwnd))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}