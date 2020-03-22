#include "cmd.h"
#include "layer.h"
#include "draw.h"
#include "sheet.h"
#include "project.h"
#include <assert.h>
#include <cstdio>

Cmd_Draw::Cmd_Draw(Project& proj, NodePath const& target, Box const& affected, Img const& undoimg) :
    Cmd(proj, DONE),
    m_Target(target),
    m_Img(undoimg, affected),
    m_Affected( affected )
{
}

void Cmd_Draw::Do()
{
    assert( State() == NOT_DONE );
    Box dirty( m_Affected );
    Img& targImg = Proj().GetImg(m_Target);
    BlitSwap(m_Img, m_Img.Bounds(), targImg, dirty);
    Proj().NotifyDamage(m_Target, dirty);
    SetState(DONE);
}

void Cmd_Draw::Undo()
{
    assert( State() == DONE );
    Box dirty( m_Affected );
    Img& targImg = Proj().GetImg(m_Target);
    BlitSwap( m_Img, m_Img.Bounds(), targImg, dirty );
    Proj().NotifyDamage(m_Target, dirty);
    SetState( NOT_DONE );
}


Cmd_Resize::Cmd_Resize(Project& proj, Box const& new_area, int framefirst, int framelast, PenColour const& fillpen) :
    Cmd(proj,NOT_DONE),
    m_First(framefirst),
    m_Last(framelast)
{
    assert(false);  // TODO: implement this!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);

    // populate frameswap with the resized frames
    int n;
    for(n=m_First; n<m_Last; ++n)
    {
        NodePath id = {0,n};
        Img const& src_img = Proj().GetImgConst(id);
        Img* dest_img = new Img(src_img.Fmt(), new_area.w, new_area.h);
        Box foo(dest_img->Bounds());
        dest_img->FillBox(fillpen,foo);
        Box src_area(src_img.Bounds());
        Box dest_area(src_area);
        dest_area -= new_area.TopLeft();

        Blit(src_img, src_area, *dest_img, dest_area);
        m_FrameSwap.Append(dest_img);
    }
#endif
}


Cmd_Resize::~Cmd_Resize()
{
}


void Cmd_Resize::Swap()
{
    assert(false);  // TODO: implement this!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& targLayer = Proj().GetLayer(0);

    Layer tmp;
    targLayer.TransferFrames(m_First, m_Last, tmp, 0);
    m_FrameSwap.TransferFrames(0,m_FrameSwap.NumFrames(), targLayer, m_First);
    assert(m_FrameSwap.NumFrames()==0);
    tmp.TransferFrames(0,tmp.NumFrames(),m_FrameSwap,0);

    Box b1,b2;
    targLayer.CalcBounds(b1, m_First, m_Last);
    m_FrameSwap.CalcBounds(b2, 0, m_FrameSwap.NumFrames());
    b1.Merge(b2);
    int n;
    for( n=m_First; n<m_Last; ++n)
    {
        NodePath id = {0,n};   // TODO: multilayer...
        Proj().NotifyDamage(id, b1);
    }
#endif
}

void Cmd_Resize::Do()
{
    Swap();
    SetState( DONE );
}

void Cmd_Resize::Undo()
{
    Swap();
    SetState( NOT_DONE );
}



Cmd_InsertFrames::Cmd_InsertFrames(Project& proj, int position, int numframes) :
    Cmd(proj,NOT_DONE),
    m_Pos(position),
    m_Num(numframes)
{
}

Cmd_InsertFrames::~Cmd_InsertFrames()
{
}



void Cmd_InsertFrames::Do()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& targLayer = Proj().GetLayer(0);

    Img const& srcimg(targLayer.GetFrameConst(m_Pos));
    Layer tmp;
    int i;
    for(i=0;i<m_Num;++i)
        tmp.Append(new Img(srcimg));

    tmp.TransferFrames(0,tmp.NumFrames(), targLayer, m_Pos+1);

    Proj().NotifyFramesAdded(m_Pos+1,m_Pos+1+m_Num);
    SetState( DONE );
#endif
}


