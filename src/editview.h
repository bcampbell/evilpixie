#ifndef EDITVIEW_H
#define EDITVIEW_H

#include "box.h"
#include "project.h"
#include "projectlistener.h"
#include "point.h"
#include "img.h"
#include "tool.h"
#include "editor.h"
#include "global.h"

#include <vector>

class Editor;

// EditView is a gui-neutral view for editing a project.
// Maintains a backing canvas (raw bitmap image) for displaying project
// and tool cursors. GUI windows can just blit that canvas to screen.
// Also handles tool interaction.
class EditView : public ProjectListener
{
public:

	EditView( Editor& editor, NodePath const& focus, int frame, int w, int h );
	virtual ~EditView();

    // ProjectListener Implementation
	virtual void OnDamaged(NodePath const& target, int frame, Box const& dmg) override;
    virtual void OnPaletteChanged(NodePath const& target, int frame, int index, Colour const& c) override;
    virtual void OnPaletteReplaced(NodePath const& target, int frame) override;
    virtual void OnModifiedFlagChanged(bool changed) override;
    virtual void OnFramesAdded(NodePath const& target, int first, int count) override;
    virtual void OnFramesRemoved(NodePath const& target, int first, int count) override;
    virtual void OnFramesBlatted(NodePath const& target, int first, int count) override;

	// these will all cause listener RedrawAll request
	void Resize( int w, int h );
	void SetZoom( int zoom );
    void SetFocus(NodePath const& focus);
    void SetFrame(int frame);
	void SetOffset( Point const& projpos );	// in project coord (pixels)
    void AlignView( Point const& viewp, Point const& projp );
    void CenterView();

	Point const& Offset() const { return m_Offset; }
	int Zoom() const { return m_Zoom; }
	int XZoom() const { return m_XZoom; }
	int YZoom() const { return m_YZoom; }
    NodePath const& Focus() const {return m_Focus;}
    int Frame() const {return m_Frame;}

	void OnMouseDown( Point const& viewpos, Button button );
	void OnMouseMove( Point const& viewpos );
	void OnMouseUp( Point const& viewpos, Button button );

	Img const& CanvasConst() const { return *m_Canvas; }
	int Width() const { return m_ViewBox.w; }
	int Height() const { return m_ViewBox.h; }

	Point ViewToProj( Point const& viewpos ) const;
	Point ProjToView( Point const& projpos ) const;
	Box ViewToProj( Box const& viewbox ) const;
	Box ProjToView( Box const& projbox ) const;


    // helper to find the currently-focused image
    Img& FocusedImg() {return Proj().GetImg(m_Focus, m_Frame);}
    Img const& FocusedImgConst() const {return Proj().GetImgConst(m_Focus, m_Frame);}
    Palette const& FocusedPaletteConst() const {return Proj().PaletteConst(m_Focus, m_Frame);}

	// For tools...
    // TODO: not enough to overlay brushes upon the view canvas.
    // need to integrate into view composition logic (eg, showing an
    // RGBA8 brush in REPLACE mode needs to be able to redraw the
    // underlying image and blend with it).
	void AddCursorDamage( Box const& viewdmg );
	Img& Canvas() { return *m_Canvas; }

    Project& Proj() const { return m_Editor.Proj(); }
    Editor& Ed() const { return m_Editor; }

    void EraseCursor();


protected:
    // Needs to be implemented by the GUI layer
    virtual void Redraw( Box const& b ) = 0;
private:
    Editor& m_Editor;   // the editor this view belongs to

    Point m_PrevPos;  // proj coords of last mouse action (-1,-1)=none

    // TODO: canvas should probably be held by the gui layer... (editviewwidget)
	Img* m_Canvas;
	Box m_ViewBox;	// x,y always 0

    // current layer & frame being edited
    NodePath m_Focus;
    int m_Frame;

	int m_Zoom;
	int m_XZoom;
	int m_YZoom;
	// In project coords
	Point m_Offset;


    bool m_Panning;
    Point m_PanAnchor;

    // list of view rects affected by cursor drawing
    std::vector<Box> m_CursorDamage;

    void DrawView( Box const& viewbox, Box* affectedview=0  );
    void ConfineView();
};



inline Point EditView::ViewToProj( Point const& viewpos ) const
{
	return Point(
		viewpos.x/m_XZoom + m_Offset.x,
		viewpos.y/m_YZoom + m_Offset.y
		);
}

inline Point EditView::ProjToView( Point const& projpos ) const
{
	return Point(
		(projpos.x-m_Offset.x)*m_XZoom,
		(projpos.y-m_Offset.y)*m_YZoom
		);
}

inline Box EditView::ViewToProj( Box const& viewbox ) const
{
    return Box(
        ViewToProj( viewbox.TopLeft() ),
        viewbox.w/m_XZoom,
        viewbox.h/m_YZoom );
}

inline Box EditView::ProjToView( Box const& projbox ) const
{
    return Box(
        ProjToView( projbox.TopLeft() ),
        projbox.w*m_XZoom,
        projbox.h*m_YZoom );
}


#endif // EDITVIEW_H

