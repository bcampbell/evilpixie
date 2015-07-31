#ifndef PALETTE_H
#define PALETTE_H

#include "colours.h"


class Palette
{
public:
    Palette(int numcolours=256);

    int NumColours() const {return m_NumColours;}
    Colour const& GetColour( int n ) const { return m_Colours[n]; }
    void SetColour( int n, Colour const& c ) { m_Colours[n]=c; }

    void LerpRange( int n0, Colour const& c0, int n1, Colour const& c1 );

    static Palette* Load(const char* filename);

    void SetNumColours(int ncolours) {m_NumColours=ncolours;}
private:
    int m_NumColours;
    Colour m_Colours[256];
};


#endif // PALETTE_H

