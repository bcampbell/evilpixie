#include "editview.h"
#include "editor.h"
#include <cstdio>
#include <cassert>


EditView::EditView(Editor& editor, NodePath const& focus, int frame, int w, int h) :
    m_Editor(editor),
    m_PrevPos(-1,-1),
    m_Canvas( new Img(FMT_RGBX8,w,h ) ),
    m_ViewBox(0,0,w,h),
    m_Focus(focus),
    m_Frame(frame),
    m_Zoom(4),
    m_Offset(0,0),
    m_Panning(false),
    m_PanAnchor(0,0)
{
    m_XZoom = m_Zoom*editor.Proj().Settings().PixW;
    m_YZoom = m_Zoom*editor.Proj().Settings().PixH;
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
    Box const& p = FocusedImgConst().Bounds();
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
    m_XZoom = Proj().Settings().PixW*zoom;
    m_YZoom = Proj().Settings().PixH*zoom;
    ConfineView();
    DrawView(m_ViewBox);
    Redraw(m_ViewBox);
}

void EditView::SetFocus(NodePath const& focus)
{
    m_Focus = focus;
    ConfineView();
    DrawView(m_ViewBox);
    Redraw(m_ViewBox);
}

void EditView::SetFrame(int frame)
{
    //printf("EditView::SetFrame(%d->%d)\n", m_Frame, frame);
    m_Frame = frame;
    ConfineView();
    DrawView(m_ViewBox);
    Redraw(m_ViewBox);
    //printf("end EditView::SetFrame()\n");
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
    Box const& p = FocusedImgConst().Bounds();
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
    Box const& p = FocusedImgConst().Bounds();
    Box v = ViewToProj( m_ViewBox );
    m_Offset.x = -(v.w - p.w) / 2;
    m_Offset.y = -(v.h - p.h) / 2;
}

// TODO: maybe editview shouldn't deal with mousemovements:
// - Have a "default" tool which handles panning etc...
// - tools to handle their own grid snapping.
// - move all mouse handling up to GUI layer.
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



// within canvas
static RGBX8 checker(int x,int y) {
    if((x & 16) ^ (y & 16))
        return RGBX8(192,192,192);
    else
        return RGBX8(224,224,224);
}

// outside canvas
static RGBX8 checker2(int x,int y) {
    if((x & 16) ^ (y & 16))
        return RGBX8(192/2,192/2,192/2);
    else
        return RGBX8(224/2,224/2,224/2);
}

// Render project to canvas, with zooming.
void EditView::DrawView( Box const& viewbox, Box* affectedview )
{
    // note: viewbox can be outside the project boundary

//    Colour checkerboard[2] = { Colour(192,192,192), Colour(224,224,224) }; 

    Box vb(viewbox);
    vb.ClipAgainst(m_ViewBox);

    Img const& img = FocusedImgConst();
    // get project bounds in view coords (unclipped)
    Box pbox(ProjToView(img.Bounds()));

    // step x,y through view coords of the area to draw
    int y;
//    int xmin = std::min(pbox.XMin(), vb.XMax()+1);
    int xbegin = std::min(pbox.x, vb.x + vb.w);
    int xend = std::min(pbox.x + pbox.w, vb.x + vb.w);
    for(y=vb.YMin(); y<=vb.YMax(); ++y) {
        RGBX8* dest = m_Canvas->Ptr_RGBX8(vb.x,y);
        int x=vb.XMin();

        // scanline intersects canvas?
        if(y<pbox.YMin() || y>pbox.YMax()) {
            // line is above or below the project
            while(x<=vb.XMax()) {
                *dest++ = checker2(x,y);
                ++x;
            }
            continue;
        }

        // left of project canvas
        while(x<xbegin) {
            *dest++ = checker2(x,y);
            ++x;
        }

        if(x<xend) {
            // on the project canvas
            Point p( ViewToProj(Point(x,y)) );
            switch( img.Fmt() ) {

            case FMT_I8:
                {
                    Palette const& pal = FocusedPaletteConst();
                    I8 const* src = img.PtrConst_I8( p.x,p.y );
                    while(x<xend) {
                        int cx = x + (m_Offset.x*m_XZoom);
                        int pixstop = x + (m_XZoom-(cx%m_XZoom));
                        if(pixstop>xend)
                            pixstop=xend;
                        RGBA8 c = pal.GetColour(*src++);
                        while(x<pixstop)
                        {
                            //*dest++ = c;
                            *dest++ = Blend(c,checker(x,y));
                            ++x;
                        }
                    }
                }
                break;
            case FMT_RGBX8:
                {
                    RGBX8 const* src = img.PtrConst_RGBX8( p.x,p.y );
                    while(x<xend) {
                        int cx = x + (m_Offset.x*m_XZoom);
                        int pixstop = x + (m_XZoom-(cx%m_XZoom));
                        if(pixstop>xend)
                            pixstop=xend;
                        RGBX8 c = *src++;
                        while(x<pixstop) {
                            *dest++ = c;
                            ++x;
                        }
                    }
                }
                break;
            case FMT_RGBA8:
                {
                    RGBA8 const* src = img.PtrConst_RGBA8( p.x,p.y );
                    while(x<xend) {
                        int cx = x + (m_Offset.x*m_XZoom);
                        int pixstop = x + (m_XZoom-(cx%m_XZoom));
                        if(pixstop>xend)
                            pixstop=xend;
                        RGBA8 c = *src++;
                        while(x<pixstop) {
                            *dest++ = Blend(c,checker(x,y));
                            ++x;
                        }
                    }
                }
                break;
            default:
                assert(false);
                break;
            }
        }
        // right of canvas
        while(x < vb.x+vb.w)
        {
            *dest++ = checker2(x,y);
            ++x;
        }
    }

    if(affectedview)
        *affectedview = vb;
}


