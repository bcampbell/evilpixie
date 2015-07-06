

#ifndef Exception_H
#define Exception_H

// Exception class

class Exception
{
public:
	Exception( const char* fmt, ... );
	const char* what() const;
protected:
	enum { MAXLEN=256 };
	char m_Message[ MAXLEN ];
};


inline const char* Exception::what() const
{
	return m_Message;
}

#endif

