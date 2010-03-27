#include "qtapp.h"
#include <IL/il.h>

int main(int argc, char *argv[])
{
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION )
    {
        fprintf( stderr, "bad IL version\n" );
        return 1;
    }

    ilInit();

    QTApp app;


    int ret = app.Run( argc, argv );
    return ret;
}

