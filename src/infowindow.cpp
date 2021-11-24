#include "infowindow.h"
#include <ncurses.h>
#include <vector>

InfoWindow::InfoWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
{}
void InfoWindow::printTitle(void) const
{
    mvwprintw(mWindow, 1, 1, "%8s%8s%10s", "PID", "PPID", "COMM");
    touchwin(mWindow);
    wrefresh(mWindow);
}

void InfoWindow::printProcInfo(vector<ProcInfo> &procInfo) const
{
    for (int i = 0; i < getmaxy(mWindow)-3; i++ )
    {
        if (i < procInfo.size())
        {
            mvwprintw(mWindow, i + 2, 1, "%8d%8d%10s",
                      procInfo[i].pid, procInfo[i].ppid, procInfo[i].comm.c_str());
        }
        else{
            mvwprintw(mWindow, i + 2, 1, "%26c", ' ');
        }
    }
    touchwin(mWindow);
    wrefresh(mWindow);
}

void InfoWindow::windowClear(void) const
{
    for (int i = 0; i < getmaxy(mWindow) - 3; i++)
    {
        mvwprintw(mWindow, i + 2, 1, "%26c", ' ');
    }
    
/*
    werase(mWindow);
    box(mWindow, '|', '-');
*/
    touchwin(mWindow);
    wrefresh(mWindow);
}