#include "tool.h"
#include "app.h"
#include "project.h"
#include "editor.h"
#include "editview.h"
#include "cmd.h"
#include "global.h"
#include "brush.h"

#include <algorithm>    // for min,max
#include <vector>


// helper
void line(int x0, int y0, int x1, int y1, void (*plot)(int x, int y, void* user ), void* userdata )
{
    int dy = y1-y0;
    int dx = x1-x0;
    int xinc;
    int yinc;

    if( dx<0 )
        { xinc=-1; dx=-dx; }
    else
        { xinc=1; }
    dx*=2;

    if( dy<0 )
        { dy=-dy; yinc=-1; }
    else
        { yinc = 1; }
    dy*=2;

    plot( x0, y0, userdata );
    if( dx>dy )
    {
        // step along x axis
        int f = dy-(dx/2);
        while (x0 != x1)
        {
            if( f>=0 )
            {
                y0 += yinc;
                f -= dx;
            }
            f += dy;
            x0 += xinc;
            plot( x0, y0, userdata );
        }
    }
    else
    {
        // step along y axis
        int f = dx-(dy/2);
        while( y0 != y1 )
        {
            if (f >= 0)
            {
                x0 += xinc;
                f -= dy;
            }
            f += dx;
            y0 += yinc;
            plot( x0, y0, userdata );
        }
    }
}


//
// Adapted from:
// http://willperone.net/Code/ellipse.php
//
void EllipseBresenham(int xc, int yc, int r1, int r2,
    void (*drawpixel)(int x, int y, void* user ), void* userdata )
{
	int x= 0, y= r2, 
		a2= r1*r1, b2= r2*r2, 
		S, T;

	S = a2*(1-2*r2) + 2*b2;
	T = b2 - 2*a2*(2*r2-1);
	drawpixel(xc-x, yc-y, userdata);
	drawpixel(xc+x, yc+y, userdata);
	drawpixel(xc-x, yc+y, userdata);
	drawpixel(xc+x, yc-y, userdata);	
	do {
		if (S < 0)
		{
			S += 2*b2*(2*x + 3);
			T += 4*b2*(x + 1);
			x++;
		} else 
		if (T < 0)
		{
			S += 2*b2*(2*x + 3) - 4*a2*(y - 1);
			T += 4*b2*(x + 1) - 2*a2*(2*y - 3);
			x++;
			y--;
		} else {
			S -= 4*a2*(y - 1);
			T -= 2*a2*(2*y - 3);
			y--;
		}
		drawpixel(xc-x, yc-y, userdata);
		drawpixel(xc+x, yc+y, userdata);
		drawpixel(xc-x, yc+y, userdata);
		drawpixel(xc+x, yc-y, userdata);
	} while (y > 0);
}


void FilledEllipseBresenham(int xc, int yc, int r1, int r2,
    void (*drawhline)(int x0, int x1, int y, void* user ), void* userdata )
{
	int x= 0, y= r2, 
		a2= r1*r1, b2= r2*r2, 
		S, T;

	S = a2*(1-2*r2) + 2*b2;
	T = b2 - 2*a2*(2*r2-1);
    drawhline( xc-x, xc+x, yc-y, userdata );
    drawhline( xc-x, xc+x, yc+y, userdata );
	do {
		if (S < 0)
		{
			S += 2*b2*(2*x + 3);
			T += 4*b2*(x + 1);
			x++;
		} else 
		if (T < 0)
		{
			S += 2*b2*(2*x + 3) - 4*a2*(y - 1);
			T += 4*b2*(x + 1) - 2*a2*(2*y - 3);
			x++;
			y--;
		} else {
			S -= 4*a2*(y - 1);
			T -= 2*a2*(2*y - 3);
			y--;
		}
        drawhline( xc-x, xc+x, yc-y, userdata );
        drawhline( xc-x, xc+x, yc+y, userdata );
	} while (y > 0);
}

