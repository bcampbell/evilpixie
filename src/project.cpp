#include "project.h"
#include "projectlistener.h"
#include "editview.h"
#include "cmd.h"
#include "editor.h"
#include "util.h"
#include "wobbly.h"

#include <assert.h>

extern bool LoadImg( IndexedImg& img, RGBx* palette, const char* filename );
extern void SaveImg( IndexedImg const& img, RGBx const* palette, const char* filename );

extern void LoadPalette( RGBx* palette, const char* filename );



Project::Project( int w, int h ) :
    m_FGPen(1),
    m_BGPen(0),
    m_Img(w,h),
    m_DrawTool(0),
    m_DrawBackup(1,1),
    m_Modified( false )
{
    int n;
    for( n=0; n<256; ++n )
    {
        SetColour( n, RGBx(0,0,0) );
    }
    SetColour( 1, RGBx(255,0,0) );
    SetColour( 2, RGBx(0,255,0) );
    SetColour( 3, RGBx(0,0,255) );
}


Project::~Project()
{
    DiscardUndoAndRedos();
}

void Project::DiscardUndoAndRedos()
{
    bool stacksempty = m_UndoStack.empty() && m_RedoStack.empty();

    while( !m_UndoStack.empty() )
    {
        delete m_UndoStack.back();
        m_UndoStack.pop_back();
    }
    while( !m_RedoStack.empty() )
    {
        delete m_RedoStack.back();
        m_RedoStack.pop_back();
    }

    if( !stacksempty )
    {
        std::set<ProjectListener*>::iterator it;
        for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
        {
            (*it)->OnUndoRedoChanged();
        }
    }
}

void Project::SetModifiedFlag( bool newmodifiedflag )
{
    if( m_Modified == newmodifiedflag )
        return;

    m_Modified = newmodifiedflag;

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnModifiedFlagChanged( newmodifiedflag );
    }
}


void Project::LoadPalette( std::string const& filename )
{
    ::LoadPalette( m_Palette, filename.c_str() );
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
        (*it)->OnPaletteReplaced();
}

void Project::Load( std::string const& filename )
{
    LoadImg( m_Img, m_Palette, filename.c_str() );
#if 0
    std::string ext = ToLower( ExtName(filename) );
    if( ext==".png" )
        LoadPNG( m_Img, m_Palette, filename.c_str() );
    else if( ext==".gif" )
        LoadGIF( m_Img, m_Palette, filename.c_str() );
    else if( ext==".xpm" )
        LoadXPM( m_Img, m_Palette, filename.c_str() );
    else
        throw Wobbly( "unsupported load format - %s", ext.c_str() );
#endif

    SetModifiedFlag( false );
    m_Filename = filename;

    DiscardUndoAndRedos();
    Damage( m_Img.Bounds() );

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
        (*it)->OnPaletteReplaced();

}


void Project::Save( std::string const& filename )
{
/*    std::string ext = ToLower( ExtName(filename) );
    if( ext==".png" )
        SavePNG( m_Img, m_Palette, filename.c_str() );
    else if( ext==".gif" )
        SaveGIF( m_Img, m_Palette, filename.c_str() );
    else
        throw Wobbly( "unsupported save format - %s", ext.c_str() );
*/
    SaveImg( m_Img, m_Palette, filename.c_str() );

    SetModifiedFlag( false );
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


void Project::Draw_Begin( Tool* tool )
{
    assert( m_DrawTool == 0 );

    m_DrawTool = tool;
    m_DrawDamage.SetEmpty();
    // take a copy of the pristine image so we can
    // generate an undo buffer or roll back
    m_DrawBackup.Copy( m_Img );
}


void Project::Draw_Damage( Box const& b )
{
    assert( m_DrawTool != 0 );
    assert( Img().Bounds().Contains(b) );
    m_DrawDamage.Merge(b);

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged( b );
    }
}


void Project::Draw_Commit()
{
    assert( m_DrawTool != 0 );
    Cmd* c = new Cmd_Draw( *this, m_DrawDamage, m_DrawBackup );
    AddCmd( c );
    m_DrawTool = 0;
}

void Project::Draw_Rollback()
{
    assert( m_DrawTool != 0 );
    m_DrawTool = 0;

    Box b( m_DrawDamage );
    BlitIndexed( m_DrawBackup, m_DrawDamage,
        m_Img, b );

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged( b );
    }
}


// Adds a command to the undo stack, and calls its Do() fn
void Project::AddCmd( Cmd* cmd )
{
    const int maxundos = 128;

    m_UndoStack.push_back( cmd );
    if( cmd->State() == Cmd::NOT_DONE )
        cmd->Do();

    // adding a new command renders the redo stack obsolete.
    while( !m_RedoStack.empty() )
    {
        delete m_RedoStack.back();
        m_RedoStack.pop_back();
    }

    // limit amount of undos to something reasonable.
    int trimcount = m_UndoStack.size() - maxundos;
    while( trimcount > 0 )
    {
        delete m_UndoStack.front();
        m_UndoStack.pop_front();
        --trimcount;
    }

    SetModifiedFlag( true );
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnUndoRedoChanged();
    }

}


void Project::Undo()
{
    if( m_UndoStack.empty() )
    {
        return;
    }
//    HideToolCursor();

    Cmd* cmd = m_UndoStack.back();
    m_UndoStack.pop_back();
    cmd->Undo();
    m_RedoStack.push_back( cmd );

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnUndoRedoChanged();
    }
//    ShowToolCursor();
}

void Project::Redo()
{
    if( m_RedoStack.empty() )
        return;
//    HideToolCursor();
    Cmd* cmd = m_RedoStack.back();
    m_RedoStack.pop_back();
    cmd->Do();
    m_UndoStack.push_back( cmd );

    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnUndoRedoChanged();
    }
//    ShowToolCursor();
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


void Project::SetFGPen( int c )
{
    m_FGPen=c;
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnPenChange();
    }
}

void Project::SetBGPen( int c )
{
    m_BGPen=c;
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnPenChange();
    }
}

