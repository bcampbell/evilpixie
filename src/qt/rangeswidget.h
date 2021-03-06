#ifndef RANGESWIDGET_H
#define RANGESWIDGET_H

#include <QtWidgets/QWidget>

#include <cstdio>

#include "../colours.h"
#include "../layer.h"  // for NodePath
#include "../point.h"
#include "../projectlistener.h"
#include "../ranges.h"

class Editor;

// Widget for picking and editing colour ranges.
class RangesWidget : public QWidget, ProjectListener
{
    Q_OBJECT
public:
	RangesWidget(QWidget *parent, Editor& ed, NodePath const& target, int frame);
    ~RangesWidget();

    // Reconfigure for another layer/frame.
    // (doesn't emit any signals)
    void SetFocus(NodePath const& target, int frame);

    void SetFGPen(PenColour const& pen) {m_FGPen = pen; update();}
    void SetBGPen(PenColour const& pen) {m_BGPen = pen; update();}

    Box CurrentRange() const {return m_CurrRange;}
signals:
    // upon sucessful drop
    void colourDropped(int idx, Colour const& c);

    void pickedFGPen(PenColour fg);
    void pickedBGPen(PenColour bg);

    void pickedRange();
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

    // ProjectListener implementation
    // (we're only interested in range-related stuff)
    virtual void OnRangesBlatted(NodePath const& target, int frame) override;

private:
    RangesWidget();    // disallowed
    Point PickCell(QPoint const& pos) const;
    QRect CalcRect(Box const& box) const;
    void delSelected();
    Editor& m_Ed;
    NodePath m_Focus;
    int m_Frame;

    int m_Cols;
    int m_Rows;

    PenColour m_FGPen;
    PenColour m_BGPen;

    // Currently-selected range (can be empty)
    Box m_CurrRange;

};

#endif // RANGESWIDGET_H

