#include "palettewidget.h"
#include "guistuff.h"

#include "../project.h"

#include <algorithm>
#include <cassert>

#include <QPainter>
#include <QMouseEvent>


PaletteWidget::PaletteWidget(Palette const& src) :
    m_Palette(src),
    m_Hover(-1),
    m_RangePickingEnabled(false),
    m_DragAnchor(-1),
    m_DraggingOutRange(false),
    m_RangeFirst(-1),
    m_RangeLast(-1),
    m_LeftSelected(-1),
    m_RightSelected(-1)
{
    setMouseTracking(true);

    setMinimumSize( Cols()*4, Rows()*4 );
}



void PaletteWidget::SetLeftSelected( int n )
{
    if( n==m_LeftSelected )
        return;
    m_LeftSelected = n;
    update();
}

void PaletteWidget::SetRightSelected( int n )
{
    if( n==m_RightSelected )
        return;
    m_RightSelected = n;
    update();
}

QSize PaletteWidget::sizeHint () const
    { return QSize( Cols()*10, Rows()*12 ); }

QSize PaletteWidget::minimumSizeHint () const
    { return QSize( Cols()*8, Rows()*8 ); }

void PaletteWidget::mousePressEvent(QMouseEvent *event)
{
    int cell = PickCell( event->pos().x(), event->pos().y() );
    if( cell == -1 )
        return;

    if( RangeValid() )
    {
        m_RangeFirst = m_RangeLast = -1;
        emit rangeAltered();
    }

    if( event->button() == Qt::LeftButton )
    {
        if( m_RangePickingEnabled ) // && (event->modifiers() & Qt::ShiftModifier) )
        {
            m_DraggingOutRange = true;
            m_DragAnchor = m_Hover = cell;
        }
        else
        {
            m_LeftSelected = cell;
            emit pickedLeftButton( cell );
        }
    }
    if( event->button() == Qt::RightButton )
    {
        m_RightSelected = cell;
        emit pickedRightButton( cell );
    }
    update();
}

void PaletteWidget::mouseMoveEvent(QMouseEvent *event)
{
    int cell = PickCell( event->pos().x(), event->pos().y() );
    if( cell == m_Hover )
        return;

    m_Hover = cell;

    update();
}

void PaletteWidget::mouseReleaseEvent(QMouseEvent *)
{
    if( m_DraggingOutRange )
    {
        m_DraggingOutRange = false;
        m_RangeFirst = std::min(m_DragAnchor,m_Hover );
        m_RangeLast = std::max(m_DragAnchor,m_Hover );
        if( m_RangeFirst == m_RangeLast )
        {
            m_LeftSelected = m_RangeFirst;
            emit pickedLeftButton( m_LeftSelected );
        }
        else
        {
            emit rangeAltered();
        }
        update();
    }
}

void PaletteWidget::leaveEvent( QEvent *event )
{
    m_Hover = -1;
    update();
}


void PaletteWidget::wheelEvent(QWheelEvent *)
{
}

void PaletteWidget::resizeEvent(QResizeEvent *)
{
}


void PaletteWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
   	painter.setPen(Qt::NoPen);
    //painter.setBrush( QColor( 0,0,0) );
    painter.setBrush( *g_GUIStuff.checkerboard );
   	painter.drawRect( rect() );

    int n;
    for( n=0; n<256; ++n )
    {
        DrawCell( painter, n );
    }
    DrawOverlays( painter );
}


void PaletteWidget::CalcCellRect( int n, QRect& r ) const
{
    int row = n%Rows();
    int col = n/Rows();

    // cell size is not uniform - use noddy fixedpoint to calculate it
    const int S = 4096;
    int cw = size().width()*S / Cols();
    int ch = size().height()*S / Rows();

    int x = (col*cw)/S;
    int xnext = ((col+1)*cw)/S;
    int y = (row*ch)/S;
    int ynext = ((row+1)*ch)/S;

    int w = xnext-x;
    int h = ynext-y;

    r.setRect( x,y,w,h ); 
}

QRect PaletteWidget::CellRect( int n ) const
{
    QRect r;
    CalcCellRect( n,r);
    return r;
}



void PaletteWidget::DrawCell( QPainter& painter, int n )
{
    QRect cellrect;
    CalcCellRect( n, cellrect );

    //cellrect.adjust(0,0,-1,-1);

    Colour c(m_Palette.GetColour(n));

    painter.setPen(Qt::NoPen);

    painter.setBrush(QColor(c.r, c.g, c.b, c.a));
    painter.drawRect( cellrect );

}


int PaletteWidget::Cols() const
{
    int n=m_Palette.NumColours();
    if(n<=8)
        return 1;
    if(n<=16)
        return 2;
    if(n<=32)
        return 4;
    return 8;
}

int PaletteWidget::Rows() const
{
    int n=m_Palette.NumColours();
    return n/Cols();
}


