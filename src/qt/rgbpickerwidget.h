#ifndef RGBPICKERWIDGET_H
#define RGBPICKERWIDGET_H


#include <QWidget>

#include <cstdio>
#include "../colours.h"


// Widget to show a RGB colourspace, and let the user pick colours with
// both left and right mousebutton.

class RGBPickerWidget : public QWidget
{
    Q_OBJECT
public:
	RGBPickerWidget();
	~RGBPickerWidget();

    // set the selected colour (but don't emit pickedLeft/RightButton() signal)
    void SetLeftSelected( Colour c );
    void SetRightSelected( Colour c );

    Colour LeftSelected() const { return PointToRGB(m_RightSel); }
    Colour RightSelected() const { return PointToRGB(m_LeftSel); }

signals:
    void pickedLeftButton( Colour c );
    void pickedRightButton( Colour c );

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void leaveEvent( QEvent *event );
    QSize sizeHint () const;
    QSize minimumSizeHint () const;

private:

    void UpdateBacking();
    QImage* m_Backing;

    QPoint RGBToPoint(Colour c) const;
    Colour PointToRGB(QPoint const&p) const;
    QPoint m_LeftSel;
    QPoint m_RightSel;
};

#endif // RGBPICKERWIDGET_H

