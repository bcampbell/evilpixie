#include "blit_range.h"
#include "blit.h"

#include "box.h"
#include "img.h"

#include <algorithm>

//----
// range inc/dec, using a src img as key.

static void scan_rangeinc_I8_I8_keyed(I8 const* src, I8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        I8 c = *src++;
        if( c != transparent) {
            I8 pix = *dest;
            auto it = std::find_if(range.begin(), range.end(),
                [pix](PenColour const& pen) -> bool { return pen.idx() == pix;});
            if (it < range.end()-1) {
                *dest = (it+1)->idx();
            }
        }
        ++dest;
    }
}


static void scan_rangedec_I8_I8_keyed(I8 const* src, I8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        I8 c = *src++;
        if( c != transparent) {
            I8 pix = *dest;
            auto it = std::find_if(range.begin(), range.end(),
                [pix](PenColour const& pen) -> bool { return pen.idx() == pix;});

            if (it != range.end() && it > range.begin()) {
                *dest = (it-1)->idx();
            }
        }
        ++dest;
    }
}


static void scan_rangeinc_I8_RGBX8_keyed(I8 const* src, RGBX8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        I8 c = *src++;
        if( c != transparent) {
            RGBX8 pix = *dest;
            auto it = std::find_if(range.begin(), range.end(),
                [pix](PenColour const& pen) -> bool { return pen.toRGBX8() == pix;});
            if (it < range.end()-1) {
                *dest = (it+1)->toRGBX8();
            }
        }
        ++dest;
    }
}


static void scan_rangedec_I8_RGBX8_keyed(I8 const* src, RGBX8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        I8 c = *src++;
        if( c != transparent) {
            RGBX8 pix = *dest;
            auto it = std::find_if(range.begin(), range.end(),
                [pix](PenColour const& pen) -> bool { return pen.toRGBX8() == pix;});

            if (it != range.end() && it > range.begin()) {
                *dest = (it-1)->toRGBX8();
            }
        }
        ++dest;
    }
}


static void scan_rangeinc_I8_RGBA8_keyed(I8 const* src, RGBA8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        I8 c = *src++;
        if( c != transparent) {
            RGBA8 pix = *dest;
            auto it = std::find_if(range.begin(), range.end(),
                [pix](PenColour const& pen) -> bool { return pen.toRGBA8() == pix;});
            if (it < range.end()-1) {
                *dest = (it+1)->toRGBA8();
            }
        }
        ++dest;
    }
}


static void scan_rangedec_I8_RGBA8_keyed(I8 const* src, RGBA8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        I8 c = *src++;
        if( c != transparent) {
            RGBA8 pix = *dest;
            auto it = std::find_if(range.begin(), range.end(),
                [pix](PenColour const& pen) -> bool { return pen.toRGBA8() == pix;});

            if (it != range.end() && it > range.begin()) {
                *dest = (it-1)->toRGBA8();
            }
        }
        ++dest;
    }
}



// Uses the srcimg as a mask, incrementing or decrementing pixels on destimg
// up or down the the range.
void BlitRangeShiftI8Keyed(Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentPen,    // TODO: should just be int
    std::vector<PenColour> const& range,
    int direction)
{
    assert(srcimg.Fmt() == FMT_I8);
    assert(transparentPen.IdxValid());

    if (range.empty()) {
        destbox.w = 0;
        destbox.h = 0;
        return;
    }

    Box srcclipped(srcbox);
    clip_blit(srcimg.Bounds(), srcclipped, destimg.Bounds(), destbox);


    const int w = destbox.w;
    int y;
    const int x0 = destbox.x;
    const int y0 = destbox.y;
    for (y = 0; y < destbox.h; ++y)
    {
        I8 const* src = srcimg.PtrConst_I8(srcclipped.x + 0, srcclipped.y + y);
        if (direction > 0) {
            // Increment along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangeinc_I8_I8_keyed(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangeinc_I8_RGBX8_keyed(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangeinc_I8_RGBA8_keyed(src, destimg.Ptr_RGBA8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                default:
                    assert(false);
                    break;
            }
        } else {
            // Decrement along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangedec_I8_I8_keyed(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangedec_I8_RGBX8_keyed(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangedec_I8_RGBA8_keyed(src, destimg.Ptr_RGBA8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
}


//-------
// Range inc/dec for solid regions (no keying)


static void scan_rangeinc_I8(I8* dest, int w, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        I8 pix = *dest;
        auto it = std::find_if(range.begin(), range.end(),
            [pix](PenColour const& pen) -> bool { return pen.idx() == pix;});
        if (it < range.end()-1) {
            *dest = (it+1)->idx();
        }
        ++dest;
    }
}