int PaletteWidget::PickCell( int x, int y )
{
    // cell size is not uniform - use noddy fixedpoint to calculate it
    const int S = 4096;
    const int cw = size().width()*S/Cols();
    const int ch = size().height()*S/Rows();
    int col = (x*S/cw);
    int row = (y*S/ch);
    if( col < 0 )
        col=0;
    if(col>=Cols() )
        col=Cols()-1;
    if( row < 0 )
        row=0;
    if(row>=Rows() )
        row=Rows()-1;

    int cell = col*Rows() + row;
    return cell;
}


void PaletteWidget::DrawOverlays( QPainter& painter )
{


    if( m_DraggingOutRange )
    {
        int first = std::min(m_DragAnchor,m_Hover );
        int last = std::max(m_DragAnchor,m_Hover );
        DrawRangeOverlay( painter, first, last, false );
    }
    else if( RangeValid() )
    {
        DrawRangeOverlay( painter, m_RangeFirst, m_RangeLast, true );
    }

    if( m_Hover != -1 )
    {
 
        QRect r = CellRect( m_Hover );

        QPen blackpen( Qt::SolidLine );
        blackpen.setWidth(3);
        blackpen.setColor( QColor(0,0,0,128) );
        blackpen.setJoinStyle( Qt::RoundJoin );
        blackpen.setCapStyle( Qt::RoundCap );

        QPen whitepen( Qt::SolidLine );
        whitepen.setWidth(1);
        whitepen.setColor( QColor(255,255,255,128) );

        painter.setPen( blackpen );
        painter.setBrush( QBrush( QColor( 255,255,255,16 ) ) );
        painter.drawRect(r);

        painter.setPen( whitepen );
        painter.setBrush( Qt::NoBrush );
        painter.drawRect(r);
    }

    if( m_LeftSelected != -1 )
    {
        QRect r = CellRect( m_LeftSelected );
        r.adjust(0,0,-1,-1);

        QPen blackpen( Qt::SolidLine );
        blackpen.setWidth(3);
        blackpen.setColor( QColor(0,0,0,128) );
        blackpen.setJoinStyle( Qt::RoundJoin );
        blackpen.setCapStyle( Qt::RoundCap );

        QPen pen( Qt::SolidLine );
        pen.setWidth(1);
        pen.setColor( QColor(255,255,255) );

        painter.setPen( blackpen );
        painter.setBrush( Qt::NoBrush );
        painter.drawRect(r);

        painter.setPen( pen );
        painter.setBrush( Qt::NoBrush );
        painter.drawRect(r);

    }
    if( m_RightSelected != -1 )
    {
        QRect r = CellRect( m_RightSelected );
        r.adjust(0,0,-1,-1);

        QPen blackpen( Qt::SolidLine );
        blackpen.setWidth(3);
        blackpen.setColor( QColor(0,0,0,128) );
        blackpen.setJoinStyle( Qt::RoundJoin );
        blackpen.setCapStyle( Qt::RoundCap );

        QPen whitepen( Qt::SolidLine );
        whitepen.setWidth(1);
        whitepen.setColor( QColor(128,128,128 ) );

        painter.setPen( blackpen );
        painter.setBrush( Qt::NoBrush );
        painter.drawRect(r);

        painter.setPen( whitepen );
        painter.setBrush( Qt::NoBrush );
        painter.drawRect(r);

    }
}


void PaletteWidget::DrawRangeOverlay( QPainter& painter, int from, int to, bool strong )
{
    int from_col = from / Rows();
//    int from_row = from % N_ROWS;

    int to_col = to / Rows();
//    int to_row = to % N_ROWS;

    assert( to>=from );


    QPainterPath p;

    // add columns of section
    int c;
    for( c=from_col; c<=to_col; ++c )
    {
        int top = std::max( from, c*Rows() );
        int bot = std::min( to, (c*Rows())+(Rows()-1));

        QRect rtop = CellRect(top);
        rtop.adjust(-1,-1,-1,-1);
        QRect rbot = CellRect(bot);
        rtop.adjust(-1,-1,-1,-1);

        QPainterPath ptmp;
        ptmp.addRect( rtop.united( rbot ) );
        p = p.united( ptmp );
    }


    QPen blackpen( Qt::SolidLine );
    blackpen.setWidth(7);
    blackpen.setColor( QColor(0,0,0,128) );
    blackpen.setJoinStyle( Qt::RoundJoin );
    blackpen.setCapStyle( Qt::RoundCap );

    QPen whitepen( Qt::SolidLine );
    whitepen.setWidth(1);
    whitepen.setColor( QColor(255,255,255) );

    if( strong )
        painter.setBrush( Qt::NoBrush );
    else
        painter.setBrush( QBrush( QColor( 255,255,255,32 ) ) );
    painter.setPen(blackpen);
    painter.drawPath( p );

    painter.setBrush( Qt::NoBrush );
    painter.setPen(whitepen);
    painter.drawPath( p );
}


