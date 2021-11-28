#include "infowindow.h"
#include "cmdwindow.h"
#include "command.h"
#include <unistd.h>
#include <thread>
#include <mutex>

#define TITLE_COLOR_PAIR 1

using namespace std;

thread *infoWinThread;
thread *cmdWinThread;

InfoWindow *infoWindow;
CmdWindow *cmdWindow;
Command *cmd;

mutex *mutPrintScr;
mutex *mutGetch;

void InitWindow(void)
{
    initscr();
    start_color();
    // color setting
    init_pair(TITLE_COLOR_PAIR, COLOR_BLACK, COLOR_WHITE);

    mutPrintScr = new mutex;
    mutGetch = new mutex;

    cmd = new Command();
    infoWindow = new InfoWindow(getmaxy(stdscr) - 11, getmaxx(stdscr), 0, 0);
    cmdWindow = new CmdWindow(9, getmaxx(stdscr), getmaxy(stdscr) - 9, 0, cmd);
    
    wattron(infoWindow->GetWindow(), COLOR_PAIR(TITLE_COLOR_PAIR) );
    infoWindow->PrintTitle();
    wattroff(infoWindow->GetWindow(), COLOR_PAIR(TITLE_COLOR_PAIR) );

    for(int i=0; i<getmaxx(stdscr); i++)
        mvwaddch(stdscr, getmaxy(stdscr)-10, i, '-');
    touchwin(stdscr);
    wrefresh(stdscr); 
}

void InfoWindowThreadFunc(void)
{
    while ( true ) {
        switch( cmd->GetMode() )
        {
        case Command::Mode::PRINTPROCINFO:
            cmd->UpdateProcStat();
            mutPrintScr->lock();
            infoWindow->PrintProcInfo(cmd->GetProcInfos());
            mutGetch->unlock();
            break;
        case Command::Mode::CLEAR:
            mutPrintScr->lock();
            infoWindow->WindowClear();
            mutGetch->unlock();
            break;
        case Command::Mode::EXIT:
            return;
        default:
            break;
        }
    }
}

void CmdWindowThreadFunc(void)
{
    cmdWindow->StartShell(*mutPrintScr, *mutGetch);
}

void EndWindow(void)
{
    endwin();
}

void Working(void)
{
    infoWinThread = new thread(InfoWindowThreadFunc);
    cmdWinThread = new thread(CmdWindowThreadFunc);
    
    cmdWinThread->join();
    infoWinThread->join();
}

int main(void)
{
    InitWindow();
    Working();
    EndWindow();
}