void Cmd_InsertFrames::Undo()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& targLayer = Proj().GetLayer(0);

    Layer tmp;
    targLayer.TransferFrames(m_Pos+1,m_Pos+1+m_Num,tmp,0);
    // tmp will delete the frames as it goes out of scope
    Proj().NotifyFramesRemoved(m_Pos+1, m_Pos + 1 + m_Num);
    SetState(NOT_DONE);
#endif
}





Cmd_DeleteFrames::Cmd_DeleteFrames(Project& proj, int first, int last) :
    Cmd(proj,NOT_DONE),
    m_First(first),
    m_Last(last)
{
}

Cmd_DeleteFrames::~Cmd_DeleteFrames()
{
}



void Cmd_DeleteFrames::Do()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& targLayer = Proj().GetLayer(0);
    targLayer.TransferFrames(m_First, m_Last, m_FrameSwap,0);
    Proj().NotifyFramesRemoved(m_First,m_Last);
    SetState(DONE);
#endif
}


void Cmd_DeleteFrames::Undo()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& targLayer = Proj().GetLayer(0);
    m_FrameSwap.TransferFrames( 0,m_FrameSwap.NumFrames(), targLayer, m_First);
    Proj().NotifyFramesAdded(m_First,m_Last);
    SetState(NOT_DONE);
#endif
}



//-----------
//
Cmd_ToSpriteSheet::Cmd_ToSpriteSheet(Project& proj, int nWide) :
    Cmd(proj,NOT_DONE),
    m_NumFrames(/*proj.NumFrames()*/),
    m_NWide(nWide)
{
    assert(false);  // TODO: implement!
#if 0
    if( m_NWide>m_NumFrames)
    {
        m_NWide = m_NumFrames;
    }
#endif
}

Cmd_ToSpriteSheet::~Cmd_ToSpriteSheet()
{
}



void Cmd_ToSpriteSheet::Do()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& layer = Proj().GetLayer(0);

    Img* sheet = GenerateSpriteSheet(layer, m_NWide);
    m_NumFrames = layer.NumFrames();

    layer.Zap();
    layer.Append(sheet);

    Proj().NotifyLayerReplaced();
    SetState(DONE);
#endif
}


void Cmd_ToSpriteSheet::Undo()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& targLayer = Proj().GetLayer(0);

    assert(targLayer.NumFrames()==1);
    Img const& src = targLayer.GetFrameConst(0);

    std::vector<Img*> frames;
    FramesFromSpriteSheet(src,m_NWide,m_NumFrames, frames);
    targLayer.Zap();
    unsigned int n;
    for(n=0;n<frames.size();++n) {
        targLayer.Append(frames[n]);
    }
    Proj().NotifyLayerReplaced();
    SetState(NOT_DONE);
#endif
}

//-----------
//
Cmd_FromSpriteSheet::Cmd_FromSpriteSheet(Project& proj, int nWide, int numFrames) :
    Cmd(proj,NOT_DONE),
    m_NumFrames(numFrames),
    m_NWide(nWide)
{
    if( m_NWide>m_NumFrames)
    {
        m_NWide = m_NumFrames;
    }
}

Cmd_FromSpriteSheet::~Cmd_FromSpriteSheet()
{
}



void Cmd_FromSpriteSheet::Do()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& layer = Proj().GetLayer(0);
    assert(layer.NumFrames()==1);

    Img const& src = layer.GetFrameConst(0);

    std::vector<Img*> frames;
    FramesFromSpriteSheet(src,m_NWide,m_NumFrames, frames);
    layer.Zap();
    unsigned int n;
    for(n=0;n<frames.size();++n)
    {
        layer.Append(frames[n]);
    }
    Proj().NotifyLayerReplaced();
    SetState( DONE );
#endif
}

void Cmd_FromSpriteSheet::Undo()
{
    assert(false);  // TODO: implement!
#if 0
    // TODO: figure out what to do with multiple layers
    assert(Proj().NumLayers() == 1);
    Layer& layer = Proj().GetLayer(0);

    Img* sheet = GenerateSpriteSheet(layer,m_NWide);
    m_NumFrames = layer.NumFrames();

    layer.Zap();
    layer.Append(sheet);

    Proj().NotifyLayerReplaced();
    SetState( NOT_DONE );
#endif
}

