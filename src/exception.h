#ifndef Exception_H
#define Exception_H

#include <string>

// Exception class

class Exception
{
public:
	Exception(const char* fmt, ...);
	Exception(std::string const& msg);
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

