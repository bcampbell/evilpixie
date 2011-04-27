#include <IL/il.h>
//#include <IL/ilu.h>
#include <cassert>

#include "img.h"
#include "wobbly.h"
#include "util.h"

//extern bool LoadGIF( IndexedImg& img, RGBx* palette, const char* filename );
//extern bool LoadPNG( IndexedImg& img, RGBx* palette, const char* filename );
//extern bool LoadXPM( IndexedImg& img, RGBx* palette, const char* filename );
extern void SaveGIF( IndexedImg const& img, RGBx const* palette, const char* filename );
//extern void SavePNG( IndexedImg const& img, RGBx const* palette, const char* filename );


void LoadImg( IndexedImg& img, RGBx* palette, const char* filename, int* transparent_idx )
{

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

    if(transparent_idx)
    {
        *transparent_idx = -1;
        if(ext==".png") // ugh
        {
            // TODO: does IL_PNG_ALPHA even work for loading?
            *transparent_idx = (int)ilGetInteger(IL_PNG_ALPHA_INDEX);
        }
    }
    printf("IL_NUM_IMAGES: %d\n",ilGetInteger(IL_NUM_IMAGES));
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
//   int palette_type = (int)ilGetInteger( IL_PALETTE_TYPE );
//    printf("palette: %d colours, bpp=%d, type=0x%x\n", num_cols, pal_bytesperpixel, palette_type );

    uint8_t const* p = ilGetPalette();
    assert( p );
    int i=0;
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
    IndexedImg tmp( w, h, pixels );
    img.Copy( tmp );



    ilDeleteImages(1,&im);
}


void SaveImg( IndexedImg const& img, RGBx const* palette, const char* filename, int transparent_idx )
{
    std::string ext = ToLower( ExtName(filename) );
    if( ext==".gif" )
    {
        // special case for gif - DevIL can't save it
        SaveGIF( img, palette, filename );
        return;
    }

    ilSetInteger(IL_PNG_ALPHA_INDEX,transparent_idx);

    ILuint im;
    ilGenImages( 1, &im );
    ilBindImage( im );

    if( !ilTexImage( img.W(), img.H(), 1, 1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, NULL ) )
    {
        ilDeleteImages(1,&im);
        throw Wobbly( "ilTexImage() failed)" );
    }

    /* copy in image, flipped (ilTexImage sets ORIGIN_LOWER_LEFT) */
    int y;
    for( y=0; y<img.H(); ++y )
    {
        const uint8_t* src = img.PtrConst( 0, (img.H()-1)-y );
        ilSetPixels( 0,y,0, img.W(),1,1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, (void*)src );
    }



    uint8_t tmp_palette[3*256];
    int i;
    uint8_t* p = tmp_palette;
    for( i=0; i<=255; ++i )
    {
        *p++ = palette[i].r;
        *p++ = palette[i].g;
        *p++ = palette[i].b;
    }
    ilRegisterPal( tmp_palette, 3*256, IL_PAL_RGB24 );

    ilEnable(IL_FILE_OVERWRITE);
    if( !ilSaveImage( filename ) )
    {
        ilDeleteImages(1,&im);
        ILenum err = ilGetError();

        if( err == IL_INVALID_EXTENSION )
            throw Wobbly( "Invalid filename extension" );
        if( err == IL_FORMAT_NOT_SUPPORTED )
            throw Wobbly( "File format not supported" );
        else
        {
            throw Wobbly( "ilSaveImage() failed (err 0x%x)", err );
        }
    }
    ilDeleteImages(1,&im);
}






