// TerminateProcess.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <atlstr.h>

#include "process.h"

int main()
{
    MOONG::PROCESS::Process process;

    if (process.IsExistProcess("cAlcUlAtOr.ExE") == MOONG::PROCESS::RETURN_CODE::FIND_PROCESS)
    {
        printf("process find.\n");
    }

    //terminateProcess.TerminateProcessNormal("chrome.exe");

    std::vector<std::string> test;

    test.push_back("calculator.exe");

    process.TerminateProcessNormal(test);

    return 0;
}