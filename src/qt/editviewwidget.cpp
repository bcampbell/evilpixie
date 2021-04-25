#include "editviewwidget.h"

#include "../project.h"
#include "../tool.h"


#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QtWidgets/QShortcut>
#include <cassert>

EditViewWidget::EditViewWidget(Editor& editor, NodePath const& focus, int frame) :
	EditView(editor, focus, frame, 500, 500),
	m_Anchor(0, 0),
    m_Panning(false)
{
    setMouseTracking(true);
    // some keyboard shortcuts
    {
        QShortcut* s;

        s = new QShortcut( QKeySequence( "+" ), this );
        connect(s, SIGNAL( activated()), this, SLOT( zoomIn()));

        s = new QShortcut( QKeySequence( "-" ), this );
        connect(s, SIGNAL( activated()), this, SLOT( zoomOut()));

    }

}




static Button translatebutton( QMouseEvent* event )
{
    Qt::MouseButton qtmb = event->button();
    Qt::KeyboardModifiers mods = event->modifiers();

    if (qtmb == Qt::LeftButton)
    {
        if(mods & Qt::ControlModifier)
            return PAN;
        else
            return DRAW;
    }
    if (qtmb == Qt::RightButton)
        return ERASE;
    if (qtmb == Qt::MidButton)
        return PAN;
    return NONE;
}

void EditViewWidget::mousePressEvent(QMouseEvent *event)
{
    Point pos( event->pos().x(), event->pos().y() );
    OnMouseDown( pos, translatebutton(event) );
}

void EditViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    Point pos( event->pos().x(), event->pos().y() );
    OnMouseMove( pos );
}

void EditViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Point pos( event->pos().x(), event->pos().y() );
    OnMouseUp( pos, translatebutton(event) );
}

void EditViewWidget::wheelEvent(QWheelEvent *event)
{
    auto const wpos(event->position());
    Point viewpos((int)wpos.x(), (int)wpos.y());
    Point projpos = ViewToProj(viewpos);

	int dy = ((int)event->angleDelta().y()) / 120;
	int z = Zoom();
	z += dy;
	SetZoom(z);
	AlignView(viewpos, projpos);
}

void EditViewWidget::paintEvent(QPaintEvent * /* event */)
{
    Img const& src = Canvas();
    QImage image( (const uchar *)src.PtrConst_RGBX8(0,0), Canvas().W(), Canvas().H(), QImage::Format_RGB32 );

    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), image);
}

void EditViewWidget::resizeEvent(QResizeEvent *event)
{
    Resize( event->size().width(), event->size().height() );
}




// EditViewListener fn
void EditViewWidget::Redraw( Box const& b )
{
    update( b.x, b.y, b.w, b.h );
}


void EditViewWidget::zoomIn()
{
    SetZoom(Zoom()+1);
}

void EditViewWidget::zoomOut()
{
    SetZoom(Zoom()-1);
}

