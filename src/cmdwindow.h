#include "window.h"
#include "command.h"
#include "processinfo.h"
#include <mutex>

using namespace std;

typedef struct CommandEntry {
    CommandEntry(string _cmd, string _help)
        : cmd(_cmd)
        , help(_help)
    {}
    string cmd;
    string help;
} CommandEntry;

class CmdWindow : public Window
{
public:
    CmdWindow(int endY, int endX, int begY, int begX);
    void StartShell(mutex &mutPrintScr, mutex &mutGetch);
    
private:
    void   executeCommand     (string &args);
    void   executeInfo        (void);
    void   executePath        (void);
    void   executeVirusCheck  (void);
    void   executeKill        (void);
    void   executeSearch      (void);
    void   executeHelp        (void);
    int    printArgs          (string input);
    void   initArgList        (string args);
    int    getNextArg         (char *arg);
    
    struct argList {
        string argBuffer;
        int curArgIdx;
        int curArgc;
    } arglist;

    vector<CommandEntry> *mCmdEntry;
    Command *mCmd;
};