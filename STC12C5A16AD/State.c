#include "State.h"

void MainCycle()
{
    DATA_CL();
    if (F_Run)
    {
        ProgramRun();
        PositioningSignal();
        Motor_Set();
    }
    if (T_YZ)
    {
        SendData();
    }
    else
    {
        KeyTrigger();
    }
}
