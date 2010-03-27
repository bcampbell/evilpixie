#ifndef QTAPP_H
#define QTAPP_H

#include "../app.h"


class QTApp : public App
{
public:
    QTApp();
    virtual ~QTApp();

    virtual int Run( int argc, char* argv[] );
};

#endif // QTAPP_H
