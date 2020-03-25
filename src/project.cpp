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
    root(nullptr),
    m_Expendable(false),
    m_Modified( false )
{
    m_Filename = filename;
    Layer *l = new Layer();
    l->Load(filename.c_str());
    assert(false);  //TODO: implement!
    root = new Stack();
    root->AddChild(l);
}


Project::Project() :
    root(nullptr),
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
    root = new Stack();
    root->AddChild(l);
}


Project::Project( PixelFormat fmt, int w, int h, Palette* palette, int num_frames ) :
    root(nullptr),
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
    root = new Stack();
    root->AddChild(l);
}


Project::~Project()
{
    delete root;
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

// KILL THIS!
void Project::ReplacePalette(Palette* newpalette)
{
    /*
    m_Layers[0]->SetPalette(*newpalette);
    delete newpalette;  // UGH!
    NotifyPaletteReplaced(everything);
    */
}

bool Project::IsSamePalette(NodePath const& a, NodePath const& b) const
{
    // TODO: handle palette/layer policy when implemented!
    return a==b;
}

void Project::Save( std::string const& filename )
{
/*
    m_Layers[0]->Save(filename.c_str());
    SetModifiedFlag(false);
    m_Filename = filename;
*/
}



void Project::NotifyDamage(NodePath const& target, Box const& b )
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged(target, b);
    }
}

void Project::NotifyFramesAdded(NodePath const& first, int cnt)
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnFramesAdded(first, cnt);
    }
}

void Project::NotifyFramesRemoved(NodePath const& first, int cnt)
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnFramesRemoved(first, cnt);
    }
}

void Project::NotifyLayerReplaced()
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnLayerReplaced();
    }
}





void Project::NotifyPaletteChange(NodePath const& owner, int first, int cnt )
{
    std::set<ProjectListener*>::iterator it;
    if (cnt == 1)
    {
        // TODO: which palette?
        Colour c = PaletteConst(owner).GetColour(first);
        for (auto l : m_Listeners) {
            l->OnPaletteChanged(owner, first, c);
        }
    } else {
        for (auto l : m_Listeners) {
            l->OnPaletteReplaced(owner);
        }
    }
}

void Project::NotifyPaletteReplaced(NodePath const& owner)
{
    for (auto l: m_Listeners) {
        l->OnPaletteReplaced(owner);
    }
}




PenColour Project::PickUpPen(NodePath const& target, Point const& pt) const
{
    Img const& srcimg = GetImgConst(target);
    switch(srcimg.Fmt())
    {
    case FMT_I8:
        {
            I8 const* src = srcimg.PtrConst_I8(pt.x,pt.y);
            return PenColour(PaletteConst(target).GetColour(*src),*src);
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

