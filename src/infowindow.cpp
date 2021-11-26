#include "infowindow.h"
#include <ncurses.h>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

InfoWindow::InfoWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
{}

void InfoWindow::PrintLine(string s)
{
    mvwaddnstr(mWindow, getcury(mWindow), 0, s.c_str(), getmaxx(mWindow));
}

void InfoWindow::PrintTitle(void)
{
    stringstream ss;

    ss << setw(8)  << "PID" 
       << setw(8)  << "PPID" 
       << setw(10) << "COMM"
       << setw(8)  << "CPU"
       << setw(12) << "MEM"
       << setw(8)  << "STAT"
       << setw(8)  << "USER"
       << setw(8)  << "START"
       << setw(8)  << "TH"
       << setw(8)  << "NAME"
       ;

    PrintLine(ss.str());
    touchwin(mWindow);
    wrefresh(mWindow);
}

void InfoWindow::PrintProcInfo(vector<ProcInfo> &procInfo)
{
    curs_set(0);
    for ( int i = 0; i < getmaxy(mWindow)-3; i++ ) {
        if ( i < procInfo.size() ) {
            mvPrintLine(i+1, 0, procInfo[i].procInfoToStr());
        }
        else {
            for( int j=0; j<getmaxx(mWindow); j++ ) {
                mvPrintLine(i+1, 0, "");
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