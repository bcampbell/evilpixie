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


/*
void Anim::Dump() const
{
    printf("Dump---\n");
    unsigned int i;
    for (i=0; i<m_Frames.size(); ++i )
    {
        Img* im = m_Frames[i];
        printf("%d: %p", i, im);
        if(im) {
            Box const& b = im->Bounds();
            printf(" %d %d %d %d\n",b.x,b.y,b.w,b.h);
        } else {
            printf("\n");
        }
    }
}
*/

PixelFormat Anim::Fmt() const
    { return m_Frames.front()->Fmt(); }

void Anim::TransferFrames(int srcfirst, int srclast, Anim& dest, int destfirst)
{
    dest.m_Frames.insert( dest.m_Frames.begin()+destfirst, m_Frames.begin()+srcfirst, m_Frames.begin()+srclast );
    m_Frames.erase(m_Frames.begin()+srcfirst, m_Frames.begin()+srclast);
}

void Anim::CalcBounds(Box& bound, int first, int last) const
{
    int n;
    bound = GetFrameConst(first).Bounds();
    for(n=first+1;n<last; ++n)
        bound.Merge(GetFrameConst(n).Bounds());
}


class iltmp
{
public:
    iltmp()     { m_id=ilGenImage(); }
    ~iltmp()    { ilDeleteImage(m_id); }
    void Bind() { ilBindImage(m_id); }
private:
    ILuint m_id;
};


void Anim::Load( const char* filename )
{
    std::string ext = ToLower( ExtName(filename) );
#if 0
    int transparent_idx = -1;
#endif

    ilDisable( IL_CONV_PAL );

    iltmp im;
    im.Bind();

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
        im.Bind();  // kludge to work around ilActiveImage() oddness...
        if(ilActiveImage(frame)==IL_FALSE)
        {
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
            throw Exception( "unsupported pixelformat" );
        }
    }
}



// load img into the currently-bound IL image context
static void plonkFrame( Img const& img, Palette const& pal )
{
    if (img.Fmt() == FMT_I8)
    {

        if( !ilTexImage( img.W(), img.H(), 1, 1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, NULL ) )
        {
            throw Exception( "ilTexImage() failed)" );
        }

        /* copy in image, flipped (ilTexImage sets ORIGIN_LOWER_LEFT) */
        int y;
        for( y=0; y<img.H(); ++y )
        {
            I8 const* src = img.PtrConst_I8( 0, (img.H()-1)-y );
            ilSetPixels( 0,y,0, img.W(),1,1, IL_COLOUR_INDEX, IL_UNSIGNED_BYTE, (void*)src );
        }

        uint8_t tmp_palette[4*pal.NumColours()];
        int i;
        uint8_t* p = tmp_palette;
        for( i=0; i<=pal.NumColours(); ++i )
        {
            Colour c = pal.GetColour(i);
            *p++ = c.r;
            *p++ = c.g;
            *p++ = c.b;
            *p++ = c.a;
        }
        ilRegisterPal( tmp_palette, 4*pal.NumColours(), IL_PAL_RGBA32 );
    }
    else if (img.Fmt() == FMT_RGBX8)
    {
        if( !ilTexImage( img.W(), img.H(), 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL ) )
        {
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
        throw Exception( "internal - bad image format" );
    }
}



void Anim::Save( const char* filename )
{
    ILenum fileType = ilTypeFromExt(filename);
    if( fileType == IL_TYPE_UNKNOWN) {
        throw Exception("Sorry... unknown/unsupported file type");
    }

    iltmp im;
    im.Bind();

    int frameNum;
    if (!ilRegisterNumImages(NumFrames()))
    {
        throw Exception( "ilRegisterNumImages() failed (err 0x%x)", ilGetError() );
    }

    for (frameNum=0; frameNum<NumFrames(); ++frameNum)
    {
        if(!ilActiveImage(frameNum))
        {
            throw Exception( "ilActiveImage() failed (err 0x%x)", ilGetError() );
        }
        plonkFrame(GetFrameConst(frameNum), GetPaletteConst());
    }

    ilEnable(IL_FILE_OVERWRITE);
    if( !ilSaveImage( filename ) )
    {
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
}


