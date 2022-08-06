#include "qtapp.h"
#include "guistuff.h"

#include "../editor.h"
#include "../exception.h"
#include "../file_load.h"
#include "../sheet.h"
#include "../util.h"

#include "editorwindow.h"
#include "newprojectdialog.h"
#include "spritesheetdialogs.h"

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
            // Create a default project & window.
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

EditorWindow* QTApp::NewProject()
{
    NewProjectDialog dlg(nullptr);
    if( dlg.exec() != QDialog::Accepted ) {
        return nullptr;
    }

    QSize sz = dlg.GetSize();
    Palette* pal = Palette::Load( JoinPath(g_App->DataPath(), "default.gpl").c_str());
    if( dlg.pixel_format == FMT_I8)
        pal->SetNumColours(dlg.num_colours);
    else
        pal->SetNumColours(256);
    Project* p = new Project( dlg.pixel_format, sz.width(), sz.height(), pal, dlg.num_frames );
//        printf("%d frames, %d colours\n",dlg.num_frames,dlg.num_colours);
    EditorWindow* fenster = new EditorWindow(p);
    fenster->show();
    fenster->raise();
    fenster->activateWindow();
    return fenster;
}


EditorWindow* QTApp::LoadProject(std::string const& filename)
{
    Layer* l = LoadLayer(filename);
    assert(!l->mFrames.empty());
    // TODO: check for hints of spritesheet, and prompt a conversion.
#if 0
    Img const& srcImg = *(l->mFrames[0]->mImg);
    FromSpritesheetDialog dlg(nullptr, srcImg);

    if (dlg.exec() != QDialog::Accepted) {
        return nullptr;
    }

    std::vector<Img*> frames;
    //printf("nwide: %d nhigh=%d\n",dlg.getNWide(), dlg.getNHigh());
    FramesFromSpriteSheet(srcImg, dlg.getNWide(), dlg.getNWide()*dlg.getNHigh(), frames);

    l->ZapFrames();
    for (Img* img : frames) {
        // TODO: duration!
        l->mFrames.push_back(new Frame(img,0));
    }
#endif

    Project* new_proj = new Project(l);
    EditorWindow* fenster = new EditorWindow(new_proj);
    fenster->show();
    fenster->activateWindow();
    fenster->raise();
    return fenster;
}



