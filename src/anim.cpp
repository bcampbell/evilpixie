#include <cassert>

#include <IL/il.h>
#include <gif_lib.h>

#include "anim.h"
#include "img.h"
#include "wobbly.h"
#include "util.h"


Anim::Anim() :
    m_TransparentIdx(-1),
    m_FPS(10)
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
    }
}


void Anim::TransferFrames(int srcfirst, int srclast, Anim& dest, int destfirst)
{
    dest.m_Frames.insert( dest.m_Frames.begin()+destfirst, m_Frames.begin()+srcfirst, m_Frames.begin()+srclast );
    m_Frames.erase(m_Frames.begin()+srcfirst, m_Frames.begin()+srclast);
}



void Anim::Load( const char* filename )
{
    std::string ext = ToLower( ExtName(filename) );
    if(ext == ".gif")
    {
        LoadGif(filename);
        return;
    }

    int transparent_idx = -1;

    ilDisable( IL_CONV_PAL );

    ILuint im;
    ilGenImages( 1, &im );
    ilBindImage( im );
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
   //int palette_type = (int)ilGetInteger( IL_PALETTE_TYPE );
//    printf("palette: %d colours, bpp=%d, type=0x%x\n", num_cols, pal_bytesperpixel, palette_type );

        uint8_t const* p = ilGetPalette();
        assert( p );
        int i=0;
        while( i<num_cols && i<=255 )
        {
            RGBx c;
            c.r = *p++;
            c.g = *p++;
            c.b = *p++;
            m_Palette.SetColour(i,c);
            ++i;
        }
        while( i<=255 )
        {
            m_Palette.SetColour(i,RGBx(0,0,0));
            ++i;
        }
 
        const uint8_t* pixels = ilGetData();
        IndexedImg* img = new IndexedImg(w,h,pixels);
        m_Frames.push_back(img);
    }

    ilDeleteImages(1,&im);
}



void Anim::LoadGif( const char* filename )
{
    GifFileType* f = DGifOpenFileName( filename );
    if( !f )
        throw Wobbly( "couldn't open '%s' (code %d)", filename, GifLastError() );

    if( DGifSlurp( f ) != GIF_OK )
    {
        DGifCloseFile(f);
        throw Wobbly( "couldn't load '%s' (code %d)", filename, GifLastError() );
    }
#if 0
    printf("SWidth: %d\n", f->SWidth );
    printf("SHeight: %d\n", f->SHeight );
    printf("SColorResolution: %d\n", f->SColorResolution );
    printf("SBackGroundColor: %d\n", f->SBackGroundColor );
    printf("ImageCount: %d\n", f->ImageCount );

    printf("ColorMap->ColorCount: %d\n", f->SColorMap->ColorCount );
    printf("ColorMap->BitsPerPixel: %d\n", f->SColorMap->BitsPerPixel );
#endif

    int i=0;
    ColorMapObject* cm = f->SColorMap;
    while(i<cm->ColorCount)
    {
        GifColorType const& c = cm->Colors[i];
        m_Palette.SetColour(i,RGBx( c.Red, c.Green, c.Blue ));
        ++i;
    }
    while(i<256)
    {
        m_Palette.SetColour(i,RGBx(0,0,0));
        ++i;
    }

    int n;
    for( n=0; n<f->ImageCount; ++n)
    {
        SavedImage* si = &f->SavedImages[n];
        // TODO: per-frame palettes
/*
        ColorMapObject* cm = si->ImageDesc.ColorMap;
        if( !cm )
            cm = f->SColorMap;

        // install palette
        Palette* pal = new Palette();
        int i;
        for(i=0;i<256;++i)
            pal->SetColour(i,RGBx(0,0,0));
        for( i=0; i<cm->ColorCount; ++i )
        {
            GifColorType const& c = cm->Colors[i];
            pal->SetColour(i,RGBx( c.Red, c.Green, c.Blue ));
        }
*/
        IndexedImg* tmp = new IndexedImg(si->ImageDesc.Width, si->ImageDesc.Height, si->RasterBits);

        Append(tmp);
    }
    DGifCloseFile(f);
}


