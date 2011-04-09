#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>
#include <set>
#include <list>
#include <string>

#include "point.h"

#include "colours.h"
#include "palette.h"
#include "box.h"
#include "img.h"

class Tool;
class ProjectListener;
class Cmd;

class Project
{
public:
	Project( int w, int h );
	virtual ~Project();

	void AddListener( ProjectListener* l )
		{ m_Listeners.insert( l ); }
	void RemoveListener( ProjectListener* l )
		{ m_Listeners.erase( l ); }

	// Add a cmd to the undo stack.
	// cmd->Do() will be called.
	// Ownership of cmd passes to Project.
	void AddCmd( Cmd* cmd );

	// undo last cmd (or do nothing)
	void Undo();
	// redo last undo (or do nothing)
	void Redo();

	bool CanUndo() const;
	bool CanRedo() const;

    bool ModifiedFlag() const { return m_Modified; }

    void LoadPalette( std::string const& filename );
    void Load( std::string const& filename );
    void Save( std::string const& filename );

    // pixel access
    IndexedImg& Img() { return m_Img; }
    IndexedImg const& ImgConst() const { return m_Img; }

    // kill!
    // palette manipulation
	RGBx const& GetColour(int n ) { return m_Palette.GetColour(n); }
	void SetColour( int n, RGBx const& c ) { m_Palette.SetColour(n,c); }
    RGBx const* GetPaletteConst() const { return m_Palette.rawconst(); }
//    RGBx* GetPalette() { return m_Palette.raw(); }

    Palette const& PaletteConst() const { return m_Palette; }

    // project holds FG and BG pens
	int FGPen() const { return m_FGPen; }
	int BGPen() const { return m_BGPen; }
	void SetFGPen( int c );
	void SetBGPen( int c );


    // return current filename of project (empty string if no name)
    std::string const& Filename() const { return m_Filename; }

    //--------------------------------------
    // Interface for Cmd to use
    //
    // tell project it's been modified
    // (within draw operation, use Draw_Damage() instead!)
	void Damage( Box const& b );


    //--------------------------------------
    // interface for tools to use, to enable the capturing of multiple
    // modifications into a single cmd.

    // a tool which modified the image starts here...
    void Draw_Begin( Tool* tool );

	// inform the project that a change has been made - tool calls this
	// as many times as required.
	// (will be passed on to the listeners so the display can be updated)
	void Draw_Damage( Box const& b );

    // when finished, commit() stores the modifications into a single Cmd,
    // ready for undoing.
    void Draw_Commit();

    // or just abort the drawing and restore the image to what it was.
    void Draw_Rollback();


    void PaletteChange_Begin();
    void PaletteChange_Alter( int n, RGBx const& c );
    void PaletteChange_Replace( Palette const& p );
    void PaletteChange_Commit();
    void PaletteChange_Rollback();


private:
    Project();                  // disallowed
    Project( Project const& );  // disallowed

    Palette m_Palette;
    int m_FGPen;
    int m_BGPen;
    IndexedImg m_Img;

	std::set< ProjectListener* > m_Listeners;

	std::list< Cmd* > m_UndoStack;
	std::list< Cmd* > m_RedoStack;

    void DiscardUndoAndRedos();
    void SetModifiedFlag( bool newmodifiedflag );

    //
    Tool* m_DrawTool;

    // backup copy of image, used for rollback or undo generation during
    // drawing operation
    IndexedImg m_DrawBackup;
    Box m_DrawDamage;

    // has project been modified?
    bool m_Modified;

    // last known filename of project (empty string for none)
    std::string m_Filename;
};


inline bool Project::CanUndo() const
    { return !m_UndoStack.empty(); }

inline bool Project::CanRedo() const
    { return !m_RedoStack.empty(); }

#endif // PROJECT_H

