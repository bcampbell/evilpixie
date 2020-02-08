#ifndef CMD_CHANGEFMT_H
#define CMD_CHANGEFMT_H

#include "cmd.h"

// Change format of a project
class Cmd_ChangeFmt : public Cmd
{
public:
    Cmd_ChangeFmt(Project& proj, int layerNum, PixelFormat newFmt, int nColours);
    virtual ~Cmd_ChangeFmt();
    virtual void Do();
    virtual void Undo();
private:
    void Swap();
    int m_LayerNum;
    int m_First;
    int m_Last;
    Layer m_FrameSwap;
};

#endif // CMD_CHANGEFMT_H

