#ifndef GUISTUFF_H
#define GUISTUFF_H

class QPixmap;
class QRect;
class QPainter;

class Colour;

// any globally-required stuff
struct GUIStuff {
    // checkerboard pattern for widget backgrounds
    QPixmap *checkerboard;
};

extern GUIStuff g_GUIStuff;

extern void initGUIStuff();
extern void freeGUIStuff();

// draw a cell in palette or ranges widget.
void RenderCell(QPainter& painter, QRect const& r, Colour const& c, bool fg, bool bg);

#endif // GUISTUFF_H

