#include "exception.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#endif

Exception::Exception( const char* fmt, ... )
{
	va_list ap;
	va_start( ap,fmt);
	int n = vsnprintf( m_Message, MAXLEN, fmt, ap );
	va_end( ap );
	if(n==MAXLEN)
		m_Message[MAXLEN-1] = '\0';
}


Exception::Exception(std::string const& msg)
{
    strncpy(m_Message, msg.c_str(), MAXLEN-1);
	m_Message[MAXLEN-1] = '\0';
}


