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

// A gui-neutral view for editing a project

class Editor;

class EditView : public ProjectListener
{
public:

	EditView( Editor& editor, int w, int h );
	virtual ~EditView();

    // ProjectListener Implementation
	virtual void OnDamaged( Box const& viewdmg );
    virtual void OnPaletteChanged( int n, RGBx const& c );
    virtual void OnPaletteReplaced();

	// these will all cause listener RedrawAll request
	void Resize( int w, int h );
	void SetZoom( int zoom );
	void SetOffset( Point const& projpos );	// in project coord (pixels)
    void AlignView( Point const& viewp, Point const& projp );

	Point const& Offset() const { return m_Offset; }
	int Zoom() const { return m_Zoom; }

	void OnMouseDown( Point const& viewpos, Button button );
	void OnMouseMove( Point const& viewpos );
	void OnMouseUp( Point const& viewpos, Button button );

	RGBImg const& CanvasConst() const { return *m_Canvas; }
	int Width() const { return m_ViewBox.w; }
	int Height() const { return m_ViewBox.h; }

	Point ViewToProj( Point const& viewpos ) const;
	Point ProjToView( Point const& projpos ) const;
	Box ViewToProj( Box const& viewbox ) const;
	Box ProjToView( Box const& projbox ) const;


	// For tools...
	void AddCursorDamage( Box const& viewdmg );
	RGBImg& Canvas() { return *m_Canvas; }

    Project& Proj() { return m_Editor.Proj(); }
    Editor& Ed() { return m_Editor; }

    void EraseCursor();


protected:
    // Needs to be implemented by the GUI layer
    virtual void Redraw( Box const& b ) = 0;
private:
    Editor& m_Editor;   // the editor this view belongs to

    Point m_PrevPos;  // proj coords of last mouse action (-1,-1)=none

    // TODO: canvas should probably be held by the gui layer... (editviewwidget)
	RGBImg* m_Canvas;
//	RGB* m_Canvas;
	Box m_ViewBox;	// x,y always 0

	int m_Zoom;
	// In project coords
	Point m_Offset;


    bool m_Panning;
    Point m_PanAnchor;

    //
    std::vector<Box> m_CursorDamage;

    void DrawView( Box const& viewbox, Box* affectedview=0  );
    void ConfineView();
};


inline Point EditView::ViewToProj( Point const& viewpos ) const
{
	return Point(
		viewpos.x/m_Zoom + m_Offset.x,
		viewpos.y/m_Zoom + m_Offset.y
		);
}

inline Point EditView::ProjToView( Point const& projpos ) const
{
	return Point(
		(projpos.x-m_Offset.x)*m_Zoom,
		(projpos.y-m_Offset.y)*m_Zoom
		);
}

inline Box EditView::ViewToProj( Box const& viewbox ) const
{
    return Box(
        ViewToProj( viewbox.TopLeft() ),
        viewbox.w/m_Zoom,
        viewbox.h/m_Zoom );
}

inline Box EditView::ProjToView( Box const& projbox ) const
{
    return Box(
        ProjToView( projbox.TopLeft() ),
        projbox.w*m_Zoom,
        projbox.h*m_Zoom );
}


#endif // EDITVIEW_H

