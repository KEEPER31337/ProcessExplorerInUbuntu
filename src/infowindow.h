#include "window.h"
#include "command.h"
#include <vector>

class InfoWindow : public Window
{
public:
    InfoWindow(int endY, int endX, int begY, int begX);
    void PrintTitle(void);
    void PrintProcInfo(vector<ProcInfo> &procInfo) const;
    void WindowClear(void) const;
    void PrintLine(string s);
};