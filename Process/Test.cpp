#include "process.h"

int main()
{
    //MOONG::Process::CreateProcessWithIntegrityLevel(MOONG::PROCESS::INTEGRITY_LEVEL::MEDIUM, "C:\\WINDOWS\\system32\\mspaint.exe");

    //if (MOONG::Process::IsExistProcess("mspaint.exe") == MOONG::PROCESS::RETURN::FIND_PROCESS)
    //{
    //    printf("process find.\n");
    //}

    //system("pause");
    //MOONG::Process::TerminateProcessNormal("mspaint.exe");

    //MOONG::Process::CreateProcessWithIntegrityLevel(MOONG::PROCESS::INTEGRITY_LEVEL::MEDIUM, "C:\\WINDOWS\\system32\\mspaint.exe");

    //system("pause");
    //MOONG::Process::TerminateProcess("mspaint.exe");

    //MOONG::Process::CreateProcessWithIntegrityLevel(MOONG::PROCESS::INTEGRITY_LEVEL::MEDIUM, "C:\\WINDOWS\\system32\\mspaint.exe");

    //std::vector<std::string> test;

    //test.push_back("mspaint.exe");

    //system("pause");
    //MOONG::Process::TerminateProcessNormal(test);

    //std::cout << "�ߺ� ���� ����[" << MOONG::Process::CheckDuplicateExecution() << "]" << std::endl;
    //system("pause");

    //std::cout << "msdege üũ ��׶��� ����[" << MOONG::Process::IsExistProcess("msedge.exe") << "]" << std::endl;
    //std::cout << "msdege üũ ��׶��� ������[" << MOONG::Process::IsExistProcess("msedge.exe", false) << "]" << std::endl;

	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;

	HANDLE handle = MOONG::Process::GetProcessHandle("chrome.exe", false);
	std::cout << "GetProcessHandle[" << handle << "]" << std::endl;
	std::cout << "GetPath[" << MOONG::Process::GetPath(handle).c_str() << "]" << std::endl;
	
	handle = MOONG::Process::GetProcessHandle();
	std::cout << "GetProcessHandle[" << handle << "]" << std::endl;
	std::cout << "GetPath[" << MOONG::Process::GetPath(handle).c_str() << "]" << std::endl;

	handle = MOONG::Process::GetProcessHandle("", false);
	std::cout << "GetProcessHandle[" << handle << "]" << std::endl;
	std::cout << "GetPath[" << MOONG::Process::GetPath(handle).c_str() << "]" << std::endl;

    return 0;
}