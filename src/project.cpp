#include "project.h"
#include "projectlistener.h"
#include "editview.h"
#include "cmd.h"
#include "editor.h"
#include "util.h"
#include "exception.h"

#include <assert.h>
#include <cstdio>


Project::Project( std::string const& filename ) :
    m_Expendable(false),
    m_FGPen(1),
    m_BGPen(0),
    m_DrawTool(0),
    m_DrawFrame(0),
    m_DrawBackup(1,1),
    m_Modified( false )
{
    m_Anim.Load( filename.c_str() );
    if(m_Anim.TransparentIdx() != -1)
        SetBGPen(m_Anim.TransparentIdx());
    m_Filename = filename;
}


Project::Project() :
    m_Expendable(true),
    m_FGPen(1),
    m_BGPen(0),
    m_DrawTool(0),
    m_DrawFrame(0),
    m_DrawBackup(1,1),
    m_Modified( false )
{
    int w = 128;
    int h = 128;

    Palette* tmp = Palette::Load( EVILPIXIE_DATA_DIR "/default.gpl");
    m_Anim.SetPalette(*tmp);
    delete tmp;
    m_Anim.Append(new IndexedImg(w,h));
}


Project::Project( int w, int h, Palette* palette, int num_frames ) :
    m_Expendable(false),
    m_FGPen(1),
    m_BGPen(0),
    m_DrawTool(0),
    m_DrawFrame(0),
    m_DrawBackup(1,1),
    m_Modified( false )
{
    assert(num_frames>=1);
    if(!palette)
        palette = Palette::Load( EVILPIXIE_DATA_DIR "/default.gpl");
    m_Anim.SetPalette(*palette);
    delete palette;
    int i;
    for(i=0;i<num_frames;++i)
        m_Anim.Append(new IndexedImg(w,h));
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


void Project::Save( std::string const& filename, bool savetransparency )
{
    m_Anim.SetTransparentIdx(savetransparency ? BGPen() : -1);
    m_Anim.Save(filename.c_str());
    SetModifiedFlag(false);
    m_Filename = filename;
}


// NOTE: doesn't delete old image - just passes it back to the caller
// TODO: frame number
IndexedImg* Project:: ReplaceImg(IndexedImg* new_img)
{
    return new_img;
#if 0
    IndexedImg* old = m_Frames.front();
    m_Frames.front() = new_img;

    // tell listeners which bit needs a redraw...
    Box b(old->Bounds());
    b.Merge(new_img->Bounds());
    std::set<ProjectListener*>::iterator it;
    for( it=m_Listeners.begin(); it!=m_Listeners.end(); ++it )
    {
        (*it)->OnDamaged( b );
    }

    return old;
#endif
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



// TODO: get rid of this... build up the drawing operation in a Cmd_Draw object held by the tool instead.
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
    assert( Img(m_DrawFrame).Bounds().Contains(b) );
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
    Cmd* c = new Cmd_Draw( *this, m_DrawFrame, m_DrawDamage, m_DrawBackup );
    AddCmd( c );
    m_DrawTool = 0;
}

void Project::Draw_Rollback()
{
    assert( m_DrawTool != 0 );
    m_DrawTool = 0;

    Box b( m_DrawDamage );
    BlitIndexed( m_DrawBackup, m_DrawDamage,
        Img(m_DrawFrame), b );

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

