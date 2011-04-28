#include <IL/il.h>
#include <cassert>

#include "anim.h"
#include "img.h"
#include "palette.h"
#include "wobbly.h"
#include "util.h"


Anim::Anim()
{
}

Anim::~Anim()
{
    Zap();
}

void Anim::Zap()
{
    while( !m_Frames.empty() )
    {
        delete m_Frames.back();
        m_Frames.pop_back();
        delete m_Palettes.back();
        m_Palettes.pop_back();
    }
}


void Anim::TransferFrames(int srcfirst, int srclast, Anim& dest, int destfirst)
{
    dest.m_Frames.insert( dest.m_Frames.begin()+destfirst, m_Frames.begin()+srcfirst, m_Frames.begin()+srclast );
    m_Frames.erase(m_Frames.begin()+srcfirst, m_Frames.begin()+srclast);

    dest.m_Palettes.insert( dest.m_Palettes.begin()+destfirst, m_Palettes.begin()+srcfirst, m_Palettes.begin()+srclast );
    m_Palettes.erase(m_Palettes.begin()+srcfirst, m_Palettes.begin()+srclast);
}



void Anim::Load( const char* filename )
{
    int transparent_idx = -1;

    ilDisable( IL_CONV_PAL );

    ILuint im;
    ilGenImages( 1, &im );
    ilBindImage( im );
    std::string ext = ToLower( ExtName(filename) );
    // cheesy hack to bias il in favour of amiga-style iffs
    if( ext==".iff" )
    {
        ilLoad(IL_ILBM, (char*)filename);
    }
    else
    {
        ilLoadImage((char*)filename);
    }

    ILenum err = ilGetError();
    if( err != IL_NO_ERROR )
    {
        ilDeleteImages(1,&im);
        throw Wobbly( "ILerror: 0x%x", err );
    }

    int num_frames = ilGetInteger(IL_NUM_IMAGES);
    if(num_frames == 0)
        num_frames=1;
    int frame;
    for(frame=0; frame<num_frames; ++frame)
    {
        if(ilActiveImage(frame)==IL_FALSE)
        {
            ilDeleteImages(1,&im);
            throw Wobbly("ilActiveImage() failed (err 0x%x)",ilGetError());
        }

        int w = (int)ilGetInteger( IL_IMAGE_WIDTH );
        int h = (int)ilGetInteger( IL_IMAGE_HEIGHT );
        ILint fmt = ilGetInteger( IL_IMAGE_FORMAT );
//    ILint type = ilGetInteger( IL_IMAGE_TYPE );
//    ILint bytesperpixel = ilGetInteger( IL_IMAGE_BYTES_PER_PIXEL );
//    ILint bitsperpixel = ilGetInteger( IL_IMAGE_BITS_PER_PIXEL );
//    printf("%dx%d fmt=%x type=%x %x %x\n", w,h,fmt,type, bytesperpixel, bitsperpixel );

        if( fmt != IL_COLOUR_INDEX )
        {
            ilDeleteImages(1,&im);
            throw Wobbly( "No palette - not an indexed image" );
        }

        transparent_idx = -1;
        if(ext==".png") // ugh
        {
            // TODO: does IL_PNG_ALPHA even work for loading?
            transparent_idx = (int)ilGetInteger(IL_PNG_ALPHA_INDEX);
        }
// ILuint ili;
// for(i=0, ili=0; i<TotalAnimationFrames; i++, ili++)
// {
//   ilActiveImage(0);
//   if(ilActiveImage(ili)==IL_FALSE)
//      {error;}
//   pAnimationDuration[i]=(sreal)ilGetInteger(IL_IMAGE_DURATION);
//   pColorData[i*TotalColorDataWithPadding+j]=ilGetData();
// }


        /* make sure everything is in the format we expect! */
        ilConvertImage( IL_COLOUR_INDEX, IL_UNSIGNED_BYTE );
        ilConvertPal( IL_PAL_RGB24 );

        int pal_bytesperpixel = (int)ilGetInteger( IL_PALETTE_BPP );
        if( pal_bytesperpixel != 3 )
        {
            ilDeleteImages(1,&im);
            throw Wobbly( "Unsupported palette type (%d bytes/pixel)", pal_bytesperpixel );
        }

        int num_cols = (int)ilGetInteger( IL_PALETTE_NUM_COLS );
   int palette_type = (int)ilGetInteger( IL_PALETTE_TYPE );
    printf("palette: %d colours, bpp=%d, type=0x%x\n", num_cols, pal_bytesperpixel, palette_type );

        uint8_t const* p = ilGetPalette();
        assert( p );
        int i=0;
        RGBx palette[256];
        while( i<num_cols && i<=255 )
        {
            palette[i].r = *p++;
            palette[i].g = *p++;
            palette[i].b = *p++;
            ++i;
        }
        while( i<=255 )
        {
            palette[i++] = RGBx(0,0,0);
        }
 
        const uint8_t* pixels = ilGetData();
        IndexedImg* img = new IndexedImg(w,h,pixels);
        m_Frames.push_back(img);
        m_Palettes.push_back( new Palette(palette) );
    }

    ilDeleteImages(1,&im);
}

