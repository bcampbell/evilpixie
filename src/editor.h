#ifndef EDITOR_H
#define EDITOR_H

class EditView;
class Brush;
class Tool;


#include "project.h"
#include "projectlistener.h"
#include "mousestyle.h"
#include <set>


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

	PenColour FGPen() const { return m_FGPen; }
	PenColour BGPen() const { return m_BGPen; }
	void SetFGPen( PenColour const& pen );
	void SetBGPen( PenColour const& pen );


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
    Project& Proj() { return *m_Project; }


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

	bool CanUndo() const;
	bool CanRedo() const;

    // projectlistener implementation:
    // editor is a projectlistener so that gui can react to changes
    // (editviews owned by the editor are also projectlisteners)
	virtual void OnDamaged( Box const& ) {}
    virtual void OnPaletteChanged( int, RGBx const& ) {}


protected:
    // stuff to notify the gui...
    virtual void OnToolChanged() = 0;
    virtual void OnBrushChanged() = 0;
    virtual void OnPenChanged() = 0;

    // options
    bool m_SaveBGAsTransparent;
private:
    Editor();                   // disallowed
    Editor( Editor const& );    // disallowed

	// editing state stuff
    Tool* m_Tool;
    int m_CurrentToolType;

    int m_Brush; // StdBrush index, or -1 for custombrush

    Project* m_Project;

    // track the views this editor is managing
	std::set< EditView* > m_Views;

    // grid stuff
    bool m_GridActive;
    Box m_Grid;

    PenColour m_FGPen;
    PenColour m_BGPen;

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



