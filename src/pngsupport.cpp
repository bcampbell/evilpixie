#include <cstdio>
#include <png.h>
#include <assert.h>
#include <cerrno>
#include <cstring>

#include "img.h"
#include "wobbly.h"

// TODO: implement custom error handler to capture libpng error message

void LoadPNG( Img& img, Colour* palette, const char* filename )
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        throw Wobbly( "open failed: %s", strerror(errno) );

    unsigned char header[8];
    fread( header, 1, 8, fp );
    bool is_png = !png_sig_cmp( header, 0, 8 );
    if( !is_png )
    {
        fclose(fp);
        throw Wobbly( "Not a PNG file." );
    }

    png_structp png_ptr = png_create_read_struct
        (PNG_LIBPNG_VER_STRING,
        (png_voidp)NULL,
        NULL, NULL );
    if( !png_ptr )
    {
        fclose(fp);
        throw Wobbly( "png_create_read_struct() failed" );
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr,
           (png_infopp)NULL, (png_infopp)NULL);
        throw Wobbly( "png_create_info_struct() failed" );
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr,
          (png_infopp)NULL);
        throw Wobbly( "png_create_info_struct() failed" );
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
           &end_info);
        fclose(fp);
        throw Wobbly( "error reading PNG" );
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 width;
    png_uint_32 height;
    int color_type;
    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
       &bit_depth, &color_type, NULL,
       NULL, NULL );

    if( color_type != PNG_COLOR_TYPE_PALETTE   )
    {
        throw Wobbly( "PNG does not have a palette" );
    }

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (bit_depth < 8)
        png_set_packing(png_ptr);

    // read in the image data
    Img tmp( Img::INDEXED8BIT, width, height );
    png_bytep row_pointers[ tmp.H() ];
    int y;
    for( y=0;y<tmp.H();++y)
        row_pointers[y] = (png_bytep)tmp.Ptr(0,y);

    png_read_image( png_ptr, row_pointers );

    img.Copy(tmp);

    // now read in the palette
    png_colorp colours;
    int num_colours;
    png_get_PLTE(png_ptr, info_ptr, &colours,
                     &num_colours);
    assert( num_colours <= 256 );
    int i=0;
    while( i<num_colours )
    {
        png_color c = colours[i];
        palette[i] = Colour( c.red, c.green, c.blue );
        ++i;
    }
    // black out any missing colours
    while( i<=255 )
    {
        palette[i] = Colour(0,0,0);
        ++i;
    }

    png_read_end(png_ptr, end_info);

    png_destroy_read_struct(&png_ptr, &info_ptr,
        &end_info);
    fclose(fp);
}


void SavePNG( Img const& img, Colour const* palette, const char* filename )
{
    assert(img.Format()==Img::INDEXED8BIT);
    // TODO: set error handler to capture error msg?

    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        throw Wobbly( "open failed: %s", strerror(errno) );
    }

    png_structp png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, (png_voidp)0,0,0 );
    if (!png_ptr)
    {
        fclose(fp);
        throw Wobbly( "failed writing PNG (png_create_write() failed)" );
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        throw Wobbly( "failed writing PNG (png_create_info_struct() failed)" );
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        throw Wobbly( "failed writing PNG" );
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR( png_ptr,
        info_ptr,
        img.W(), img.H(),
        8,
        PNG_COLOR_TYPE_PALETTE,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT );

    {
        png_color tmp_palette[256];
        int i;
        for(i=0;i<256;++i)
        {
            tmp_palette[i].red = palette[i].r;
            tmp_palette[i].green = palette[i].g;
            tmp_palette[i].blue = palette[i].b;
        }
        png_set_PLTE( png_ptr, info_ptr, tmp_palette, 256 );
    }

    //png_set_bKGD( png_ptr, info_ptr, 0 );

    {
        png_bytep row_pointers[ img.H() ];
        int y;
        for( y=0;y<img.H();++y)
            row_pointers[y] = (png_bytep)img.PtrConst(0,y);

        png_set_rows( png_ptr, info_ptr, row_pointers );

        png_write_png(png_ptr, info_ptr, 0, NULL);
    }

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
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

