#ifndef BOX_H
#define BOX_H

#include "point.h"

struct Box
{
    Box( int xo, int yo, int width, int height ) : x(xo),y(yo),w(width),h(height)
        {}
    Box( Point const& topleft, int width, int height ) : x(topleft.x),y(topleft.y),w(width),h(height)
        {}
    Box()
        {}
    // construct from any two points (result will contain p1 and p2)
    Box( Point const& p1, Point const& p2 );

    int x,y,w,h;

    bool Empty() const { return w==0 || h==0; }

    int Left() const { return x; }
    int Right() const { return x+w-1; }
    int Top() const { return y; }
    int Bottom() const { return y+h-1; }
    int W() const { return w; }
    int H() const { return h; }

    Point TopLeft() const { return Point( x,y ); }

    Box& operator+=( Point const& delta )
        { x+=delta.x; y+=delta.y; return *this; }

    void SetEmpty();
    void ClipAgainst( Box const& clipper );
    void Expand( int amount );
    void Merge( Box const& other );
    bool Contains( Box const& other ) const;
    bool Contains( Point const& pt ) const;
};


inline bool Box::Contains( Point const& pt ) const
{
    return pt.x>=Left() && pt.x<Right() &&
        pt.y>=Top() && pt.y<Bottom();
}


#endif // BOX_H


