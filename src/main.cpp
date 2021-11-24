#include "infowindow.h"
#include "cmdwindow.h"
#include "command.h"
#include <unistd.h>
#include <thread>
//#include <semaphore>

InfoWindow *infoWindow;
CmdWindow *cmdWindow;
Command *cmd;
char buf[1024];

//std::binary_semaphore semaPrintScreen(0);

void initWindow(void)
{
    initscr();
    start_color();

    infoWindow = new InfoWindow(getmaxy(stdscr) - 11, getmaxx(stdscr), 0, 0);
    cmdWindow = new CmdWindow(9, getmaxx(stdscr), getmaxy(stdscr) - 9, 0);
    cmd = new Command();

    infoWindow->printTitle();
    for(int i=0; i<getmaxx(stdscr); i++)
        mvwaddch(stdscr, getmaxy(stdscr)-10, i, '-');
    touchwin(stdscr);
    wrefresh(stdscr);
    
}

void infoWindowThreadFunc(void)
{
    /*
    while (true)
    {
        switch(cmd->getMode())
        {
        case Command::Mode::PRINTPROCINFO:
            cmd->updateProcStat();
            infoWindow->printProcInfo(cmd->getProcInfo());
            break;
        case Command::Mode::CLEAR:
            infoWindow->windowClear();
            break;
        default:
            break;
        }
    }
    */
}

void cmdWindowThreadFunc(void)
{
    cmdWindow->startShell();
}

void endWindow(void)
{
    endwin();
    infoWindow->printSize();
    cmdWindow->printSize();
}

void working(void)
{
    std::thread infoWinThread(infoWindowThreadFunc);
    std::thread cmdWinThread(cmdWindowThreadFunc);

    infoWinThread.join();
    cmdWinThread.join();
}

int main(void)
{
    initWindow();
    working();
    endWindow();
}