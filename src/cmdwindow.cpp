#include "cmdwindow.h"
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <mutex>
#include "command.h"
#include "processinfo.h"

using namespace std;

CmdWindow::CmdWindow(int endY, int endX, int begY, int begX, Command *cmd)
    : Window(endY, endX, begY, begX)
    , mCmd(cmd)
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
    halfdelay(1);
    
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
            mutGetch.lock();
            executeCommand(s);
            wprintw(mWindow, "> ");
            psY = getcury(mWindow);
            psX = 2;
            mutPrintScr.unlock();
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
        if( mCmd->GetMode() == Command::Mode::EXIT )
            return;
    }
}

void CmdWindow::executeCommand(string &args)
{
    initArgList(args.c_str());
    int argCount = 0;
    string *cmd;
    vector<CommandEntry>::iterator it;
    lineFeed();
    if ( ( cmd = getNextArg() ) == NULL ) {
        printStr( string("wrong input : ")+args );
        return;
    }

    for ( it=mCmdEntry->begin(); it != mCmdEntry->end(); it++ ) {
        if( cmd->compare(it->cmd) == 0 ) {

            if( cmd->compare("help") == 0 )
                executeHelp();
            else if( cmd->compare("path") == 0 )
                executePath();
            else if( cmd->compare("viruscheck") == 0 )
                executeVirusCheck();
            else if( cmd->compare("kill") == 0 )
                executeKill();
            else if( cmd->compare("search") == 0 )
                executeSearch();
            else if( cmd->compare("info") == 0 )
                executeInfo();
            else if( cmd->compare("exit") == 0 )
                mCmd->SetMode(Command::Mode::EXIT);

            break;

        }
    }
    if ( it == mCmdEntry->end() )
        printStr( string("not found command : ") + args );
}

void CmdWindow::executeInfo(void)
{
    int pid;
    string *arg;
    stringstream ss;
    ProcInfo information;

    if ( ( arg = getNextArg() ) == NULL ) {
        printStr("wrong input : need parameter");
        return;
    }
    for ( int i=0; i<arg->size(); i++ ) {
        if( !isdigit(arg->at(i)) ) {
            printStr("wrong parameter : must be pid");
            return;
        }
    }
    if( arg->size() > 8 )
        information.pid = -1;
    else {
        pid = stoi(*arg);
        information = mCmd->GetProcInfoByPID(pid);
    }
    
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
    string *arg;

    if ( ( arg = getNextArg() ) == NULL ) {
        printStr("wrong input : need parameter");
        return;
    }
    for ( int i=0; i<arg->size(); i++ ) {
        if( !isdigit(arg->at(i)) ) {
            printStr("wrong parameter : must be pid");
            return;
        }
    }
    if( arg->size() > 8 ) {
        pid = -1;
    }
    else {
        pid = stoi(*arg);
    }

    printStr(mCmd->GetProcPath(pid));
}

void CmdWindow::executeVirusCheck(void)
{
    int pid;
    string *arg;
    string filepath;

    if ( ( arg = getNextArg() ) == NULL ) {
        printStr("wrong input");
        return;
    }
    for ( int i=0; i<arg->size(); i++ ) {
        if( !isdigit(arg->at(i)) ) {
            printStr("wrong parameter : must be pid");
            return;
        }
    }
    if( arg->size() > 8 ) {
        pid = -1;
    }
    else {
        pid = stoi(*arg);
    }
    filepath = mCmd->GetProcPath(pid);
    if( filepath.compare("cannot get path") == 0 ) {
        printStr(filepath);
        return;
    }
    else {
        printStr(string("file path : ")+filepath);
    }

    printStr(mCmd->GetVirusTotalReport(filepath));
}


void CmdWindow::executeKill(void)
{
    int pid, sigNum;
    string *arg;

    if ( ( arg = getNextArg() ) == NULL ) {
        printStr("wrong input");
        return;
    }
    pid = stoi(*arg);

    if ( ( arg = getNextArg() ) == NULL ) {
        printStr("wrong input");
        return;
    }
    sigNum = stoi(*arg);

    mCmd->SendSignal(pid, sigNum);
}

void CmdWindow::executeSearch(void)
{
    string *arg;
    string kind;
    string keyword;
    vector<ProcInfo> *procinfos;

    if ( ( arg = getNextArg() ) == NULL ) {
        printStr("usage: search \"PID\"/\"PPID\"/\"STATE\"/\"COMM\"/\"START\" [keyword]");
        return;
    }
    kind = *arg;
    if ( ( arg = getNextArg() ) == NULL ) {
        printStr("usage: search \"PID\"/\"PPID\"/\"STATE\"/\"COMM\"/\"START\" [keyword]");
        return;
    }
    keyword = *arg;
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
    string *arg;
    int argCount = 0;

    lineFeed();
    while ( ( arg = getNextArg() ) == NULL )
    {
        stringstream ss;
        ss << "arg " << argCount
           << ": " << arg; 
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

string *CmdWindow::getNextArg(void)
{
    string *res;
    int argLen, i;
    if (arglist.argBuffer.size() <= arglist.curArgIdx)
        return 0;

    for ( i = arglist.curArgIdx; i < arglist.argBuffer.size(); i++ ) {
        if ( arglist.argBuffer[i] == ' ' ) {
            break;
        }
    }

    if( argLen < 1 ) {
        return NULL;
    }
    else { 
        res = new string(arglist.argBuffer.substr(arglist.curArgIdx, i-arglist.curArgIdx));
    }

    arglist.curArgIdx += i - arglist.curArgIdx + 1;

    return res;
}