// helper for drawing cursor (using FG pen)
void PlonkBrushToViewFG( EditView& view, Point const& pos, Box& viewdmg )
{
    Brush& b = view.Ed().CurrentBrush();
    Box pb( pos-b.Handle(), b.W(), b.H() );

    viewdmg = view.ProjToView( pb );

    int maskcolour = -1;
    if( b.Style() == MASK )
        maskcolour = view.Proj().FGPen();

    BlitZoomIndexedToRGBx( b, b.Bounds(),
        view.Canvas(), viewdmg,
        view.Proj().GetPaletteConst(),
        view.Zoom(),
        b.TransparentColour(),
        maskcolour );
}

// helper for drawing cursor (using BG colour, _overriding_ any colour
// in the brush)
void PlonkBrushToViewBG( EditView& view, Point const& pos, Box& viewdmg )
{
    Brush& b = view.Ed().CurrentBrush();
    Box pb( pos-b.Handle(), b.W(), b.H() );

    viewdmg = view.ProjToView( pb );

    int maskcolour = view.Proj().BGPen();

    BlitZoomIndexedToRGBx( b, b.Bounds(),
        view.Canvas(), viewdmg,
        view.Proj().GetPaletteConst(),
        view.Zoom(),
        b.TransparentColour(),
        maskcolour );
}


void PlonkBrushToView( EditView& view, Point const& pos, Box& viewdmg, Button button )
{
    if( button==DRAW )
        PlonkBrushToViewFG( view,pos,viewdmg );
    else if( button==ERASE )
        PlonkBrushToViewBG( view,pos,viewdmg );
}



// helper to draw the current brush on the project
void PlonkBrushToProj( Editor& ed, Point const& pos, Box& projdmg, Button button )
{
    Project& proj = ed.Proj();
    Brush const& brush = ed.CurrentBrush();
    Box dmg(
        pos.x - brush.Handle().x,
        pos.y - brush.Handle().y,
        0,0);
    if( button == DRAW )
    {
        int maskcolour = -1;
        if( brush.Style() == MASK )
            maskcolour = proj.FGPen();

        BlitIndexed( brush, brush.Bounds(),
            proj.Img(), dmg,
            brush.TransparentColour(), maskcolour );
    }
    else if( button == ERASE )
    {
        BlitIndexed( brush, brush.Bounds(),
            proj.Img(), dmg,
            brush.TransparentColour(), proj.BGPen() );
    }

    projdmg=dmg;
}


// helper - draw a crosshair cursor, centred on the given point (project coords)
void DrawCrossHairCursor( EditView& view, Point const& centre, RGBx const& c )
{
    Box const& bnd = view.Canvas().Bounds();
    int zoom = view.Zoom();
    Point p = view.ProjToView(centre);

    Box hbox( bnd.Left(), p.y, bnd.W(), 1*zoom );
    view.Canvas().FillBox( c,hbox );
    view.AddCursorDamage( hbox );

    Box vbox( p.x, bnd.Top(), 1*zoom, bnd.H() );
    view.Canvas().FillBox( c,vbox );
    view.AddCursorDamage( vbox );
}


// helper
void FloodFill( IndexedImg& img, Point const& start, int newcolour, Box& damage )
{
    damage.SetEmpty();
    int oldcolour = img.GetPixel( start );
    if( oldcolour == newcolour )
        return;

    std::vector< Point > q;
    q.push_back( start );
    while( !q.empty() )
    {
        Point pt = q.back();
        q.pop_back();
        if( img.GetPixel(pt) != oldcolour )
            continue;

        // scan left and right to find span
        int y = pt.y;
        int l = pt.x;
        while( l>0 && img.GetPixel( Point(l-1,y)) == oldcolour )
            --l;
        int r = pt.x;
        while( r<img.W()-1 && img.GetPixel( Point(r+1,y)) == oldcolour )
            ++r;

        // fill the span
        uint8_t* dest = img.Ptr( l,y );
        int x;
        for( x=l; x<=r; ++x )
            *dest++ = newcolour;
        // expand the damage box to include the affected span
        damage.Merge( Box( l,y, (r+1)-l,1) );
        // add pixels above the span to the queue 
        y = pt.y-1;
        if( y>=0 )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.GetPixel(x,y) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }

        // add pixels below the span to the queue
        y = pt.y+1;
        if( y<img.H() )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.GetPixel(x,y) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }
    }
}





