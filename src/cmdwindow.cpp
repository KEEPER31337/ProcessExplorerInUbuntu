#include "cmdwindow.h"
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <mutex>

CmdWindow::CmdWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
{
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
            printArgs(s);
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

// require string new line (x == 0)
void CmdWindow::printStr(string s)
{
    int curIdx = 0;

    while ( curIdx < s.size() ) {

        mvwaddnstr(mWindow, getcury(mWindow), 0, s.c_str()+curIdx, getmaxx(mWindow));

        curIdx += getmaxx(mWindow);
        if (getcury(mWindow) == getmaxy(mWindow) - 1)
            lineClear();
        else
            wmove(mWindow, getcury(mWindow) + 1, 0);
    }
}

int CmdWindow::printArgs(string input)
{
    initArgList(input.c_str());
    char buf[1024];
    int argCount = 0;

    if (getcury(mWindow) == getmaxy(mWindow) - 1)
        lineClear();
    else
        wmove(mWindow, getcury(mWindow) + 1, 0);

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