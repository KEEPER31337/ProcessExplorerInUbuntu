#include "cmdwindow.h"
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <mutex>
#include "command.h"
#include "processinfo.h"

CmdWindow::CmdWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
    , mCmd(new Command())
{
    mCmdEntry = new vector<CommandEntry>;

    mCmdEntry->push_back( CommandEntry("info",        "info [PID] - display the information of processs' detail informain") );
    mCmdEntry->push_back( CommandEntry("path",        "path [PID] - display the path of each process") );
    mCmdEntry->push_back( CommandEntry("viruscheck",  "viruscheck [PID] - check a status which process is infection") );
    mCmdEntry->push_back( CommandEntry("kill",        "kill [PID] [SGINAL_NUM] - send a signal to process") );
    mCmdEntry->push_back( CommandEntry("search",      "search [PID] [KEYWORD] - search the processes using keyword") );
    mCmdEntry->push_back( CommandEntry("help",        "print help") );
}

void CmdWindow::StartShell(std::mutex &mutPrintScr, std::mutex &mutGetch)
{
    char c;
    std::string s;
    int idx = 0;
    bool bPrevSpace = false;
    
    nodelay(mWindow, true);
    
    mvwprintw(mWindow, 0, 0, "> ");
    while ( true )
    {
        mutGetch.lock();
        if ( getcurx(mWindow) == getmaxx(mWindow) - 1 &&
             getcury(mWindow) == getmaxy(mWindow) - 1 )
        {
            c = wgetch(mWindow);
            lineClear();
        }
        else {
            c = wgetch(mWindow);
        }
        mutPrintScr.unlock();

        switch (c)
        {
        case ' ':
        case '\t':
            if( !bPrevSpace ) {
                s.push_back(' ');
                bPrevSpace = true;
            }
            break;

        case '\n':
            mCmd->UpdateProcStat();
            executeCommand(s);
            wprintw(mWindow, "> ");
            s.clear();
            bPrevSpace = false;
            break;

        case ERR:
            break;

        default:
            s.push_back(c);
            bPrevSpace = false;
            break;
        }
    }
}

void CmdWindow::executeCommand(string &args)
{
    initArgList(args.c_str());
    char buf[1024];
    int argCount = 0;
    string cmd;
    vector<CommandEntry>::iterator it;
    lineFeed();
    if ( getNextArg(buf) == 0 ) {
        printStr("wrong input");
        return;
    }
    cmd = buf;

    for ( it=mCmdEntry->begin(); it != mCmdEntry->end(); it++ ) {
        if( cmd.compare(it->cmd) == 0 ) {

            if( cmd.compare("help") == 0 )
                executeHelp();
            else if( cmd.compare("path") == 0 )
                executePath();
            else if( cmd.compare("viruscheck") == 0 )
                executeVirusCheck();
            else if( cmd.compare("kill") == 0 )
                executeKill();
            //else if( cmd.compare("search") == 0 )
                //executeSearch();
            else if( cmd.compare("info") == 0 )
                executeInfo();

            break;

        }
    }
    if ( it == mCmdEntry->end() )
        printStr("not found command");
}

void CmdWindow::executeInfo(void)
{
    int pid;
    char buf[1024];
    stringstream ss;

    if ( getNextArg(buf) == 0 ) {
        printStr("wrong input");
        return;
    }
    pid = stoi(buf);

    ProcInfo information = mCmd->GetProcInfoByPID(pid);
    if( information.pid == -1 ) {
        printStr("not found pid");
    } 
    else {
        printStr(procInfoToStr(information));
    }
}

void CmdWindow::executePath(void)
{
    int pid;
    char buf[1024];

    if ( getNextArg(buf) == 0 ) {
        printStr("wrong input");
        return;
    }
    pid = stoi(buf);

    printStr(mCmd->GetProcPath(pid));

}

void CmdWindow::executeVirusCheck(void)
{
    
}


