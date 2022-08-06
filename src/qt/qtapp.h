#ifndef QTAPP_H
#define QTAPP_H

#include "../app.h"
#include <string>


class EditorWindow;

class QTApp : public App
{
public:
    QTApp();
    virtual ~QTApp();

    virtual int Run( int argc, char* argv[] );

    EditorWindow* NewProject();
    EditorWindow* LoadProject(std::string const& filename);
};

#endif // QTAPP_H
