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

	HANDLE handle = MOONG::Process::get_process_handle("chrome.exe", false);
	std::cout << "GetProcessHandle[" << handle << "]" << std::endl;
	std::cout << "GetPath[" << MOONG::Process::get_path(handle).c_str() << "]" << std::endl;
	
	handle = MOONG::Process::get_process_handle();
	std::cout << "GetProcessHandle[" << handle << "]" << std::endl;
	std::cout << "GetPath[" << MOONG::Process::get_path(handle).c_str() << "]" << std::endl;

	handle = MOONG::Process::get_process_handle("", false);
	std::cout << "GetProcessHandle[" << handle << "]" << std::endl;
	std::cout << "GetPath[" << MOONG::Process::get_path(handle).c_str() << "]" << std::endl;

    return 0;
}