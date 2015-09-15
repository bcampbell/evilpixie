#ifndef POINT_H
#define POINT_H

class Point
{
public:
	Point( int xpos, int ypos ) : x(xpos), y(ypos ) {}
	int x,y;

    bool operator==( Point const& other ) const
        { return x==other.x && y==other.y; }
    bool operator!=( Point const& other ) const
        { return x!=other.x || y!=other.y; }

	Point& operator+=( Point const& other )
		{ x+=other.x; y+=other.y; return *this; }
	Point& operator-=( Point const& other )
		{ x-=other.x; y-=other.y; return *this; }

	friend Point operator+( Point const& a, Point const& b );
	friend Point operator-( Point const& a, Point const& b );
	friend Point operator-( Point const& a );
};


inline Point operator+( Point const& a, Point const& b )
	{ return Point( a.x + b.x, a.y + b.y ); }

inline Point operator-( Point const& a, Point const& b )
	{ return Point( a.x - b.x, a.y - b.y ); }

inline Point operator-( Point const& a )
	{ return Point(-a.x, -a.y); }

#endif // POINT_H