//------------------------------

PencilTool::PencilTool( Editor& owner ) :
    Tool( owner ),
    m_Pos(0,0),
    m_DownButton(NONE),
    m_View(0)
{
}


void PencilTool::OnDown( EditView& view, Point const& p, Button b )
{
//    printf("tool::down(%d)\n",b );
    m_Pos = p;
    if( m_DownButton!=NONE )
        return;

    m_DownButton = b;
    m_View = &view;
    view.Proj().Draw_Begin( this );

    // draw 1st pixel
    Plot_cb( p.x, p.y, (void*)this );
}

void PencilTool::OnMove( EditView&, Point const& p)
{
    if( m_DownButton == NONE )
    {
        m_Pos = p;  // for cursor
        return;
    }

    // feels 'wrong' to do continuous lines if grid is on...
    if( Owner().GridActive() )
        Plot_cb( p.x, p.y, (void*)this );
    else
    {
        // TODO: should not draw first point (m_Pos) - it's already been drawn.
        line( m_Pos.x, m_Pos.y, p.x, p.y, Plot_cb, this );
    }
    m_Pos = p;
}

void PencilTool::OnUp( EditView& view, Point const& p, Button b )
{
    if( m_DownButton != b )
        return;

    m_Pos = p;
    m_DownButton = NONE;
    m_View = 0;
    view.Proj().Draw_Commit();
}

void PencilTool::DrawCursor( EditView& view )
{
    if( m_DownButton != NONE )
        return;
    Box viewdmg;
    PlonkBrushToViewFG( view, m_Pos, viewdmg );
    view.AddCursorDamage( viewdmg );
}

//static
void PencilTool::Plot_cb( int x, int y, void* user )
{

    PencilTool* that = (PencilTool*)user;
    Editor& ed = that->Owner();
    Brush const& brush = ed.CurrentBrush();
    Project& proj = that->m_View->Proj();

    Box dmg(
        x - brush.Handle().x,
        y - brush.Handle().y,
        0,0);
    int maskcolour = -1;
    if( that->m_DownButton == DRAW )
    {
        if( brush.Style() == MASK )
            maskcolour = proj.FGPen();
    }
    else if( that->m_DownButton == ERASE )
    {
        maskcolour = proj.BGPen();
    }

    BlitIndexed( brush, brush.Bounds(),
        proj.Img(), dmg,
        brush.TransparentColour(), maskcolour );
    proj.Draw_Damage( dmg );
}


//------------------------------


LineTool::LineTool( Editor& owner ) :
    Tool( owner ),
    m_From(0,0),
    m_To(0,0),
    m_DownButton(NONE),
    m_View(0)
{
}


void LineTool::OnDown( EditView& view, Point const& p, Button b )
{
//    printf("tool::down(%d)\n",b );
    if( m_DownButton!=NONE )
        return;
    m_View = &view;

    m_DownButton = b;
    m_From = p;
    m_To = p;
}

void LineTool::OnMove( EditView&, Point const& p)
{
    if( m_DownButton == NONE )
        m_From = p;     // needed to draw cursor
    else
        m_To = p;       // dragging for end of line
}

void LineTool::OnUp( EditView& view, Point const& p, Button b )
{
    if( m_DownButton != b )
        return;

    m_To = p;

    view.Proj().Draw_Begin(this);
    line( m_From.x, m_From.y, m_To.x, m_To.y, Plot_cb, this );
    view.Proj().Draw_Commit();
    m_DownButton = NONE;
    m_View = 0;
    m_From = m_To;
}

