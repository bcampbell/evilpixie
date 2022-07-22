#include <impy.h>

#include "file_save.h"
#include "file_type.h"
#include "layer.h"
#include "img.h"
#include "exception.h"
#include "util.h"


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


static void throwImpyErr(ImErr err) {
    switch(err) {
        case IM_ERR_NOMEM:
           throw Exception("Ran out of memory");
        case IM_ERR_UNKNOWN_FILE_TYPE:
           throw Exception("Unknown file type (try .png or .gif maybe?)");
        case IM_ERR_COULDNTOPEN:
        case IM_ERR_FILE:
            throw Exception("File error (imerr code %d)",err);
        case IM_ERR_UNSUPPORTED:
            throw Exception("Unsupported");
        default:
            throw Exception("Error (impy err %d)",err);
    }
}

void SaveLayer(Layer const& layer, std::string const& filename)
{
    ImErr err;
    im_write* writer = im_write_open_file( filename.c_str(), &err);
    if (!writer) {
        throwImpyErr(err);
    }

    for (Frame const* frame : layer.mFrames) {
        Img const* img = frame->mImg;
        ImFmt fmt;
        switch (img->Fmt()) {
            case FMT_RGBX8: fmt = IM_FMT_RGBX; break;
            case FMT_RGBA8: fmt = IM_FMT_RGBA; break;
            case FMT_I8: fmt = IM_FMT_INDEX8; break;
            default:
            {
                im_write_finish(writer);
                throw Exception("Unsupported source image format.");
            }
        }
        im_write_img(writer, img->W(), img->H(), fmt);

        // TODO: handle global and per-frame palettes...
        Palette const& pal = layer.mPalette;
        if (pal.NColours > 0) {
            std::vector<uint8_t> colbuf(pal.NColours * 4);
            uint8_t* dest = colbuf.data();
            Colour const* src = pal.Colours;
            for (int i = 0; i < pal.NColours; ++i) {
                *dest++ = src->r;
                *dest++ = src->g;
                *dest++ = src->b;
                *dest++ = src->a;
                ++src;
            }
            im_write_palette(writer, IM_FMT_RGBA, pal.NColours, colbuf.data());
            err = im_write_err(writer);
        } 
        im_write_rows(writer, img->H(), img->PtrConst(0, 0), img->Pitch());
    }

    err = im_write_finish(writer);
    if (err != IM_ERR_NONE) {
        throwImpyErr(err);
    }
}

