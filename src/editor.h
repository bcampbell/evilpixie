#ifndef EDITOR_H
#define EDITOR_H

class EditView;
class Brush;
class Tool;
class Cmd;

#include "project.h"
#include "projectlistener.h"
#include "mousestyle.h"
#include <set>




struct DrawMode { 
    enum Mode {
        DM_NORMAL=0, // use brush colour and transparency rules
        DM_COLOUR,  // use normal transparency rules, but replace colour with constant
        DM_REPLACE,  // replace wholesale (ignore transparency).
        DM_RANGE,   // increment/decrement according to current range.
        DM_NUM_MODES
    };
    Mode mode;
    // TODO: params for drawmodes here (eg lighten/darken ammount)

    DrawMode() : mode(DM_NORMAL) {}
    DrawMode(Mode m) : mode(m) {}
};


// the core backend (non-gui) part of the editor
class Editor : public ProjectListener
{
public:
    Editor( Project* proj );    // editor takes ownership of project
    ~Editor();

    bool GridActive() const           { return m_GridActive; }
    void ActivateGrid( bool yesno )     { m_GridActive=yesno; }
    void SetGrid( Box const& g )        { m_Grid=g; }
    Box const& Grid() const             { return m_Grid; }

    // snap p to grid, if active (else left unchanged)
    void GridSnap( Point& p );

    void UseTool( int tooltype, bool notifygui=true );
    int CurrentToolType() const { return m_CurrentToolType; }
    Tool& CurrentTool() { return *m_Tool; }
//    void SetTool( Tool* newtool, bool notifygui=true );

    DrawMode const& Mode() const { return m_Mode; }
    void SetMode( DrawMode const& mode) { m_Mode= mode; }

	PenColour FGPen() const { return m_FGPen; }
	PenColour BGPen() const { return m_BGPen; }
	void SetFGPen( PenColour const& pen );
	void SetBGPen( PenColour const& pen );

    // TODO: notifications?
	void SetCurrentRange(Box const& range) {m_CurrRange = range;}
    Box CurrentRange() {return m_CurrRange;}

    // -1 for custom brush, 0-3 for stdbrush
    void SetBrush( int n );
    int GetBrush() const { return m_Brush; }
    Brush& CurrentBrush();

    // stuff for the GUI to implement
    virtual void GUIShowError( const char* msg ) = 0;
    virtual void UpdateMouseInfo( Point const& ) =0;
    virtual void SetMouseStyle( MouseStyle ) {} // see note in ~Editor()
    virtual void OnUndoRedoChanged() {}

    // the project that this editor owns
    Project& Proj() const { return *m_Project; }

    // automatically called by EditView ctor/dtor
    void AddView( EditView* v )
		{ m_Views.insert( v ); }
    void RemoveView( EditView* v )
		{ m_Views.erase( v ); }

    // show/hide the current tool cursor for all the views this editor has
    void ShowToolCursor();
    void HideToolCursor();

	// Add a cmd to the undo stack.
	// cmd->Do() will be called.
	// Ownership of cmd passes to Project.
	void AddCmd( Cmd* cmd );

	// undo last cmd (or do nothing)
	void Undo();
	// redo last undo (or do nothing)
	void Redo();

    // returns the most recent cmd on the stack (or null if none)
    // The idea is that for some operations (eg palette editing)
    // it makes more sense to accumulate changes in a single cmd
    // than to add lots of new ones.
    Cmd* TopCmd()
        { return m_UndoStack.empty() ? 0:m_UndoStack.back(); }


	bool CanUndo() const;
	bool CanRedo() const;

    // projectlistener implementation:
    // Not used by Editor itself, but GUI overrides some.

protected:
    // stuff to notify the gui...
    virtual void OnToolChanged() = 0;
    virtual void OnBrushChanged() = 0;
    virtual void OnPenChanged() = 0;

private:
    Editor();                   // disallowed
    Editor( Editor const& );    // disallowed

    Project* m_Project;

    // track the views this editor is managing
	std::set< EditView* > m_Views;

    Tool* m_Tool;
    int m_CurrentToolType;

    DrawMode m_Mode;

    int m_Brush; // StdBrush index, or -1 for custombrush

    // grid stuff
    bool m_GridActive;
    Box m_Grid;

    PenColour m_FGPen;
    PenColour m_BGPen;

    Box m_CurrRange;

    // undo/redo stuff
	std::list< Cmd* > m_UndoStack;
	std::list< Cmd* > m_RedoStack;

    void DiscardUndoAndRedos();
};


inline bool Editor::CanUndo() const
    { return !m_UndoStack.empty(); }

inline bool Editor::CanRedo() const
    { return !m_RedoStack.empty(); }



#endif // EDITOR_H