//static
void LineTool::Plot_cb( int x, int y, void* user )
{
    LineTool* that = (LineTool*)user;
    Editor& ed = that->Owner();
    Brush const& brush = ed.CurrentBrush();
    Project& proj = that->m_View->Proj();

    Box dmg(
        x - brush.Handle().x,
        y - brush.Handle().y,
        0,0);
    int maskcolour = -1;
    if( that->m_DownButton == DRAW )
    {
        if( brush.Style() == MASK )
            maskcolour = proj.FGPen();
    }
    else if( that->m_DownButton == ERASE )
    {
        maskcolour = proj.BGPen();
    }

    BlitIndexed( brush, brush.Bounds(),
        proj.Img(), dmg,
        brush.TransparentColour(), maskcolour );
    proj.Draw_Damage( dmg );
}



void LineTool::DrawCursor( EditView& view )
{
    m_CursorDamage.SetEmpty();
    if( m_DownButton == NONE )
    {
        PlonkBrushToViewFG( view, m_From, m_CursorDamage );
    }
    else
    {
        line( m_From.x, m_From.y, m_To.x, m_To.y, PlotCursor_cb, this );
    }
    view.AddCursorDamage( m_CursorDamage );
}


//static
void LineTool::PlotCursor_cb( int x, int y, void* user )
{
    Point pos(x,y);

    LineTool* that = (LineTool*)user;
    EditView& view = *that->m_View;
    Editor& ed = that->Owner();
    Brush const& b = ed.CurrentBrush();
//    Project& proj = view.Proj();

    Box pb( Point(x,y)-b.Handle(), b.W(), b.H() );

    Box dmg;
    PlonkBrushToView( view, pos, dmg, that->m_DownButton );
    that->m_CursorDamage.Merge( dmg );
}


//------------------------------


BrushPickupTool::BrushPickupTool( Editor& owner ) :
    Tool( owner ),
    m_Anchor(0,0),
    m_DragPoint(0,0),
    m_DownButton(NONE),
    m_View(0)
{
}



void BrushPickupTool::OnDown( EditView&, Point const& p, Button b )
{
    if( m_DownButton!=NONE )
        return;
    m_DownButton = b;
    m_Anchor = p;
    m_DragPoint = p;
}

void BrushPickupTool::OnMove( EditView&, Point const& p)
{
    m_DragPoint = p;
}

void BrushPickupTool::OnUp( EditView& view, Point const& p, Button )
{
    Project& proj = view.Proj();

    if( m_DownButton == NONE )
        return;
    m_DragPoint = p;


    bool erase = (m_DownButton == ERASE);
    m_DownButton = NONE;

    Box pickup( m_Anchor, m_DragPoint );
    pickup.ClipAgainst( proj.Img().Bounds() );

    if( pickup.Empty() )
        return;

    Brush* brush = new Brush( FULLCOLOUR, pickup.W(), pickup.H(), 0, proj.BGPen() );

    // copy in palette
    brush->SetPalette( proj.GetPalette() );

    if( Owner().GridActive() )
        brush->SetHandle( Point(0,0) );


    Box bb(0,0,0,0);
    BlitIndexed( proj.Img(), pickup,
        *brush, bb, -1, -1 );
    g_App->SetCustomBrush( brush );
    Owner().SetBrush( -1 );

    // if picking up with right button, erase area after pickup
    if( erase )
    {
        proj.Draw_Begin(this);
        proj.Img().FillBox( proj.BGPen(), pickup );
        proj.Draw_Damage( pickup );
        proj.Draw_Commit();
    }

    // NOTE: this is effectively "delete this!"
    Owner().UseTool( TOOL_PENCIL );
    // this ptr no longer valid...
}

