#include "window.h"
#include "command.h"
#include <mutex>

class CmdWindow : public Window
{
public:
    CmdWindow(int endY, int endX, int begY, int begX);
    void StartShell(std::mutex &mutPrintScr, std::mutex &mutGetch);
    
private:
    
    void InitArgList(string args);
    int GetNextArg(char *arg);
    int PrintArgs(std::string input);
    void LineClear(void);

    struct argList
    {
        std::string argBuffer;
        int curArgIdx;
        int curArgc;
    } arglist;

    Command mcmd;
};