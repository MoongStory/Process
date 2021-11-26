// TODO: ���� �̸�, Ŭ���� �̸� Process�� ����.
// TODO: �� �Ʒ� �Լ��� ����


#pragma region ���μ��� ų �� �� ����
//PID���� HWND�� ��ų�
//HANDLE���� HWND�� ��ų�
//(HANDLE hGetFromPID = OpenProcess(MAXIMUM_ALLOWED, FALSE, pe32.th32ProcessID)
//
//EnumWindows()
//https://stackoverflow.com/questions/11711417/get-hwnd-by-process-id-c
#pragma endregion ���μ��� ų �� �� ����


#include "TerminateProcess.h"

#include <tlhelp32.h>
#include <tchar.h>
#include <algorithm>

int Moong_TerminateProcess::TerminateProcess::TerminateProcessNormal(const std::string& processName)
{
	std::vector<std::string> processNameList;
	processNameList.push_back(processName);

	return this->TerminateProcessNormal(processNameList);
}

int Moong_TerminateProcess::TerminateProcess::TerminateProcessNormal(const std::vector<std::string>& processNameList)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return 1;
	}

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);

		return 2;
	}

	do {
		for (size_t i = 0; i < processNameList.size(); i++)
		{
			std::wstring ws(pe32.szExeFile);
			// https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
			std::string szExeFile(ws.begin(), ws.end());	// TODO: ���⼭ warning �߻�
			if (_stricmp(processNameList[i].c_str(), szExeFile.c_str()) == 0)
			{
				//_tprintf(TEXT("TerminateProcessNormal(), ExeFile[%s], ProcessID[%d]\n"), pe32.szExeFile, pe32.th32ProcessID);

				// TODO: ProcessID�� �ٷ� HWND�� �����ϴ� ��� ã�ƺ���.

				SendTerminateMessageToProcessWithSamePID(GetDesktopWindow(), pe32.th32ProcessID);

				break;
			}
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return 0;
}

int Moong_TerminateProcess::TerminateProcess::SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid)
{
	std::vector<HWND> startHWND;
	startHWND.push_back(hWnd);

	return SendTerminateMessageToProcessWithSamePID(startHWND, pid);
}

int Moong_TerminateProcess::TerminateProcess::SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, DWORD pid)
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
					//_tprintf(TEXT("SendTerminateMessageToProcessWithSamePID(), dwProcId[%d], hWnd[%p], szCaption[%s]\n"), dwProcId, hWnd, szCaption);

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

	return 0;
}











