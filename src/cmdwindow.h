#include "window.h"
#include "command.h"

class CmdWindow : public Window
{
public:
    CmdWindow(int endY, int endX, int begY, int begX);
    void startShell(void);
    
private:
    
    void initArgList(string args);
    int getNextArg(char *arg);
    int printArgs(std::string input);
    void lineClear(void);

    struct argList
    {
        std::string argBuffer;
        int curArgIdx;
        int curArgc;
    } arglist;

    Command cmd;
};