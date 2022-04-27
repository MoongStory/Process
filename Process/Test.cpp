#include "process.h"

int main()
{
    if (MOONG::PROCESS::Process::IsExistProcess("cAlcUlAtOr.ExE") == MOONG::PROCESS::RETURN::FIND_PROCESS)
    {
        printf("process find.\n");
    }

    //MOONG::PROCESS::Process::TerminateProcessNormal("chrome.exe");

    std::vector<std::string> test;

    test.push_back("calculator.exe");

    MOONG::PROCESS::Process::TerminateProcessNormal(test);

    return 0;
}