#ifndef APP_H
#define APP_H

#include <cassert>
#include <string>

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

    // path to find static files needed by the app - icons, default palette, helpfile etc etc
    // depends upon platform.
    //  osx: "bundle/Content/Resources"
    //  windows: .
    //  unix: $PREFIX/share/evilpixie
    std::string const& DataPath() const { return m_DataPath; }

private:
    Brush* m_CustomBrush;
    Brush* m_StdBrushes[NUM_STD_BRUSHES];

    std::string m_DataPath;

    void SetupPaths();
};


#endif  // APP_H

