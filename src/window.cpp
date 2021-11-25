#include "window.h"
#include <ncurses.h>
#include <unistd.h>

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