#include "infowindow.h"
#include <ncurses.h>
#include <vector>
#include <sstream>
#include <iomanip>

InfoWindow::InfoWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
{}

void InfoWindow::PrintLine(std::string s)
{
    mvwaddnstr(mWindow, getcury(mWindow), 0, s.c_str(), getmaxx(mWindow));
}

void InfoWindow::PrintTitle(void)
{
    stringstream ss;

    ss << setw(8) << "PID" 
       << setw(8) << "PPID" 
       << setw(10) << "COMM";

    PrintLine(ss.str());
    touchwin(mWindow);
    wrefresh(mWindow);
}

void InfoWindow::PrintProcInfo(vector<ProcInfo> &procInfo) const
{
    for ( int i = 0; i < getmaxy(mWindow)-3; i++ ) {
        if ( i < procInfo.size() ) {
            mvwprintw(mWindow, i + 2, 0, "%8d%8d%10s",
                      procInfo[i].pid, procInfo[i].ppid, procInfo[i].comm.c_str());
        }
        else {
            for( int j=0; j<getmaxx(mWindow); j++ ) {
                mvwaddch(mWindow, i+2, j, ' ');
            }
        }
    }
    
    touchwin(mWindow);
    wrefresh(mWindow);
}

void InfoWindow::WindowClear(void) const
{
    for ( int i = 0; i < getmaxy(mWindow) - 3; i++ ) {
        mvwprintw(mWindow, i + 2, 1, "%26c", ' ');
    }
    
    touchwin(mWindow);
    wrefresh(mWindow);
}