#include "qtapp.h"

#include "../editor.h"
#include "editorwindow.h"
#include <cstdio>
#include <QApplication>



QTApp::QTApp()
{
}

QTApp::~QTApp()
{
}


int QTApp::Run( int argc, char* argv[] )
{
    QApplication app(argc, argv);

    EditorWindow fenster(new Project());
    fenster.show();
    return app.exec();
}


