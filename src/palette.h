#ifndef PALETTE_H
#define PALETTE_H

#include "colours.h"


struct Palette
{
public:
    int NColours;
    Colour* Colours;

    Palette(int numcolours=256);

    int NumColours() const {return NColours;}
    Colour const& GetColour( int n ) const { return Colours[n]; }
    void SetColour( int n, Colour const& c ) { Colours[n]=c; }

    void LerpRange( int n0, Colour const& c0, int n1, Colour const& c1 );

    static Palette* Load(const char* filename);

    void SetNumColours(int ncolours) {NColours=ncolours;}

};


#endif // PALETTE_H

