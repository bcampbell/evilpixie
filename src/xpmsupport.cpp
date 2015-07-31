#include <cstdio>
#ifdef WIN32
#define FOR_MSW
#endif
#include <X11/xpm.h>
#include "img.h"

// NOTE: Not actually included in build right now, but left in for easy reference

bool LoadXPM( IndexedImg& img, Colour* palette, const char* filename )
{
    XpmImage image;
    XpmInfo info;
    int r = XpmReadFileToXpmImage( (char*)filename, &image, &info );

    if( r != 0 )
    {
        printf("XpmReadFileToXpmImage() failed (code %d)\n",r);
        return false;
    }


    printf("xpm is %dx%d, %d colours, %d cpp\n",
        image.width,
        image.height,
        image.ncolors,
        image.cpp );


    // install palette
    int i;
    for( i=0; i<image.ncolors; ++i )
    {
        XpmColor const& c = image.colorTable[i];
        printf("%d: '%s'\n", i, c.c_color );
//        palette[i] = Colour( c.Red, c.Green, c.Blue );
    }

    IndexedImg tmp( image.width, image.height );

    const unsigned int* src = image.data;
    int y;
    for( y=0; y<image.height; ++y )
    {
        int x;
        uint8_t* dest = tmp.Ptr(0,y);
        for( x=0; x<image.width; ++x )
        {
            *dest++ = (uint8_t)*src++;
        }
    }

    img.Copy( tmp );

/*
    GifImageDesc* desc = si[0]

    printf("ltwh: %d,%d,%d,%d\n",
        desc->Left,
        desc->Top,
        desc->Width,
        desc->Height ); 
*/

    XpmFreeXpmImage( &image );

    return true;
}



void BuildHistogram( IndexedImg const& img, int* hist )
{
    int i;
    for( i=0; i<=255; ++i )
        hist[i]=0;
    int y;
    for( y=0; y<img.H(); ++y )
    {
        int x;
        const uint8_t* src = img.PtrConst( 0,y );
        for( x=0; x<img.W(); ++x )
            hist[*src++]++;
    }
}

int CountUsedColours( const int* hist )
{
    int cnt=0;
    int i;
    for( i=0; i<=255; ++i )
    {
        if( hist[i]>0 )
            ++cnt;
    }
    return cnt;
}

#if 0
void SaveXPM( IndexedImg& img, Colour* palette, char* filename )
{
    // use same chars as gimp
    static const char linenoise [] =
        " .+@#$%&*=-;>,')!~{]^/(_:<[}|1234567890abcdefghijklmnopqrstuvwxyz\
        ABCDEFGHIJKLMNOPQRSTUVWXYZ`";

    XpmImage outimage;
    XpmInfo outinfo = { 0,NULL,NULL,NULL,0,0,0,NULL };

    int hist[256];
    BuildHistogram( img, hist );
    int usedcolors = CountUsedColours( hist );


    outimage.width = img.W();
    outimage.height = img.H();
    outimage.cpp = cpp;
    outimage.ncolors = usedcolours;
    outimage.colorTable = 0;
    outimage.data = stringbuf;
}


#endif
