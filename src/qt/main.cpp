#include "qtapp.h"
#include <IL/il.h>
#include "../anim.h"
#include "../wobbly.h"

int main(int argc, char *argv[])
{

    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION )
    {
        fprintf( stderr, "bad IL version\n" );
        return 1;
    }

    ilInit();
#if 0
    Anim a;
    try
    {
        a.Load("examples/favicon.gif");
    }
    catch( Wobbly e )
    {
        printf("ERROR: %s\n",e.what());
    }
    return 0;
#endif
    QTApp app;


    int ret = app.Run( argc, argv );
    return ret;
}

