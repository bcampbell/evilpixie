#ifndef APP_H
#define APP_H

#include <cassert>

class App;
class Brush;

extern App* g_App;   // set in App ctor

enum { NUM_STD_BRUSHES = 4 };


// gui-neutral parts of main app framework
class App
{
public:
    App();
    virtual ~App();

    virtual int Run( int argc, char* argv[] ) = 0;

    Brush* CustomBrush()    { return m_CustomBrush; }
    Brush* StdBrush(int n ) { return m_StdBrushes[n]; }

    void SetCustomBrush( Brush* b );


private:
    Brush* m_CustomBrush;
    Brush* m_StdBrushes[NUM_STD_BRUSHES];
};


#endif  // APP_H