//-----------
//
Cmd_PaletteModify::Cmd_PaletteModify(Project& proj, int first, int cnt, Colour const* colours) :
    Cmd(proj,NOT_DONE),
    m_First(first),
    m_Cnt(cnt)
{
    m_Colours = new Colour[cnt];
    int i;
    for (i=0; i<cnt; ++i )
        m_Colours[i] = colours[i];
}

Cmd_PaletteModify::~Cmd_PaletteModify()
{
    delete [] m_Colours;
}


void Cmd_PaletteModify::swap()
{
    assert(false);  // TODO: implement!
#if 0
    assert(Proj().NumLayers()==1);  // TODO: figure out multilayer behavour!
    Palette& pal = Proj().GetLayer(0).GetPalette();
    int i;
    for (i=0; i<m_Cnt; ++i)
    {
        Colour tmp = pal.GetColour(m_First+i);
        pal.SetColour(m_First+i, m_Colours[i]);
        m_Colours[i] = tmp;
    }

    Proj().NotifyPaletteChange(m_First, m_Cnt);
#endif
}

void Cmd_PaletteModify::Do()
{
    swap();
    SetState( DONE );
}

void Cmd_PaletteModify::Undo()
{
    swap();
    SetState( NOT_DONE );
}

// attempts to merge a single colour change into the existing cmd.
// this lets us keep the project up-to-date as user twiddles the colour,
// but also avoids clogging up the undo stack with insane numbers of operations.
// returns true if the merge occured, false if a new cmd is required.
bool Cmd_PaletteModify::Merge( int idx, Colour const& newc)
{
    assert(false);  // TODO: implement!
#if 0
    if (m_Cnt!=1 || m_First!=idx)
        return false;
    if (State()!=DONE)
        return false;

    assert(Proj().NumLayers()==1);  // TODO: figure out multilayer behavour!
    Palette& pal = Proj().GetLayer(0).GetPalette();
    pal.SetColour(m_First, newc);
    Proj().NotifyPaletteChange(m_First, m_Cnt);
    return true;
#endif
}


//-----------
//
//

Cmd_Batch::Cmd_Batch( Project& proj, CmdState initialstate) :
    Cmd(proj,initialstate)
{
}

Cmd_Batch::~Cmd_Batch()
{
    while (!m_Cmds.empty()) {
        delete m_Cmds.back();
        m_Cmds.pop_back();
    }
}


void Cmd_Batch::Append(Cmd* c)
{
    assert(c->State() == State());
    m_Cmds.push_back(c);
}


void Cmd_Batch::Do()
{
    std::vector<Cmd*>::iterator it;
    for (it=m_Cmds.begin(); it!=m_Cmds.end(); ++it) {
        (*it)->Do(); 
    }
}

void Cmd_Batch::Undo()
{
    std::vector<Cmd*>::reverse_iterator it;
    for (it=m_Cmds.rbegin(); it!=m_Cmds.rend(); ++it) {
        (*it)->Undo(); 
    }
}


//-----------
//
//

Cmd_NewLayer::Cmd_NewLayer(Project& proj, Layer* l, int pos) :
    Cmd(proj,NOT_DONE),
    m_Layer(l),
    m_Pos(pos)
{
}

Cmd_NewLayer::~Cmd_NewLayer()
{
    delete m_Layer;
}

void Cmd_NewLayer::Do()
{
    assert(false);  // TODO: implement!
#if 0
    assert(m_Layer);
    Proj().InsertLayer(m_Layer, m_Pos);
    m_Layer = nullptr;
    Proj().NotifyLayerReplaced();
    SetState( DONE );
#endif
}


void Cmd_NewLayer::Undo()
{
    assert(false);  // TODO: implement!
#if 0
    assert(!m_Layer);
    m_Layer = Proj().DetachLayer(m_Pos);
    Proj().NotifyLayerReplaced();
    SetState(NOT_DONE);
#endif
}


