#include "palettewidget.h"
#include "guistuff.h"

#include "../project.h"
#include "../colours.h"

#include <algorithm>
#include <cassert>

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>

PaletteWidget::PaletteWidget(Palette const& src) :
    m_Palette(src),
    m_Hover(-1),
    m_DnDEnabled(false),
    m_RangePickingEnabled(false),
    m_DraggingOutRange(false),
    m_RangeFirst(-1),
    m_RangeLast(-1),
    m_LeftSelected(-1),
    m_RightSelected(-1)
{
    setMouseTracking(true);
    setMinimumSize( Cols()*4, Rows()*4 );
}

void PaletteWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->possibleActions() & Qt::CopyAction) {
        event->acceptProposedAction();
    }
}

void PaletteWidget::dropEvent(QDropEvent *event)
{
    if( event->mimeData()->hasColor() ) {
        QColor c = qvariant_cast<QColor>(event->mimeData()->colorData());
        emit colourDropped(m_Hover, Colour(c.red(), c.green(), c.blue(), c.alpha()));
    }
}

void PaletteWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->proposedAction() != Qt::CopyAction) {
        return;
    }
    if( !event->mimeData()->hasColor() ) {
        return;
    }

    int cell = PickCell( event->pos().x(), event->pos().y() );
    if( cell != -1 ) {
        event->acceptProposedAction();
    }
    if( cell != m_Hover ) {
        m_Hover = cell;
        update();
    }
}

void PaletteWidget::EnableDnD(bool yesno) {
    setAcceptDrops(yesno);
    m_DnDEnabled = yesno;
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
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ShiftModifier) {
            // shift-click: select range
            int cell = PickCell(event->pos().x(),event->pos().y());
            if( cell != -1 && m_LeftSelected != -1 ) {
                m_RangeFirst = std::min(m_LeftSelected, cell);
                m_RangeLast = std::max(m_LeftSelected, cell);
                emit rangeAltered();
                update();
            }
        } else {
            m_Anchor = event->pos();
            if (m_RangeFirst >= 0 || m_RangeLast >= 0) {
                m_RangeFirst = -1;
                m_RangeLast = -1;
                emit rangeAltered();
                update();
            }
        }
    }
}

void PaletteWidget::mouseMoveEvent(QMouseEvent *event)
{
    float distsq = (event->pos() - m_Anchor).manhattanLength();

    int cell = PickCell( event->pos().x(), event->pos().y() );
    if (m_DraggingOutRange) {
        if (cell >= 0) {
            int first = PickCell( m_Anchor.x(), m_Anchor.y() );
            m_RangeFirst = std::min(first, cell);
            m_RangeLast = std::max(first, cell);
            emit rangeAltered();
            update();
        }
        return;
    }

    if (distsq > 1.0f &&
        m_RangePickingEnabled && cell >= 0 &&
        event->buttons() & Qt::LeftButton &&
        event->modifiers() & Qt::ShiftModifier) {
        // Start range selection.
        m_DraggingOutRange = true;
        m_RangeFirst = cell;
        m_RangeLast = cell;
        emit rangeAltered();
        update();
        return;
    }
   
    if (distsq > 1.0f &&
        cell >=0 && m_DnDEnabled && event->buttons() & Qt::LeftButton)  {
        // Start DnD drag operation.
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // "application/x-color"
        Colour c(m_Palette.GetColour(cell));
        QColor qc(c.r, c.g, c.b, c.a);
        mimeData->setColorData(qc);

        // rgba + palette index
        // TODO: encode index validity.
        uint8_t buf[5] = {c.r, c.g, c.b, c.a, (uint8_t)cell};
        mimeData->setData("application/x-evilpixie-pen", QByteArray((const char*)buf,5));

        drag->setMimeData(mimeData);
        QPixmap swatch(16,16);
        swatch.fill(qc);
        drag->setPixmap(swatch);

        drag->exec(Qt::CopyAction);
        return;
    }

    if (m_Hover != cell) {
        m_Hover = cell;
        update();
    }
}

void PaletteWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if( m_DraggingOutRange )
    {
        m_DraggingOutRange = false;
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
    } else if (m_Hover >= 0) {
        if (event->button() == Qt::LeftButton) {
            m_LeftSelected = m_Hover;
            emit pickedLeftButton( m_LeftSelected );
        } else if (event->button() == Qt::RightButton) {
            m_RightSelected = m_Hover;
            emit pickedRightButton( m_RightSelected );
        }
        update();
    }
}

void PaletteWidget::leaveEvent( QEvent * /*event*/ )
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
    for( n=0; n<m_Palette.NumColours(); ++n )
    {
        bool isFG = m_LeftSelected == n;
        bool isBG = m_RightSelected == n;
        RenderCell(painter, CellRect(n), m_Palette.GetColour(n), isFG, isBG);
    }
    DrawOverlays( painter );
}


QRect PaletteWidget::CellRect(int n) const
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

    return QRect(x, y, w, h);
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
    if(col < 0 || col >=Cols() ||
       row < 0 || row >= Rows()) {
      return -1;
    }
    int cell = col*Rows() + row;
    return cell;
}


void PaletteWidget::DrawOverlays( QPainter& painter )
{
    if (RangeValid()) {
        if (m_DraggingOutRange) {
            DrawRangeOverlay(painter, m_RangeFirst, m_RangeLast, false);
        } else {
            DrawRangeOverlay(painter, m_RangeFirst, m_RangeLast, true);
        }
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

#if 0
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
#endif
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


