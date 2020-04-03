#ifndef CMD_CHANGEFMT_H
#define CMD_CHANGEFMT_H

#include "cmd.h"

class Layer;

// Change format of a layer
class Cmd_ChangeFmt : public Cmd
{
public:
    Cmd_ChangeFmt(Project& proj, NodePath const& target, PixelFormat newFmt, int nColours);
    virtual ~Cmd_ChangeFmt();
    virtual void Do();
    virtual void Undo();
private:
    void Swap();
    NodePath m_Target;
    Layer* m_Other;
};

#endif // CMD_CHANGEFMT_H

