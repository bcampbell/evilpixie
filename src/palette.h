#ifndef PALETTE_H
#define PALETTE_H

#include "colours.h"
#include <cassert>
#include <string>

struct Palette
{
public:
    int NColours;
    Colour* Colours;

    Palette(int numcolours=256);
    Palette(Palette const& src);
    ~Palette();
    Palette& operator=( const Palette& other );

    int NumColours() const {return NColours;}

    // Return the colour at index n, or black if beyond end of palette.
    Colour GetColour(int n) const {
        assert(n >= 0);
        return (n<NColours) ? Colours[n] : Colour(0, 0, 0);
    }
    void SetColour( int n, Colour const& c ) {
        assert(n >= 0 && n < NColours);
        Colours[n] = c;
    }

    int Closest(const Colour targ) const;
    void SpreadHSV( int n0, Colour const& c0, int n1, Colour const& c1 );

    void Save(std::string const& filename) const;
    static Palette* Load(const char* filename);

    void SetNumColours(int ncolours);

};


#endif // PALETTE_H

