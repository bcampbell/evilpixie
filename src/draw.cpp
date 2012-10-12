#include "draw.h"
#include "img.h"

#include <algorithm>    // for min,max
#include <vector>



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

