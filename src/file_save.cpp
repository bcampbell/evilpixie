#include <impy.h>

#include "file_save.h"
#include "file_type.h"
#include "layer.h"
#include "img.h"
#include "exception.h"
#include "util.h"

// defined in file_load.cpp
extern std::string const impyErrToMsg(ImErr err);

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
    ImErr err;
    im_write* writer = im_write_open_file( filename.c_str(), &err);
    if (!writer) {
        throw Exception(std::string("Save failed: ") + impyErrToMsg(err));
    }

    for (Frame const* frame : layer.mFrames) {
        Img const* img = frame->mImg;
        ImFmt fmt;
        switch (img->Fmt()) {
            // Our internal component ordering is set up to match QImage ARGB.
            // (But Qt accesses it as uint32_t and we're little-endian specific
            // at the moment, so bytewise it comes out as BGRA!).
            // Luckily, impy can just supply whatever we ask for.
            // TODO: handle big-endian!
            case FMT_RGBX8: fmt = IM_FMT_BGRX; break;
            case FMT_RGBA8: fmt = IM_FMT_BGRA; break;
            case FMT_I8: fmt = IM_FMT_INDEX8; break;
            default:
            {
                im_write_finish(writer);
                throw Exception("Unsupported source image format.");
            }
        }
        im_write_img(writer, img->W(), img->H(), fmt);

        // Write out palette?
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

        // Write metadata
        {
            SpriteGrid const& g = layer.mSpriteSheetGrid;
            unsigned int cnt = (g.numFrames > 0) ? g.numFrames : g.numColumns * g.numRows;
            if (cnt > 1) {
                im_write_kv(writer, "SpriteSheet", layer.mSpriteSheetGrid.Stringify(img->Bounds()).c_str());
            }
        }

        im_write_rows(writer, img->H(), img->PtrConst(0, 0), img->Pitch());
    }

    err = im_write_finish(writer);
    if (err != IM_ERR_NONE) {
        throw Exception(std::string("Save failed: ") + impyErrToMsg(err));
    }
}

