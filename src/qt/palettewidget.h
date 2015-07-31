#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H


#include <QtWidgets/QWidget>

#include <cstdio>

#include "../palette.h"


class PaletteWidget : public QWidget
{
    Q_OBJECT
public:
	PaletteWidget(Palette const& src);

    void SetPalette( Palette const& pal )
        { m_Palette = pal; update(); }

    void SetColour( int n, Colour c )
        { m_Palette.SetColour(n,c); update(); }

    void EnableRangePicking( bool yesno )
        { m_RangePickingEnabled=yesno; }

    bool RangeValid() const { return m_RangeFirst!=-1 && m_RangeLast!=-1 && m_RangeFirst!=m_RangeLast; }
    int RangeFirst() const { return m_RangeFirst; }
    int RangeLast() const { return m_RangeLast; }

    // set the selected colour indexes (but don't emit pickedLeft/RightButton() signal)
    void SetLeftSelected( int n );
    void SetRightSelected( int n );

    int LeftSelected() const { return m_LeftSelected; }
    int RightSelected() const { return m_RightSelected; }

signals:
    void pickedLeftButton( int c );
    void pickedRightButton( int c );
    void rangeAltered();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void leaveEvent( QEvent *event );

    QSize sizeHint () const;
    QSize minimumSizeHint () const;

private:
    PaletteWidget();    // disallowed

    Palette m_Palette;
	int m_Hover;

    void CalcCellRect( int n, QRect& r ) const;
    QRect CellRect( int n ) const;

	void DrawCell( QPainter& painter, int n );
    void DrawOverlays( QPainter& painter );
    void DrawRangeOverlay( QPainter& painter, int from, int to, bool strong );
	int PickCell( int x, int y );
//	enum {N_COLS=8};
//	enum {N_ROWS=32};

    int Cols() const;
    int Rows() const;

    bool m_RangePickingEnabled;

    int m_DragAnchor;
    bool m_DraggingOutRange;

    int m_RangeFirst;
    int m_RangeLast;

    int m_LeftSelected;
    int m_RightSelected;
};

#endif // PALETTEWIDGET_H

