#include "infowindow.h"
#include "cmdwindow.h"
#include "command.h"
#include <unistd.h>
#include <thread>
#include <mutex>

InfoWindow *infoWindow;
CmdWindow *cmdWindow;
Command *cmd;
char buf[1024];

std::mutex *mutPrintScr;
std::mutex *mutGetch;

void InitWindow(void)
{
    initscr();
    start_color();

    mutPrintScr = new std::mutex;
    mutGetch = new std::mutex;

    infoWindow = new InfoWindow(getmaxy(stdscr) - 11, getmaxx(stdscr), 0, 0);
    cmdWindow = new CmdWindow(9, getmaxx(stdscr), getmaxy(stdscr) - 9, 0);
    cmd = new Command();

    infoWindow->PrintTitle();
    for(int i=0; i<getmaxx(stdscr); i++)
        mvwaddch(stdscr, getmaxy(stdscr)-10, i, '-');
    touchwin(stdscr);
    wrefresh(stdscr); 
}

void InfoWindowThreadFunc(void)
{
    while (true)
    {
        switch(cmd->GetMode())
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
    infoWindow->PrintSize();
    cmdWindow->PrintSize();
}

void Working(void)
{
    std::thread infoWinThread(InfoWindowThreadFunc);
    std::thread cmdWinThread(CmdWindowThreadFunc);

    infoWinThread.join();
    cmdWinThread.join();
}

int main(void)
{
    InitWindow();
    Working();
    EndWindow();
}