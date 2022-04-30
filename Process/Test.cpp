#include "process.h"

int main()
{
    if (MOONG::Process::IsExistProcess("cAlcUlAtOr.ExE") == MOONG::PROCESS::RETURN::FIND_PROCESS)
    {
        printf("process find.\n");
    }

    //MOONG::Process::TerminateProcessNormal("chrome.exe");

    std::vector<std::string> test;

    test.push_back("calculator.exe");

    MOONG::Process::TerminateProcessNormal(test);

    return 0;
}