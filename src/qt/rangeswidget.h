#ifndef RANGESWIDGET_H
#define RANGESWIDGET_H


#include <QtWidgets/QWidget>

#include <cstdio>

#include "../palette.h"



struct Range {
    std::vector<PenColour> pens;
    // Cosmetic stuff. For editing ranges with pretty layout.
    int x;
    int y;
    bool horizontal;
};


class RangesWidget : public QWidget
{
    Q_OBJECT
public:
	RangesWidget(QWidget *parent, Palette const& palette);

signals:
    // upon sucessful drop
    void colourDropped(int idx, Colour const& c);

    void pickedFGPen(PenColour fg);
    void pickedBGPen(PenColour bg);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void leaveEvent( QEvent *event );
#if 0
    QSize sizeHint () const;
    QSize minimumSizeHint () const;
#endif

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    RangesWidget();    // disallowed
    void CalcCellRect(int col, int row, QRect& r) const;

    Palette m_Palette;
    std::vector<Range> m_Ranges;

    int m_Rows;
    int m_Cols;

    PenColour m_FGPen;
    PenColour m_BGPen;
};

#endif // RANGESWIDGET_H

