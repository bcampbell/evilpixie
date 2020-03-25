#ifndef CMD_H
#define CMD_H

#include "layer.h"
#include "project.h"
#include "point.h"
#include "img.h"
#include "brush.h"

#include <vector>

class Project;
class Cmd_PaletteModify;

class Cmd
{
public:
    enum CmdState { NOT_DONE, DONE };
    Cmd( Project& proj, CmdState initialstate=NOT_DONE ) :
        m_Proj( proj ),
        m_State( initialstate )
        {}
    virtual ~Cmd()
        {}

    virtual void Do() = 0;
    virtual void Undo() = 0;

    // cheesy RTTI for types that need it
    virtual Cmd_PaletteModify* ToPaletteModify() { return 0; }

    CmdState State() const
        { return m_State; }
    Project& Proj()
        { return m_Proj; }
protected:
    void SetState( CmdState s )
        { m_State=s; }
private:
    Project& m_Proj;
    CmdState m_State;
};


// A cmd to encapsulate an image modification which
// has already been applied to the project.
class Cmd_Draw : public Cmd
{
public:
    Cmd_Draw( Project& proj, NodePath const& target, Box const& affected, Img const& undoimg );
    virtual void Do();
    virtual void Undo();
private:
    NodePath m_Target;
    Img m_Img;
    Box m_Affected;
};



// Resize/clip a project
class Cmd_Resize : public Cmd
{
public:
    Cmd_Resize(Project& proj, Box const& new_area, int framefirst, int framelast, PenColour const& fillpen);
    virtual ~Cmd_Resize();
    virtual void Do();
    virtual void Undo();
private:
    void Swap();
    int m_First;
    int m_Last;
    Layer m_FrameSwap;
};


class Cmd_InsertFrames : public Cmd
{
public:
    Cmd_InsertFrames(Project& proj, int position, int numframes);
    virtual ~Cmd_InsertFrames();
    virtual void Do();
    virtual void Undo();
private:
    int m_Pos;
    int m_Num;
};

class Cmd_DeleteFrames : public Cmd
{
public:
    Cmd_DeleteFrames(Project& proj, int first, int last);
    virtual ~Cmd_DeleteFrames();
    virtual void Do();
    virtual void Undo();
private:
    int m_First;
    int m_Last;
    Layer m_FrameSwap;
};


class Cmd_ToSpriteSheet : public Cmd
{
public:
    Cmd_ToSpriteSheet(Project& proj, int maxAcross);
    virtual ~Cmd_ToSpriteSheet();
    virtual void Do();
    virtual void Undo();
private:
    int m_NumFrames;
    int m_NWide;
};

class Cmd_FromSpriteSheet : public Cmd
{
public:
    Cmd_FromSpriteSheet(Project& proj, int maxAcross, int numFrames);
    virtual ~Cmd_FromSpriteSheet();
    virtual void Do();
    virtual void Undo();
private:
    int m_NumFrames;
    int m_NWide;
};



// palette modification
class Cmd_PaletteModify : public Cmd
{
public:
    Cmd_PaletteModify(Project& proj, NodePath const& targ, int first, int cnt, Colour const* colours);
    virtual ~Cmd_PaletteModify();
    virtual void Do();
    virtual void Undo();

    // cheesy RTTI
    virtual Cmd_PaletteModify* ToPaletteModify() { return this; }

    bool Merge(NodePath const& targ, int idx, Colour const& newc);
private:
    void swap();
    NodePath m_Targ;
    int m_First;
    int m_Cnt;
    Colour* m_Colours;
};


class Cmd_Batch : public Cmd
{
public:
    Cmd_Batch( Project& proj, CmdState initialstate=NOT_DONE );
    virtual ~Cmd_Batch();
    virtual void Do();
    virtual void Undo();

    // add another command to this batch - must be in same state as overall batch!
    void Append(Cmd* c);

private:
    std::vector<Cmd*> m_Cmds;
};

class Cmd_NewLayer : public Cmd
{
public:
    Cmd_NewLayer(Project& proj, Layer* layer, int pos);
    virtual ~Cmd_NewLayer();
    virtual void Do();
    virtual void Undo();
private:
    Layer* m_Layer;
    int m_Pos;
};


#endif // CMD_H

