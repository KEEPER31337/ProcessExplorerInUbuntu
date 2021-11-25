#include "window.h"
#include "command.h"
#include <mutex>

typedef struct CommandEntry {
    string cmd;
    string help;
} CommandEntry;

class CmdWindow : public Window
{
public:
    CmdWindow(int endY, int endX, int begY, int begX);
    void StartShell(std::mutex &mutPrintScr, std::mutex &mutGetch);
    
private:
    void executeCommand (string &args);
    void executeHelp    (void);
    void lineFeed       (void);
    void printStr       (string s);
    int  printArgs      (std::string input);
    void lineClear      (void);
    void initArgList    (string args);
    int  getNextArg     (char *arg);
    
    struct argList {
        std::string argBuffer;
        int curArgIdx;
        int curArgc;
    } arglist;

    vector<CommandEntry> *mCmdEntry;
    Command *mCmd;
};