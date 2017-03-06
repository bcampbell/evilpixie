#import <Foundation/Foundation.h>

#include <stdio.h>

bool bundle_path(char* buf, size_t bufsize)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    bool result = false;
    NSBundle * bundle = [NSBundle mainBundle];
    //printf("bundle: %p\n",bundle);
    NSString * s = [bundle resourcePath];
    //printf("s: %p\n",s);
    bool success = ( [s getFileSystemRepresentation: buf maxLength: bufsize] == YES );
    //printf("buf: '%s'\n",buf);
    [pool drain];
    return success;
}

bool app_support_path( char * buf, size_t bufsize )
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init] ;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory ,NSUserDomainMask, YES);
    NSString *path = [paths objectAtIndex:0];
    bool success = ( [path getFileSystemRepresentation:buf maxLength: bufsize] == YES );
    [pool drain] ;
    return success;
}

