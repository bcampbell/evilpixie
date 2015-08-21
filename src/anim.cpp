#include <cassert>

#include <IL/il.h>
#include <gif_lib.h>

#include "anim.h"
#include "img.h"
#include "exception.h"
#include "util.h"


Anim::Anim() :
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

PixelFormat Anim::Fmt() const
    { return m_Frames.front()->Fmt(); }

void Anim::TransferFrames(int srcfirst, int srclast, Anim& dest, int destfirst)
{
    dest.m_Frames.insert( dest.m_Frames.begin()+destfirst, m_Frames.begin()+srcfirst, m_Frames.begin()+srclast );
    m_Frames.erase(m_Frames.begin()+srcfirst, m_Frames.begin()+srclast);
}

void Anim::CalcBounds(Box& bound, int first, int last)
{
    int n;
    bound = GetFrameConst(first).Bounds();
    for(n=first+1;n<last; ++n)
        bound.Merge(GetFrameConst(n).Bounds());
}


void Anim::Load( const char* filename )
{
    std::string ext = ToLower( ExtName(filename) );
    if(ext == ".gif")
    {
        // TODO: try using DevIL Gif loader
        LoadGif(filename);
        return;
    }

#if 0
    int transparent_idx = -1;
#endif

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
        if(err==IL_INVALID_VALUE)
        {
            throw Exception( "Load failed - mistaken or badly formed %s file?", ext.c_str() );
        }
        throw Exception( "Load failed (code: 0x%x)", err );
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
            throw Exception("ilActiveImage() failed (err 0x%x)",ilGetError());
        }

        int w = (int)ilGetInteger( IL_IMAGE_WIDTH );
        int h = (int)ilGetInteger( IL_IMAGE_HEIGHT );
        ILint fmt = ilGetInteger( IL_IMAGE_FORMAT );
//    ILint type = ilGetInteger( IL_IMAGE_TYPE );
//    ILint bytesperpixel = ilGetInteger( IL_IMAGE_BYTES_PER_PIXEL );
//    ILint bitsperpixel = ilGetInteger( IL_IMAGE_BITS_PER_PIXEL );
//    printf("%dx%d fmt=%x type=%x %x %x\n", w,h,fmt,type, bytesperpixel, bitsperpixel );

        if( fmt == IL_COLOUR_INDEX )
        {
    #if 0
            transparent_idx = -1;
            if(ext==".png") // ugh
            {
                transparent_idx = (int)ilGetInteger(IL_PNG_ALPHA_INDEX);
            }
    #endif

            /* make sure everything is in the format we expect! */
            ilConvertImage( IL_COLOUR_INDEX, IL_UNSIGNED_BYTE );
            ilConvertPal( IL_PAL_RGBA32 );

            int pal_bytesperpixel = (int)ilGetInteger( IL_PALETTE_BPP );
            if( pal_bytesperpixel != 4 )
            {
                ilDeleteImages(1,&im);
                throw Exception( "Unsupported palette type (%d bytes/pixel)", pal_bytesperpixel );
            }

            int num_cols = (int)ilGetInteger( IL_PALETTE_NUM_COLS );
       //int palette_type = (int)ilGetInteger( IL_PALETTE_TYPE );
    //    printf("palette: %d colours, bpp=%d, type=0x%x\n", num_cols, pal_bytesperpixel, palette_type );
            if( num_cols>256)
                num_cols=256;
            m_Palette.SetNumColours(num_cols);
            uint8_t const* p = ilGetPalette();
            assert( p );
            int i=0;
            while(i<num_cols)
            {
                Colour c;
                c.r = *p++;
                c.g = *p++;
                c.b = *p++;
                c.a = *p++;
                m_Palette.SetColour(i,c);
                ++i;
            }
     
            const uint8_t* pixels = ilGetData();
            Img* img = new Img(FMT_I8,w,h,pixels);
            m_Frames.push_back(img);
        }
        else if( fmt == IL_RGB || fmt == IL_BGR )
        {
            ilConvertImage( IL_RGB, IL_UNSIGNED_BYTE );

            const uint8_t* raw = ilGetData();
            Img* img = new Img(FMT_RGBX8,w,h);
            int y;
            for(y=0;y<h;++y)
            {
                RGBX8 *dest = img->Ptr_RGBX8(0,y);
                int x;
                for( x=0;x<w;++x)
                {
                    RGBX8 c;
                    c.r = *raw++;
                    c.g = *raw++;
                    c.b = *raw++;
                    *dest++ = c;
                }
            }
            m_Frames.push_back(img);
        }
        else if( fmt == IL_RGBA || fmt == IL_BGRA )
        {
            ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );

            const uint8_t* raw = ilGetData();
            Img* img = new Img(FMT_RGBA8,w,h);
            int y;
            for(y=0;y<h;++y)
            {
                RGBA8 *dest = img->Ptr_RGBA8(0,y);
                int x;
                for( x=0;x<w;++x)
                {
                    RGBA8 c;
                    c.r = *raw++;
                    c.g = *raw++;
                    c.b = *raw++;
                    c.a = *raw++;
                    *dest++ = c;
                }
            }
            m_Frames.push_back(img);
        }
        else
        {
            ilDeleteImages(1,&im);
            throw Exception( "unsupported pixelformat" );
        }

    }
    ilDeleteImages(1,&im);
}