// TODO: �Ʒ� �Լ��� �߰��ؼ� Ŭ���� �ۼ�
//int CGlobalUtil::TerminateProcess(const std::vector<std::string>& processNameList)
//{
//	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
//
//	if (hProcessSnap == INVALID_HANDLE_VALUE)
//	{
//		return 1;
//	}
//
//	PROCESSENTRY32 pe32 = { 0 };
//	pe32.dwSize = sizeof(PROCESSENTRY32);
//
//	if (!Process32First(hProcessSnap, &pe32))
//	{
//		CloseHandle(hProcessSnap);
//
//		return 2;
//	}
//
//	bool is_process_name_same = false;
//	do {
//		for (size_t i = 0; i < processNameList.size(); i++)
//		{
//			if (_stricmp(processNameList[i].c_str(), pe32.szExeFile) == 0)
//			{
//				is_process_name_same = true;
//
//				break;
//			}
//		}
//
//		if (is_process_name_same)
//		{
//			is_process_name_same = false;
//			HANDLE hGetFromPID = OpenProcess(MAXIMUM_ALLOWED, FALSE, pe32.th32ProcessID);
//
//			if (hGetFromPID)
//			{
//				// TerminatePrcess�� �������� �ʴ� ���, ������ ���� �Լ��� �ٲ� ��
//				::TerminateProcess(hGetFromPID, 0); // TerminateProcess ����ó�� ����
//				CloseHandle(hGetFromPID);
//			}
//
//			//break; // �ּ� ������ ��� ������ �̸��� ���μ����� 2�� �̻� �������� ��� �ϳ��� �����.
//		}
//	} while (Process32Next(hProcessSnap, &pe32));
//
//	CloseHandle(hProcessSnap);
//
//	return 0;
//}
//
//int CGlobalUtil::TerminateProcess(const char* const file_name)
//{
//	std::string temp_file_name = file_name;
//
//	return TerminateProcess(temp_file_name);
//}
//
//int CGlobalUtil::TerminateProcess(const std::string file_name)
//{
//	std::vector<std::string> processNameList;
//
//	processNameList.push_back(file_name);
//
//	return TerminateProcess(processNameList);
//}
//
//BOOL CGlobalUtil::TerminateProcess(HWND hwnd)
//{
//	if (hwnd == NULL) {
//		logger.debug("TerminateProcess(), parameter value is NULL.");
//
//		return FALSE;
//	}
//
//	DWORD dwPid = 0;
//	::GetWindowThreadProcessId(hwnd, &dwPid);
//
//	HANDLE hGetFromPID = ::OpenProcess(MAXIMUM_ALLOWED, FALSE, dwPid);
//	if (hGetFromPID)
//	{
//		// TerminatePrcess�� �������� �ʴ� ���, ������ ���� �Լ��� �ٲ� ��
//		::TerminateProcess(hGetFromPID, 0); // TerminateProcess ����ó�� ����
//		CloseHandle(hGetFromPID);
//	}
//	else
//	{
//		logger.debug("TerminateProcess(), OpenProcess failed.");
//
//		return FALSE;
//	}
//
//	return TRUE;
//}
//
//int CGlobalUtil::TerminateProcessForce(const std::vector<std::string>& processNameList)
//{
//	if (this->GetIsEndSessionLogout() == true)
//	{
//		logger.debug("TerminateProcessForce(), PC ����� ���� �α׾ƿ�.");
//
//		return 1;
//	}
//
//	return TerminateProcess(processNameList);
//}









/**

@Date2011-03-07

@BriefƯ�� ���μ����� �����ϴ��� üũ

@ParamȮ���� ���μ��� ��

@returnTrue/False

@Comment

*/

bool CGlobalUtil::IsExistProcess(std::string strProcessName)

{

	logger.debug("CGlobalUtil::IsExistProcess[%s]", strProcessName.c_str());



	PROCESSENTRY32 ProcessEntry = { 0 };

	HANDLE hProcessSnapshot;

	StringTool st;



	std::string strCurrentProcess;



	//���� ���μ����� �������� ��´�

	hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnapshot != INVALID_HANDLE_VALUE)

	{

		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnapshot, &ProcessEntry) == TRUE)

		{

			do

			{

				strCurrentProcess = ProcessEntry.szExeFile;//���� ���μ��� ���� ��´�.



				if (st.compare_nocase(strCurrentProcess, strProcessName) == 0)//������

				{

					logger.info("Find Process[%s], inputProcess[%s]", strCurrentProcess.c_str(), strProcessName.c_str());



					return true;

				}

			} while (Process32Next(hProcessSnapshot, &ProcessEntry) != FALSE);

		}

		else

		{

			CloseHandle(hProcessSnapshot);

			logger.error("Process32First is FALSE");



			return false;

		}



		CloseHandle(hProcessSnapshot);

	}

	else

	{

		logger.warn("hPricessSnapshot ==> INVALID_HANDLE_VALUE");



		return false;

	}



	return false;

}



int CGlobalUtil::TerminateProcess(const std::vector<std::string>& processNameList)

{

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

	if (hProcessSnap == INVALID_HANDLE_VALUE)

	{

		return 1;

	}

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))

	{

		CloseHandle(hProcessSnap);

		return 2;

	}

	bool is_process_name_same = false;

	do {

		for (size_t i = 0; i < processNameList.size(); i++)

		{

			if (_stricmp(processNameList[i].c_str(), pe32.szExeFile) == 0)

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

				// TerminatePrcess�� �������� �ʴ� ���, ������ ���� �Լ��� �ٲ� ��

				::TerminateProcess(hGetFromPID, 0); // TerminateProcess ����ó�� ����

				CloseHandle(hGetFromPID);

			}

			//break; // �ּ� ������ ��� ������ �̸��� ���μ����� 2�� �̻� �������� ��� �ϳ��� �����.

		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return 0;

}



