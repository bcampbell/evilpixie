


#include "exception.h"
#include <cstdio>
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



