#include "State.h"

void MainCycle()
{
    KeyTrigger();
    if (F_Run)
    {
        ProgramRun();
        PositioningSignal();
        Motor_Set();
    }
}
