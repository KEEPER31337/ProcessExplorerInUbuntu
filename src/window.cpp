#include "window.h"
#include <ncurses.h>
#include <unistd.h>
#include <string>

using namespace std;

/*
    base Window functions
*/
Window::Window(int endY, int endX, int begY, int begX)
    : mEndY(endY)
    , mEndX(endX)
    , mBegY(begY)
    , mBegX(begX)
{
    mWindow = newwin(endY, endX, begY, begX);
}


void Window::PrintSize(void) const
{
    printf("beg  : %d, %d\n", getbegy(mWindow), getbegx(mWindow));
    printf("max  : %d, %d\n", getmaxy(mWindow), getmaxx(mWindow));
}

WINDOW *Window::GetWindow(void) const
{
    return mWindow;
}

void Window::lineClear(void)
{
    wmove(mWindow, 0, 0);
    wdeleteln(mWindow);
    wmove(mWindow, getmaxy(mWindow) - 1, 0);
}

void Window::lineFeed(void)
{
    if (getcury(mWindow) == getmaxy(mWindow) - 1)
        lineClear();
    else
        wmove(mWindow, getcury(mWindow) + 1, 0);
}

// require string new line (x == 0)
void Window::printStr(string s)
{
    int curIdx = 0;

    while ( curIdx < s.size() ) {

        mvwaddnstr(mWindow, getcury(mWindow), 0, s.c_str()+curIdx, getmaxx(mWindow));

        curIdx += getmaxx(mWindow);
        lineFeed();
    }
}

void Window::mvPrintLine(int y, int x, string s)
{
    mvwaddnstr(mWindow, y, x, s.c_str(), getmaxx(mWindow));
}