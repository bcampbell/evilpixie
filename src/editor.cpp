#include "editor.h"
#include "editview.h"
#include "tool.h"
#include "brush.h"
#include "project.h"
#include "app.h"
#include "cmd.h"

#include <cassert>
#include <stdint.h>
#include <cstdio>



Editor::Editor( Project* proj ) :
    m_SaveBGAsTransparent(true),
    m_Tool(0),
    m_Mode(DrawMode::DM_NORMAL),
    m_Brush(0),
    m_Project( proj ),
    m_GridActive(false),
    m_Grid( 0,0,8,8 )
{
    m_Tool = new PencilTool(*this);
    m_Project->AddListener(this);

    Palette const& pal = Proj().PaletteConst();
    m_BGPen = PenColour(pal.GetColour(0), 0);
    m_FGPen = PenColour(pal.GetColour(1), 1);
}


Editor::~Editor()
{
    m_Project->RemoveListener(this);
    DiscardUndoAndRedos();

    // ugliness - tool dtor might call Editor::SetMouseStyle()
    // we really want it to call the one in the derived (GUI-specific) class
    // but here in ~Editor() we'll get Editor::SetMouseStyle() instead...
    delete m_Tool;
    delete m_Project;
}

void Editor::UseTool( int tooltype, bool notifygui )
{
    HideToolCursor();

    delete m_Tool;
    m_Tool = 0;
    ToolType prev = (ToolType)m_CurrentToolType;
    m_CurrentToolType = tooltype;
    switch( (ToolType)tooltype )
    {
    case TOOL_PENCIL:
        m_Tool = new PencilTool(*this);
        break;
    case TOOL_LINE:
        m_Tool = new LineTool(*this);
        break;
    case TOOL_BRUSH_PICKUP:
        m_Tool = new BrushPickupTool(*this);
        break;
    case TOOL_FLOODFILL:
        m_Tool = new FloodFillTool(*this);
        break;
    case TOOL_RECT:
        m_Tool = new RectTool(*this);
        break;
    case TOOL_CIRCLE:
        m_Tool = new CircleTool(*this);
        break;
    case TOOL_FILLEDRECT:
        m_Tool = new FilledRectTool(*this);
        break;
    case TOOL_FILLEDCIRCLE:
        m_Tool = new FilledCircleTool(*this);
        break;
    case TOOL_EYEDROPPER:
        m_Tool = new EyeDropperTool(*this,prev);
        break;
    default:
        assert(false);  // BAD TOOL!
        // uh-oh...
        break;
    }
    assert(m_Tool != 0 );

    ShowToolCursor();

    if( notifygui )
        OnToolChanged();
}

Brush& Editor::CurrentBrush()
{
    if( m_Brush == -1 )
    {
        assert( g_App->CustomBrush() !=0 );
        return *g_App->CustomBrush();
    }

    return *g_App->StdBrush( m_Brush );
}


void Editor::SetBrush( int n )
{
    HideToolCursor();
    m_Brush = n;
    OnBrushChanged();
    ShowToolCursor();
}




void Editor::ShowToolCursor()
{
    if( !m_Tool )
        return;

    std::set<EditView*>::iterator it;
    for( it=m_Views.begin(); it!=m_Views.end(); ++it )
    {
        m_Tool->DrawCursor( *(*it) );
    }
}


void Editor::HideToolCursor()
{
    std::set<EditView*>::iterator it;
    for( it=m_Views.begin(); it!=m_Views.end(); ++it )
    {
        (*it)->EraseCursor();
//        tool.EraseCursor( *(*it) );
    }
}


void Editor::GridSnap( Point& p )
{
    if( !GridActive() )
        return;
    Box const& grid = m_Grid;

    p.x += grid.W()/2;
    p.x -= p.x % grid.W();
    p.x += grid.x;

    p.y += grid.H()/2;
    p.y -= p.y % grid.H();
    p.y += grid.y;
}


void Editor::SetFGPen( PenColour const& pen )
{
    m_FGPen=pen;
    OnPenChanged();
}


void Editor::SetBGPen( PenColour const& pen )
{
    m_BGPen = pen;
    OnPenChanged();
}



// advance the FG pen to the next palette index (if sane)
void Editor::NextFGPen()
{
    if (!m_FGPen.IdxValid()) {
        return;
    }

    Palette const& pal = Proj().PaletteConst();
    int idx = m_FGPen.idx()+1;
    if (idx<pal.NumColours()) {
        SetFGPen( PenColour( pal.GetColour(idx), idx) );
    }
}

// move the FG pen back a palette index (if sane)
void Editor::PrevFGPen()
{
    if (!m_FGPen.IdxValid()) {
        return;
    }
    Palette const& pal = Proj().PaletteConst();
    int idx = m_FGPen.idx()-1;
    if (idx>=0) {
        SetFGPen( PenColour( pal.GetColour(idx), idx) );
    }
}



// Adds a command to the undo stack, and calls its Do() fn
void Editor::AddCmd( Cmd* cmd )
{
    const int maxundos = 128;

    m_UndoStack.push_back( cmd );
    if( cmd->State() == Cmd::NOT_DONE )
        cmd->Do();

    // adding a new command renders the redo stack obsolete.
    while( !m_RedoStack.empty() )
    {
        delete m_RedoStack.back();
        m_RedoStack.pop_back();
    }

    // limit amount of undos to something reasonable.
    int trimcount = m_UndoStack.size() - maxundos;
    while( trimcount > 0 )
    {
        delete m_UndoStack.front();
        m_UndoStack.pop_front();
        --trimcount;
    }

    m_Project->SetModifiedFlag( true );
    OnUndoRedoChanged();
}


void Editor::Undo()
{
    if( m_UndoStack.empty() )
    {
        return;
    }
//    HideToolCursor();

    Cmd* cmd = m_UndoStack.back();
    m_UndoStack.pop_back();
    cmd->Undo();
    m_RedoStack.push_back( cmd );

    OnUndoRedoChanged();
//    ShowToolCursor();
}

void Editor::Redo()
{
    if( m_RedoStack.empty() )
        return;
//    HideToolCursor();
    Cmd* cmd = m_RedoStack.back();
    m_RedoStack.pop_back();
    cmd->Do();
    m_UndoStack.push_back( cmd );

    OnUndoRedoChanged();
//    ShowToolCursor();
}


void Editor::DiscardUndoAndRedos()
{
//    bool stacksempty = m_UndoStack.empty() && m_RedoStack.empty();

    while( !m_UndoStack.empty() )
    {
        delete m_UndoStack.back();
        m_UndoStack.pop_back();
    }
    while( !m_RedoStack.empty() )
    {
        delete m_RedoStack.back();
        m_RedoStack.pop_back();
    }

    /* pointless - editor is going away anyway!
    if( !stacksempty )
    {
        OnUndoRedoChanged();
    }
    */
}