void BrushPickupTool::DrawCursor( EditView& view )
{
    if( m_DownButton == NONE )
    {
        DrawCrossHairCursor( view, m_DragPoint, RGBx(255,255,255) );
        return;
    }

    Box pickup( m_Anchor, m_DragPoint );
    pickup.ClipAgainst( view.Proj().Img().Bounds() );

    Box vb = view.ProjToView( pickup );
    RGBx white( RGBx(255,255,255) );
    RGBx black( RGBx(0,0,0) );
    view.Canvas().OutlineBox( white,vb );
    vb.Expand(-1);
    view.Canvas().OutlineBox( black,vb );
    vb.Expand(1);
    view.AddCursorDamage( vb );
}




//------------------------------


FloodFillTool::FloodFillTool( Editor& owner ) :
    Tool(owner),
    m_Pos(0,0)
{
}



void FloodFillTool::OnDown( EditView& view, Point const& p, Button b )
{
    Project& proj = view.Proj();

    if( b != DRAW && b != ERASE )
        return;
    if( !proj.Img().Bounds().Contains(p ) )
        return;

    int fillcolour = proj.FGPen();
    if( b == ERASE )
        fillcolour = proj.BGPen();

    Box dmg;
    proj.Draw_Begin( this );
    FloodFill( proj.Img(), p, fillcolour, dmg );
    if( dmg.Empty() )
    {
        // no pixels changed.
        proj.Draw_Rollback();
    }
    else
    {
        proj.Draw_Damage( dmg );
        proj.Draw_Commit();
    }
}

void FloodFillTool::OnMove( EditView& , Point const& p)
{
    m_Pos = p;
}

void FloodFillTool::OnUp( EditView& , Point const& , Button )
{
}

void FloodFillTool::DrawCursor( EditView& view )
{
    Box c( m_Pos, 1,1 );
    Box vb = view.ProjToView( c );
    RGBx white( RGBx(255,255,255) );
    RGBx black( RGBx(0,0,0) );
    view.Canvas().OutlineBox( white,vb );
    vb.Expand(-1);
    view.Canvas().OutlineBox( black,vb );
    vb.Expand(1);
    view.AddCursorDamage( vb );
}


//------------------------------


RectTool::RectTool( Editor& owner ) :
    Tool( owner ),
    m_From(0,0),
    m_To(0,0),
    m_DownButton(NONE)
{
}


void RectTool::OnDown( EditView& , Point const& p, Button b )
{
//    printf("tool::down(%d)\n",b );
    if( m_DownButton!=NONE )
        return;
    m_DownButton = b;
    m_From = p;
    m_To = p;
}

void RectTool::OnMove( EditView& , Point const& p)
{
    if( m_DownButton == NONE )
        m_From = p;     // needed to draw cursor
    else
        m_To = p;       // dragging for end of line
}

