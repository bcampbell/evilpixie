#ifndef CMD_REMAP_H
#define CMD_REMAP_H

#include "cmd.h"

class Layer;

// Change format of a layer, using the given palette.
class Cmd_Remap : public Cmd
{
public:
    Cmd_Remap(Project& proj, NodePath const& target, PixelFormat newFmt, Palette const& destPalette);
    virtual ~Cmd_Remap();
    virtual void Do();
    virtual void Undo();
private:
    void Swap();
    NodePath m_Target;
    Layer* m_Other;
};

#endif // CMD_CHANGEFMT_H

