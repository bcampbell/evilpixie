

#ifndef WOBBLY_H
#define WOBBLY_H

// Exception class

class Wobbly
{
public:
	Wobbly( const char* fmt, ... );
	const char* what() const;
protected:
	enum { MAXLEN=256 };
	char m_Message[ MAXLEN ];
};


inline const char* Wobbly::what() const
{
	return m_Message;
}

#endif

