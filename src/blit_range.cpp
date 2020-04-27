#include "blit_range.h"
#include "blit.h"

#include "box.h"
#include "img.h"

#include <algorithm>

static void scan_rangeinc_I8_I8_keyed(I8 const* src, I8* dest, int w, I8 transparent, std::vector<PenColour> const& range)
{
    assert(!range.empty());
    int x;
    for( x=0; x<w; ++x )
    {
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
    for( x=0; x<w; ++x )
    {
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

    Box srcclipped(srcbox);
    clip_blit(srcimg.Bounds(), srcclipped, destimg.Bounds(), destbox);

    if (range.empty()) {
        return;
    }

    const int w = destbox.w;
    int y;
    for (y = 0; y < destbox.h; ++y)
    {
        I8 const* src = srcimg.PtrConst_I8(srcclipped.x + 0, srcclipped.y + y);
        I8* dest = destimg.Ptr_I8(destbox.x + 0,destbox.y + y);
        if (direction > 0) {
            // Increment along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangeinc_I8_I8_keyed(src, dest, w, transparentPen.idx(), range);
                    break;
                case FMT_RGBX8:
                case FMT_RGBA8:
                default:
                    assert(false);
                    break;
            }
        } else {
            // Decrement along range.
            switch(destimg.Fmt())
            {
                case FMT_I8:
                    scan_rangedec_I8_I8_keyed(src, dest, w, transparentPen.idx(), range);
                    break;
                case FMT_RGBX8:
                case FMT_RGBA8:
                default:
                    assert(false);
                    break;
            }
        }
    }
}


