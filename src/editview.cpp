#include "editview.h"
#include "editor.h"
#include <cstdio>


EditView::EditView( Editor& editor, int w, int h ) :
    m_Editor(editor),
    m_PrevPos(-1,-1),
    m_Canvas( new RGBImg( w,h ) ),
    m_ViewBox(0,0,w,h),
    m_Zoom(4),
    m_Offset(0,0),
    m_Panning(false),
    m_PanAnchor(0,0)
{
    DrawProj( Box( m_Offset.x, m_Offset.y, Width()/m_Zoom, Height()/m_Zoom ) );
    Proj().AddListener( this );
    editor.AddView( this );
}


EditView::~EditView()
{
    Proj().RemoveListener( this );
    Ed().RemoveView( this );
    delete m_Canvas;
}




void EditView::Resize( int w, int h )
{
    if( !m_Canvas )
    {
        delete m_Canvas;
        m_Canvas = 0;
    }

    m_ViewBox.w = w;
    m_ViewBox.h = h;

    m_Canvas = new RGBImg( w,h );

    ConfineView();
    DrawProj( Box( m_Offset.x, m_Offset.y, Width()/m_Zoom, Height()/m_Zoom ) );
    Redraw( m_ViewBox );
}

void EditView::SetZoom( int zoom )
{
    m_Zoom = zoom;
    ConfineView();
    DrawProj(m_ViewBox);  //Box( m_Offset.x, m_Offset.y, Width()/m_Zoom, Height()/m_Zoom ) );
    Redraw(m_ViewBox);
}

void EditView::SetOffset( Point const& projpos )
{
    m_Offset = projpos;
    ConfineView();
    DrawProj( Box( m_Offset.x, m_Offset.y, Width()/m_Zoom, Height()/m_Zoom ) );
    Redraw( m_ViewBox );
}

// Offsets the view to make sure viewspace point viewp is over
// projectspace point projp.
void EditView::AlignView( Point const& viewp, Point const& projp )
{
    Point p = ViewToProj( viewp );
    int dx = projp.x - p.x;
    int dy = projp.y - p.y;
    Point o = Offset();
    o.x += dx;
    o.y += dy;
    SetOffset( o);
}


// confine the view to keep as much of the image onscreen as possible.
void EditView::ConfineView()
{
    Box const& p = Proj().ImgConst().Bounds();
    Box v = ViewToProj( m_ViewBox );

    if( p.W() < v.W() )
        m_Offset.x = -(v.W()-p.W())/2;   // center
    else if( v.XMin() < 0 )
        m_Offset.x = 0;
    else if( v.XMax() > p.XMax() )
        m_Offset.x = (p.x + p.W()) - v.W();

    if( p.H() < v.H() )
        m_Offset.y = -(v.H()-p.H())/2;   // center
    else if( v.YMin() < 0 )
        m_Offset.y = 0;
    else if( v.YMax() > p.YMax() )
        m_Offset.y = (p.y+p.H()) - v.H();

}


void EditView::OnMouseDown( Point const& viewpos, Button button )
{
    Point p = ViewToProj( viewpos );
    if( button == PAN )
    {
        m_PanAnchor = p;
        m_Panning = true;
        return;
    }

    if( Ed().CurrentTool().ObeyGrid() )
        Ed().GridSnap(p);

    Ed().HideToolCursor();
    Ed().CurrentTool().OnDown( *this, p, button );
    // NOTE: Tool might have changed!
    Ed().ShowToolCursor();
    m_PrevPos = p;
}

void EditView::OnMouseMove( Point const& viewpos )
{
    Point p = ViewToProj( viewpos );
    if( m_Panning && !(p == m_PrevPos) )
    {
        AlignView( viewpos, m_PanAnchor );
        m_PrevPos = p;
        return;
    }

    if( Ed().CurrentTool().ObeyGrid() )
        Ed().GridSnap(p);

    if( p == m_PrevPos )
        return;

    Ed().UpdateMouseInfo( p );


    Ed().HideToolCursor();
    Ed().CurrentTool().OnMove( *this, p );
    // NOTE: Tool might have changed!
    Ed().ShowToolCursor();
    m_PrevPos = p;
}

void EditView::OnMouseUp( Point const & viewpos, Button button )
{
    if( button == PAN )
    {
        m_Panning = false;
        return;
    }

    Point p = ViewToProj( viewpos );
    if( Ed().CurrentTool().ObeyGrid() )
        Ed().GridSnap(p);

    Ed().HideToolCursor();
    Ed().CurrentTool().OnUp( *this, p, button );
    // NOTE: Tool might have changed!
    Ed().ShowToolCursor();
    m_PrevPos = p;
}



#if 0
void EditView::DrawProj( Box const& projbox, Box* affectedview )
{
    Point viewpos = ProjToView( Point( projbox.x, projbox.y ) );

    // translate into view coords
    Box vb(0,0,0,0);
    vb.x = viewpos.x;
    vb.y = viewpos.y;
    vb.w = projbox.w * m_Zoom;
    vb.h = projbox.h * m_Zoom;

    vb.ClipAgainst( m_ViewBox );

    int y;
    for( y=vb.y; y<vb.y+vb.h; ++y )
    {
        RGBx* dest = m_Canvas->Ptr(vb.x,y);
        int x;
        for( x=vb.x; x<vb.x+vb.w; ++x )
        {
            Point p = ViewToProj( Point(x,y) );
            if( p.x < 0 || p.x >= Proj().Img().W()
                || p.y <0 || p.y >= Proj().Img().H() )
            {
                *dest++ = RGBx(0,0,128);
            }
            else
            {
                *dest++ = Proj().GetColour( Proj().Img().GetPixel( p.x,p.y ) );
            }
        }
    }
    if( affectedview )
        *affectedview = vb;
}
#endif



