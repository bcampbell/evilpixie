#ifndef TOOL_H
#define TOOL_H

#include "point.h"
#include "box.h"
#include "global.h" // for Button


enum ToolType
{
    TOOL_PENCIL=0,
    TOOL_LINE,
    TOOL_BRUSH_PICKUP,
    TOOL_FLOODFILL,
    TOOL_RECT,
    TOOL_CIRCLE,
    TOOL_FILLEDRECT,
    TOOL_FILLEDCIRCLE,
    TOOL_EYEDROPPER,
    NUM_TOOLS
};


class EditView;
class Project;
class Editor;
class DrawTransaction;
class Cmd_Batch;


class Tool
{
public:
	Tool( Editor& owner ) : m_Owner(owner)
		{}
	virtual ~Tool()
		{}

	// all points passed in as project coords
	virtual void OnDown( EditView& view, Point const& p, Button b ) = 0;
	virtual void OnMove( EditView& view, Point const& p ) = 0;
	virtual void OnUp( EditView& view, Point const& p, Button b ) = 0;
	virtual void DrawCursor( EditView& view )=0;
    virtual bool ObeyGrid() { return true; }
protected:
    Editor& Owner() { return m_Owner; }
private:
    Editor& m_Owner;
};


class PencilTool : public Tool
{
public:
	PencilTool( Editor& owner );
	~PencilTool();
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
	virtual void DrawCursor( EditView& view );
private:
    static void Plot_cb( int x, int y, void* user );
	Point m_Pos;
	Button m_DownButton;
    EditView* m_View;

    DrawTransaction *m_Tx;
};


class LineTool : public Tool
{
public:
    LineTool( Editor& owner );
    virtual ~LineTool();
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
    virtual void DrawCursor( EditView& view );
private:
    static void Plot_cb( int x, int y, void* user );
    static void PlotCursor_cb( int x, int y, void* user );
    Point m_From;
    Point m_To;
    Button m_DownButton;
    EditView* m_View;
    Box m_CursorDamage;
    DrawTransaction *m_Tx;
};



class BrushPickupTool : public Tool
{
public:
    BrushPickupTool( Editor& owner );
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
    virtual void DrawCursor( EditView& view );
private:
    static void Plot_cb( int x, int y, void* user );
    static void PlotCursor_cb( int x, int y, void* user );

    Point m_Anchor;
    Point m_DragPoint;
    Button m_DownButton;
    EditView* m_View;
};



class FloodFillTool : public Tool
{
public:
	FloodFillTool( Editor& owner );
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
	virtual void DrawCursor( EditView& view );
    virtual bool ObeyGrid() { return false; }
private:
    Point m_Pos;
};


class RectTool : public Tool
{
public:
    RectTool( Editor& owner );
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
    virtual void DrawCursor( EditView& view );
private:
    static void Plot_cb( int x, int y, void* user );
    static void PlotCursor_cb( int x, int y, void* user );
    Point m_From;
    Point m_To;
    Button m_DownButton;
    EditView* m_View;
    Box m_CursorDamage;
};


class FilledRectTool : public Tool
{
public:
    FilledRectTool( Editor& owner );
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
    virtual void DrawCursor( EditView& view );
private:
    static void Plot_cb( int x, int y, void* user );
    static void PlotCursor_cb( int x, int y, void* user );
    Point m_From;
    Point m_To;
    Button m_DownButton;
    EditView* m_View;
    Box m_CursorDamage;
};


class CircleTool : public Tool
{
public:
    CircleTool( Editor& owner );
    virtual ~CircleTool();
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
    virtual void DrawCursor( EditView& view );
private:
    static void Plot_cb( int x, int y, void* user );
    static void PlotCursor_cb( int x, int y, void* user );
    Point m_From;
    Point m_To;
    Button m_DownButton;
    EditView* m_View;
    Box m_CursorDamage;
    DrawTransaction *m_Tx;
};



class FilledCircleTool : public Tool
{
public:
    FilledCircleTool( Editor& owner );
    virtual ~FilledCircleTool();
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
    virtual void DrawCursor( EditView& view );
private:
    static void Draw_hline_cb( int x0, int x1, int y, void* user );
    static void Cursor_hline_cb( int x0, int x1, int y, void* user );
    Point m_From;
    Point m_To;
    Button m_DownButton;
    EditView* m_View;
    Box m_CursorDamage;
    DrawTransaction *m_Tx;
};


class EyeDropperTool : public Tool
{
public:
    EyeDropperTool( Editor& owner, ToolType previoustooltype=TOOL_PENCIL );
    virtual ~EyeDropperTool();
	virtual void OnDown( EditView& view, Point const& p, Button b );
	virtual void OnMove( EditView& view, Point const& p);
	virtual void OnUp( EditView& view, Point const& p, Button b );
    virtual void DrawCursor( EditView& view );
    virtual bool ObeyGrid() { return false; }
private:
    EditView* m_View;
    Box m_CursorDamage;
    ToolType m_PreviousToolType;
};

#endif

