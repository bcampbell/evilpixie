#include "palette.h"

#include <cstdio>
#include <cstdlib>
#include "colours.h"
#include "util.h"
#include "exception.h"
#include <string>
#include <vector>


static void LoadGimpPalette(FILE* fp, Palette& pal);


Palette::Palette(int numcolours) :
    NColours(numcolours)
{
    Colours = new Colour[numcolours];
    int i;
    Colour black(0,0,0);
    for( i=0;i<numcolours;++i)
        SetColour(i,black);
}

Palette::Palette(Palette const& src) :
    NColours(src.NColours)
{
    Colours = new Colour[src.NColours];
    int i;
    for (i=0; i<NColours; ++i)
    {
        Colours[i] = src.Colours[i];
    }
}

Palette::~Palette()
{
    delete [] Colours;
}


Palette& Palette::operator=( const Palette& other ) {
    delete [] Colours;
    NColours = other.NColours;
    Colours = new Colour[NColours];
    int i;
    for (i=0; i<NColours; ++i)
        Colours[i] = other.Colours[i];
    return *this;
}

// ugh.
void Palette::SetNumColours(int ncolours)
{
    Colour* tmp = new Colour[ncolours];
    int i;
    for(i=0;i<ncolours;++i)
    {
        if (i<NColours)
            tmp[i] = Colours[i];
        else
            tmp[i] = Colour(0,0,0,255);
    }
    delete [] Colours;
    Colours = tmp;
    NColours = ncolours;
}

// TODO: BUG: not quite right off-by-one error...
//
void Palette::LerpRange( int n0, Colour const& c0, int n1, Colour const& c1 )
{
    if( n0==n1 )
        return;

    const int S=65536;
    int n;
    for( n=n0; n<=n1; ++n )
    {

        int t = ((n-n0)*S) / (n1-n0);
        Colour& c = Colours[n];
        c.r = c0.r + ((c1.r-c0.r)*t)/S;
        c.g = c0.g + ((c1.g-c0.g)*t)/S;
        c.b = c0.b + ((c1.b-c0.b)*t)/S;
        c.a = c0.a + ((c1.a-c0.a)*t)/S;
    }
}


// static
Palette* Palette::Load( const char* filename )
{
    Palette* palette = new Palette();
    FILE* fp = fopen( filename, "rb" );
    if( !fp )
        throw Exception( "fopen failed" );
    try
    {
        LoadGimpPalette(fp,*palette);
    }
    catch( Exception const& e )
    {

        fclose(fp);
        delete palette;
        throw;
    }

    fclose(fp);
    return palette;
}




static void LoadGimpPalette(FILE* fp, Palette& pal)
{
    int idx=0;

    char line[256];
    int linenum=0;
    bool gotcookie=false;
    while( fgets( line, sizeof(line), fp ) )
    {
        ++linenum;
        std::vector< std::string > args;
        SplitLine(line, args);
        if( args.empty() )
            continue;

        if( !gotcookie )
        {
            if(args.size()==2 && args[0] == "GIMP" && args[1] == "Palette")
                gotcookie = true;
            else
                throw Exception("not a GIMP palette");
        }

        if( args[0] == "Name:" )
        {
            continue;
        }
        if( args[0] == "Column:" )
        {
            continue;
        }


        if( args.size() >= 3 && idx <=255 )
        {
            Colour c;
            c.r = atoi( args[0].c_str() );
            c.g = atoi( args[1].c_str() );
            c.b = atoi( args[2].c_str() );
            // ignore name, if there is one

            pal.SetColour(idx,c);
            ++idx;
        }
    }

    if( !feof(fp ) )
    {
        throw Exception( "read error" );
    }

    pal.SetNumColours(idx);
}


// notes for future:
/*.pal file extension

.pal
    1. Colour palette file. A generic file extension used to denote a colour palette. A colour palette defines the colour depth (i.e. number of bits per pixel) and the red-greed-blue colour definition for each colour in the palette.
    2. Paint Shop Pro colour palette file. Contains the colour palette table saved from a 16 or 256 colour image. It can be loaded for another image and this provides a means of copying colour palettes from one image to another.

    These files are text editable. The format of the file is as follows:

       1. The first line contains the header "JASC-PAL" (without the quotes).
       2. The second line contains the palette file version. For example "0100" (without the quotes)
       3. The third line contains the number of colours, either 16 or 256.
       4. Subsequent lines contain the Red, Green and Blue components of each palette entry, with a space between each of the Red, Green and Blue numbers. These numbers are in decimal, so each attribute should be in the range 0 to 255.
*/

