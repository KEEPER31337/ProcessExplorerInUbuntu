#include "cmdwindow.h"
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <mutex>
#include "command.h"
#include "processinfo.h"

using namespace std;

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
    mCmdEntry->push_back( CommandEntry("exit",        "exit program") );
}

void CmdWindow::StartShell(mutex &mutPrintScr, mutex &mutGetch)
{
    int c;
    string s;
    bool bPrevSpace = false;
    int psX;
    int psY;
    
    curs_set(2);
    keypad(mWindow, true);
    nodelay(mWindow, true);
    
    mvwprintw(mWindow, 0, 0, "> ");
    psY = getcury(mWindow);
    psX = 2;

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
            psY = getcury(mWindow);
            psX = 2;
            s.clear();
            bPrevSpace = false;
            break;

        case KEY_IL:
        case KEY_DL:
        case KEY_PPAGE:
        case KEY_NPAGE:
        case KEY_HOME:
        case KEY_END:
        case ERR:
            break;

        case KEY_BACKSPACE:
            if ( !(getcurx(mWindow) <= psX && getcury(mWindow) == psY) )
            {
                if( !s.empty() ) s.pop_back();
                wdelch(mWindow);
            }
            else {
                if( !s.empty() ) s.pop_back();
                wdelch(mWindow);
                wmove(mWindow, psY, psX);
            }
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
        printStr( string("wrong input : ")+args );
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
            else if( cmd.compare("search") == 0 )
                executeSearch();
            else if( cmd.compare("info") == 0 )
                executeInfo();
            else if( cmd.compare("exit") == 0 )
                system("kill -9 `pidof ./main`");

            break;

        }
    }
    if ( it == mCmdEntry->end() )
        printStr( string("not found command : ") + args );
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
        printStr(information.procInfoToStrWithName());
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
    int pid;
    char buf[1024];

    if ( getNextArg(buf) == 0 ) {
        printStr("wrong input");
        return;
    }
    pid = stoi(buf);

    printStr(mCmd->GetVirusTotalReport(pid));
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
        printStr("usage: search \"PID\"/\"PPID\"/\"STATE\"/\"COMM\"/\"START\" [keyword]");
        return;
    }
    kind = buf;
    if ( getNextArg(buf) == 0 ) {
        printStr("usage: search \"PID\"/\"PPID\"/\"STATE\"/\"COMM\"/\"START\" [keyword]");
        return;
    }
    keyword = buf;
    procinfos = mCmd->SearchProc( mCmd->GetProcInfos(), kind, keyword );

    if( procinfos == NULL ) {
        // handle error
        printStr("usage: search \"PID\"/\"PPID\"/\"STATE\"/\"COMM\"/\"START\" [keyword]");
        return;
    }

    for( auto it = procinfos->begin(); it != procinfos->end(); ++it) {
        printStr( it->procInfoToStrWithName() );
    }
}


void CmdWindow::executeHelp(void)
{
    vector<CommandEntry>::iterator it;
    for ( it=mCmdEntry->begin(); it != mCmdEntry->end(); it++ ) {
        printStr(it->help);
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

void CmdWindow::initArgList(string args)
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