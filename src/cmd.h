#ifndef CMD_H
#define CMD_H

#include "point.h"
#include "img.h"
#include "brush.h"

class Project;

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
    Cmd_Draw( Project& proj, Box const& affected, IndexedImg const& undoimg );
    virtual void Do();
    virtual void Undo();
private:
    IndexedImg m_Img;
    Box m_Affected;
};


#if 0
class Cmd_PlonkBrush : public Cmd
{
public:
    Cmd_PlonkBrush( Project& proj, Point const& pos, Brush const& b, BrushStyle style, uint8_t pen );
    virtual void Do();
    virtual void Undo();
private:
    IndexedImg m_Img;
    Point m_Pos;
    int m_Transparent;  // -1 = none
    BrushStyle m_Style;
    uint8_t m_Pen;
};
#endif

#endif // CMD_H

