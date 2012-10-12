#include "draw.h"
#include "img.h"
#include "palette.h"

#include <algorithm>    // for min,max
#include <vector>

static void FloodFill_I8( Img& img, Point const& start, I8 newcolour, Box& damage );
static void FloodFill_RGBX8( Img& img, Point const& start, RGBX8 newcolour, Box& damage );

//-----------------------
// BLITTING FUNCTIONS
//-----------------------


// clips a blit against the destination boundary.
// assumes srcbox is already valid.
// modifies srcbox and destbox appropriately.
static void clip_blit(
    Box const& srcbounds, Box& srcbox,
    Box const& destbounds, Box& destbox,
    int zoom=1 )
{
    // srcbox has blit dimensions
    destbox.w = srcbox.w*zoom;
    destbox.h = srcbox.h*zoom;

    // clip destbox, then adjust srcbox to take into account any
    // modifications
    int destx = destbox.x;
    int desty = destbox.y;
    destbox.ClipAgainst( destbounds );

    srcbox.x += (destbox.x - destx)/zoom;
    srcbox.y += (destbox.y - desty)/zoom;
    srcbox.w = destbox.w/zoom;
    srcbox.h = destbox.h/zoom;
}





void BlitTransparent(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentcolour )
{
    assert(srcimg.Fmt()==destimg.Fmt());

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
                I8* dest = destimg.Ptr_I8( destclipped.x+0, destclipped.y+y );

                int x;
                for( x=0; x<destclipped.w; ++x )
                {
                    I8 c = *src++;
                    if( c != transparentcolour.idx() )
                        *dest = c;
                    ++dest;
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );

                int x;
                for( x=0; x<destclipped.w; ++x )
                {
                    RGBX8 c = *src++;
                    if( c != transparentcolour.rgb() )
                        *dest = c;
                    ++dest;
                }
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}


// blit the src as a matte
void BlitMatte(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    PenColour const& transparentcolour,
    PenColour const& mattecolour )
{
    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );

    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        switch(destimg.Fmt())
        {
        case FMT_I8:
            {
                assert(mattecolour.IdxValid()); // can't blit rgb colour onto indexed image!

                I8* dest = destimg.Ptr_I8( destclipped.x+0, destclipped.y+y );
                if(srcimg.Fmt()==FMT_I8)
                {
                    I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        I8 c = *src++;
                        if( c != transparentcolour.idx() )
                            *dest = mattecolour.idx();
                        ++dest;
                    }
                }
                else if(srcimg.Fmt()==FMT_RGBX8)
                {
                    RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        RGBX8 c = *src++;
                        if( c != transparentcolour.rgb() )
                            *dest = mattecolour.idx();
                        ++dest;
                    }
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );
                if(srcimg.Fmt()==FMT_I8)
                {
                    I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        I8 c = *src++;
                        if( c != transparentcolour.idx() )
                            *dest = mattecolour.rgb();
                        ++dest;
                    }
                }
                else if(srcimg.Fmt()==FMT_RGBX8)
                {
                    RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                    int x;
                    for( x=0; x<destclipped.w; ++x )
                    {
                        RGBX8 c = *src++;
                        if( c != transparentcolour.rgb() )
                            *dest = mattecolour.rgb();
                        ++dest;
                    }
                }
            }
            break;
        default:
            assert(false);
            break;
        }
    }
    destbox = destclipped;
}



void BlitZoomTransparent(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    Palette const& palette,
    int zoom,
    PenColour const& transparentcolour)
{
    assert( destimg.Fmt()==FMT_RGBX8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( zoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, zoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        int x;
        RGBX8* dest = destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y );
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8 const* src = srcimg.PtrConst_I8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.idx())
                    {
                        RGBX8 c = palette.GetColour(*src);
                        *dest = c;
                    }
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    RGBX8 c=*src;
                    if( c != transparentcolour.rgb())
                        *dest = c;
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;

        }
    }
}


void BlitZoomMatte(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox,
    int zoom,
    PenColour const& transparentcolour,
    PenColour const& mattecolour )
{
    assert( destimg.Fmt()==FMT_RGBX8);
    assert( srcimg.Bounds().Contains( srcbox ) );
    assert( zoom >= 1 );

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped, zoom );

    int y;
    for( y=0; y<destclipped.H(); ++y )
    {
        int x;
        RGBX8* dest = destimg.Ptr_RGBX8( destclipped.XMin() + 0, destclipped.YMin() + y );
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8 const* src = srcimg.PtrConst_I8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.idx())
                        *dest = mattecolour.rgb();
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.XMin()+0, srcclipped.YMin()+y/zoom );
                int n=0;
                for( x=0; x<destclipped.W(); ++x )
                {
                    if( *src != transparentcolour.rgb())
                        *dest = mattecolour.rgb();
                    ++dest;
                    if( ++n >= zoom )
                    {
                        ++src;
                        n=0;
                    }
                }
            }
            break;

        }
    }
}


