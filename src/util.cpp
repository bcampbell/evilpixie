#include "util.h"

#include <algorithm>
#include <cctype> // for std::tolower


// Given a path, returns just the file part.
// eg "/foo/bar/wibble.txt" will return "wibble.txt"
// note: results undefined if there is no file part (ie is fullpath ends with
// a trailing slash).
std::string BaseName( std::string const& fullpath )
{
    std::string::size_type pos = fullpath.find_last_of( "/\\:" );
    if( pos == std::string::npos )
        return fullpath;
    ++pos;
    if( pos > fullpath.length() )
        return "";
    return fullpath.substr( pos );
}

// returns the directory part of fullpath
// eg "/foo/bar/wibble.txt" will return "/foo/bar"
std::string DirName( std::string const& fullpath )
{
    std::string::size_type pos = fullpath.find_last_of( "/\\:" );
    if( pos == std::string::npos )
        return ".";
    return fullpath.substr( 0, pos );
}

// returns file extension, including '.'
// if no extension, returns empty string.
// "foo.png"            -> ".png"
// "foo.tar.gz"         -> ".tar.gz" (correct behaviour???)
// "foo."               -> "."
// "foo"                -> ""
// "foo.bar/bar.foo"    -> ".foo"
std::string ExtName( std::string const& fullpath )
{
    // start search at beginning of basename
    std::string::size_type filestart = fullpath.find_last_of( "/\\:" );
    if( filestart == std::string::npos )
        filestart = 0;  // no path separators

    std::string::size_type pos = fullpath.find( '.', filestart );
    if( pos == std::string::npos )
        return "";          // no extension
    else
        return fullpath.substr( pos );
}




std::string ToLower( std::string const& s )
{
    std::string l(s);
    std::transform(l.begin(), l.end(), l.begin(), (int(*)(int)) std::tolower);
    return l;
}


// Splits a line of text into whitespace-delimited parts.
// Quoted strings are handled
// Unix-style comments are handled - any text after a '#' is ignored.
//
// Resultant parts returned in args.
void SplitLine( std::string const& line,
	std::vector<std::string>& args )
{
	std::string::const_iterator it=line.begin();

	while( true ) 	//it != line.end() )
	{
		// skip whitespace
		while( it!=line.end() && isspace( *it ) )
			++it;

		if( it==line.end() )
			break;

		// comment?
		if( *it == '#' )
			break;		// skip rest of line

		// collate arg
		args.push_back("");
		std::string& a = args.back();

		if( *it == '\"' || *it == '\'' )
		{
			// quoted string
			char q = *it++;
			while( it!=line.end() && *it != q )
				a += *it++;

			if( it != line.end() )
				++it;	// skip final quote
		}
		else
		{
			// unquoted string
			while( it!=line.end() && !isspace(*it) )
				a += *it++;
		}
	}
}

