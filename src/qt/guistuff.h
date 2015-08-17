#ifndef GUISTUFF_H
#define GUISTUFF_H

class QPixmap;

// any globally-required stuff
struct GUIStuff {
    // checkerboard pattern for widget backgrounds
    QPixmap *checkerboard;
};

extern GUIStuff g_GUIStuff;

extern void initGUIStuff();
extern void freeGUIStuff();

#endif // GUISTUFF_H

