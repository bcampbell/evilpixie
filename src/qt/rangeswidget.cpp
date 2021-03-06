#include "rangeswidget.h"
#include "guistuff.h"

#include "../cmd.h"
#include "../project.h"
#include "../editor.h"

#include <algorithm>
#include <cassert>

#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QShortcut>

RangesWidget::RangesWidget(QWidget *parent, Editor& ed, NodePath const& target, int frame) :
    QWidget(parent),
    m_Ed(ed),
    m_Focus(target),
    m_Frame(frame),
    m_Cols(4),
    m_Rows(8),
    m_CurrRange(0, 0, 0, 0)
{
    setMouseTracking(true);
    //setMinimumSize( Cols()*4, Rows()*4 );
    setAcceptDrops(true);

    RangeGrid& ranges = m_Ed.Proj().Ranges(m_Focus, m_Frame);
    m_Cols = ranges.Bound().w;
    m_Rows = ranges.Bound().h;

    m_Ed.Proj().AddListener(this);

    QShortcut* sht = new QShortcut(QKeySequence::Delete, this);
    connect(sht, &QShortcut::activated, this, &RangesWidget::delSelected);

}

RangesWidget::~RangesWidget()
{
    m_Ed.Proj().RemoveListener(this);
}


void RangesWidget::SetFocus(NodePath const& target, int frame)
{
    m_Focus = target;
    m_Frame = frame;
    RangeGrid& ranges = m_Ed.Proj().Ranges(m_Focus, m_Frame);
    m_Cols = ranges.Bound().w;
    m_Rows = ranges.Bound().h;
    update();
}


// Map mouse coords to cell col&row.
Point RangesWidget::PickCell(QPoint const& pos) const
{
   const int S = 4096;
   int cw = size().width()*S / m_Cols;
   int ch = size().height()*S / m_Rows;
   Point cell;
   cell.x = (S * pos.x()) / cw;
   cell.y = (S * pos.y()) / ch;
   return cell;
}

void RangesWidget::delSelected() {
    if (m_CurrRange.Empty()) {
        return;
    }

    int n = m_CurrRange.W() * m_CurrRange.H();
    std::vector<bool> existData(n);     // all false
    std::vector<PenColour> penData(n);  // default pens

    // don't erase any slots which are shared by another range
    RangeGrid& ranges = m_Ed.Proj().Ranges(m_Focus, m_Frame);
    int i=0;
    for (int cy = m_CurrRange.YMin(); cy<=m_CurrRange.YMax(); ++cy) {
        for (int cx = m_CurrRange.XMin(); cx<=m_CurrRange.XMax(); ++cx) {
            Point cell(cx,cy);
            if (ranges.IsShared(cell)) {
                existData[i] = ranges.Get(cell, penData[i]);
            }
            ++i;
        }
    }

    Cmd* cmd = new Cmd_RangeEdit(m_Ed.Proj(), m_Focus, m_Frame, m_CurrRange, existData, penData);
    m_Ed.AddCmd(cmd);
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
    const char* penMimeType = "application/x-evilpixie-pen";
    const QMimeData* md = event->mimeData();
    if (md->hasFormat(penMimeType)) {
        QByteArray buf = md->data(penMimeType);
        if (buf.size()>=5) {
            Colour c;
            c.r = (uint8_t)buf[0];
            c.g = (uint8_t)buf[1];
            c.b = (uint8_t)buf[2];
            c.a = (uint8_t)buf[3];
            int idx = (int)buf[4];
            // TODO: no-index encoding!

            RangeGrid& ranges = m_Ed.Proj().Ranges(m_Focus, m_Frame);
            PenColour pen(c, idx);
            Point cell = PickCell(event->pos());
            if (ranges.Bound().Contains(cell)) {
                std::vector<bool> existData = {true};
                std::vector<PenColour> penData = {pen};
                Cmd* cmd = new Cmd_RangeEdit(m_Ed.Proj(), m_Focus, m_Frame,
                        Box(cell, 1, 1), existData, penData);
                m_Ed.AddCmd(cmd);
            }
        }
    }
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

    // find range(s) and pen under the click.
    Point cell = PickCell(event->pos());
    RangeGrid& ranges = m_Ed.Proj().Ranges(m_Focus, m_Frame);
    PenColour pen;
    if (ranges.Get(cell, pen)) {
        if (event->button() == Qt::LeftButton) {
            emit pickedFGPen(pen);
        } else if (event->button() == Qt::RightButton) {
            emit pickedBGPen(pen);
        }
    }

    // update range selection
    if (event->button() == Qt::LeftButton) {
        // prefer to keep existing range
        if (!m_CurrRange.Contains(cell)) {
            m_CurrRange = ranges.PickRange(cell); 
            emit pickedRange();
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

    RangeGrid& ranges = m_Ed.Proj().Ranges(m_Focus, m_Frame);
    Box bbox = ranges.Bound();
    Point pos;
    for (pos.y = bbox.YMin(); pos.y <= bbox.YMax(); ++pos.y) {
        for (pos.x = bbox.XMin(); pos.x <= bbox.XMax(); ++pos.x) {
            PenColour pen;
            if (ranges.Get(pos, pen)) {
                QRect cellrect = CalcRect(Box(pos.x, pos.y, 1, 1));
                bool isFG = (m_FGPen == pen);
                bool isBG = (m_BGPen == pen);
                RenderCell(painter, cellrect, pen.rgb(), isFG, isBG);
            }
        }
    }


    if (!m_CurrRange.Empty()) {
        QRect r = CalcRect(m_CurrRange);

        QPen blackpen( Qt::SolidLine );
        blackpen.setColor( QColor(0, 0, 0, 255) );

        QPen whitepen( Qt::SolidLine );
        whitepen.setColor( QColor(255, 255, 255, 255) );

        painter.setBrush(Qt::NoBrush);
        painter.setPen(blackpen);
        painter.drawRect(r);

        r.adjust(1, 1, -1, -1);
        painter.setPen(whitepen);
        painter.drawRect(r);
    }
}


// Map a box in cell coords to the drawing area.
QRect RangesWidget::CalcRect(Box const& box) const
{
    // cell size is not uniform - use noddy fixedpoint to calculate it
    const int S = 4096;
    int cw = (size().width() * S) / m_Cols;
    int ch = (size().height() * S) / m_Rows;

    int x = (box.x * cw) / S;
    int y = (box.y * ch) / S;

    int w = (box.w * cw) / S;
    int h = (box.h * ch) / S;

    return QRect(x, y, w, h);
}


// ---------------------------
// ProjectListener

void RangesWidget::OnRangesBlatted(NodePath const& target, int frame)
{
    if (!m_Ed.Proj().SharesPalette(target, frame, m_Focus, m_Frame)) {
        return;
    }

    m_CurrRange.SetEmpty();
    emit pickedRange();

    // just redraw everything.
    update();
}

