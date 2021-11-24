#include "window.h"
#include "command.h"
#include <vector>

class InfoWindow : public Window
{
public:
    InfoWindow(int endY, int endX, int begY, int begX);
    void printTitle(void) const;
    void printProcInfo(vector<ProcInfo> &procInfo) const;
    void windowClear(void) const;
};