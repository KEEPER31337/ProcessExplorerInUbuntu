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
    mvwprintw(mWindow, 1, 1, "This is new window");
    touchwin(mWindow);
    wrefresh(mWindow);
    sleep(1);
    mvwprintw(mWindow, 1, 1, "                  ");
    touchwin(mWindow);
    wrefresh(mWindow);
}


void Window::printSize(void) const
{
    printf("beg  : %d, %d\n", getbegy(mWindow), getbegx(mWindow));
    printf("max  : %d, %d\n", getmaxy(mWindow), getmaxx(mWindow));
}

WINDOW *Window::getWindow(void) const
{
    return mWindow;
}

/*
    no more need
void Window::drawFrame(int endY, int endX, int begY, int begX) const
{

    int mx = endX;
    int my = endY;
    int bx = begX;
    int by = begY;

    mvprintw(by, bx, "+");
    mvprintw(by, mx - 1, "+");
    mvprintw(my - 1, bx, "+");
    mvprintw(my - 1, mx - 1, "+");
    for (int x = bx + 1, y = by; x < mx - 1; x++)
    {
        mvprintw(y, x, "-");
    }
    for (int x = mx - 1, y = by + 1; y < my - 1; y++)
    {
        mvprintw(y, x, "|");
    }
    for (int x = mx - 2, y = my - 1; x > bx; x--)
    {
        mvprintw(y, x, "-");
    }
    for (int x = bx, y = my - 2; y > by; y--)
    {
        mvprintw(y, x, "|");
    }

    refresh();
}
void Window::drawBorder(void) const
{

    int mx = getmaxx(mWindow);
    int my = getmaxy(mWindow);
    int bx = getbegx(mWindow);
    int by = getbegy(mWindow);

    mvprintw(by, bx, "+");
    mvprintw(by, mx - 1, "+");
    mvprintw(my - 1, bx, "+");
    mvprintw(my - 1, mx - 1, "+");
    for (int x = bx + 1, y = by; x < mx - 1; x++)
    {
        mvprintw(y, x, "-");
    }
    for (int x = mx - 1, y = by + 1; y < my - 1; y++)
    {
        mvprintw(y, x, "|");
    }
    for (int x = mx - 2, y = my - 1; x > bx; x--)
    {
        mvprintw(y, x, "-");
    }
    for (int x = bx, y = my - 2; y > by; y--)
    {
        mvprintw(y, x, "|");
    }

    refresh();
}
*/