void CmdWindow::executeKill(void)
{
    int pid, sigNum;
    char buf[1024];
    if ( getNextArg(buf) == 0 ) {
        printStr("wrong input");
        return;
    }
    pid = stoi(buf);

    if ( getNextArg(buf) == 0 ) {
        printStr("wrong input");
        return;
    }
    sigNum = stoi(buf);

    mCmd->SendSignal(pid, sigNum);
}

void CmdWindow::executeSearch(void)
{
    char buf[1024];
    string kind;
    string keyword;
    vector<ProcInfo> *procinfos;
    if ( getNextArg(buf) == 0 ) {
        printStr("!need kind!");
        return;
    }
    kind = buf;
    if ( getNextArg(buf) == 0 ) {
        printStr("!need keyword!");
        return;
    }
    keyword = buf;
    *procinfos = mCmd->SearchProc( mCmd->GetProcInfos(), kind, keyword );

    for( auto it = procinfos->begin(); it != procinfos->end(); ++it) {
        printStr( procInfoToStr(*it) );
    }
}


void CmdWindow::executeHelp(void)
{
    vector<CommandEntry>::iterator it;
    for ( it=mCmdEntry->begin(); it != mCmdEntry->end(); it++ ) {
        printStr(it->help);
    }
}

void CmdWindow::lineFeed(void)
{
    if (getcury(mWindow) == getmaxy(mWindow) - 1)
        lineClear();
    else
        wmove(mWindow, getcury(mWindow) + 1, 0);
}

// require string new line (x == 0)
void CmdWindow::printStr(string s)
{
    int curIdx = 0;

    while ( curIdx < s.size() ) {

        mvwaddnstr(mWindow, getcury(mWindow), 0, s.c_str()+curIdx, getmaxx(mWindow));

        curIdx += getmaxx(mWindow);
        lineFeed();
    }
}

int CmdWindow::printArgs(string input)
{
    initArgList(input.c_str());
    char buf[1024];
    int argCount = 0;

    lineFeed();
    while (getNextArg(buf) != 0)
    {
        stringstream ss;
        ss << "arg " << argCount
           << ": " << buf; 
        printStr(ss.str());
        argCount++;
    }

    return argCount;
}

void CmdWindow::lineClear(void)
{
    wmove(mWindow, 0, 0);
    wdeleteln(mWindow);
    wmove(mWindow, getmaxy(mWindow) - 1, 0);
}

void CmdWindow::initArgList(std::string args)
{
    arglist.argBuffer = args;
    arglist.curArgIdx = 0;
    arglist.curArgc = 0;
}

int CmdWindow::getNextArg(char *arg)
{
    int argLen, i;
    if (arglist.argBuffer.size() <= arglist.curArgIdx)
        return 0;

    for (i = arglist.curArgIdx; i < arglist.argBuffer.size(); i++)
    {
        if (arglist.argBuffer[i] == ' ')
        {
            break;
        }
    }
    argLen = i - arglist.curArgIdx;
    memcpy(arg, arglist.argBuffer.c_str() + arglist.curArgIdx, argLen);
    arg[argLen] = '\0';
    arglist.curArgIdx += argLen + 1;
    return argLen;
}

string CmdWindow::procInfoToStr(ProcInfo &pi)
{
    stringstream ss;
    ss << "PID : "      << pi.pid     << ", "
       << "PPID : "     << pi.ppid    << ", "
       << "COMMAND : "  << pi.comm    << ", "
       << "CPU : "      << pi.cpu     << ", "
       << "MEM : "      << pi.vmem    << ", "
       << "STAT : "     << pi.state   << ", "
       << "USER : "     << pi.user    << ", "
       << "START : "    << pi.start   << ", "
       << "THREADS : "  << pi.nlwp    << ", "
       << "PNAME : "    << pi.name    << ", "
       ;

    return ss.str();
}