void Blit(
    Img const& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox)
{
    assert( srcimg.Fmt() == destimg.Fmt());

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );


    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8 const* src = srcimg.PtrConst_I8( srcclipped.x+0, srcclipped.y+y );
                I8* dest = destimg.Ptr_I8( destclipped.x+0, destclipped.y+y );
                std::copy( src,src+destclipped.w, dest);
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 const* src = srcimg.PtrConst_RGBX8( srcclipped.x+0, srcclipped.y+y );
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );
                std::copy( src,src+destclipped.w, dest);
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}


// TODO: src,dest names meaningless. Should be a,b or something neutral
void BlitSwap(
    Img& srcimg, Box const& srcbox,
    Img& destimg, Box& destbox)
{
    assert( srcimg.Fmt() == destimg.Fmt());

    Box destclipped( destbox );
    Box srcclipped( srcbox );
    clip_blit( srcimg.Bounds(), srcclipped, destimg.Bounds(), destclipped );


    int y;
    for( y=0; y<destclipped.h; ++y )
    {
        switch(srcimg.Fmt())
        {
        case FMT_I8:
            {
                I8* src = srcimg.Ptr_I8( srcclipped.x+0, srcclipped.y+y );
                I8* dest = destimg.Ptr_I8( destclipped.x+0, destclipped.y+y );
                std::swap_ranges( src,src+destclipped.w, dest);
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8* src = srcimg.Ptr_RGBX8( srcclipped.x+0, srcclipped.y+y );
                RGBX8* dest = destimg.Ptr_RGBX8( destclipped.x+0, destclipped.y+y );
                std::swap_ranges( src,src+destclipped.w, dest);
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    destbox = destclipped;
}



//--------------------
//


void FloodFill( Img& img, Point const& start, PenColour const& newcolour, Box& damage )
{
    if(img.Fmt()==FMT_I8)
        FloodFill_I8(img,start,newcolour.idx(),damage);
    else
        FloodFill_RGBX8(img,start,newcolour.rgb(),damage);
}


void FloodFill_I8( Img& img, Point const& start, I8 newcolour, Box& damage )
{
    assert(img.Fmt()==FMT_I8);

    damage.SetEmpty();
    I8 oldcolour = img.Get_I8(start);
    if( oldcolour == newcolour )
        return;

    std::vector< Point > q;
    q.push_back( start );
    while( !q.empty() )
    {
        Point pt = q.back();
        q.pop_back();
        if( img.Get_I8(pt) != oldcolour )
            continue;

        // scan left and right to find span
        int y = pt.y;
        int l = pt.x;
        while( l>0 && img.Get_I8( Point(l-1,y)) == oldcolour )
            --l;
        int r = pt.x;
        while( r<img.W()-1 && img.Get_I8( Point(r+1,y)) == oldcolour )
            ++r;

        // fill the span
        I8* dest = img.Ptr_I8( l,y );
        int x;
        for( x=l; x<=r; ++x )
            *dest++ = newcolour;
        // expand the damage box to include the affected span
        damage.Merge( Box( l,y, (r+1)-l,1) );
        // add pixels above the span to the queue 
        y = pt.y-1;
        if( y>=0 )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_I8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }

        // add pixels below the span to the queue
        y = pt.y+1;
        if( y<img.H() )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_I8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }
    }
}



void FloodFill_RGBX8( Img& img, Point const& start, RGBX8 newcolour, Box& damage )
{
    assert(img.Fmt()==FMT_RGBX8);

    damage.SetEmpty();
    RGBX8 oldcolour = img.Get_RGBX8(start);
    if( oldcolour == newcolour )
        return;

    std::vector< Point > q;
    q.push_back( start );
    while( !q.empty() )
    {
        Point pt = q.back();
        q.pop_back();
        if( img.Get_RGBX8(pt) != oldcolour )
            continue;

        // scan left and right to find span
        int y = pt.y;
        int l = pt.x;
        while( l>0 && img.Get_RGBX8( Point(l-1,y)) == oldcolour )
            --l;
        int r = pt.x;
        while( r<img.W()-1 && img.Get_RGBX8( Point(r+1,y)) == oldcolour )
            ++r;

        // fill the span
        RGBX8* dest = img.Ptr_RGBX8( l,y );
        int x;
        for( x=l; x<=r; ++x )
            *dest++ = newcolour;
        // expand the damage box to include the affected span
        damage.Merge( Box( l,y, (r+1)-l,1) );
        // add pixels above the span to the queue 
        y = pt.y-1;
        if( y>=0 )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_RGBX8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }

        // add pixels below the span to the queue
        y = pt.y+1;
        if( y<img.H() )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_RGBX8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }
    }
}