static void scan_rangedec_I8(I8* dest, int w, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for (x=0; x < w; ++x) {
        I8 pix = *dest;
        auto it = std::find_if(range.begin(), range.end(),
            [pix](PenColour const& pen) -> bool { return pen.idx() == pix;});

        if (it != range.end() && it > range.begin()) {
            *dest = (it-1)->idx();
        }
        ++dest;
    }
}

static void scan_rangeinc_RGBX8(RGBX8* dest, int w, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for (x = 0; x < w; ++x) {
        RGBX8 pix = *dest;
        auto it = std::find_if(range.begin(), range.end(),
            [pix](PenColour const& pen) -> bool {return pen.toRGBX8() == pix;});
        if (it < range.end()-1) {
            *dest = (it+1)->toRGBX8();
        }
        ++dest;
    }
}

static void scan_rangedec_RGBX8(RGBX8* dest, int w, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for (x = 0; x < w; ++x) {
        RGBX8 pix = *dest;
        auto it = std::find_if(range.begin(), range.end(),
            [pix](PenColour const& pen) -> bool { return pen.toRGBX8() == pix;});

        if (it != range.end() && it > range.begin()) {
            *dest = (it-1)->toRGBX8();
        }
        ++dest;
    }
}

static void scan_rangeinc_RGBA8(RGBA8* dest, int w, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for (x = 0; x < w; ++x) {
        RGBA8 pix = *dest;
        auto it = std::find_if(range.begin(), range.end(),
            [pix](PenColour const& pen) -> bool { return pen.toRGBA8() == pix;});
        if (it < range.end()-1) {
            *dest = (it+1)->toRGBA8();
        }
        ++dest;
    }
}

static void scan_rangedec_RGBA8(RGBA8* dest, int w, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for (x = 0; x < w; ++x) {
        RGBA8 pix = *dest;
        auto it = std::find_if(range.begin(), range.end(),
            [pix](PenColour const& pen) -> bool { return pen.toRGBA8() == pix;});

        if (it != range.end() && it > range.begin()) {
            *dest = (it-1)->toRGBA8();
        }
        ++dest;
    }
}


void DrawRectRangeShift(Img& destimg, Box& rect, std::vector<PenColour> const& range, int direction)
{
    if (range.empty()) {
        rect.w = 0;
        rect.h = 0;
        return;
    }

    rect.ClipAgainst(destimg.Bounds());

    const int w = rect.w;
    const int x0 = rect.x;
    const int y0 = rect.y;
    int y;
    for (y = 0; y < rect.h; ++y)
    {
        if (direction > 0) {
            // Increment along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangeinc_I8(destimg.Ptr_I8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBX8:
                    scan_rangeinc_RGBX8(destimg.Ptr_RGBX8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBA8:
                    scan_rangeinc_RGBA8(destimg.Ptr_RGBA8(x0, y0 + y), w, range);
                    break;
                default:
                    assert(false);
                    break;
            }
        } else {
            // Decrement along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangedec_I8(destimg.Ptr_I8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBX8:
                    scan_rangedec_RGBX8(destimg.Ptr_RGBX8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBA8:
                    scan_rangedec_RGBA8(destimg.Ptr_RGBA8(x0, y0 + y), w, range);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
}

