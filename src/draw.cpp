#include "draw.h"
#include "img.h"
#include "palette.h"

#include <algorithm>    // for min,max
#include <vector>

static void FloodFill_I8( Img& img, Point const& start, I8 newcolour, Box& damage );
static void FloodFill_RGBX8( Img& img, Point const& start, RGBX8 newcolour, Box& damage );
static void FloodFill_RGBA8( Img& img, Point const& start, RGBA8 newcolour, Box& damage );



//--------------------
//


void FloodFill( Img& img, Point const& start, PenColour const& newcolour, Box& damage )
{
    switch(img.Fmt())
    {
        case FMT_I8:
            FloodFill_I8(img,start,newcolour.idx(),damage);
            break;
        case FMT_RGBX8:
            FloodFill_RGBX8(img,start,newcolour.rgb(),damage);
            break;
        case FMT_RGBA8:
            FloodFill_RGBA8(img,start,newcolour.rgb(),damage);
            break;
        default:
            assert(false);
            break;
    }
}


static void FloodFill_I8( Img& img, Point const& start, I8 newcolour, Box& damage )
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



static void FloodFill_RGBX8( Img& img, Point const& start, RGBX8 newcolour, Box& damage )
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

// TODO: should fill across differing alpha value?
static void FloodFill_RGBA8( Img& img, Point const& start, RGBA8 newcolour, Box& damage )
{
    assert(img.Fmt()==FMT_RGBA8);

    damage.SetEmpty();
    RGBA8 oldcolour = img.Get_RGBA8(start);
    if( oldcolour == newcolour )
        return;

    std::vector< Point > q;
    q.push_back( start );
    while( !q.empty() )
    {
        Point pt = q.back();
        q.pop_back();
        if( img.Get_RGBA8(pt) != oldcolour )
            continue;

        // scan left and right to find span
        int y = pt.y;
        int l = pt.x;
        while( l>0 && img.Get_RGBA8( Point(l-1,y)) == oldcolour )
            --l;
        int r = pt.x;
        while( r<img.W()-1 && img.Get_RGBA8( Point(r+1,y)) == oldcolour )
            ++r;

        // fill the span
        RGBA8* dest = img.Ptr_RGBA8( l,y );
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
                if( img.Get_RGBA8(Point(x,y)) == oldcolour )
                    q.push_back( Point(x,y) );
            }
        }

        // add pixels below the span to the queue
        y = pt.y+1;
        if( y<img.H() )
        {
            for(x=l; x<=r; ++x)
            {
                if( img.Get_RGBA8(Point(x,y)) == oldcolour )
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


void RectFill(Img& destimg, Box& destbox, PenColour const& pen )
{
    destbox.ClipAgainst(destimg.Bounds());
    
    int y;
    for( y=0; y<destbox.h; ++y )
    {
        switch(destimg.Fmt())
        {
        case FMT_I8:
            {
                I8 c = pen.idx();
                I8* dest = destimg.Ptr_I8( destbox.x+0, destbox.y+y );
                int x;
                for (x=0; x<destbox.w; ++x)
                    *dest++ = c;
            }
            break;
        case FMT_RGBX8:
            {
                RGBX8 c = pen.rgb();
                RGBX8* dest = destimg.Ptr_RGBX8( destbox.x+0, destbox.y+y );
                int x;
                for (x=0; x<destbox.w; ++x)
                    *dest++ = c;
            }
            break;
        case FMT_RGBA8:
            {
                RGBA8 c = pen.rgb();
                RGBA8* dest = destimg.Ptr_RGBA8( destbox.x+0, destbox.y+y );
                int x;
                for (x=0; x<destbox.w; ++x)
                    *dest++ = c;
            }
            break;
        default:
            assert(false);
            break;
        }
    }


}

