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

    //std::cout << "중복 실행 여부[" << MOONG::Process::CheckDuplicateExecution() << "]" << std::endl;
    //system("pause");

    //std::cout << "msdege 체크 백그라운드 포함[" << MOONG::Process::IsExistProcess("msedge.exe") << "]" << std::endl;
    //std::cout << "msdege 체크 백그라운드 미포함[" << MOONG::Process::IsExistProcess("msedge.exe", false) << "]" << std::endl;

	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << std::endl;

	std::vector<DWORD> process_id = MOONG::Process::get_process_id("Project1.exe", false);
    for (size_t i = 0; i < process_id.size(); i++)
    {
        std::cout << "process_id[" << process_id[i] << "]" << std::endl;
        std::cout << "get_path[" << MOONG::Process::get_path(process_id[i]).c_str() << "]" << std::endl;
    }
    
    std::cout << std::endl;
	
	process_id = MOONG::Process::get_process_id();
    for (size_t i = 0; i < process_id.size(); i++)
    {
        std::cout << "process_id[" << process_id[i] << "]" << std::endl;
        std::cout << "get_path[" << MOONG::Process::get_path(process_id[i]).c_str() << "]" << std::endl;
    }

    std::cout << std::endl;

	process_id = MOONG::Process::get_process_id("", false);
    for (size_t i = 0; i < process_id.size(); i++)
    {
        std::cout << "process_id[" << process_id[i] << "]" << std::endl;
        std::cout << "get_path[" << MOONG::Process::get_path(process_id[i]).c_str() << "]" << std::endl;
    }

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

    return 0;
}