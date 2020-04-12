#ifndef POINT_H
#define POINT_H

class Point
{
public:
    Point() : x(0), y(0) {}
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
	Point& operator*=(float scale)
		{ x *= scale; y *= scale; return *this; }

	friend Point operator+( Point const& a, Point const& b );
	friend Point operator-( Point const& a, Point const& b );
	friend Point operator-( Point const& a );
	friend Point operator*(Point const& a, float scale);
};


inline Point operator+( Point const& a, Point const& b )
	{ return Point( a.x + b.x, a.y + b.y ); }

inline Point operator-( Point const& a, Point const& b )
	{ return Point( a.x - b.x, a.y - b.y ); }

inline Point operator-( Point const& a )
	{ return Point(-a.x, -a.y); }

inline Point operator*(Point const& a, float scale)
	{ return Point(a.x * scale, a.y * scale); }

#endif // POINT_H

