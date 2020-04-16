#include "guistuff.h"

#include "../colours.h"

#include <QPainter>
#include <QPixmap>


GUIStuff g_GUIStuff = {0};

void initGUIStuff()
{
    const int n=8;  // use something less even to avoid lining up? (7 perhaps?)
    g_GUIStuff.checkerboard = new QPixmap(n*2,n*2);
    {
        QPainter p(g_GUIStuff.checkerboard);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(128,128,128));
        p.drawRect(0,0,n,n);
        p.drawRect(n,n,n,n);
        p.setBrush(QColor(224,224,224));
        p.drawRect(n,0,n,n);
        p.drawRect(0,n,n,n);
    }
}

void freeGUIStuff()
{
    delete g_GUIStuff.checkerboard;
}


void RenderCell(QPainter& painter, QRect const& r, Colour const& c, bool fg, bool bg)
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(c.r, c.g, c.b, c.a));
    painter.drawRect(r);
    const int k = 8;
    if (fg) {
        QPoint q0 = r.topLeft();
        QPoint q1 = q0 + QPoint(k, 0);
        QPoint q2 = q0 + QPoint(0, k);

        painter.setBrush(QColor(255,255,255,255));
        painter.setPen(QColor(0,0,0,255));
        painter.drawConvexPolygon(QPolygon({q0,q1,q2}));
    }
    if (bg) {
        QPoint q0 = r.bottomRight();
        QPoint q1 = q0 + QPoint(-k, 0);
        QPoint q2 = q0 + QPoint(0, -k);

        painter.setBrush(QColor(0,0,0,255));
        painter.setPen(QColor(255,255,255,255));
        painter.drawConvexPolygon(QPolygon({q0,q1,q2}));
    }
}

