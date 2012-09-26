#include <cstdio>
#include <gif_lib.h>
#include <assert.h>

#include "img.h"
#include "wobbly.h"

// NOTE: Not actually included in build right now, but left in for easy reference


void LoadGIF( Img& img, RGBx* palette, const char* filename )
{
    GifFileType* f = DGifOpenFileName( filename );
    if( !f )
        throw Wobbly( "couldn't open '%s' (code %d)", filename, GifLastError() );

    if( DGifSlurp( f ) != GIF_OK )
    {
        throw Wobbly( "couldn't load '%s' (code %d)", filename, GifLastError() );
    }

//    printf("SWidth: %d\n", f->SWidth );
//    printf("SHeight: %d\n", f->SHeight );
//    printf("SColorResolution: %d\n", f->SColorResolution );
//    printf("ImageCount: %d\n", f->ImageCount );

//    printf("ColorMap->ColorCount: %d\n", f->SColorMap->ColorCount );
//    printf("ColorMap->BitsPerPixel: %d\n", f->SColorMap->BitsPerPixel );

    SavedImage* si = &f->SavedImages[0];
    ColorMapObject* cm = si->ImageDesc.ColorMap;
    if( !cm )
        cm = f->SColorMap;

    // install palette
    int i;
    for( i=0; i<cm->ColorCount; ++i )
    {
        GifColorType const& c = cm->Colors[i];
        palette[i] = RGBx( c.Red, c.Green, c.Blue );
    }

    Img tmp( Img::INDEXED8BIT, si->ImageDesc.Width, si->ImageDesc.Height, si->RasterBits );

    img.Copy( tmp );
}


void SaveGIF( Img const& img, RGBx const* palette, const char* filename )
{
    int status;
    assert(img.Format()==Img::IndexedImg);
    GifFileType* gif = EGifOpenFileName(filename, 0 );
    if( gif == NULL )
    {
        throw Wobbly( "EGifOpenFileName failed (%d)", GifLastError() );
    }

    ColorMapObject* colormap = MakeMapObject( 256, NULL );
    if( !colormap )
        throw Wobbly( "MakeMapObject failed" );
    int i;
    assert( colormap->ColorCount == 256 );
    for( i=0; i<=255; ++i )
    {
       colormap->Colors[i].Red = palette[i].r; 
       colormap->Colors[i].Green = palette[i].g; 
       colormap->Colors[i].Blue = palette[i].b; 
    }
    
    status = EGifPutScreenDesc( gif,
        img.W(), img.H(),
        7,
        0,
        colormap );
    if( status != GIF_OK )
    {
        FreeMapObject( colormap );
        throw Wobbly( "EGifPutScreenDesc failed (%d)", GifLastError() );
    }

    status = EGifPutImageDesc( gif,
        0,0, img.W(), img.H(),
        0,
        NULL );
    if( status != GIF_OK )
    {
        FreeMapObject( colormap );
        throw Wobbly( "EGifPutImageDesc failed (%d)", GifLastError() );
    }


    int y;
    for( y=0; y<img.H(); ++y )
    {
        GifPixelType* src = (GifPixelType*)img.PtrConst( 0,y );
        status = EGifPutLine(gif, src, img.W() );
        if( status != GIF_OK )
        {
            FreeMapObject( colormap );
            throw Wobbly( "EGifPutLine failed (%d)", GifLastError() );
        }
    }

    EGifCloseFile( gif );
    FreeMapObject( colormap );
}

#if 0
const char* GifErrTxt( int err )
{

    switch( err )
    {
    case E_GIF_ERR_OPEN_FAILED:     // 1
    case E_GIF_ERR_WRITE_FAILED:    // 2
    case E_GIF_ERR_HAS_SCRN_DSCR:   // 3
    case E_GIF_ERR_HAS_IMAG_DSCR:   // 4
    case E_GIF_ERR_NO_COLOR_MAP:    // 5
    case E_GIF_ERR_DATA_TOO_BIG:    // 6
    case E_GIF_ERR_NOT_ENOUGH_MEM:  // 7
    case E_GIF_ERR_DISK_IS_FULL:    // 8
    case E_GIF_ERR_CLOSE_FAILED:    // 9
    case E_GIF_ERR_NOT_WRITEABLE:   // 10
    case D_GIF_ERR_OPEN_FAILED:     // 101
    case D_GIF_ERR_READ_FAILED:     // 102
    case D_GIF_ERR_NOT_GIF_FILE:    // 103
    case D_GIF_ERR_NO_SCRN_DSCR:    // 104
    case D_GIF_ERR_NO_IMAG_DSCR:    // 105
    case D_GIF_ERR_NO_COLOR_MAP:    // 106
    case D_GIF_ERR_WRONG_RECORD:    // 107
    case D_GIF_ERR_DATA_TOO_BIG:    // 108
    case D_GIF_ERR_NOT_ENOUGH_MEM:  // 109
    case D_GIF_ERR_CLOSE_FAILED:    // 110
    case D_GIF_ERR_NOT_READABLE:    // 111
    case D_GIF_ERR_IMAGE_DEFECT:    // 112
    case D_GIF_ERR_EOF_TOO_SOON:    // 113
    }
}
#endif

