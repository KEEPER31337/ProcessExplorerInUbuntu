#include <ncurses.h>
#pragma once

class Window
{
public:
    Window(int endY, int endX, int begY, int begX);
    void PrintSize(void) const;
    WINDOW *GetWindow(void) const;
    /*
        no more need
    void drawBorder(void) const;
    void drawFrame(int endY, int endX, int begY, int begX) const;
    */
protected:
    WINDOW *mWindow;

private:
    
    int mBegX;
    int mBegY;
    int mEndX;
    int mEndY;
};