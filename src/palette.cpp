#include "palette.h"


// TODO: BUG: not quite right off-by-one error...
//
void Palette::LerpRange( int n0, RGBx const& c0, int n1, RGBx const& c1 )
{
    if( n0==n1 )
        return;

    const int S=65536;
    int n;
    for( n=n0; n<=n1; ++n )
    {

        int t = ((n-n0)*S) / (n1-n0);
        m_Colours[n] = RGBx(
            c0.r + ((c1.r-c0.r)*t)/S,
            c0.g + ((c1.g-c0.g)*t)/S,
            c0.b + ((c1.b-c0.b)*t)/S );
    }
}