// Bresenham line
void WalkLine(int x0, int y0, int x1, int y1, void (*plot)(int x, int y, void* user ), void* userdata )
{
    int dy = y1-y0;
    int dx = x1-x0;
    int xinc;
    int yinc;

    if( dx<0 )
        { xinc=-1; dx=-dx; }
    else
        { xinc=1; }
    dx*=2;

    if( dy<0 )
        { dy=-dy; yinc=-1; }
    else
        { yinc = 1; }
    dy*=2;

    plot( x0, y0, userdata );
    if( dx>dy )
    {
        // step along x axis
        int f = dy-(dx/2);
        while (x0 != x1)
        {
            if( f>=0 )
            {
                y0 += yinc;
                f -= dx;
            }
            f += dy;
            x0 += xinc;
            plot( x0, y0, userdata );
        }
    }
    else
    {
        // step along y axis
        int f = dx-(dy/2);
        while( y0 != y1 )
        {
            if (f >= 0)
            {
                x0 += xinc;
                f -= dy;
            }
            f += dx;
            y0 += yinc;
            plot( x0, y0, userdata );
        }
    }
}


// Bresenham ellipse
//
// Adapted from:
// http://willperone.net/Code/ellipse.php
//
void WalkEllipse(int xc, int yc, int r1, int r2,
    void (*drawpixel)(int x, int y, void* user ), void* userdata )
{
	int x= 0, y= r2, 
		a2= r1*r1, b2= r2*r2, 
		S, T;

	S = a2*(1-2*r2) + 2*b2;
	T = b2 - 2*a2*(2*r2-1);
	drawpixel(xc-x, yc-y, userdata);
	drawpixel(xc+x, yc+y, userdata);
	drawpixel(xc-x, yc+y, userdata);
	drawpixel(xc+x, yc-y, userdata);	
	do {
		if (S < 0)
		{
			S += 2*b2*(2*x + 3);
			T += 4*b2*(x + 1);
			x++;
		} else 
		if (T < 0)
		{
			S += 2*b2*(2*x + 3) - 4*a2*(y - 1);
			T += 4*b2*(x + 1) - 2*a2*(2*y - 3);
			x++;
			y--;
		} else {
			S -= 4*a2*(y - 1);
			T -= 2*a2*(2*y - 3);
			y--;
		}
		drawpixel(xc-x, yc-y, userdata);
		drawpixel(xc+x, yc+y, userdata);
		drawpixel(xc-x, yc+y, userdata);
		drawpixel(xc+x, yc-y, userdata);
	} while (y > 0);
}

// Bresenham ellipse
void WalkFilledEllipse(int xc, int yc, int r1, int r2,
    void (*drawhline)(int x0, int x1, int y, void* user ), void* userdata )
{
	int x= 0, y= r2, 
		a2= r1*r1, b2= r2*r2, 
		S, T;

	S = a2*(1-2*r2) + 2*b2;
	T = b2 - 2*a2*(2*r2-1);
    drawhline( xc-x, xc+x, yc-y, userdata );
    drawhline( xc-x, xc+x, yc+y, userdata );
	do {
		if (S < 0)
		{
			S += 2*b2*(2*x + 3);
			T += 4*b2*(x + 1);
			x++;
		} else 
		if (T < 0)
		{
			S += 2*b2*(2*x + 3) - 4*a2*(y - 1);
			T += 4*b2*(x + 1) - 2*a2*(2*y - 3);
			x++;
			y--;
		} else {
			S -= 4*a2*(y - 1);
			T -= 2*a2*(2*y - 3);
			y--;
		}
        drawhline( xc-x, xc+x, yc-y, userdata );
        drawhline( xc-x, xc+x, yc+y, userdata );
	} while (y > 0);
}