void Anim::Save( const char* filename )
{
    std::string ext = ToLower( ExtName(filename) );
    if(ext == ".gif")
    {
        SaveGif(filename);
        return;
    }

    if(NumFrames()>1)
        throw Wobbly("Sorry... to save anims you need to use GIF format (for now)");

    // OK then... just save first frame
    IndexedImg const& img = GetFrameConst(0);

    ilSetInteger(IL_PNG_ALPHA_INDEX,TransparentIdx());

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
        RGBx c = m_Palette.GetColour(i);
        *p++ = c.r;
        *p++ = c.g;
        *p++ = c.b;
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


void Anim::SaveGif( const char* filename )
{
    bool animated = NumFrames()>1 ? true:false;

    EGifSetGifVersion("89a");
    GifFileType* f=0;
    ColorMapObject* cmap=0;

    try
    {
        f = EGifOpenFileName(filename, 0);

        if( !f )
            throw Wobbly( "couldn't open '%s' (gif code %d)", filename, GifLastError() );

        // TODO: per-frame palette support
        ColorMapObject* cmap = MakeMapObject( 256, NULL);
        int i;
        for( i=0;i<256;++i)
        {
            GifColorType& c = cmap->Colors[i];
            RGBx rgb = m_Palette.GetColour(i);
            c.Red = rgb.r;
            c.Green = rgb.g;
            c.Blue = rgb.b;
        }

        int n;
        Box screen = GetFrameConst(0).Bounds();
        for(n=1;n<NumFrames(); ++n)
            screen.Merge(GetFrameConst(n).Bounds());

        if( EGifPutScreenDesc(f,
            screen.w, screen.h,
            cmap->BitsPerPixel,
            0,  // GifBackGround,
            cmap) != GIF_OK )
        {
            throw Wobbly( "gif error (code %d)", filename, GifLastError() );
        }

        if(TransparentIdx()!=-1) {
            uint16_t delay = 100/FPS(); // in 1/100ths of a second

            unsigned char gc_ext[4] = {
                0x09, /* transparency=1, disposal=2 (Restore to background color) */
                delay&0xff, delay>>8, /* delay time (little endian) */
                TransparentIdx()
            };
            EGifPutExtension(f, GRAPHICS_EXT_FUNC_CODE, 4, gc_ext);
        }

        if(animated)
        {
            // insert extension block to indicate looping
            unsigned char buf[3] = {1, 0,0}; // 1, 16 bit loopcount (0=infinite)
            EGifPutExtensionFirst(f, APPLICATION_EXT_FUNC_CODE, 11, "NETSCAPE2.0");
            EGifPutExtensionLast(f, APPLICATION_EXT_FUNC_CODE, 3, buf);
        }

        for(n=0; n<NumFrames(); ++n)
        {
            IndexedImg const& img = GetFrameConst(n);
/*
 * anim delay and transparent colour in extension?
            int EGifPutExtension(
                GifFileType *GifFile,
                int GifExtCode,
                int GifExtLen,
                void *GifExtension)
*/

            if( EGifPutImageDesc(f,
                0,0,    // GifLeft, GifTop,
                img.W(), img.H(),
                FALSE,
                NULL) != GIF_OK )
            {
                throw Wobbly( "gif error (code %d)", filename, GifLastError() );
            }

            int y;
            for(y=0; y<img.H(); ++y)
            {
                GifPixelType* pix = (GifPixelType*)img.PtrConst(0,y);
                if(EGifPutLine(f, pix, img.W()) != GIF_OK)
                {
                    throw Wobbly( "gif error (code %d)", filename, GifLastError() );
                }
            }

        }
        FreeMapObject(cmap);
        EGifCloseFile(f);
    }
    catch(...)
    {
        if(cmap)
            FreeMapObject(cmap);
        if(f)
            EGifCloseFile(f);
        throw;
    }
}


