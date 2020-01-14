#include "img.h"
#include <cassert>

// src1 is the current line, src0 the one above, src2 the one below
// dest0 and dest1 are the output lines.
static void doLine(const I8* src0, const I8* src1, const I8* src2, I8* dest0, I8* dest1, int srcw) {
    // Using pixel naming convention from:
    // https://en.wikipedia.org/wiki/Pixel-art_scaling_algorithms#EPX/Scale2%C3%97/AdvMAME2%C3%97
    //   a     (src0)
    // c p b   (src1-1, src1, src1+1)
    //   d     (src2)
    assert(srcw > 0);

    I8 b = *src1++; // src leads by 1
    I8 p = b;   // special case: duplicate first pixel
    int i;
    for (i=0; i<srcw-1; ++i) {
        I8 a = *src0++;
        I8 c = p;
        p = b;
        b = *src1++;
        I8 d = *src2++;

        *dest0++ = (c==a && c!=d && a!=b) ? a : p;  // 1
        *dest0++ = (a==b && a!=c && b!=d) ? b : p;  // 2
        *dest1++ = (d==c && d!=b && c!=a) ? c : p;  // 3
        *dest1++ = (b==d && b!=a && d!=c) ? d : p;  // 4
    }

    // special case for last pixel (b is off image, so reuse last pixel)
    {
        I8 a = *src0++;
        I8 c = p;
        p = b;
        I8 d = *src2++;

        *dest0++ = (c==a && c!=d && a!=b) ? a : p;  // 1
        *dest0++ = (a==b && a!=c && b!=d) ? b : p;  // 2
        *dest1++ = (d==c && d!=b && c!=a) ? c : p;  // 3
        *dest1++ = (b==d && b!=a && d!=c) ? d : p;  // 4
    }
}


Img* DoScale2x(Img const& src) {
    assert(src.Fmt() == FMT_I8);
    assert(src.W() > 0 && src.H() > 0);
    Img* dest = new Img(FMT_I8, src.W()*2, src.H()*2);

    // special case for first line
    doLine(src.PtrConst_I8(0, 0),  // replicated
           src.PtrConst_I8(0, 0),
           src.PtrConst_I8(0, 1),
           dest->Ptr_I8(0, 0),
           dest->Ptr_I8(0, 1),
           src.W());
    int y;
    for (y=1; y<src.H()-1; ++y) {
        doLine(src.PtrConst_I8(0, y-1),
               src.PtrConst_I8(0, y),
               src.PtrConst_I8(0, y+1),
               dest->Ptr_I8(0, y*2),
               dest->Ptr_I8(0, (y*2)+1),
               src.W());
    }
    if(y<src.H()) {
        // special case for last line
        doLine(src.PtrConst_I8(0, y-1),
               src.PtrConst_I8(0, y),
               src.PtrConst_I8(0, y),   // replicated
               dest->Ptr_I8(0, y*2),
               dest->Ptr_I8(0, (y*2)+1),
               src.W());
    }
    return dest;
}


