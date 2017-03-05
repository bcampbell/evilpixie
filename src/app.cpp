#include "app.h"
//#include "editor.h"
//#include "tool.h"
#include "brush.h"
//#include "project.h"

#include <cassert>
#include <stdint.h>
#include <cstdio>

App* g_App = 0;   // set in App ctor

static uint8_t brush1x1[1*1] = { 1 };

static uint8_t brush3x3[3*3] =
{
    0,1,0,
    1,1,1,
    0,1,0,
};

static uint8_t brush5x5[5*5] =
{
    0,1,1,1,0,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    0,1,1,1,0,
};

static uint8_t brush7x7[7*7] =
{
    0,0,1,1,1,0,0,
    0,1,1,1,1,1,0,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    0,1,1,1,1,1,0,
    0,0,1,1,1,0,0,
};


App::App() :
    m_CustomBrush(0),
    m_DataPath("./data")
{
    PenColour t(Colour(0,0,0),0);
    assert( g_App == 0 );
    m_StdBrushes[0] = new Brush( MASK, 1,1,brush1x1,t );
    m_StdBrushes[1] = new Brush( MASK, 3,3,brush3x3,t );
    m_StdBrushes[2] = new Brush( MASK, 5,5,brush5x5,t );
    m_StdBrushes[3] = new Brush( MASK, 7,7,brush7x7,t );

    g_App = this;
}

App::~App()
{
    int i;
    for( i=0; i<NUM_STD_BRUSHES; ++i )
        delete m_StdBrushes[i];

    delete m_CustomBrush;
    g_App = 0;
}


void App::SetCustomBrush( Brush* b )
{
    delete m_CustomBrush;
    m_CustomBrush = b;
}