void Anim::LoadGif( const char* filename )
{
    GifFileType* f = DGifOpenFileName( filename );
    if( !f )
        throw Exception( "couldn't open '%s' (code %d)", filename, GifLastError() );

    if( DGifSlurp( f ) != GIF_OK )
    {
        DGifCloseFile(f);
        throw Exception( "couldn't load '%s' (code %d)", filename, GifLastError() );
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
    m_Palette.SetNumColours(cm->ColorCount);
    while(i<cm->ColorCount)
    {
        GifColorType const& c = cm->Colors[i];
        m_Palette.SetColour(i,Colour( c.Red, c.Green, c.Blue ));
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
            pal->SetColour(i,Colour(0,0,0));
        for( i=0; i<cm->ColorCount; ++i )
        {
            GifColorType const& c = cm->Colors[i];
            pal->SetColour(i,Colour( c.Red, c.Green, c.Blue ));
        }
*/
        // transparent-colour?
        for( i=0; i<si->ExtensionBlockCount; ++i) {
            ExtensionBlock* eb = &si->ExtensionBlocks[i];
            if( eb->Function==GRAPHICS_EXT_FUNC_CODE && eb->ByteCount==4 )
            {
                if( eb->Bytes[0] & 1)
                {
                    // set the colourkeyed palette entry to transparent
                    int key = (int)eb->Bytes[3];
                    Colour c = m_Palette.GetColour(key);
                    c.a = 0;
                    m_Palette.SetColour(key,c);
                }
            }
        }
        Img* tmp = new Img(FMT_I8, si->ImageDesc.Width, si->ImageDesc.Height, si->RasterBits);

        Append(tmp);
    }
    DGifCloseFile(f);
}


void Anim::Save( const char* filename )
{
    Img const& img = GetFrameConst(0);

    std::string ext = ToLower( ExtName(filename) );
    if(ext == ".gif")
    {
        if( img.Fmt() != FMT_I8) {
            throw Exception("Sorry... .gif only handles paletted images (maybe try .png instead?)");
        }

        // DevIL lib doesn't save Gifs
        SaveGif(filename);
        return;
    }

    if(NumFrames()>1)
        throw Exception("Sorry... to save anims you need to use GIF format (for now)");

    // OK then... just save first frame
    ILenum fileType = ilTypeFromExt(filename);
    if( fileType == IL_TYPE_UNKNOWN) {
        throw Exception("Sorry... unknown/unsupported file type");
    }

    ILuint im;
    ilGenImages( 1, &im );
    ilBindImage( im );

    //ILboolean ilTexImage(ILuint Width, ILuint Height, ILuint Depth, ILubyte NumChannels, ILenum Format, ILenum Type, void *Data);
    //
    if (img.Fmt() == FMT_I8)
    {

        if( !ilTexImage( img.W(), img.H(), 1, 1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, NULL ) )
        {
            ilDeleteImages(1,&im);
            throw Exception( "ilTexImage() failed)" );
        }

        /* copy in image, flipped (ilTexImage sets ORIGIN_LOWER_LEFT) */
        int y;
        for( y=0; y<img.H(); ++y )
        {
            I8 const* src = img.PtrConst_I8( 0, (img.H()-1)-y );
            ilSetPixels( 0,y,0, img.W(),1,1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, (void*)src );
        }



        uint8_t tmp_palette[4*m_Palette.NumColours()];
        int i;
        uint8_t* p = tmp_palette;
        for( i=0; i<=m_Palette.NumColours(); ++i )
        {
            Colour c = m_Palette.GetColour(i);
            *p++ = c.r;
            *p++ = c.g;
            *p++ = c.b;
            *p++ = c.a;
        }
        ilRegisterPal( tmp_palette, 4*m_Palette.NumColours(), IL_PAL_RGBA32 );
    }
    else if (img.Fmt() == FMT_RGBX8)
    {
        if( !ilTexImage( img.W(), img.H(), 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL ) )
        {
            ilDeleteImages(1,&im);
            throw Exception( "ilTexImage() failed)" );
        }

        /* copy in image, flipped (ilTexImage sets ORIGIN_LOWER_LEFT) */
        int y;
        for( y=0; y<img.H(); ++y )
        {
            RGBX8 const* src = img.PtrConst_RGBX8( 0, (img.H()-1)-y );
            // KLUDGE: note IL_BGRA order (to match our real in-memory order BGRx. See colours.h)
            ilSetPixels( 0,y,0, img.W(),1,1, IL_BGRA, IL_UNSIGNED_BYTE, (void*)src );
        }
    }
    else if (img.Fmt() == FMT_RGBA8)
    {
        if( !ilTexImage( img.W(), img.H(), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL ) )
        {
            ilDeleteImages(1,&im);
            throw Exception( "ilTexImage() failed)" );
        }

        /* copy in image, flipped (ilTexImage sets ORIGIN_LOWER_LEFT) */
        int y;
        for( y=0; y<img.H(); ++y )
        {
            RGBA8 const* src = img.PtrConst_RGBA8( 0, (img.H()-1)-y );
            // KLUDGE: note IL_BGRA order (to match our real in-memory order BGRx. See colours.h)
            ilSetPixels( 0,y,0, img.W(),1,1, IL_BGRA, IL_UNSIGNED_BYTE, (void*)src );
        }
    }
    else
    {
        assert(false);  // uhoh...
    }

    ilEnable(IL_FILE_OVERWRITE);
    if( !ilSaveImage( filename ) )
    {
        ilDeleteImages(1,&im);
        ILenum err = ilGetError();

        if( err == IL_INVALID_EXTENSION )
            // if we got this far, INVALID_EXTENSION really just means we can't save in that format.
            throw Exception( "Sorry, can't save in that file format" );
        if( err == IL_FORMAT_NOT_SUPPORTED )
            throw Exception( "File format not supported" );
        else
        {
            throw Exception( "ilSaveImage() failed (err 0x%x)", err );
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
            throw Exception( "couldn't open '%s' (gif code %d)", filename, GifLastError() );

        // TODO: per-frame palette support
        ColorMapObject* cmap = MakeMapObject( m_Palette.NumColours(), NULL);
        int i;
        int colourkey=-1;
        for( i=0; i<m_Palette.NumColours(); ++i)
        {
            GifColorType& c = cmap->Colors[i];
            Colour rgb = m_Palette.GetColour(i);
            c.Red = rgb.r;
            c.Green = rgb.g;
            c.Blue = rgb.b;

            // use first totally-transparent colour (if any) as colourkey
            if(rgb.a == 0 && colourkey==-1)
            {
                colourkey = i;
            }
        }
        Box screen;
        CalcBounds(screen,0,NumFrames());

        if( EGifPutScreenDesc(f,
            screen.w, screen.h,
            cmap->BitsPerPixel,
            0,  // GifBackGround,
            cmap) != GIF_OK )
        {
            throw Exception( "gif error (code %d)", filename, GifLastError() );
        }

        if(colourkey!=-1) {
            uint16_t delay = 100/FPS(); // in 1/100ths of a second

            unsigned char gc_ext[4] = {
                0x09, /* transparency=1, disposal=2 (Restore to background color) */
                delay&0xff, delay>>8, /* delay time (little endian) */
                colourkey
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

        int n;
        for(n=0; n<NumFrames(); ++n)
        {
            Img const& img = GetFrameConst(n);
            if(img.Fmt() != FMT_I8)
                throw Exception("Sorry... must be paletted image (for now)");
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
                throw Exception( "gif error (code %d)", filename, GifLastError() );
            }

            int y;
            for(y=0; y<img.H(); ++y)
            {
                GifPixelType* pix = (GifPixelType*)img.PtrConst_I8(0,y);
                if(EGifPutLine(f, pix, img.W()) != GIF_OK)
                {
                    throw Exception( "gif error (code %d)", filename, GifLastError() );
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


