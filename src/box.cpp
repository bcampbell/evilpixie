#include "box.h"
#include <algorithm>    // for min,max

Box::Box( Point const& p1, Point const& p2 ) :
    x( std::min(p1.x, p2.x) ),
    y( std::min(p1.y, p2.y) ),
    w( std::abs(p2.x-p1.x)+1 ),
    h( std::abs(p2.y-p1.y)+1 )
{
}



void Box::SetEmpty()
{
    x=y=w=h=0;
}

void Box::Expand( int amount )
{
    x-=amount;
    y-=amount;
    w+=amount*2;
    h+=amount*2;
}

void Box::ClipAgainst( Box const& other )
{
	int left = ( x>other.x ) ? x : other.x;
	int right = ( x+w<other.x+other.w ) ? x+w : other.x+other.w;

	int top = ( y>other.y ) ? y : other.y;
	int bottom = ( y+h<other.y+other.h ) ? y+h : other.y+other.h;

	w = right -left;
	h = bottom - top;
    x = left;
    y = top;
	if( w<0 || h < 0 )
		x=y=w=h=0;
}


void Box::Merge( Box const& other )
{
    if( other.Empty() )
        return;
    if( Empty() )
    {
        *this = other;
        return;
    }

    int l = std::min(x, other.x );
    int t = std::min(y, other.y );
    int r = std::max( Right(), other.Right() );
    int b = std::max( Bottom(), other.Bottom() );

    x = l;
    y = t;
    w = (r-l)+1;
    h = (b-t)+1;
}

bool Box::Contains( Box const& other ) const
{
    return ( Left() <= other.Left() &&
        Right() >= other.Right() &&
        Top() <= other.Top() &&
        Bottom() >= other.Bottom() );
}