#if 0
void EditView::DrawProj( Box const& projbox, Box* affectedview )
{
    // note: projbox can be outside the project boundary

    Point viewpos = ProjToView( Point( projbox.x, projbox.y ) );

    // translate into view coords
/*
    Box vb(0,0,0,0);
    vb.x = viewpos.x;
    vb.y = viewpos.y;
    vb.w = (projbox.w * m_Zoom);// + (m_Zoom-1);
    vb.h = (projbox.h * m_Zoom);// + (m_Zoom-1);
*/
    Box vb = ProjToView(projbox);
    vb.ClipAgainst( m_ViewBox );

    int y;
    for( y=vb.y; y<vb.y+vb.h; ++y )
    {
        RGBx* dest = m_Canvas->Ptr(vb.x,y);

        int x = vb.x;

        int px = x/m_Zoom + m_Offset.x;
        int py = y/m_Zoom + m_Offset.y;

        int pstart = projbox.x<0 ? 0:projbox.x;
        int start = (pstart-m_Offset.x)*m_Zoom;
        int pend = (projbox.x+projbox.w)>Proj().Img().W() ? Proj().Img().W() : projbox.x+projbox.w;
        int end = (pend-m_Offset.x)*m_Zoom; // + (m_Zoom-1);

        if(py >= 0 && py < Proj().Img().H())
        {
            // line is within project vertical bounds

            // draw blank to the left of image
            while(x<start)
            {
                *dest++ = ((x & 16) ^ (y & 16)) ? RGBx(128,128,128):RGBx(224,224,224);
                ++x;
            }

            px = x/m_Zoom + m_Offset.x;
            // the in-view part of the image
            uint8_t const* src = Proj().Img().PtrConst( px,py );
            int i;
            // whole pixels
            while(x<end-(m_Zoom-1))
            {
                const RGBx c(Proj().GetColour(*src++));
                for(i=0;i<m_Zoom;++i)
                    *dest++ = c;
                x += m_Zoom;
            }
            // trailing clipped pixel, if any
            while(x<end)
            {
                const RGBx c(Proj().GetColour(*src++));
                for(i=0;i<m_Zoom && x<end; ++i)
                {
                    *dest++ = c;
                    ++x;
                }
            }
        }
        // blank to the right of image
        while( x<vb.x+vb.w )
        {
            *dest++ = ((x & 16) ^ (y & 16)) ? RGBx(128,128,128):RGBx(224,224,224);
            ++x;
        }
    }
    if( affectedview )
        *affectedview = vb;
}
#endif

void EditView::DrawProj( Box const& projbox, Box* affectedview )
{
    // note: projbox can be outside the project boundary

    Box vb(ProjToView(projbox));
    vb.ClipAgainst(m_ViewBox);

    // get project bounds in view coords (unclipped)
    Box pbox(ProjToView(Proj().Img().Bounds()));

    // step x,y through view coords of the area to draw
    int y;
    for(y=vb.YMin(); y<=vb.YMax(); ++y)
    {
        RGBx* dest = m_Canvas->Ptr(vb.x,y);
        int x=vb.XMin();

        // scanline intersects canvas?
        if(y>=pbox.YMin() && y<=pbox.YMax())
        {
            int xmin = std::min(pbox.XMin(), vb.XMax()+1);
            int xmax = std::min(pbox.XMax(), vb.XMax());

            // left of canvas
            while(x<xmin)
            {
                *dest++ = ((x & 16) ^ (y & 16)) ? RGBx(128,128,128):RGBx(224,224,224);
                ++x;
            }

            // on the canvas
            Point p( ViewToProj(Point(x,y)) );
            uint8_t const* src = Proj().Img().PtrConst( p.x,p.y );
            while(x<=xmax)
            {
                int cx = x + (m_Offset.x*m_Zoom);
                int pixstop = x + (m_Zoom-(cx%m_Zoom));
                if(pixstop>xmax)
                    pixstop=xmax+1;
                RGBx c(Proj().PaletteConst().GetColour(*src++));
                while(x<pixstop)
                {
                    *dest++ = c;
                    ++x;
                }
            }
        }

        // right of canvas (and above and below)
        while(x<=vb.XMax())
        {
            *dest++ = ((x & 16) ^ (y & 16)) ? RGBx(128,128,128):RGBx(224,224,224);
            ++x;
        }
    }

    if(affectedview)
        *affectedview = vb;
}



// called when project has been modified
void EditView::OnDamaged( Box const& projdmg )
{
    Box viewdirtied;

    // just redraw the damaged part of the project...
    DrawProj(projdmg, &viewdirtied );

    // tell the gui to display damaged part
    Redraw( viewdirtied );
}


void EditView::OnPaletteChanged( int, RGBx const& )
{
    OnPaletteReplaced();
}

void EditView::OnPaletteReplaced()
{
    // redraw everything in view
    Box projdmg = ViewToProj( m_ViewBox );
    DrawProj(projdmg);
    Redraw( m_ViewBox );
}


void EditView::AddCursorDamage( Box const& viewdmg )
{
    Box projdmg = ViewToProj( viewdmg );
    m_CursorDamage.push_back( projdmg );
    // tell GUI to show it
    Redraw( viewdmg );
}


void EditView::EraseCursor()
{
    std::vector<Box>::const_iterator it;
    std::vector<Box>::const_iterator itend = m_CursorDamage.end();
    for( it=m_CursorDamage.begin(); it!=itend; ++it )
        OnDamaged( *it );
    m_CursorDamage.clear();
}


