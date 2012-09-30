#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>
#include <list>
#include <set>
#include <vector>
#include <string>

#include "anim.h"
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
    Project();
	Project( int w, int h, Palette* palette=0, int num_frames=1 );
    Project(std::string const& filename);
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

    // expendable is set if project is default and unmodified, and can be
    // deleted without care (eg if user loads another project)
    bool Expendable() const { return m_Expendable; }

    // replace palette, informing listeners. ownership is passed to proj.
    void ReplacePalette(Palette* newpalette);

    void Save( std::string const& filename, bool savetransparency );

    // pixel access
    Anim& GetAnim() { return m_Anim; }
    Anim const& GetAnimConst() const { return m_Anim; }
    // shortcuts
    //Img& Img(int frame) { return m_Anim.GetFrame(frame); }
    Img const& ImgConst(int frame) const { return m_Anim.GetFrameConst(frame); }

    int NumFrames() const { return m_Anim.NumFrames(); }

    // kill!
    // palette manipulation
	RGBx const& GetColour(int n ) { return GetAnim().GetPaletteConst().GetColour(n); }
	void SetColour( int n, RGBx const& c ) { GetAnim().GetPalette().SetColour(n,c); }
//    RGBx const* GetPaletteConst() const { return GetAnimConst().GetPaletteConst().rawconst(); }

    Palette const& PaletteConst() const { return m_Anim.GetPaletteConst(); }

    // project holds FG and BG pens
	VColour FGPen() const { return m_FGPen; }
	VColour BGPen() const { return m_BGPen; }
	RGBx FGPenRGB() const;
	RGBx BGPenRGB() const;
	void SetFGPen( VColour c );
	void SetBGPen( VColour c );


    // return current filename of project (empty string if no name)
    std::string const& Filename() const { return m_Filename; }

    //--------------------------------------
    // Interface for Cmd to use
    //
    // tell project it's been modified
    // (within draw operation, use Draw_Damage() instead!)
	void Damage( Box const& b );
    // notify operations on frames in range [first,last)
    void Damage_FramesAdded(int first, int last);
    void Damage_FramesRemoved(int first, int last);

    //--------------------------------------
    // interface for tools to use, to enable the capturing of multiple
    // modifications into a single cmd.
    // TODO: KILLKILLKILL!
    // (use a Cmd_Draw held by the tool instead)

    // a tool which modified the image starts here...
    void Draw_Begin( Tool* tool, int frame );

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
    Project( Project const& );  // disallowed

    bool m_Expendable;

    VColour m_FGPen;
    VColour m_BGPen;

    // anim stores all the image data (even if it's a single frame anim :-)
    Anim m_Anim;

	std::set< ProjectListener* > m_Listeners;

	std::list< Cmd* > m_UndoStack;
	std::list< Cmd* > m_RedoStack;

    void DiscardUndoAndRedos();
    void SetModifiedFlag( bool newmodifiedflag );

    //
    Tool* m_DrawTool;
    int m_DrawFrame;

    // backup copy of image, used for rollback or undo generation during
    // drawing operation
    Img m_DrawBackup;
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

