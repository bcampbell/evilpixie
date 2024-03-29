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

    int XMin() const { return x; }
    int XMax() const { return x+w-1; }
    int YMin() const { return y; }
    int YMax() const { return y+h-1; }
    int W() const { return w; }
    int H() const { return h; }

    Point TopLeft() const {return Point(x, y); }

    Box& operator+=( Point const& delta )
        { x+=delta.x; y+=delta.y; return *this; }
    Box& operator-=( Point const& delta )
        { x-=delta.x; y-=delta.y; return *this; }

    void Translate( Point const& delta )
        { x+=delta.x; y+=delta.y; }
    void SetEmpty();
    void ClipAgainst( Box const& clipper );
    void Expand( int amount );
    void Merge( Box const& other );
    bool Contains( Box const& other ) const;
    bool Contains( Point const& pt ) const;


    bool operator==(Box const& other) const {
        return x == other.x && y == other.y && w == other.w && h == other.h;
    }
};


inline bool Box::Contains( Point const& pt ) const
{
    return pt.x>=XMin() && pt.x<=XMax() &&
        pt.y>=YMin() && pt.y<=YMax();
}


#endif // BOX_H


