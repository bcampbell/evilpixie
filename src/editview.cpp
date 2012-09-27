#include "editview.h"
#include "editor.h"
#include <cstdio>
#include <cassert>


EditView::EditView( Editor& editor, int w, int h ) :
    m_Editor(editor),
    m_PrevPos(-1,-1),
    m_Canvas( new Img(FMT_RGBX8,w,h ) ),
    m_ViewBox(0,0,w,h),
    m_Frame(0),
    m_Zoom(4),
    m_Offset(0,0),
    m_Panning(false),
    m_PanAnchor(0,0)
{
    CenterView();
    DrawView(m_ViewBox);
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
    if(m_Canvas)
    {
        delete m_Canvas;
        m_Canvas = 0;
    }

    m_ViewBox.w = w;
    m_ViewBox.h = h;

    m_Canvas = new Img( FMT_RGBX8, w,h );
    ConfineView();

    // if view is wider/taller than image, center it
    Box const& p = Proj().ImgConst(Frame()).Bounds();
    Box v = ViewToProj( m_ViewBox );
    if( v.w>p.w)
        m_Offset.x = -(v.w - p.w) / 2;
    if( v.h>p.h)
        m_Offset.y = -(v.h - p.h) / 2;

    DrawView(m_ViewBox);
    Redraw(m_ViewBox);
}

void EditView::SetZoom( int zoom )
{
    if(zoom<1)
        zoom=1;
    if(zoom>128)
        zoom=128;
    if(zoom == m_Zoom)
        return;
    m_Zoom = zoom;
    ConfineView();
    DrawView(m_ViewBox);
    Redraw(m_ViewBox);
}

void EditView::SetFrame( int frame )
{
    assert( frame>=0);
    assert( frame<Proj().GetAnim().NumFrames());

    m_Frame = frame;
    ConfineView();
    DrawView(m_ViewBox);
    Redraw(m_ViewBox);
}

void EditView::SetOffset( Point const& projpos )
{
    m_Offset = projpos;
    ConfineView();
    DrawView(m_ViewBox);
    Redraw(m_ViewBox);
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
    Box const& p = Proj().ImgConst(Frame()).Bounds();
    Box v = ViewToProj( m_ViewBox );
#if 0
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
#endif

    if( p.W() < v.W() )
    {
        // view is wider than proj
        if( v.XMin() > p.XMin() )
            m_Offset.x = p.XMin();
        if( v.XMax() < p.XMax() )
            m_Offset.x = (p.x + p.W())-v.W();
    }
    else
    {
        // proj is wider than view
        if( v.XMin() < 0 )
            m_Offset.x = 0;
        else if( v.XMax() > p.XMax() )
            m_Offset.x = (p.x + p.W()) - v.W();
    }

    if( p.H() < v.H() )
    {
        // view is taller than proj
        if( v.YMin() > p.YMin() )
            m_Offset.y = p.YMin();
        if( v.YMax() < p.YMax() )
            m_Offset.y = (p.y + p.H())-v.H();
    }
    else
    {
        // proj is taller than view
        if( v.YMin() < 0 )
            m_Offset.y = 0;
        else if( v.YMax() > p.YMax() )
            m_Offset.y = (p.y+p.H()) - v.H();
    }

}

void EditView::CenterView()
{
    Box const& p = Proj().ImgConst(Frame()).Bounds();
    Box v = ViewToProj( m_ViewBox );
    m_Offset.x = -(v.w - p.w) / 2;
    m_Offset.y = -(v.h - p.h) / 2;
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




void EditView::DrawView( Box const& viewbox, Box* affectedview )
{
    // note: projbox can be outside the project boundary

    RGBx checkerboard[2] = { RGBx(192,192,192), RGBx(224,224,224) }; 

    Box vb(viewbox);
    vb.ClipAgainst(m_ViewBox);

    Img const& img = Proj().GetAnim().GetFrame(Frame());
    // get project bounds in view coords (unclipped)
    Box pbox(ProjToView(img.Bounds()));

    // step x,y through view coords of the area to draw
    int y;
    for(y=vb.YMin(); y<=vb.YMax(); ++y)
    {
        RGBX8* dest = m_Canvas->Ptr_RGBX8(vb.x,y);
        int x=vb.XMin();

        // scanline intersects canvas?
        if(y>=pbox.YMin() && y<=pbox.YMax())
        {
            int xmin = std::min(pbox.XMin(), vb.XMax()+1);
            int xmax = std::min(pbox.XMax(), vb.XMax());

            // left of canvas
            while(x<xmin)
            {
                *dest++ = checkerboard[((x & 16) ^ (y & 16)) ? 0:1];
                ++x;
            }

            // on the canvas
            Point p( ViewToProj(Point(x,y)) );
            uint8_t const* src = img.PtrConst( p.x,p.y );
            assert(img.Fmt()==FMT_I8);
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
            *dest++ = checkerboard[((x & 16) ^ (y & 16)) ? 0:1];
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
    Box area(ProjToView(projdmg));
    DrawView(area, &viewdirtied );

    // tell the gui to display damaged part
    Redraw( viewdirtied );
}


void EditView::OnPaletteChanged( int, RGBx const& )
{
    OnPaletteReplaced();
}

void EditView::OnPaletteReplaced()
{
    // redraw the whole project
    Box area(ProjToView(Proj().GetAnim().GetFrame(Frame()).Bounds()));
    Box affected;
    DrawView(area,&affected);
    Redraw(affected);
}

void EditView::OnFramesAdded(int first, int last)
{
    if(Frame()>first)
        SetFrame(Frame()+(last-first));
}

void EditView::OnFramesRemoved(int first, int last)
{
    if(Frame()>=first)
    {
        int newframe = Frame()-(last-first);
        if(newframe<first)
            newframe=first;
        if(newframe>Proj().GetAnim().NumFrames()-1)
            newframe = Proj().GetAnim().NumFrames()-1;
        SetFrame(newframe);
    }
}

void EditView::AddCursorDamage( Box const& viewdmg )
{
    m_CursorDamage.push_back( viewdmg );
    Redraw( viewdmg );
}


void EditView::EraseCursor()
{
    std::vector<Box>::const_iterator it;
    std::vector<Box>::const_iterator itend = m_CursorDamage.end();
    for( it=m_CursorDamage.begin(); it!=itend; ++it )
    {
        Box clipped;
        DrawView(*it, &clipped );
        Redraw( clipped );
    }

    m_CursorDamage.clear();
}


