#include "project.h"
#include "projectlistener.h"
#include "draw.h"
#include "colours.h"
#include "util.h"
#include "exception.h"
#include "global.h"

#include <assert.h>
#include <cstdio>


Project::Project( std::string const& filename ) :
    m_Expendable(false),
    m_Modified( false )
{
    m_Anim.Load( filename.c_str() );
    m_Filename = filename;
}


Project::Project() :
    m_Expendable(true),
    m_Modified( false )
{
    int w = 128;
    int h = 128;

    Palette* tmp = Palette::Load( EVILPIXIE_DATA_DIR "/default.gpl");
    m_Anim.SetPalette(*tmp);
    delete tmp;
    m_Anim.Append(new Img(FMT_I8,w,h));
}


Project::Project( PixelFormat fmt, int w, int h, Palette* palette, int num_frames ) :
    m_Expendable(false),
    m_Modified( false )
{
    assert(num_frames>=1);
    if(!palette)
        palette = Palette::Load( EVILPIXIE_DATA_DIR "/default.gpl");
    m_Anim.SetPalette(*palette);
    delete palette;
    int i;
    for(i=0;i<num_frames;++i)
        m_Anim.Append(new Img(fmt,w,h));
}


Project::~Project()
{
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


void Project::ReplacePalette(Palette* newpalette)
{
    m_Anim.SetPalette(*newpalette);
    delete newpalette;  // UGH!
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
        (*it)->OnPaletteReplaced();
}


void Project::Save( std::string const& filename )
{
    m_Anim.Save(filename.c_str());
    SetModifiedFlag(false);
    m_Filename = filename;
}



void Project::Damage( int frame, Box const& b )
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged( frame,b );
    }
}

void Project::Damage_FramesAdded(int first, int last)
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnFramesAdded( first,last );
    }
}

void Project::Damage_FramesRemoved(int first, int last)
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnFramesRemoved( first,last );
    }
}

void Project::Damage_AnimReplaced()
{
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnAnimReplaced();
    }
}





void Project::Damage_Palette( int first, int cnt )
{
    std::set<ProjectListener*>::iterator it;
    if (cnt == 1)
    {
        Colour c = GetColour(first);
        for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
            (*it)->OnPaletteChanged(first,c);
    } else {
        for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
            (*it)->OnPaletteReplaced();
    }
}




PenColour Project::PickUpPen(Point const& pt, int frame) const
{
    Img const& srcimg = GetAnimConst().GetFrameConst(frame);
    switch(srcimg.Fmt())
    {
    case FMT_I8:
        {
            I8 const* src = srcimg.PtrConst_I8(pt.x,pt.y);
            return PenColour(GetColour(*src),*src);
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

