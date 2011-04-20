#ifndef PALETTE_H
#define PALETTE_H

#include "colours.h"


// placeholder palette class... to be fleshed out

class Palette
{
public:
    Palette();

    // TODO: Kill these...
    Palette( RGBx const* src );
    RGBx* raw() { return m_Colours; }   // should be private
    RGBx const* rawconst() const { return m_Colours; }

    RGBx const& GetColour( int n ) const { return m_Colours[n]; }
    void SetColour( int n, RGBx const& c ) { m_Colours[n]=c; }

    void LerpRange( int n0, RGBx const& c0, int n1, RGBx const& c1 );

    static Palette* Load(const char* filename);

private:
    int m_NumColours;
    RGBx m_Colours[256];
};

inline Palette::Palette() :
    m_NumColours(256)
{
}

inline Palette::Palette( RGBx const* src )
{
    int i;
    for( i=0; i<=255; ++i )
        m_Colours[i] = src[i];
}

#endif // PALETTE_H

