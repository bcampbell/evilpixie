#include "guistuff.h"

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


