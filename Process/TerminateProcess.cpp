// TODO: 파일 이름, 클래스 이름 Process로 변경.
// TODO: 맨 아래 함수들 정리


#pragma region 프로세스 킬 할 때 참조
//PID에서 HWND를 얻거나
//HANDLE에서 HWND를 얻거나
//(HANDLE hGetFromPID = OpenProcess(MAXIMUM_ALLOWED, FALSE, pe32.th32ProcessID)
//
//EnumWindows()
//https://stackoverflow.com/questions/11711417/get-hwnd-by-process-id-c
#pragma endregion 프로세스 킬 할 때 참조


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
			std::string szExeFile(ws.begin(), ws.end());	// TODO: 여기서 warning 발생
			if (_stricmp(processNameList[i].c_str(), szExeFile.c_str()) == 0)
			{
				//_tprintf(TEXT("TerminateProcessNormal(), ExeFile[%s], ProcessID[%d]\n"), pe32.szExeFile, pe32.th32ProcessID);

				// TODO: ProcessID를 바로 HWND로 변경하는 방법 찾아보기.

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

					// 프로세스 종료 시 WM_CLOSE, WM_DESTROY, WM_QUIT 메시지가 순서대로 발생한다.
					PostMessage(hWnd, WM_CLOSE, NULL, NULL);
					PostMessage(hWnd, WM_DESTROY, NULL, NULL);
					PostMessage(hWnd, WM_QUIT, NULL, NULL);

					// 크롬 같은 경우 창을 여러개 열어 사용할 경우 특정 핸들의 동일한 자식 레벨에 윈도우 핸들이 존재하므로
					// PID가 일치하는 윈도우 핸들을 찾았더라도 형제 레벨 관계에 있는 모든 핸들들을 탐색한다.
					// 부모 레벨을 A, 자식 레벨을 B, 손자 레벨을 C라고 하고 Target이 되는 윈도우를 T라고 가정한다면
					// 크롬 윈도우가 여러개일 때 C 레벨에 형제 관계로 T가 여러개 존재.
					continue;
				}

				// 윈도우 기본 계산기의 경우 바탕화면에 윈도우가 떠 있으면 프레임 윈도우 하위에 UI 윈도우가 있어 자식노드 탐색이 필요.
				// 특이한게 최소화 버튼을 눌러 창을 최소화 시키면 UI 윈도우가 바탕화면 자식 레벨로 바뀜.
				// 부모 레벨을 A, 자식 레벨을 B, 손자 레벨을 C라고 하고 Target이 되는 윈도우를 T라고 가정한다면
				// 계산기 윈도우가 바탕화면에 떠있을 경우 A - B - T이고, 최소화 된 상태에서는 A - T 상태가 됨.
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











// TODO: 아래 함수들 추가해서 클래스 작성
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
//				// TerminatePrcess는 권장하지 않는 방법, 안전한 종료 함수로 바꿀 것
//				::TerminateProcess(hGetFromPID, 0); // TerminateProcess 예외처리 생략
//				CloseHandle(hGetFromPID);
//			}
//
//			//break; // 주석 해제할 경우 동일한 이름의 프로세스가 2개 이상 실행중일 경우 하나만 종료됨.
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
//		// TerminatePrcess는 권장하지 않는 방법, 안전한 종료 함수로 바꿀 것
//		::TerminateProcess(hGetFromPID, 0); // TerminateProcess 예외처리 생략
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
//		logger.debug("TerminateProcessForce(), PC 종료로 인한 로그아웃.");
//
//		return 1;
//	}
//
//	return TerminateProcess(processNameList);
//}









/**

@Date2011-03-07

@Brief특정 프로세스가 존재하는지 체크

@Param확인할 프로세스 명

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



	//현재 프로세스의 스냅샷을 얻는다

	hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnapshot != INVALID_HANDLE_VALUE)

	{

		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnapshot, &ProcessEntry) == TRUE)

		{

			do

			{

				strCurrentProcess = ProcessEntry.szExeFile;//현재 프로세스 명을 얻는다.



				if (st.compare_nocase(strCurrentProcess, strProcessName) == 0)//같으면

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

				// TerminatePrcess는 권장하지 않는 방법, 안전한 종료 함수로 바꿀 것

				::TerminateProcess(hGetFromPID, 0); // TerminateProcess 예외처리 생략

				CloseHandle(hGetFromPID);

			}

			//break; // 주석 해제할 경우 동일한 이름의 프로세스가 2개 이상 실행중일 경우 하나만 종료됨.

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

		// TerminatePrcess는 권장하지 않는 방법, 안전한 종료 함수로 바꿀 것

		::TerminateProcess(hGetFromPID, 0); // TerminateProcess 예외처리 생략

		CloseHandle(hGetFromPID);

	}

	else

	{

		logger.debug("TerminateProcess(), OpenProcess failed.");



		return FALSE;

	}

	return TRUE;

}



// 2021.04.27 장기간 사용 없을 시 삭제. taskkill로 프로세스 죽이는게 필요한가???

int CGlobalUtil::TerminateProcessWithTaskkill(const std::vector<std::string>& processNameList)

{

	if (this->GetIsEndSessionLogout() == true)

	{

		logger.debug("TerminateProcessWithTaskkill(), PC 종료로 인한 로그아웃.");



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

		logger.debug("TerminateProcessForce(), PC 종료로 인한 로그아웃.");



		return 1;

	}



	return TerminateProcess(processNameList);

}



int CGlobalUtil::TerminateProcessNormal(const std::vector<std::string>& processNameList)

{

	if (this->GetIsEndSessionLogout() == true)

	{

		logger.debug("TerminateProcessNormal(), PC 종료로 인한 로그아웃.");

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



// 첫번째 매개변수 윈도우 HWND 리스트의 자식 핸들에서 PID가 일치하는 핸들을 찾는다.

// PID가 일치하는 핸들이 없으면 탑다운 방식으로 한단계씩 내려간다.

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

					// 프로세스 종료 시 WM_CLOSE, WM_DESTROY, WM_QUIT 메시지가 순서대로 발생한다.

					PostMessage(hWnd, WM_CLOSE, NULL, NULL);

					PostMessage(hWnd, WM_DESTROY, NULL, NULL);

					PostMessage(hWnd, WM_QUIT, NULL, NULL);



					// 크롬 같은 경우 창을 여러개 열어 사용할 경우 특정 핸들의 동일한 자식 레벨에 윈도우 핸들이 존재하므로

					// PID가 일치하는 윈도우 핸들을 찾았더라도 형제 레벨 관계에 있는 모든 핸들들을 탐색한다.

					// 부모 레벨을 A, 자식 레벨을 B, 손자 레벨을 C라고 하고 Target이 되는 윈도우를 T라고 가정한다면

					// 크롬 윈도우가 여러개일 때 C 레벨에 형제 관계로 T가 여러개 존재.

					continue;

				}



				// 윈도우 기본 계산기의 경우 바탕화면에 윈도우가 떠 있으면 프레임 윈도우 하위에 UI 윈도우가 있어 자식노드 탐색이 필요.

				// 특이한게 최소화 버튼을 눌러 창을 최소화 시키면 UI 윈도우가 바탕화면 자식 레벨로 바뀜.

				// 부모 레벨을 A, 자식 레벨을 B, 손자 레벨을 C라고 하고 Target이 되는 윈도우를 T라고 가정한다면

				// 계산기 윈도우가 바탕화면에 떠있을 경우 A - B - T이고, 최소화 된 상태에서는 A - T 상태가 됨.

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