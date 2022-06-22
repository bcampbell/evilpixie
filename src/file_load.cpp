#include <impy.h>

#include "file_load.h"
#include "layer.h"
#include "img.h"
#include "exception.h"
#include "util.h"

static Img* from_im_img( im_img* srcimg, Palette& pal);

Layer* LoadLayer(std::string const& filename)
{
    ImErr err;

    im_bundle* bundle = im_bundle_load(filename.c_str(), &err);
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
    Layer *layer = new Layer();
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
        frame->mImg = from_im_img(srcimg, layer->mPalette);
        layer->mFrames.push_back(frame);
    }

    im_bundle_free(bundle);
    return layer;
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

