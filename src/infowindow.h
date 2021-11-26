#include "window.h"
#include "command.h"
#include <vector>
#include "processinfo.h"

class InfoWindow : public Window
{
public:
    InfoWindow(int endY, int endX, int begY, int begX);
    void PrintTitle(void);
    void PrintProcInfo(vector<ProcInfo> &procInfo);
    void WindowClear(void) const;
    void PrintLine(string s);
};