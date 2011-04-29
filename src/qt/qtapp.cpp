#include "qtapp.h"

#include "../editor.h"
#include "../wobbly.h"
#include "editorwindow.h"
#include <cstdio>
#include <QApplication>
#include <QMessageBox>



QTApp::QTApp()
{
}

QTApp::~QTApp()
{
}


int QTApp::Run( int argc, char* argv[] )
{
    QApplication app(argc, argv);

    QStringList args = app.arguments();

    int i;
    int cnt = 0;
    for(i=1;i<args.size();++i)
    {
        try
        {
            std::string filename( args.at(i).toStdString() );
            Project* p = new Project(filename.c_str());
            EditorWindow* fenster = new EditorWindow(p);
            fenster->show();
            ++cnt;
        }
        catch( Wobbly const& e )
        {
            //GUIShowError( e.what() );
            QMessageBox::warning( 0, "Error", e.what() );
        }
    }
    if(!cnt)
    {
        EditorWindow* fenster = new EditorWindow(new Project());
        fenster->show();
    }

    return app.exec();
}


