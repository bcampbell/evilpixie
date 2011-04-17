#ifndef EDITVIEWWIDGET_H
#define EDITVIEWWIDGET_H


#include <cstdio>
#include "../editview.h"

#include <QWidget>


class EditViewWidget : public QWidget, EditView
{
    Q_OBJECT
public:
	EditViewWidget( Editor& editor );

	// Editview virtuals
	virtual void Redraw( Box const& b );

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
public slots:
    void zoomIn();
    void zoomOut();

private:

	Point m_Anchor;

    bool m_Panning;

};

#endif // EDITVIEWWIDGET_H