// Begin ProjectListener implementation.

// called when project has been modified
void EditView::OnDamaged(NodePath const& target, int frame, Box const& projdmg)
{
    if (m_Focus.sel != target.sel) {
        return;
    }
    if (m_Frame != frame) {
        return;
    }
    // TODO: ignore non-visible layers...
    // TODO: account for onionskinning.

    Box viewdirtied;

    // just redraw the damaged part of the project...
    Box area(ProjToView(projdmg));
    DrawView(area, &viewdirtied );

    // tell the gui to display damaged part
    Redraw(viewdirtied);
}

void EditView::OnPaletteChanged(NodePath const& target, int frame, int /*index*/, Colour const&/*newColour*/)
{
    OnPaletteReplaced(target, frame);
}

void EditView::OnPaletteReplaced(NodePath const& target, int frame)
{
    if (!Proj().SharesPalette(target, frame, m_Focus, m_Frame)) {
        return;
    }
    // redraw the whole project (don't need to redraw padding)
    Box area(ProjToView(FocusedImgConst().Bounds()));
    Box affected;
    DrawView(area,&affected);
    Redraw(affected);
}

void EditView::OnModifiedFlagChanged(bool /*changed*/)
{
}

void EditView::OnFramesAdded(NodePath const& target, int first, int count)
{
    if (m_Focus.sel != target.sel) {
        return;
    }
    //Layer const& l = Proj().ResolveLayer(target);
    // TODO: ignore changes on non-visible layers.

    // redraw the whole view (including padding)
    Box affected;
    DrawView(m_ViewBox,&affected);
    Redraw(affected);
}

void EditView::OnFramesRemoved(NodePath const& target, int first, int count)
{
    if (m_Focus.sel != target.sel) {
        return;
    }
    Layer const& l = Proj().ResolveLayer(target);
    // TODO: ignore changes on non-visible layers.

    // make sure we're still pointing at a valid frame.
    if (m_Frame >= l.mFrames.size()) {
        m_Frame = l.mFrames.size()-1;
    }

    // redraw the whole view (including padding)
    Box affected;
    DrawView(m_ViewBox,&affected);
    Redraw(affected);
}

void EditView::OnFramesBlatted(NodePath const& target, int /*first*/, int /*count*/)
{
    if (m_Focus.sel != target.sel) {
        return;
    }

    // redraw the whole view (including padding)
    Box affected;
    DrawView(m_ViewBox,&affected);
    Redraw(affected);
}

// End of ProjectListener implementation

void EditView::AddCursorDamage(Box const& viewdmg)
{
    m_CursorDamage.push_back(viewdmg);
    Redraw(viewdmg);
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


