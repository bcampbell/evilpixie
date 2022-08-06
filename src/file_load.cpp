#include <impy.h>
#include <string>
#include <vector>

#include "file_load.h"
#include "layer.h"
#include "img.h"
#include "exception.h"
#include "util.h"

// Return a readable string from an impy errorcode.
std::string const impyErrToMsg(ImErr err) {
    const struct {ImErr code; std::string msg;} msgs[] = {
        {IM_ERR_NONE, "OK"},
        {IM_ERR_NOMEM, "Ran out of memory"},
        {IM_ERR_COULDNTOPEN, "Couldn't open file"},
        {IM_ERR_FILE, "File error"},
        {IM_ERR_UNKNOWN_FILE_TYPE, "Unknown or unsupported file type"},
        {IM_ERR_UNSUPPORTED, "Unsupported file format"},
        {IM_ERR_MALFORMED, "File malformed"},
    };

    for(auto const& ent: msgs) {
        if(err == ent.code) {
            return ent.msg;
        }
    }
    return std::string("Error code ") + std::to_string(err);
}

Layer* LoadLayer(std::string const& filename)
{
    ImErr err;

    im_imginfo inf;
    im_read* rdr = im_read_open_file(filename.c_str(), &err);
    if (!rdr) {
        throw Exception(std::string("Load failed: ") + impyErrToMsg(err));
    }

    Layer *layer = new Layer();
    while (im_read_img(rdr, &inf)) {
        Img *img = nullptr;
        if (im_fmt_is_indexed(inf.fmt)) {
            im_read_set_fmt(rdr, IM_FMT_INDEX8);
            img = new Img(FMT_I8, inf.w, inf.h);
        } else if (im_fmt_has_rgb(inf.fmt)) {
            // Our internal component ordering is set up to match QImage ARGB.
            // (But Qt accesses it as uint32_t and we're little-endian specific
            // at the moment, so bytewise it comes out as BGRA!).
            // Luckily, impy can just supply whatever we ask for.
            // TODO: handle big-endian!
            if (im_fmt_has_alpha(inf.fmt)) {
                im_read_set_fmt(rdr, IM_FMT_BGRA);
                img = new Img(FMT_RGBA8, inf.w, inf.h);
            } else {
                im_read_set_fmt(rdr, IM_FMT_BGRX);
                img = new Img(FMT_RGBX8, inf.w, inf.h);
            }
        }
        if (!img) {
            throw Exception("Unsupported pixel format.");
        }

        // read palette, if any.
        if (inf.pal_num_colours > 0) {
            Palette pal(inf.pal_num_colours);
            std::vector<uint8_t> buf(4 * inf.pal_num_colours);
            im_read_palette(rdr, IM_FMT_RGBA, buf.data());
            uint8_t* src = buf.data(); 
            for (unsigned int i = 0; i < inf.pal_num_colours; ++i) {
                pal.Colours[i] = RGBA8(src[0], src[1], src[2], src[3]);
                src += 4;
            }
            // KLUDGE!!!
            layer->mPalette = pal;
        
        }

        // read the image rows
        im_read_rows(rdr, img->H(), img->Ptr(0,0), img->Pitch());

        //
        Frame* frame = new Frame();
        frame->mImg = img;
        layer->mFrames.push_back(frame);
    }

    err = im_read_finish(rdr);
    if (err != IM_ERR_NONE) {
        throw Exception(std::string("Load failed: ") + impyErrToMsg(err));
    }

    layer->mFilename = filename;

    return layer;
}


#if 0
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
#endif
