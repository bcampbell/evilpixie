#include "editviewwidget.h"

#include "../project.h"
#include "../tool.h"


#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <cassert>

EditViewWidget::EditViewWidget( Editor& editor ) :
	EditView( editor,500,500 ),
	m_Anchor(0,0),
    m_Panning(false)
{
    setMouseTracking(true);

}




static Button translatebutton( QMouseEvent* event )
{
    Qt::MouseButton qtmb = event->button();
    Qt::KeyboardModifiers mods = event->modifiers();

    if (qtmb == Qt::LeftButton)
    {
        if(mods & Qt::AltModifier)
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
    Point viewpos( event->pos().x(), event->pos().y() );
    Point projpos = ViewToProj( viewpos );

	int dy = event->delta()/120;
	int z = Zoom();
	z += dy;
	if( z<1 )
		z=1;
	if( z>128 )
		z=128;
	SetZoom(z);
	AlignView( viewpos, projpos );
}


void EditViewWidget::paintEvent(QPaintEvent * /* event */)
{
	RGBImg const& src = Canvas();
    QImage::QImage image( (const uchar *)src.PtrConst(0,0), Canvas().W(), Canvas().H(), QImage::Format_RGB32 );

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

