#include <cassert>

#include <impy.h>

#include "layer.h"
#include "img.h"
#include "exception.h"
#include "util.h"


Layer::Layer() :
    m_FPS(10)
{
}

Layer::~Layer()
{
    Zap();
}

void Layer::Zap()
{
    while( !mFrames.empty() )
    {
        delete mFrames.back();
        mFrames.pop_back();
    }
}


static Img* from_im_img( im_img* srcimg, Palette& pal);

/*
void Layer::Dump() const
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

PixelFormat Layer::Fmt() const
    { return mFrames.front()->mImg->Fmt(); }

void Layer::CalcBounds(Box& bound, int first, int last) const
{
    int n;
    for (auto f : mFrames) {
        bound.Merge(f->mImg->Bounds());
    }
}


void Layer::Load( const char* filename )
{
    ImErr err;

    im_bundle* bundle = im_bundle_load(filename, &err);
    if (!bundle) {
            switch(err) {
                case ERR_NOMEM:
                   throw Exception("Ran out of memory");
                case ERR_COULDNTOPEN:
                case ERR_FILE:
                    throw Exception("File error (imerr code %d)",err);
                case ERR_UNKNOWN_FILE_TYPE:
                    throw Exception("Unknown or unsupported file type");
                case ERR_UNSUPPORTED:
                    throw Exception("Unsupported");
                case ERR_MALFORMED:
                    throw Exception("File malformed");
                default:
                    throw Exception( "Load failed (imerr code %d)", (int)err);
            }
    }
    int nframes = im_bundle_num_frames(bundle);
    int frame;
    for(frame=0; frame<nframes; ++frame)
    {
        im_img* srcimg = im_bundle_get_frame(bundle,frame);

        if (!srcimg) {
            // TODO: how to handle missing frames?
            continue;
        }

        Frame* frame = new Frame();
        frame->mImg = from_im_img( srcimg, m_Palette);
        mFrames.push_back(frame);
    }

    im_bundle_free(bundle);
}



static Img* from_im_img( im_img* srcimg, Palette& pal)
{
    int x,y;
    int w = im_img_w(srcimg);
    int h = im_img_h(srcimg);
    ImFmt fmt = im_img_format(srcimg);
    Img* out = 0;

    if (fmt == FMT_COLOUR_INDEX) {
        int ncolours = im_img_pal_num_colours(srcimg);
        if (ncolours>0) {
            if( ncolours>256)
                ncolours=256;
            pal.SetNumColours(ncolours);
            const uint8_t* p = (const uint8_t*)im_img_pal_data(srcimg);
            assert( p );
            int i;
            switch (im_img_pal_fmt(srcimg)) {
                case PALFMT_RGB:
                    for (i=0; i<ncolours; ++i) {
                        Colour c;
                        c.r = *p++;
                        c.g = *p++;
                        c.b = *p++;
                        c.a = 255;
                        pal.SetColour(i,c);
                    }
                    break;
                case PALFMT_RGBA:
                    for (i=0; i<ncolours; ++i) {
                        Colour c;
                        c.r = *p++;
                        c.g = *p++;
                        c.b = *p++;
                        c.a = *p++;
                        pal.SetColour(i,c);
                    }
                    break;
            }
        }

        out = new Img(FMT_I8,w,h);
        for (y=0; y<h; ++y) {
            const uint8_t* src = (const uint8_t*)im_img_row(srcimg,y);
            I8* dest = out->Ptr_I8(0,y);
            for (x=0; x<w; ++x) {
                *dest++ = (I8)*src++;
            }
        }
        return out;
    }

    if (fmt == FMT_RGB) {
        out = new Img(FMT_RGBX8,w,h);
        for (y=0; y<h; ++y) {
            const uint8_t* src = (const uint8_t*)im_img_row(srcimg,y);
            RGBX8* dest = out->Ptr_RGBX8(0,y);
            for (x=0; x<w; ++x) {
                dest->r = *src++;
                dest->g = *src++;
                dest->b = *src++;
                dest->pad = 255;
                ++dest;
            }
        }
        return out;
    }

    if (fmt == FMT_RGBA) {
        out = new Img(FMT_RGBA8,w,h);
        for (y=0; y<h; ++y) {
            const uint8_t* src = (const uint8_t*)im_img_row(srcimg,y);
            RGBA8* dest = out->Ptr_RGBA8(0,y);
            for (x=0; x<w; ++x) {
                dest->r = *src++;
                dest->g = *src++;
                dest->b = *src++;
                dest->a = *src++;
                ++dest;
            }
        }
        return out;
    }

    throw Exception( "unsupported pixelformat" );
}



static im_img* to_im_img( Img const& img, Palette const& pal )
{
    im_img* out;
    int x,y;
    if (img.Fmt() == FMT_I8) {
        out = im_img_new(img.W(), img.H(), 1, FMT_COLOUR_INDEX, DT_U8);
        if (!out) {
            // TODO: better error!
            throw Exception( "im_img_new() failed" );
        }

        for( y=0; y<img.H(); ++y )
        {
            I8 const* src = img.PtrConst_I8( 0, y );
            uint8_t* dest = (uint8_t*)im_img_row( out, y );
            for( x=0; x<img.W(); ++x) {
                *dest++ = (uint8_t)*src++;
            }
        }

        uint8_t tmp_palette[4*pal.NumColours()];
        int i;
        uint8_t* p = tmp_palette;
        for( i=0; i<pal.NumColours(); ++i )
        {
            Colour c = pal.GetColour(i);
            *p++ = c.r;
            *p++ = c.g;
            *p++ = c.b;
            *p++ = c.a;
        }
        // TODO: just use RGB palette if appropriate...
        im_img_pal_set( out, PALFMT_RGBA, pal.NumColours(), tmp_palette);
    } else if (img.Fmt() == FMT_RGBX8) {
        out = im_img_new(img.W(), img.H(), 1, FMT_RGB, DT_U8);
        if (!out) {
            // TODO: better error!
            throw Exception( "im_img_new() failed" );
        }

        for( y=0; y<img.H(); ++y )
        {
            RGBX8 const* src = img.PtrConst_RGBX8( 0, y );
            uint8_t* dest = (uint8_t*)im_img_row( out, y );
            for( x=0; x<img.W(); ++x) {
                *dest++ = src->r;
                *dest++ = src->g;
                *dest++ = src->b;
                ++src;
            }
        }
    } else if (img.Fmt() == FMT_RGBA8) {
        out = im_img_new(img.W(), img.H(), 1, FMT_RGBA, DT_U8);
        if (!out) {
            // TODO: better error!
            throw Exception( "im_img_new() failed" );
        }

        for( y=0; y<img.H(); ++y )
        {
            RGBA8 const* src = img.PtrConst_RGBA8( 0, y );
            uint8_t* dest = (uint8_t*)im_img_row( out, y );
            for( x=0; x<img.W(); ++x) {
                *dest++ = src->r;
                *dest++ = src->g;
                *dest++ = src->b;
                *dest++ = src->a;
                ++src;
            }
        }
    } else {
        assert(false);  // uhoh...
        throw Exception( "internal - bad image format" );
    }
    return out;
}



void Layer::Save( const char* filename )
{
    // sanity check
    std::string ext = ToLower(ExtName(filename));

    if (NumFrames()>1) {
        if (ext!=".gif") {
            throw Exception("To save animation, you need to save as .gif or convert to a spritesheet");
        }
    }

    if (Fmt()!= FMT_I8) {
        if( ext==".gif") {
            throw Exception("Sorry, .gif can only save 8-bit indexed images");
        }
    }
    // end sanity check

    im_bundle* bundle;

    bundle = im_bundle_new();
    if (!bundle) {
        throw Exception( "out of memory" );
    }

    try {
        int i;
        for (i=0; i<NumFrames(); ++i) {
            SlotID id = {0,0,0,0};
            id.frame = i;
            im_img* img = to_im_img(GetImgConst(i), GetPaletteConst());
            im_bundle_set( bundle, id, img);
        }
        ImErr err;
        if (!im_bundle_save( bundle, filename, &err) ) {
            switch(err) {
                case ERR_NOMEM:
                   throw Exception("Ran out of memory");
                case ERR_UNKNOWN_FILE_TYPE:
                   throw Exception("Unknown file type (try .png or .gif maybe?)");
                case ERR_COULDNTOPEN:
                case ERR_FILE:
                    throw Exception("File error (imerr code %d)",err);
                case ERR_UNSUPPORTED:
                    throw Exception("Unsupported");
                default:
                    throw Exception("Error (imerr code %d)",err);
            }
        }
    }
    catch (Exception& e) {
        im_bundle_free(bundle);
        throw e;
    }

    im_bundle_free(bundle);
}


