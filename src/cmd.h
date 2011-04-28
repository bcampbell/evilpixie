#ifndef CMD_H
#define CMD_H

#include "anim.h"
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
    Cmd_Draw( Project& proj, int frame, Box const& affected, IndexedImg const& undoimg );
    virtual void Do();
    virtual void Undo();
private:
    int m_Frame;
    IndexedImg m_Img;
    Box m_Affected;
};



// Resize/clip a project
class Cmd_Resize : public Cmd
{
public:
    Cmd_Resize(Project& proj, Box const& new_area);
    virtual ~Cmd_Resize();
    virtual void Do();
    virtual void Undo();
private:
    IndexedImg* m_Img;
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
    Anim m_FrameSwap;
};



#endif // CMD_H