void RectTool::OnUp( EditView& view, Point const& p, Button b )
{
    if( m_DownButton != b )
        return;

    m_To = p;
    Box dmg;
    Box rect( m_From, m_To );

    int x,y;
    view.Proj().Draw_Begin(this);

    // top (including left and rightmost pixels)
    dmg.SetEmpty();
    y = rect.Top();
    for( x=rect.Left(); x<=rect.Right(); ++x )
    {
        Box tmp;
        PlonkBrushToProj( Owner(), Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.Proj().Draw_Damage( dmg );

    // right edge (exclude top and bottom rows)
    dmg.SetEmpty();
    x = rect.Right();
    for( y=rect.Top()+1; y<=rect.Bottom()-1; ++y )
    {
        Box tmp;
        PlonkBrushToProj( Owner(), Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.Proj().Draw_Damage( dmg );

    // bottom edge (including right and leftmost pixels)
    dmg.SetEmpty();
    y = rect.Bottom();
    for( x=rect.Right(); x>=rect.Left(); --x )
    {
        Box tmp;
        PlonkBrushToProj( Owner(), Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.Proj().Draw_Damage( dmg );

    // left edge (exclude bottom and top rows)
    dmg.SetEmpty();
    x = rect.Left();
    for( y=rect.Bottom()-1; y>=rect.Top()+1; --y )
    {
        Box tmp;
        PlonkBrushToProj( Owner(), Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.Proj().Draw_Damage( dmg );

    view.Proj().Draw_Commit();
    m_DownButton = NONE;
    m_From = m_To;
}



void RectTool::DrawCursor( EditView& view )
{
    Box dmg;
    if( m_DownButton == NONE )
    {
        // not rubberbanding
        DrawCrossHairCursor( view, m_From, RGBx(255,255,255) );
        PlonkBrushToView( view, m_From, dmg, DRAW );
        view.AddCursorDamage( dmg );
        return;
    }

    // rubberbanding

    Box rect( m_From, m_To );
    int x,y;

    // top (include left and rightmost pixels)
    dmg.SetEmpty();
    y = rect.Top();
    for( x=rect.Left(); x<=rect.Right(); ++x )
    {
        Box tmp;
        PlonkBrushToView( view, Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.AddCursorDamage( dmg );

    // right edge (exclude top and bottom rows)
    dmg.SetEmpty();
    x = rect.Right();
    for( y=rect.Top()+1; y<=rect.Bottom()-1; ++y )
    {
        Box tmp;
        PlonkBrushToView( view, Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.AddCursorDamage( dmg );

    // bottom edge (include right and leftmost)
    dmg.SetEmpty();
    y = rect.Bottom();
    for( x=rect.Right(); x>=rect.Left(); --x )
    {
        Box tmp;
        PlonkBrushToView( view, Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.AddCursorDamage( dmg );

    // left edge (exclude bottom and top)
    dmg.SetEmpty();
    x = rect.Left();
    for( y=rect.Bottom()-1; y>=rect.Top()+1; --y )
    {
        Box tmp;
        PlonkBrushToView( view, Point(x,y), tmp, m_DownButton );
        dmg.Merge( tmp );
    }
    view.AddCursorDamage( dmg );

}



//------------------------------


FilledRectTool::FilledRectTool( Editor& owner ) :
    Tool( owner ),
    m_From(0,0),
    m_To(0,0),
    m_DownButton(NONE)
{
}


void FilledRectTool::OnDown( EditView& , Point const& p, Button b )
{
//    printf("tool::down(%d)\n",b );
    if( m_DownButton!=NONE )
        return;
    m_DownButton = b;
    m_From = p;
    m_To = p;
}

void FilledRectTool::OnMove( EditView& , Point const& p)
{
    if( m_DownButton == NONE )
        m_From = p;     // needed to draw cursor
    else
        m_To = p;       // dragging for end of line
}

void FilledRectTool::OnUp( EditView& view, Point const& p, Button b )
{
    if( m_DownButton != b )
        return;

    Project& proj = view.Proj();

    m_To = p;
    IndexedImg& img = proj.Img();
    Box r( m_From, m_To );
    proj.Draw_Begin(this);

    if( m_DownButton == DRAW )
        img.FillBox( proj.FGPen(),r );
    else    //if( m_DownButton == ERASE )
        img.FillBox( proj.BGPen(),r );
    
    proj.Draw_Damage( r );
    proj.Draw_Commit();
    m_DownButton = NONE;
    m_From = m_To;
}    




void FilledRectTool::DrawCursor( EditView& view )
{
    if( m_DownButton == NONE )
    {
        // not rubberbanding
        DrawCrossHairCursor( view, m_From, RGBx(255,255,255) );
        return;
    }

    Project& proj = view.Proj();

    // rubberbanding
    Box r = view.ProjToView( Box( m_From, m_To ) );

    RGBx c;
    if( m_DownButton == DRAW )
        c = proj.GetColour( proj.FGPen() );
    else
        c = proj.GetColour( proj.BGPen() );

    view.Canvas().FillBox(c,r );
    view.AddCursorDamage( r );
}


//------------------------------


CircleTool::CircleTool( Editor& owner ) :
    Tool( owner ),
    m_From(0,0),
    m_To(0,0),
    m_DownButton(NONE),
    m_View(0)
{
}


void CircleTool::OnDown( EditView& view, Point const& p, Button b )
{
//    printf("tool::down(%d)\n",b );
    if( m_DownButton!=NONE )
        return;
    m_View = &view;

    m_DownButton = b;
    m_From = p;
    m_To = p;
}

void CircleTool::OnMove( EditView& , Point const& p)
{
    if( m_DownButton == NONE )
        m_From = p;     // needed to draw cursor
    else
        m_To = p;       // dragging for end of line
}

void CircleTool::OnUp( EditView& view, Point const& p, Button b )
{
    if( m_DownButton != b )
        return;

    m_To = p;

    view.Proj().Draw_Begin(this);
    int rx = std::abs( m_To.x - m_From.x );
    int ry = std::abs( m_To.y - m_From.y );
    EllipseBresenham( m_From.x, m_From.y, rx, ry, Plot_cb, this );
    view.Proj().Draw_Commit();
    m_DownButton = NONE;
    m_View = 0;
    m_From = m_To;
}

//static
void CircleTool::Plot_cb( int x, int y, void* user )
{
    CircleTool* that = (CircleTool*)user;
    Editor& ed = that->Owner();
    Brush const& brush = ed.CurrentBrush();
    Project& proj = that->m_View->Proj();

    Box dmg(
        x - brush.Handle().x,
        y - brush.Handle().y,
        0,0);
    int maskcolour = -1;
    if( that->m_DownButton == DRAW )
    {
        if( brush.Style() == MASK )
            maskcolour = proj.FGPen();
    }
    else if( that->m_DownButton == ERASE )
    {
        maskcolour = proj.BGPen();
    }

    BlitIndexed( brush, brush.Bounds(),
        proj.Img(), dmg,
        brush.TransparentColour(), maskcolour );
    proj.Draw_Damage( dmg );
}



void CircleTool::DrawCursor( EditView& view )
{
    m_CursorDamage.SetEmpty();
    if( m_DownButton == NONE )
    {
        DrawCrossHairCursor( view, m_From, RGBx(255,255,255) );
        PlonkBrushToViewFG( view, m_From, m_CursorDamage );
    }
    else
    {
        int rx = std::abs( m_To.x - m_From.x );
        int ry = std::abs( m_To.y - m_From.y );
        EllipseBresenham( m_From.x, m_From.y, rx, ry, PlotCursor_cb, this );
    }
    view.AddCursorDamage( m_CursorDamage );
}


//static
void CircleTool::PlotCursor_cb( int x, int y, void* user )
{
    Point pos(x,y);

    CircleTool* that = (CircleTool*)user;
    EditView& view = *that->m_View;
    Editor& ed = that->Owner();
    Brush const& b = ed.CurrentBrush();
//    Project& proj = view.Proj();

    Box pb( Point(x,y)-b.Handle(), b.W(), b.H() );

    Box dmg;
    if( that->m_DownButton == DRAW )
    {
        PlonkBrushToViewFG( view, pos, dmg );
    }
    else if( that->m_DownButton == ERASE )
    {
        PlonkBrushToViewBG( view, pos, dmg );
    }
    that->m_CursorDamage.Merge( dmg );
}

//------------------------------


FilledCircleTool::FilledCircleTool( Editor& owner ) :
    Tool( owner ),
    m_From(0,0),
    m_To(0,0),
    m_DownButton(NONE),
    m_View(0)
{
}


void FilledCircleTool::OnDown( EditView& view, Point const& p, Button b )
{
//    printf("tool::down(%d)\n",b );
    if( m_DownButton!=NONE )
        return;
    m_View = &view;

    m_DownButton = b;
    m_From = p;
    m_To = p;
}

void FilledCircleTool::OnMove( EditView& , Point const& p)
{
    if( m_DownButton == NONE )
        m_From = p;     // needed to draw cursor
    else
        m_To = p;       // dragging for end of line
}

void FilledCircleTool::OnUp( EditView& view, Point const& p, Button b )
{
    if( m_DownButton != b )
        return;

    m_To = p;

    view.Proj().Draw_Begin(this);
    int rx = std::abs( m_To.x - m_From.x );
    int ry = std::abs( m_To.y - m_From.y );
    FilledEllipseBresenham( m_From.x, m_From.y, rx, ry, Draw_hline_cb, this );
    view.Proj().Draw_Commit();
    m_DownButton = NONE;
    m_View = 0;
    m_From = m_To;
}

//static
void FilledCircleTool::Draw_hline_cb( int x0, int x1, int y, void* user )
{
    FilledCircleTool* that = (FilledCircleTool*)user;
    Project& proj = that->m_View->Proj();

    int c = -1;
    if( that->m_DownButton == DRAW )
        c = proj.FGPen();
    else if( that->m_DownButton == ERASE )
        c = proj.BGPen();

    Box b( x0,y,x1-x0,1 );
    proj.Img().FillBox(c,b);
    proj.Draw_Damage( b );
}



void FilledCircleTool::DrawCursor( EditView& view )
{
    m_CursorDamage.SetEmpty();
    if( m_DownButton == NONE )
    {
        DrawCrossHairCursor( view, m_From, RGBx(255,255,255) );
        PlonkBrushToViewFG( view, m_From, m_CursorDamage );
    }
    else
    {
        int rx = std::abs( m_To.x - m_From.x );
        int ry = std::abs( m_To.y - m_From.y );
        FilledEllipseBresenham( m_From.x, m_From.y, rx, ry, Cursor_hline_cb, this );
    }
    view.AddCursorDamage( m_CursorDamage );
}


//static
void FilledCircleTool::Cursor_hline_cb( int x0, int x1, int y, void* user )
{
    Point pos(x0,y);

    FilledCircleTool* that = (FilledCircleTool*)user;
    EditView& view = *that->m_View;
    Project& proj = view.Proj();

    RGBx c;
    if( that->m_DownButton == DRAW )
        c = proj.GetColour( proj.FGPen() );
    else
        c = proj.GetColour( proj.BGPen() );

    Box projb( x0,y,x1-x0,1 );
    Box viewb = view.ProjToView( projb );
    view.Canvas().FillBox( c, viewb );

    that->m_CursorDamage.Merge( viewb );
}

//------------------------------


EyeDropperTool::EyeDropperTool( Editor& owner, ToolType previoustooltype ) :
    Tool(owner),
    m_PreviousToolType(previoustooltype)
{
    owner.SetMouseStyle( MOUSESTYLE_EYEDROPPER );
}


EyeDropperTool::~EyeDropperTool()
{
    Owner().SetMouseStyle( MOUSESTYLE_DEFAULT );
}


void EyeDropperTool::OnDown( EditView& view, Point const& p, Button b )
{
    if( b != DRAW && b != ERASE )
        return;
    Project& proj = view.Proj();
    if( !proj.Img().Bounds().Contains(p ) )
        return;

    uint8_t c = proj.Img().GetPixel(p);
    if( b == DRAW )
        proj.SetFGPen(c);
    else if( b == ERASE )
        proj.SetBGPen(c);

    Owner().UseTool( m_PreviousToolType );
}

void EyeDropperTool::OnMove( EditView& , Point const&)
{
}

void EyeDropperTool::OnUp( EditView& , Point const& , Button )
{
}

void EyeDropperTool::DrawCursor( EditView& )
{
/*
    Box c( m_Pos, 1,1 );
    Box vb = view.ProjToView( c );
    RGBx white( RGBx(255,255,255) );
    RGBx black( RGBx(0,0,0) );
    view.Canvas().OutlineBox( white,vb );
    vb.Expand(-1);
    view.Canvas().OutlineBox( black,vb );
    vb.Expand(1);
    view.AddCursorDamage( vb );
*/
}

