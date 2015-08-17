#include "qtapp.h"
#include "guistuff.h"

#include "../editor.h"
#include "../exception.h"

#include "editorwindow.h"
#include <cstdio>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>




QTApp::QTApp()
{
}

QTApp::~QTApp()
{
    freeGUIStuff();
}


int QTApp::Run( int argc, char* argv[] )
{
    QApplication app(argc, argv);


    initGUIStuff();

    QStringList args = app.arguments();

//    try
    {

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
            catch( Exception const& e )
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
    }
    /*
    catch( Exception const& e )
    {
        QMessageBox::warning( 0, "Error", e.what() );
    }
    */
    return app.exec();
}


