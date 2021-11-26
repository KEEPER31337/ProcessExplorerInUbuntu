#include <ncurses.h>
#include <string>

#pragma once

class Window
{
public:
    Window(int endY, int endX, int begY, int begX);
    void   PrintSize          (void) const;
    WINDOW *GetWindow         (void) const;
    void   lineFeed           (void);
    void   printStr           (std::string s);
    void   lineClear          (void);
    void   mvPrintLine         (int y, int x, std::string s);
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