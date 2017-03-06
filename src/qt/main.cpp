#include "qtapp.h"
#include "../anim.h"
#include "../exception.h"

#include <cstdio>

int main(int argc, char *argv[])
{

    try {
        QTApp app;


        int ret = app.Run( argc, argv );
        return ret;
    }
    catch (Exception& e) {
        fprintf(stderr,"ERROR: %s\n", e.what());
        return 1;
    }
}

