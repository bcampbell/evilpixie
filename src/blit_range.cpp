#include "blit_range.h"
#include "blit.h"

#include "box.h"
#include "img.h"

#include <algorithm>

//----
// range inc/dec, using a src img as key.

static void scan_rangeinc_keyed_I8_I8(I8 const* src, I8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
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


static void scan_rangedec_keyed_I8_I8(I8 const* src, I8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
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


static void scan_rangeinc_keyed_I8_RGBX8(I8 const* src, RGBX8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
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


static void scan_rangedec_keyed_I8_RGBX8(I8 const* src, RGBX8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
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


static void scan_rangeinc_keyed_I8_RGBA8(I8 const* src, RGBA8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
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


static void scan_rangedec_keyed_I8_RGBA8(I8 const* src, RGBA8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
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


static void blit_rangeshift_keyed_I8(Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentPen,
    std::vector<PenColour> const& range,
    int direction)
{
    assert(srcimg.Fmt() == FMT_I8);
    assert(transparentPen.IdxValid());
    assert(!range.empty());

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
                    scan_rangeinc_keyed_I8_I8(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangeinc_keyed_I8_RGBX8(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangeinc_keyed_I8_RGBA8(src, destimg.Ptr_RGBA8(x0, y0 + y),
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
                    scan_rangedec_keyed_I8_I8(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangedec_keyed_I8_RGBX8(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangedec_keyed_I8_RGBA8(src, destimg.Ptr_RGBA8(x0, y0 + y),
                        w, transparentPen.idx(), range);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
}


// RGBX8 -> ...

static void scan_rangeinc_keyed_RGBX8_I8(RGBX8 const* src, I8* dest, int w, RGBX8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBX8 c = *src++;
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


static void scan_rangedec_keyed_RGBX8_I8(RGBX8 const* src, I8* dest, int w, RGBX8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBX8 c = *src++;
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


static void scan_rangeinc_keyed_RGBX8_RGBX8(RGBX8 const* src, RGBX8* dest, int w, RGBX8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBX8 c = *src++;
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


static void scan_rangedec_keyed_RGBX8_RGBX8(RGBX8 const* src, RGBX8* dest, int w, RGBX8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBX8 c = *src++;
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


static void scan_rangeinc_keyed_RGBX8_RGBA8(RGBX8 const* src, RGBA8* dest, int w, RGBX8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBX8 c = *src++;
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


static void scan_rangedec_keyed_RGBX8_RGBA8(RGBX8 const* src, RGBA8* dest, int w, RGBX8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBX8 c = *src++;
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


static void blit_rangeshift_keyed_RGBX8(Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentPen,
    std::vector<PenColour> const& range,
    int direction)
{
    assert(srcimg.Fmt() == FMT_RGBX8);
    assert(!range.empty());

    Box srcclipped(srcbox);
    clip_blit(srcimg.Bounds(), srcclipped, destimg.Bounds(), destbox);

    const int w = destbox.w;
    int y;
    const int x0 = destbox.x;
    const int y0 = destbox.y;
    for (y = 0; y < destbox.h; ++y)
    {
        RGBX8 const* src = srcimg.PtrConst_RGBX8(srcclipped.x + 0, srcclipped.y + y);
        if (direction > 0) {
            // Increment along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangeinc_keyed_RGBX8_I8(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.toRGBX8(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangeinc_keyed_RGBX8_RGBX8(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.toRGBX8(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangeinc_keyed_RGBX8_RGBA8(src, destimg.Ptr_RGBA8(x0, y0 + y),
                        w, transparentPen.toRGBX8(), range);
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
                    scan_rangedec_keyed_RGBX8_I8(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.toRGBX8(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangedec_keyed_RGBX8_RGBX8(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.toRGBX8(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangedec_keyed_RGBX8_RGBA8(src, destimg.Ptr_RGBA8(x0, y0 + y),
                        w, transparentPen.toRGBX8(), range);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
}


// RGBA8 -> ...

static void scan_rangeinc_keyed_RGBA8_I8(RGBA8 const* src, I8* dest, int w, RGBA8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBA8 c = *src++;
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


static void scan_rangedec_keyed_RGBA8_I8(RGBA8 const* src, I8* dest, int w, RGBA8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBA8 c = *src++;
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


static void scan_rangeinc_keyed_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, RGBA8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBA8 c = *src++;
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


static void scan_rangedec_keyed_RGBA8_RGBX8(RGBA8 const* src, RGBX8* dest, int w, RGBA8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBA8 c = *src++;
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


static void scan_rangeinc_keyed_RGBA8_RGBA8(RGBA8 const* src, RGBA8* dest, int w, RGBA8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBA8 c = *src++;
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


static void scan_rangedec_keyed_RGBA8_RGBA8(RGBA8 const* src, RGBA8* dest, int w, RGBA8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x ) {
        RGBA8 c = *src++;
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

static void blit_rangeshift_keyed_RGBA8(Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentPen,
    std::vector<PenColour> const& range,
    int direction)
{
    assert(srcimg.Fmt() == FMT_RGBA8);
    assert(!range.empty());

    Box srcclipped(srcbox);
    clip_blit(srcimg.Bounds(), srcclipped, destimg.Bounds(), destbox);

    const int w = destbox.w;
    int y;
    const int x0 = destbox.x;
    const int y0 = destbox.y;
    for (y = 0; y < destbox.h; ++y)
    {
        RGBA8 const* src = srcimg.PtrConst_RGBA8(srcclipped.x + 0, srcclipped.y + y);
        if (direction > 0) {
            // Increment along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangeinc_keyed_RGBA8_I8(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.toRGBA8(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangeinc_keyed_RGBA8_RGBX8(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.toRGBA8(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangeinc_keyed_RGBA8_RGBA8(src, destimg.Ptr_RGBA8(x0, y0 + y),
                        w, transparentPen.toRGBA8(), range);
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
                    scan_rangedec_keyed_RGBA8_I8(src, destimg.Ptr_I8(x0, y0 + y),
                        w, transparentPen.toRGBA8(), range);
                    break;
                case FMT_RGBX8:
                    scan_rangedec_keyed_RGBA8_RGBX8(src, destimg.Ptr_RGBX8(x0, y0 + y),
                        w, transparentPen.toRGBA8(), range);
                    break;
                case FMT_RGBA8:
                    scan_rangedec_keyed_RGBA8_RGBA8(src, destimg.Ptr_RGBA8(x0, y0 + y),
                        w, transparentPen.toRGBA8(), range);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
}


// Uses the srcimg as a mask, incrementing or decrementing pixels on destimg
// up or down the the range.
void BlitRangeShiftKeyed(Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentPen,
    std::vector<PenColour> const& range,
    int direction)
{
    if (range.empty()) {
        destbox.w = 0;
        destbox.h = 0;
        return;
    }
    switch(srcimg.Fmt()) {
        case FMT_I8:
            blit_rangeshift_keyed_I8(srcimg, srcbox, destimg, destbox, transparentPen, range, direction);
            break;
        case FMT_RGBX8:
            blit_rangeshift_keyed_RGBX8(srcimg, srcbox, destimg, destbox, transparentPen, range, direction);
            break;
        case FMT_RGBA8:
            blit_rangeshift_keyed_RGBA8(srcimg, srcbox, destimg, destbox, transparentPen, range, direction);
            break;
    }
}


//-------
// Range inc/dec for solid regions (no keying)


static void scan_rangeinc_keyed_I8(I8* dest, int w, std::vector<PenColour> const& range)
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

static void scan_rangedec_keyed_I8(I8* dest, int w, std::vector<PenColour> const& range)
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

static void scan_rangeinc_keyed_RGBX8(RGBX8* dest, int w, std::vector<PenColour> const& range)
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

static void scan_rangedec_keyed_RGBX8(RGBX8* dest, int w, std::vector<PenColour> const& range)
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

static void scan_rangeinc_keyed_RGBA8(RGBA8* dest, int w, std::vector<PenColour> const& range)
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

static void scan_rangedec_keyed_RGBA8(RGBA8* dest, int w, std::vector<PenColour> const& range)
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
                    scan_rangeinc_keyed_I8(destimg.Ptr_I8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBX8:
                    scan_rangeinc_keyed_RGBX8(destimg.Ptr_RGBX8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBA8:
                    scan_rangeinc_keyed_RGBA8(destimg.Ptr_RGBA8(x0, y0 + y), w, range);
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
                    scan_rangedec_keyed_I8(destimg.Ptr_I8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBX8:
                    scan_rangedec_keyed_RGBX8(destimg.Ptr_RGBX8(x0, y0 + y), w, range);
                    break;
                case FMT_RGBA8:
                    scan_rangedec_keyed_RGBA8(destimg.Ptr_RGBA8(x0, y0 + y), w, range);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
}

