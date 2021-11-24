#include "cmdwindow.h"
#include <string>
#include <cstring>
#include <cstdlib>

CmdWindow::CmdWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
{
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

int CmdWindow::printArgs(string input)
{
    initArgList(input.c_str());
    char buf[1024];
    int argCount = 0;

    while ( getNextArg(buf) != 0 ) {
        if ( getcury(mWindow) == getmaxy(mWindow) - 1 ) {
            lineClear();
        }
        mvwprintw(mWindow, getcury(mWindow), 0, "arg %d : %s", argCount, buf);
        argCount++;
    }
    if ( getcury(mWindow) == getmaxy(mWindow) - 1 ) {
        lineClear();
    }
    else {
        wmove(mWindow, getcury(mWindow)+1, 0);
    }
    return argCount;
}

void CmdWindow::lineClear(void)
{
    wmove(mWindow, 0, 0);
    wdeleteln(mWindow);
    wmove(mWindow, getmaxy(mWindow) - 1, 0);
}

void CmdWindow::startShell(void)
{
    char c;
    std::string s;
    int idx = 0;
    mvwprintw(mWindow, 0, 0, "> ");
    while ( true )
    {
        if ( getcurx(mWindow) == getmaxx(mWindow) - 1 &&
             getcury(mWindow) == getmaxy(mWindow) - 1 )
        {
            c = wgetch(mWindow);
            lineClear();
        }
        else {
            c = wgetch(mWindow);
        }

        switch (c)
        {
        case ' ':
        case '\t':
            s.push_back(' ');
            break;

        case '\n':
            wmove(mWindow, getcury(mWindow) + 1, 0);
            printArgs(s);
            wprintw(mWindow, "> ");
            s.clear();
            break;

        default:
            s.push_back(c);
            break;
        }
    }
}