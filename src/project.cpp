#include "project.h"
#include "projectlistener.h"
#include "app.h"
#include "draw.h"
#include "colours.h"
#include "util.h"
#include "exception.h"
#include "global.h"

#include <assert.h>
#include <cstdio>


Project::Project( std::string const& filename ) :
    mRoot(nullptr),
    m_Expendable(false),
    m_Modified( false )
{
    mFilename = filename;
    Layer *l = new Layer();
    l->Load(filename.c_str());
    mRoot = new Stack();
    mRoot->AddChild(l);
}


Project::Project() :
    mRoot(nullptr),
    m_Expendable(true),
    m_Modified( false )
{
    int w = 128;
    int h = 128;

    Palette* tmp = Palette::Load( JoinPath(g_App->DataPath(), "default.gpl").c_str());
    Layer *l = new Layer();
    l->SetPalette(*tmp);
    delete tmp;
    l->Append(new Img(FMT_I8,w,h));
    mRoot = new Stack();
    mRoot->AddChild(l);
}


Project::Project( PixelFormat fmt, int w, int h, Palette* palette, int num_frames ) :
    mRoot(nullptr),
    m_Expendable(false),
    m_Modified( false )
{
    assert(num_frames>=1);
    if(!palette) {
        palette = Palette::Load( JoinPath(g_App->DataPath(), "default.gpl").c_str());
    }
    Layer *l = new Layer();
    l->SetPalette(*palette);
    delete palette;
    int i;
    for(i = 0; i < num_frames; ++i) {
        l->Append(new Img(fmt, w, h));
    }
    mRoot = new Stack();
    mRoot->AddChild(l);
}


Project::~Project()
{
    delete mRoot;
}

void Project::SetModifiedFlag( bool newmodifiedflag )
{
    if( m_Modified == newmodifiedflag )
        return;

    m_Expendable = false;
    m_Modified = newmodifiedflag;

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnModifiedFlagChanged( newmodifiedflag );
    }
}


#if 0
void Project::InsertLayer(Layer* layer, int pos)
{
    assert(pos >= 0 && pos <= (int)m_Layers.size());
    m_Layers.insert(m_Layers.begin() + pos, layer);
}

Layer* Project::DetachLayer(int pos)
{
    assert(pos >= 0 && pos < (int)m_Layers.size());
    Layer* l = m_Layers[pos];
    m_Layers.erase(m_Layers.begin() + pos);
    return l;
}
#endif

bool Project::IsSamePalette(NodePath const& a, NodePath const& b) const
{
    // TODO: handle palette/layer policy when implemented!
    return a==b;
}

void Project::NotifyDamage(NodePath const& target, int frame, Box const& b )
{
    for (auto l : m_Listeners) {
        l->OnDamaged(target, frame, b);
    }
}

void Project::NotifyFramesAdded(NodePath const& target, int first, int count)
{
    for (auto l : m_Listeners) {
        l->OnFramesAdded(target, first, count);
    }
}

void Project::NotifyFramesRemoved(NodePath const& target, int first, int count)
{
    for (auto l : m_Listeners) {
        l->OnFramesRemoved(target, first, count);
    }
}

void Project::NotifyFramesBlatted(NodePath const& target, int first, int count)
{
    for (auto l : m_Listeners) {
        l->OnFramesBlatted(target, first, count);
    }
}

void Project::NotifyPaletteChange(NodePath const& target, int frame, int first, int count )
{
    std::set<ProjectListener*>::iterator it;
    if (count == 1)
    {
        // TODO: which palette?
        Colour c = PaletteConst(target, frame).GetColour(first);
        for (auto l : m_Listeners) {
            l->OnPaletteChanged(target, frame, first, c);
        }
    } else {
        for (auto l : m_Listeners) {
            l->OnPaletteReplaced(target, frame);
        }
    }
}

void Project::NotifyPaletteReplaced(NodePath const& target, int frame)
{
    for (auto l: m_Listeners) {
        l->OnPaletteReplaced(target, frame);
    }
}




PenColour Project::PickUpPen(NodePath const& target, int frame, Point const& pt) const
{
    Img const& srcimg = GetImgConst(target, frame);
    switch(srcimg.Fmt())
    {
    case FMT_I8:
        {
            I8 const* src = srcimg.PtrConst_I8(pt.x,pt.y);
            return PenColour(PaletteConst(target, frame).GetColour(*src),*src);
        }
        break;
    case FMT_RGBX8:
        {
            RGBX8 const* src = srcimg.PtrConst_RGBX8(pt.x,pt.y);
            // TODO: could search for a palette index too...
            return PenColour(*src);
        }
        break;
    case FMT_RGBA8:
        {
            RGBA8 const* src = srcimg.PtrConst_RGBA8(pt.x,pt.y);
            // TODO: could search for a palette index too...
            RGBA8 c = *src;
            return PenColour(c);
        }
        break;
    default:
        assert(false);
        break;
    }
    return PenColour();
}

