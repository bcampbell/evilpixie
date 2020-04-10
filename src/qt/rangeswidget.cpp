#include "rangeswidget.h"
#include "guistuff.h"

//#include "../project.h"
//#include "../colours.h"

#include <algorithm>
#include <cassert>

#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>





RangesWidget::RangesWidget(QWidget *parent, Palette const& palette) :
    QWidget(parent),
    m_Palette(palette),
    m_Cols(4),
    m_Rows(8)
{
    setMouseTracking(true);
    //setMinimumSize( Cols()*4, Rows()*4 );
    setAcceptDrops(true);

    Range r;
    r.x=2;
    r.y=0;
    r.horizontal = false;
    for (int idx : {1,2,3,4,5}) {
        r.pens.push_back(PenColour(palette.GetColour(idx), idx));
    }
    m_Ranges.push_back(r);

    r.x=1;
    r.y=2;
    r.horizontal = true;
    r.pens = {};
    for (int idx : {31,3,14,15}) {
        r.pens.push_back(PenColour(palette.GetColour(idx), idx));
    }
    m_Ranges.push_back(r);


}

void RangesWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->possibleActions() & Qt::CopyAction) {
        event->acceptProposedAction();
    }
}

void RangesWidget::dropEvent(QDropEvent *event)
{
#if 0
    if( event->mimeData()->hasColor() ) {
        QColor c = qvariant_cast<QColor>(event->mimeData()->colorData());
        emit colourDropped(m_Hover, Colour(c.red(), c.green(), c.blue(), c.alpha()));
    }
#endif
}

void RangesWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->proposedAction() != Qt::CopyAction) {
        return;
    }
    if( !event->mimeData()->hasColor() ) {
        return;
    }
/*
    int cell = PickCell( event->pos().x(), event->pos().y() );
    if( cell != -1 ) {
        event->acceptProposedAction();
    }
    if( cell != m_Hover ) {
        m_Hover = cell;
        update();
    }
*/
}


/*
QSize RangesWidget::sizeHint () const
    { return QSize( Cols()*10, Rows()*12 ); }

QSize RangesWidget::minimumSizeHint () const
    { return QSize( Cols()*8, Rows()*8 ); }
*/

void RangesWidget::mousePressEvent(QMouseEvent *event)
{
#if 0
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
#endif

    // find range(s) and pen under the click.
    for (Range const& rng : m_Ranges) {
        const int S = 4096;
        int cw = size().width()*S / m_Cols;
        int ch = size().height()*S / m_Rows;
        int col = (S * event->pos().x()) / cw;
        int row = (S * event->pos().y()) / ch;

        int idx = -1;
        if (rng.horizontal) {
            if (rng.y == row) {
                idx = col - rng.x;
            }
        } else {
            if (rng.x == col) {
                idx = row - rng.y;
            }
        }

        if (idx >=0 && idx < (int)rng.pens.size()) {
            printf("PICK! %d\n", rng.pens[idx].idx());
            if (event->button() == Qt::LeftButton) {
                emit pickedFGPen(rng.pens[idx]);
            } else if (event->button() == Qt::RightButton) {
                emit pickedFGPen(rng.pens[idx]);
            }
            break;
        }
    }
}

void RangesWidget::mouseMoveEvent(QMouseEvent *event)
{
#if 0
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

        Colour c(m_Palette.GetColour(cell));
        QColor qc(c.r, c.g, c.b, c.a);
        mimeData->setColorData(qc);
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
#endif
}

void RangesWidget::mouseReleaseEvent(QMouseEvent *event)
{
#if 0
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
#endif
}

void RangesWidget::leaveEvent( QEvent * /*event*/ )
{
#if 0
    m_Hover = -1;
    update();
#endif
}


void RangesWidget::wheelEvent(QWheelEvent *)
{
}

void RangesWidget::resizeEvent(QResizeEvent *)
{
}


void RangesWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
   	painter.setPen(Qt::NoPen);
    //painter.setBrush( QColor( 0,0,0) );
    painter.setBrush( *g_GUIStuff.checkerboard );
   	painter.drawRect( rect() );

    for (Range const& range: m_Ranges) {
        int x = range.x;
        int y = range.y;
        for (auto pen : range.pens) {
            QRect cellrect;
            CalcCellRect(x, y, cellrect );
            Colour c(pen.rgb());

            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(c.r, c.g, c.b, c.a));
            painter.drawRect( cellrect );
            if (range.horizontal) {
                ++x;
            } else {
                ++y;
            }
        }
    }
}


void RangesWidget::CalcCellRect(int col, int row, QRect& r) const
{
    // cell size is not uniform - use noddy fixedpoint to calculate it
    const int S = 4096;
    int cw = size().width()*S / m_Cols;
    int ch = size().height()*S / m_Rows;

    int x = (col*cw)/S;
    int xnext = ((col+1)*cw)/S;
    int y = (row*ch)/S;
    int ynext = ((row+1)*ch)/S;

    int w = xnext-x;
    int h = ynext-y;

    r.setRect( x,y,w,h ); 
}


