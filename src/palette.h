#ifndef PALETTE_H
#define PALETTE_H

#include "colours.h"


class Palette
{
public:
    Palette(int numcolours=256);

    int NumColours() const {return m_NumColours;}
    RGBx const& GetColour( int n ) const { return m_Colours[n]; }
    void SetColour( int n, RGBx const& c ) { m_Colours[n]=c; }

    void LerpRange( int n0, RGBx const& c0, int n1, RGBx const& c1 );

    static Palette* Load(const char* filename);

private:
    int m_NumColours;
    RGBx m_Colours[256];
};

inline Palette::Palette(int numcolours) :
    m_NumColours(numcolours)
{
}


#endif // PALETTE_H

