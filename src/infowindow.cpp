#include "infowindow.h"
#include <ncurses.h>
#include <vector>
#include <cstring>

InfoWindow::InfoWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
{}

void InfoWindow::printLine(std::string s)
{
    mvwaddnstr(mWindow, getcury(mWindow), 0, s.c_str(), getmaxx(mWindow));
}

void InfoWindow::printTitle(void)
{
    char buf[1024];
    char *s;
    
    sprintf(buf, "%8s%8s%10s", "PID", "PPID", "COMM");

    s = (char *)malloc(strlen(buf)+1);

    strcpy(s, buf);

    printLine(s);
    touchwin(mWindow);
    wrefresh(mWindow);
}

void InfoWindow::printProcInfo(vector<ProcInfo> &procInfo) const
{
    for (int i = 0; i < getmaxy(mWindow)-3; i++ )
    {
        if (i < procInfo.size())
        {
            mvwprintw(mWindow, i + 2, 0, "%8d%8d%10s",
                      procInfo[i].pid, procInfo[i].ppid, procInfo[i].comm.c_str());
        }
        else{
            for(int j=0; j<getmaxx(mWindow); j++) {
                mvwaddch(mWindow, i+2, j, ' ');
            }
            
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