int CGlobalUtil::TerminateProcess(const char* const file_name)

{

	std::string temp_file_name = file_name;



	return TerminateProcess(temp_file_name);

}



int CGlobalUtil::TerminateProcess(const std::string file_name)

{

	std::vector<std::string> processNameList;

	processNameList.push_back(file_name);

	return TerminateProcess(processNameList);

}



BOOL CGlobalUtil::TerminateProcess(HWND hwnd)

{

	if (hwnd == NULL) {

		logger.debug("TerminateProcess(), parameter value is NULL.");



		return FALSE;

	}

	DWORD dwPid = 0;

	::GetWindowThreadProcessId(hwnd, &dwPid);

	HANDLE hGetFromPID = ::OpenProcess(MAXIMUM_ALLOWED, FALSE, dwPid);

	if (hGetFromPID)

	{

		// TerminatePrcess�� �������� �ʴ� ���, ������ ���� �Լ��� �ٲ� ��

		::TerminateProcess(hGetFromPID, 0); // TerminateProcess ����ó�� ����

		CloseHandle(hGetFromPID);

	}

	else

	{

		logger.debug("TerminateProcess(), OpenProcess failed.");



		return FALSE;

	}

	return TRUE;

}



// 2021.04.27 ��Ⱓ ��� ���� �� ����. taskkill�� ���μ��� ���̴°� �ʿ��Ѱ�???

int CGlobalUtil::TerminateProcessWithTaskkill(const std::vector<std::string>& processNameList)

{

	if (this->GetIsEndSessionLogout() == true)

	{

		logger.debug("TerminateProcessWithTaskkill(), PC ����� ���� �α׾ƿ�.");



		return 1;

	}

	for (unsigned i = 0; i < processNameList.size(); i++)

	{

		logger.debug("file name list : [%s]", processNameList[i].c_str());

		std::string strParam;

		strParam = "/f /im " + processNameList[i];

		Execute("taskkill.exe", strParam, false);

	}

	return 0;

}



int CGlobalUtil::TerminateProcessForce(const std::vector<std::string>& processNameList)

{

	if (this->GetIsEndSessionLogout() == true)

	{

		logger.debug("TerminateProcessForce(), PC ����� ���� �α׾ƿ�.");



		return 1;

	}



	return TerminateProcess(processNameList);

}



int CGlobalUtil::TerminateProcessNormal(const std::vector<std::string>& processNameList)

{

	if (this->GetIsEndSessionLogout() == true)

	{

		logger.debug("TerminateProcessNormal(), PC ����� ���� �α׾ƿ�.");

		return 1;

	}



	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)

	{

		return 2;

	}

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))

	{

		CloseHandle(hProcessSnap);

		return 3;

	}

	do {

		for (size_t i = 0; i < processNameList.size(); i++)

		{

			if (_stricmp(processNameList[i].c_str(), pe32.szExeFile) == 0)

			{

				logger.debug("TerminateProcessNormal(), ExeFile[%s], ProcessID[%d]\n", pe32.szExeFile, pe32.th32ProcessID);

				SendTerminateMessageToProcessWithSamePID(GetDesktopWindow(), pe32.th32ProcessID);

				break;

			}

		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return 0;

}



int CGlobalUtil::SendTerminateMessageToProcessWithSamePID(const HWND hWnd, const DWORD pid)

{

	std::vector<HWND> startHWND;

	startHWND.push_back(hWnd);



	return SendTerminateMessageToProcessWithSamePID(startHWND, pid);

}



// ù��° �Ű����� ������ HWND ����Ʈ�� �ڽ� �ڵ鿡�� PID�� ��ġ�ϴ� �ڵ��� ã�´�.

// PID�� ��ġ�ϴ� �ڵ��� ������ ž�ٿ� ������� �Ѵܰ辿 ��������.

int CGlobalUtil::SendTerminateMessageToProcessWithSamePID(const std::vector<HWND>& hWndList, const DWORD pid)

{

	HWND hWnd = NULL;

	char szCaption[1025] = { 0 };

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

					logger.debug("SendTerminateMessageToProcessWithSamePID(), dwProcId[%d], hWnd[%X], szCaption[%s]\n", dwProcId, hWnd, szCaption);

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

	return 0;

}