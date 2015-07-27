#include "project.h"
#include "projectlistener.h"
#include "draw.h"
#include "cmd.h"
#include "colours.h"
#include "util.h"
#include "exception.h"

#include <assert.h>
#include <cstdio>


Project::Project( std::string const& filename ) :
    m_Expendable(false),
    m_DrawTool(0),
    m_DrawFrame(0),
    m_DrawBackup(FMT_I8,1,1),
    m_Modified( false )
{
    m_Anim.Load( filename.c_str() );
    m_Filename = filename;
}


Project::Project() :
    m_Expendable(true),
    m_DrawTool(0),
    m_DrawFrame(0),
    m_DrawBackup(FMT_I8,1,1), // ugh
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
    m_DrawTool(0),
    m_DrawFrame(0),
    m_DrawBackup(fmt,1,1), // ugh
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


void Project::Save( std::string const& filename, bool /* savetransparency */ )
{
    //m_Anim.SetTransparentIdx(savetransparency ? BGPen().i : -1);
    m_Anim.Save(filename.c_str());
    SetModifiedFlag(false);
    m_Filename = filename;
}



void Project::Damage( Box const& b )
{
    assert( m_DrawTool == 0 );  // not allowed during draw
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged( b );
    }
}

void Project::Damage_FramesAdded(int first, int last)
{
    assert( m_DrawTool == 0 );  // not allowed during draw
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnFramesAdded( first,last );
    }
}

void Project::Damage_FramesRemoved(int first, int last)
{
    assert( m_DrawTool == 0 );  // not allowed during draw
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnFramesRemoved( first,last );
    }
}


void Project::Draw_Begin( Tool* tool, int frame )
{
    assert( m_DrawTool == 0 );
    m_DrawFrame = frame;

    m_DrawTool = tool;
    m_DrawDamage.SetEmpty();
    // take a copy of the pristine image so we can
    // generate an undo buffer or roll back
    m_DrawBackup.Copy(ImgConst(m_DrawFrame));
}


void Project::Draw_Damage( Box const& b )
{
    assert( m_DrawTool != 0 );
    assert( m_Anim.GetFrameConst(m_DrawFrame).Bounds().Contains(b) );
    m_DrawDamage.Merge(b);

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged( b );
    }
}


Cmd* Project::Draw_Commit()
{
    assert( m_DrawTool != 0 );

    Cmd* c = new Cmd_Draw( *this, m_DrawFrame, m_DrawDamage, m_DrawBackup );
    m_DrawTool = 0;
    return c;
}

void Project::Draw_Rollback()
{
    assert( m_DrawTool != 0 );
    m_DrawTool = 0;

    Box b( m_DrawDamage );
    Blit( m_DrawBackup, m_DrawDamage,
        m_Anim.GetFrame(m_DrawFrame), b );

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged( b );
    }
}







void Project::PaletteChange_Begin()
{
}


// TODO: add palette changes to the undo stack
void Project::PaletteChange_Alter( int n, RGBx const& c )
{
    SetColour( n,c );
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnPaletteChanged( n, c );
    }
}

void Project::PaletteChange_Replace( Palette const& p )
{
    int i;
    for( i=0; i<=255; ++i )
        SetColour( i,p.GetColour(i) );
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
        (*it)->OnPaletteReplaced();
}



void Project::PaletteChange_Commit()
{
}

void Project::PaletteChange_Rollback()
{
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
    }
    return PenColour();
}

