#include "editor.h"
#include "editview.h"
#include "tool.h"
#include "brush.h"
#include "project.h"
#include "app.h"

#include <cassert>
#include <stdint.h>
#include <cstdio>



Editor::Editor( Project* proj ) :
    m_SaveBGAsTransparent(true),
    m_Tool(0),
    m_Brush(0),
    m_Project( proj ),
    m_GridActive(false),
    m_Grid( 0,0,8,8 )
{
    m_Tool = new PencilTool(*this);
    m_Project->AddListener(this);


    m_BGPen = PenColour(proj->GetColour(0),0);
    m_FGPen = PenColour(proj->GetColour(1),1);
}


Editor::~Editor()
{
    m_Project->RemoveListener(this);

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

