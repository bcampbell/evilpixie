#include <impy.h>

#include "file_save.h"
#include "file_type.h"
#include "layer.h"
#include "img.h"
#include "exception.h"
#include "util.h"

static im_img* to_im_img( Img const& img, Palette const& pal );

SaveRequirements CheckSave(Stack const& stack, Filetype ft)
{
    // Get capabilities of format (TODO: move this stuff into impy).
    bool canSave = (ft == FILETYPE_PNG || ft == FILETYPE_GIF || ft == FILETYPE_BMP);
    bool fmtSupportsLayers = false;
    bool fmtIndexedOnly = (ft == FILETYPE_GIF || ft == FILETYPE_PCX || ft == FILETYPE_IFF_ILBM);
    bool fmtSupportsAnim = (ft == FILETYPE_GIF);

    // Get characteristics of project.
    std::vector<Layer const*> layers;
    stack.WalkConst([&] (BaseNode const* n ) {if (n->ToLayerConst()) {layers.push_back(n->ToLayerConst());};});

    bool isAnimated = false;
    bool isIndexed = true;
    for (auto l: layers) {
        if (l->mFrames.size()>1) {
            isAnimated = true;
        }
        if (l->Fmt() != FMT_I8) {
            isIndexed = false;
        }
    }

    SaveRequirements req;
    req.cantSave = !canSave;
    req.flatten = (!fmtSupportsLayers) && (layers.size() > 1);
    req.quantise = fmtIndexedOnly && (!isIndexed);
    req.noAnim = (!fmtSupportsAnim) && isAnimated;
    req.sheetColumns = 1;   // just so we don't leave it unset...
    return req;
}



void SaveLayer(Layer const& layer, std::string const& filename)
{
    im_bundle* bundle = im_bundle_new();
    if (!bundle) {
        throw Exception( "out of memory" );
    }

    try {
        int i=0;
        for (Frame const* frame : layer.mFrames) {
            SlotID id = {0,0,0,0};
            id.frame = i++;
            im_img* img = to_im_img(*(frame->mImg), layer.GetPaletteConst());
            im_bundle_set( bundle, id, img);
        }
        ImErr err;
        if (!im_bundle_save( bundle, filename.c_str(), &err) ) {
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

