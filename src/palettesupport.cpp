
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

#include <cstdio>
#include <cstdlib>
#include "colours.h"
#include "util.h"
#include "wobbly.h"
#include <string>
#include <vector>

static void LoadGimpPalette( RGBx* palette, FILE* fp );

void LoadPalette( RGBx* palette, const char* filename )
{
    FILE* fp = fopen( filename, "rb" );
    if( !fp )
        throw Wobbly( "fopen failed" );

    try
    {
        LoadGimpPalette( palette, fp );
    }
    catch( Wobbly const& e )
    {

        fclose(fp);
        throw;
    }

    fclose(fp);
}

static void LoadGimpPalette( RGBx* palette, FILE* fp )
{
    RGBx scratch[256];
    int idx=0;

    char line[256];
    int linenum=0;
    bool gotcookie=false;
    while( fgets( line, sizeof(line), fp ) )
    {
        ++linenum;
        std::vector< std::string > args;
        SplitLine( line, args );
        if( args.empty() )
            continue;

        if( !gotcookie )
        {
            if( args.size()==2 && args[0] == "GIMP" && args[1] == "Palette" )
                gotcookie = true;
            else
                throw Wobbly( "not a GIMP palette" );
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
            RGBx& c = scratch[idx];
            c.r = atoi( args[0].c_str() );
            c.g = atoi( args[1].c_str() );
            c.b = atoi( args[2].c_str() );
            // ignore name, if there is one
            //
            ++idx;
        }
    }

    if( !feof(fp ) )
    {
        throw Wobbly( "read error" );
    }

    while( idx<=255 )
        scratch[idx++] = RGBx(0,0,0);
    for( idx=0; idx<=255; ++idx )
        palette[idx] = scratch[idx];
}

