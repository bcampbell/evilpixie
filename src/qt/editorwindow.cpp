#include "../global.h"
#include "../project.h"
#include "../brush.h"
#include "../util.h"
#include "../exception.h"
#include "../cmd.h"
#include "../sheet.h"
#include "guistuff.h"
#include "editorwindow.h"
#include "editviewwidget.h"
#include "palettewidget.h"
#include "rgbpickerwidget.h"
#include "paletteeditor.h"
#include "newprojectdialog.h"
#include "resizeprojectdialog.h"
#include "spritesheetdialogs.h"
#include "miscwindows.h"

#include <cassert>
#ifdef WIN32
#include <unistd.h> // for getcwd()
#endif

#include <QtWidgets/QWidget>

#include <QPainter>

#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QTextEdit>

#include <QCloseEvent>
#include <QCursor>


#include <unistd.h>




void CurrentColourWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush( *g_GUIStuff.checkerboard);
    painter.drawRect( rect() );
    painter.setBrush( m_BG );
    painter.drawRect( rect() );

    int w = rect().width();
    int h = rect().height();

    QRect inner( rect().left()+w/4, rect().top()+h/4, w/2, h/2 );
    painter.setBrush( *g_GUIStuff.checkerboard /*m_BG*/ );
    painter.drawRect( inner );
    painter.setBrush( m_FG );
    painter.drawRect( inner );
}

QSize CurrentColourWidget::sizeHint () const
    { return QSize( 64,32 ); }

QSize CurrentColourWidget::minimumSizeHint () const
    { return QSize( 32,32 ); }

void CurrentColourWidget::mousePressEvent(QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
        emit left_clicked();
    if( event->button() == Qt::RightButton )
        emit right_clicked();
}


EditorWindow::EditorWindow( Project* proj, QWidget* parent ) :
    QWidget(parent),
    Editor(proj),
    m_ViewWidget(0),
    m_PaletteWidget(0),
    m_RGBPicker(0),
    m_CurrentColourWidget(0),
    m_ColourTab(0),
    m_BrushButtons(0),
    m_ToolButtons(0),
    m_PaletteEditor(0),
    m_AboutBox(0),
    m_HelpWindow(0),
    m_ActionUndo(0),
    m_ActionRedo(0),
    m_StatusViewInfo(0)
{
    // set up mouse cursors
    {
        int i;
        for( i=0;i<MOUSESTYLE_NUM; ++i )
            m_MouseCursors[i]=0;
        m_MouseCursors[MOUSESTYLE_CROSSHAIR] = new QCursor( QPixmap( JoinPath(g_App->DataPath(), "icons/cursor_crosshair.png").c_str() ),15,15 );
        m_MouseCursors[MOUSESTYLE_EYEDROPPER] = new QCursor( QPixmap( JoinPath(g_App->DataPath(), "icons/cursor_eyedropper.png").c_str() ),9,22 );
        assert( MOUSESTYLE_NUM==2 );
    }

    m_PaletteEditor = new PaletteEditor( *this, this );
    m_PaletteEditor->hide();

    resize( 700,500 );

    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setMargin(0);

    CreateActions();
    QMenuBar* menubar = CreateMenuBar();
    layout->addWidget( menubar,0,0,1,2 );

    m_ViewWidget = new EditViewWidget( *this );
    m_ViewWidget->setCursor( *m_MouseCursors[MOUSESTYLE_DEFAULT] );

    layout->addWidget( m_ViewWidget, 1,0,5,1 );


    QFrame* hr = new QFrame();  // this, "<hr>", 0 );
    hr->setFrameStyle( QFrame::Sunken + QFrame::HLine );
    hr->setFixedHeight( 8 );

//    QLabel* hr = new QLabel( "Hello" );


    layout->addLayout( CreateBrushButtons(), 1, 1 );
    layout->addWidget( hr, 2,1 );
    layout->addLayout( CreateToolButtons(), 3, 1 );

    /* set up palette widget */
    {
        m_CurrentColourWidget = new CurrentColourWidget( this );
        layout->addWidget( m_CurrentColourWidget, 4,1 );

        connect(m_CurrentColourWidget, SIGNAL(left_clicked()), this, SLOT( useeyedroppertool()));
        connect(m_CurrentColourWidget, SIGNAL(right_clicked()), this, SLOT( togglepaletteeditor()));


        m_ColourTab = new QTabWidget(this);
        m_ColourTab->setTabShape(QTabWidget::Triangular);
        m_ColourTab->setTabPosition(QTabWidget::South);
        m_ColourTab->setDocumentMode(true);
        layout->addWidget( m_ColourTab, 5,1 );

        if(Proj().Fmt() == FMT_RGBX8 || Proj().Fmt() == FMT_RGBA8 )
        {
            m_RGBPicker = new RGBPickerWidget();
            connect(m_RGBPicker, SIGNAL(pickedLeftButton(Colour)), this, SLOT( fgColourPickedRGB(Colour)));
            connect(m_RGBPicker, SIGNAL(pickedRightButton(Colour)), this, SLOT( bgColourPickedRGB(Colour)));
            m_ColourTab->addTab(m_RGBPicker, "RGB");
        }

        {
        m_PaletteWidget = new PaletteWidget(Proj().PaletteConst());
        connect(m_PaletteWidget, SIGNAL(pickedLeftButton(int)), this, SLOT( fgColourPicked(int)));
        connect(m_PaletteWidget, SIGNAL(pickedRightButton(int)), this, SLOT( bgColourPicked(int)));
        m_ColourTab->addTab(m_PaletteWidget, "Palette");
        }

    }

    /* status bar */
    {
        QStatusBar* statusbar = new QStatusBar();
        m_StatusViewInfo = new QLabel();
        m_StatusViewInfo->setFrameStyle(QFrame::Panel | QFrame::Sunken);

        statusbar->addWidget( m_StatusViewInfo );
//        statusbar->showMessage( "blah blah blah" );
        layout->addWidget( statusbar, 6,0,1,2 );
    }

    // some misc keyboard shortcuts
    {
        QShortcut* s;

        s = new QShortcut( QKeySequence( "]" ), this );
        connect(s, SIGNAL( activated()), this, SLOT( nextColour()));

        s = new QShortcut( QKeySequence( "[" ), this );
        connect(s, SIGNAL( activated()), this, SLOT( prevColour()));

    }

    layout->setRowStretch( 1,0 );   // brushes
    layout->setRowStretch( 3,1 );   // tools
    layout->setRowStretch( 4,0 );   // currentcolour
    layout->setRowStretch( 5,2 );   // palette
    layout->setColumnStretch( 0,1 );


    setLayout(layout);
    UseTool( TOOL_PENCIL );
    OnBrushChanged();
    OnPenChanged();

    RethinkWindowTitle();

    show();
}

EditorWindow::~EditorWindow()
{
    delete m_PaletteEditor;
    delete m_AboutBox;
    delete m_HelpWindow;
    delete m_ViewWidget;
}


QLayout* EditorWindow::CreateToolButtons()
{
    QGridLayout *tb = new QGridLayout();
    tb->setSpacing(0);

    QButtonGroup* grp = new QButtonGroup( tb );
    m_ToolButtons = grp;

    grp->setExclusive(true);

    struct {
        ToolType tool_id;
        const char* icon;
        QString tooltip;
        QKeySequence shortcut;
     } inf[] = {
        { TOOL_PENCIL, "penciltool.png", tr("Draw"), QKeySequence("d") },
        { TOOL_LINE, "linetool.png", tr("Line"), QKeySequence("l")},
        { TOOL_BRUSH_PICKUP, "brushpickuptool.png", tr("Pick up brush"), QKeySequence("b")},
        { TOOL_FLOODFILL, "filltool.png", tr("Flood Fill"), QKeySequence("f")},
        { TOOL_RECT, "recttool.png", tr("Rectangle"), QKeySequence("r")},
        { TOOL_FILLEDRECT, "filledrecttool.png", tr("Filled Rectangle"), QKeySequence("shift+r")},
        { TOOL_CIRCLE, "circletool.png", tr("Circle"), QKeySequence("c")},
        { TOOL_FILLEDCIRCLE, "filledcircletool.png", tr("Filled Circle"), QKeySequence("shift+c")},
        { TOOL_EYEDROPPER, "eyedroppertool.png", tr("Pick up colour"), QKeySequence(",")},
    };
    const int n = sizeof(inf) / sizeof(inf[0]);

    std::string iconDir( JoinPath(g_App->DataPath(), "icons"));

    int i;
    for( i=0; i<n; ++i )
    {
        QIcon icon;
        icon.addFile( JoinPath(iconDir, inf[i].icon).c_str(), QSize(), QIcon::Normal, QIcon::Off );

        QToolButton* b = new QToolButton();
        b->setIcon( icon );
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //b->setIconSize(QSize(32,32));
        b->setCheckable( true );
        if( i==0 )
            b->setChecked(true);
        b->setAutoRaise( true );
        b->setProperty( "tool_id", QVariant( (int)inf[i].tool_id ) );
        b->setToolTip( inf[i].tooltip );
        b->setShortcut( inf[i].shortcut );
        tb->addWidget( b, i/2, i%2 );
//        connect(b, SIGNAL(clicked()), this, SLOT(toolbuttonclicked()));
        grp->addButton( b );
    }
    connect(grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toolclicked( QAbstractButton* )));
    return tb;
}

QLayout* EditorWindow::CreateBrushButtons()
{
    QGridLayout *tb = new QGridLayout();
    tb->setSpacing(0);

    QButtonGroup* grp = new QButtonGroup( tb );
    m_BrushButtons = grp;

    grp->setExclusive(true);

    struct {
        int brush_id;
        const char* icon;
        QKeySequence shortcut;
     } inf[] = {
        { 0, "brush1.xpm",QKeySequence(".") },
        { 1, "brush2.xpm",QKeySequence() },
        { 2, "brush3.xpm",QKeySequence() },
        { 3, "brush4.xpm",QKeySequence() },
        { -1, "brushcustom.png",QKeySequence("shift+B") },
    };
    const int n = sizeof(inf) / sizeof(inf[0]);

    std::string iconDir( JoinPath(g_App->DataPath(), "icons"));

    int i;
    for( i=0; i<n; ++i )
    {
        QIcon icon;
        icon.addFile( JoinPath(iconDir, inf[i].icon).c_str(), QSize(), QIcon::Normal, QIcon::Off );

        QToolButton* b = new QToolButton();
        b->setIcon( icon );
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        b->setCheckable( true );
        if( i==0 )
            b->setChecked(true);
        b->setAutoRaise( true );
        b->setProperty( "brush_id", QVariant( inf[i].brush_id ) );
        b->setShortcut( inf[i].shortcut );
        tb->addWidget( b, i/2, i%2 );
//        connect(b, SIGNAL(clicked()), this, SLOT(toolbuttonclicked()));
        grp->addButton( b );
    }
    connect(grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(brushclicked( QAbstractButton* )));
    return tb;
}


void EditorWindow::OnPenChanged()
{
    // new fg or bg pen
    {
        Colour fg = FGPen().rgb();
        Colour bg = BGPen().rgb();
        m_CurrentColourWidget->setFGColour( QColor( fg.r, fg.g, fg.b, fg.a ) );
        m_CurrentColourWidget->setBGColour( QColor( bg.r, bg.g, bg.b, bg.a ) );

        //assert(Proj().GetAnimConst().Fmt()==FMT_I8);
        if(FGPen().IdxValid())
        {
            m_PaletteWidget->SetLeftSelected( FGPen().idx() );
        //    if( m_PaletteEditor )
        //        m_PaletteEditor->SetSelected(FGPen().idx());
        }
        if(BGPen().IdxValid())
            m_PaletteWidget->SetLeftSelected( BGPen().idx() );
    }
}

void EditorWindow::OnPaletteChanged( int n, Colour const& c )
{
    // make sure the gui reflects any palette changes
    m_PaletteWidget->SetColour(n,c);

    // one of the active pens changed?
    if( FGPen().IdxValid() && FGPen().idx()==n )
    {
        SetFGPen( PenColour(c,n) );
    }
    if( BGPen().IdxValid() && BGPen().idx()==n )
    {
        SetBGPen( PenColour(c,n) );
    }

}


void EditorWindow::OnPaletteReplaced()
{
    Palette const& pal = Proj().PaletteConst();
    m_PaletteWidget->SetPalette(pal);
}

void EditorWindow::OnModifiedFlagChanged( bool )
{
    RethinkWindowTitle();
}


void EditorWindow::OnToolChanged()
{
    QAbstractButton* b = FindButton( m_ToolButtons, "tool_id", QVariant( (int)CurrentToolType() ) );
    if( b )
        b->setChecked(true);
}

void EditorWindow::OnBrushChanged()
{
    // enable/disable custom brush
    QAbstractButton* customb = FindButton( m_BrushButtons, "brush_id", QVariant( -1 ) );
    if( g_App->CustomBrush() == 0 )
        customb->setEnabled( false );
    else
        customb->setEnabled( true );

    // show current brush
    QAbstractButton* b = FindButton( m_BrushButtons, "brush_id", QVariant( GetBrush() ) );
    if( b )
        b->setChecked(true);

    update_menu_states();
}



void EditorWindow::GUIShowError( const char* msg )
{
    QMessageBox::warning( this, "Error", msg );
}

void EditorWindow::toolclicked( QAbstractButton* b )
{

    QVariant q = b->property("tool_id" );
    if( !q.isValid() )
        return;
    ToolType t = (ToolType)q.toInt();
    UseTool( t );
}

void EditorWindow::brushclicked( QAbstractButton* b )
{
    QVariant q = b->property("brush_id" );
    if( !q.isValid() )
        return;
    SetBrush( q.toInt() );

}

void EditorWindow::fgColourPicked( int idx )
{
    Colour c = Proj().GetColour(idx);
    SetFGPen( PenColour(c,idx) );
}

void EditorWindow::bgColourPicked( int idx )
{
    Colour c = Proj().GetColour(idx);
    SetBGPen( PenColour(c,idx) );
}


void EditorWindow::fgColourPickedRGB( Colour c )
{
    // TODO: look up a palette index
    SetFGPen( PenColour(c) );
}

void EditorWindow::bgColourPickedRGB( Colour c )
{
    // TODO: look up a palette index
    SetBGPen(PenColour(c));
}

void EditorWindow::togglepaletteeditor()
{
    m_PaletteEditor->setVisible( !m_PaletteEditor->isVisible() );
}

void EditorWindow::useeyedroppertool()
{
    UseTool( TOOL_EYEDROPPER );
}

void EditorWindow::nextColour()
    { NextFGPen(); }

void EditorWindow::prevColour()
    { PrevFGPen(); }

void EditorWindow::update_menu_states()
{
    assert( m_ActionUndo && m_ActionRedo );
    m_ActionUndo->setEnabled( CanUndo() );
    m_ActionRedo->setEnabled( CanRedo() );
    m_ActionGridOnOff->setChecked( GridActive() );
    m_ActionUseBrushPalette->setEnabled( GetBrush() == -1 );

    int nframes= Proj().GetAnim().NumFrames();
    m_ActionZapFrame->setEnabled(nframes>1);
    m_ActionNextFrame->setEnabled(nframes>1);
    m_ActionPrevFrame->setEnabled(nframes>1);

    m_ActionToSpritesheet->setEnabled(nframes>1);
    m_ActionFromSpritesheet->setEnabled(nframes==1);
}

void EditorWindow::do_undo()
{
    if( CanUndo() )
        Undo();
}

void EditorWindow::do_redo()
{
    if( CanRedo() )
        Redo();
}

void EditorWindow::do_gridonoff( bool checked )
{
    ActivateGrid( checked );
    // TODO: should be some sort of Editor callback notification
}

void EditorWindow::do_drawmodeChanged( QAction* act )
{
    DrawMode::Mode newMode = (DrawMode::Mode)act->data().toInt();
    SetMode(DrawMode(newMode)); 
    // TODO: should be some sort of Editor callback notification
    RethinkWindowTitle();
}

void EditorWindow::do_resize()
{
    Box b = Proj().GetAnim().GetFrame(m_ViewWidget->Frame()).Bounds();
    ResizeProjectDialog dlg(this,QRect(b.x,b.y,b.w,b.h));
    if( dlg.exec() == QDialog::Accepted )
    {
        QRect area = dlg.GetArea();
        Cmd* c = new Cmd_Resize(Proj(),
            Box(0,0,area.width(),area.height()),
            0,
            Proj().GetAnim().NumFrames(), BGPen() );
        AddCmd(c);
    }
}

void EditorWindow::do_new()
{
    NewProjectDialog dlg(this);
    if( dlg.exec() == QDialog::Accepted )
    {
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

        if( Proj().Expendable() )
            this->close();
    }
}

void EditorWindow::do_usebrushpalette()
{
    if( GetBrush() != -1 )
        return; // std brush - do nothing


    Palette const& pal = CurrentBrush().GetPalette();
    Cmd* cmd = new Cmd_PaletteModify(Proj(), 0, pal.NColours, pal.Colours);
    AddCmd(cmd);
}

void EditorWindow::do_xflipbrush()
{
    if( GetBrush() != -1 )
        return; // std brush - do nothing
    HideToolCursor();
    CurrentBrush().XFlip();
    ShowToolCursor();
}

void EditorWindow::do_yflipbrush()
{
    if( GetBrush() != -1 )
        return; // std brush - do nothing
    HideToolCursor();
    CurrentBrush().YFlip();
    ShowToolCursor();
}


void EditorWindow::do_addframe()
{
    int frame = m_ViewWidget->Frame();
    Cmd* c = new Cmd_InsertFrames(Proj(), frame, 1);
    AddCmd(c);
}

void EditorWindow::do_zapframe()
{
    assert(Proj().GetAnim().NumFrames() > 1);
    int frame = m_ViewWidget->Frame();
    Cmd* c= new Cmd_DeleteFrames(Proj(), frame, frame+1);
    AddCmd(c);
}

void EditorWindow::do_prevframe()
{
    int n = m_ViewWidget->Frame()-1;
    if(n<0)
        n=Proj().NumFrames()-1;
    m_ViewWidget->SetFrame(n);
    RethinkWindowTitle();
}

void EditorWindow::do_nextframe()
{
    int n = m_ViewWidget->Frame()+1;
    if(n>=Proj().NumFrames())
        n=0;//Proj().NumFrames()-1;
    m_ViewWidget->SetFrame(n);
    RethinkWindowTitle();
}


void EditorWindow::do_tospritesheet()
{
    ToSpritesheetDialog dlg(this, &Proj());
    if( dlg.exec() == QDialog::Accepted )
    {
        Cmd* c= new Cmd_ToSpriteSheet(Proj(), dlg.NumAcross());
        AddCmd(c);
    }
}

void EditorWindow::do_fromspritesheet()
{
    FromSpritesheetDialog dlg(this, &Proj());
    if( dlg.exec() == QDialog::Accepted )
    {
        std::vector<Box> frames;
        SplitSpritesheet(Proj().ImgConst(0).Bounds(), dlg.getNWide(), dlg.getNHigh(), frames);

        // TODO: pass in frames
        Cmd* c= new Cmd_FromSpriteSheet(Proj(), dlg.getNWide(), frames.size());
        AddCmd(c);
    }
}


void EditorWindow::do_loadpalette()
{

    QString filename = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    "",
                    "GIMP Palette files (*.gpl)");
    if( filename.isNull() )
        return;

    try
    {
        Palette* p = Palette::Load(filename.toStdString().c_str());
        Proj().ReplacePalette(p);
    }
    catch( Exception const& e )
    {
        GUIShowError( e.what() );
    }
//    RethinkWindowTitle();
}


void EditorWindow::do_open()
{
//    if( !CheckZappingOK() )
//        return;
    QString loadfilters = "Image files (*.anim *.bmp *.gif *.iff *.ilbm *.lbm *.pbm *.pcx *.png);;Any files (*)";

    std::string startdir = Proj().Filename();
    if( startdir.empty() )
    {
        char cwd[512];
        getcwd( cwd, sizeof(cwd) );
        startdir = cwd;
    }
    else
    {
        startdir = DirName( startdir );
    }

    QString filename = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    startdir.c_str(),  loadfilters );
    if( filename.isNull() )
        return;

    try
    {

        Project* new_proj = new Project(filename.toStdString());
        EditorWindow* fenster = new EditorWindow(new_proj);
        fenster->show();
        fenster->activateWindow();
        fenster->raise();

        if( Proj().Expendable() )
            this->close();

    }
    catch( Exception const& e )
    {
        GUIShowError( e.what() );
    }
}


void EditorWindow::do_save()
{
    if( Proj().Filename().empty() )
    {
        do_saveas();
        return;
    }

    try
    {
        Proj().Save( Proj().Filename());
    }
    catch( Exception const& e )
    {
        GUIShowError( e.what() );
    }
    RethinkWindowTitle();
}




void EditorWindow::do_saveas()
{
    std::string startdir = Proj().Filename();
    if( startdir.empty() )
    {
        char cwd[512];
        getcwd( cwd, sizeof(cwd) );
        startdir = cwd;
    }
    else
    {
        startdir = DirName( startdir );
    }

    QString savefilters;
  
    if (Proj().NumFrames()>1) { 
        savefilters = "Animated GIF (*.gif);;Any files (*)";
    } else {
        savefilters = "Image files (*.gif *.png);;Any files (*)";
    }

    QString filename = QFileDialog::getSaveFileName(
                    this,
                    "Save image as",
                    startdir.c_str(),
                    savefilters);

    if( filename.isNull() )
        return;

    try
    {
        Proj().Save( filename.toStdString() );
    }
    catch( Exception const& e )
    {
        GUIShowError( e.what() );
    }

    RethinkWindowTitle();
}


void EditorWindow::showHelp()
{
    if(!m_HelpWindow)
        m_HelpWindow = new HelpWindow();

    if(m_HelpWindow->isVisible())
        m_HelpWindow->raise();
    else
        m_HelpWindow->show();
#if 0

   QTextEdit* help=new QTextEdit();
//   help->setWindowFlag(Qt::Dialog); //or Qt::Tool, Qt::Dialog if you like
   help->setWindowFlags(Qt::Dialog);
   help->setReadOnly(true);
   help->append("<h1>Help</h1>Welcom to my help.<br/> Hope you like it.");
   help->show();
#endif
}

void EditorWindow::showAbout()
{
    if(!m_AboutBox)
        m_AboutBox = new AboutBox(this);

    if(m_AboutBox->isVisible())
        m_AboutBox->raise();
    else
        m_AboutBox->show();
}

// helper - find a button in a group with a matching property
QAbstractButton* EditorWindow::FindButton( QButtonGroup* grp, const char* propname, QVariant const& val )
{
    int i;
    for( i=0; i<grp->buttons().size(); ++i)
    {
        QAbstractButton* b = grp->buttons().at(i);
        if( b->property( propname ) == val )
            return b;
    }
    return 0;
}

QMenuBar* EditorWindow::CreateMenuBar()
{
    QAction* a=0;
    QMenuBar* menubar = new QMenuBar();
    {
        QMenu* m = menubar->addMenu("&File");

        a = m->addAction( "&New...", this, SLOT( do_new()), QKeySequence::New );
        a = m->addAction( "&Open...", this, SLOT( do_open()), QKeySequence::Open );
        m->addSeparator();
        a = m->addAction( "&Save", this, SLOT( do_save()), QKeySequence::Save );
        a = m->addAction( "Save &As", this, SLOT( do_saveas()), QKeySequence("CTRL+A") );
        m->addSeparator();
        a = m->addAction( "&Close", this, SLOT( close()), QKeySequence::Close );

        connect(m, SIGNAL(aboutToShow()), this, SLOT( update_menu_states()));
    }


    // EDIT menu
    {
        QMenu* m = menubar->addMenu("&Edit");
        m_ActionUndo = a = m->addAction( "&Undo", this, SLOT(do_undo()), QKeySequence::Undo );
        m_ActionRedo = a = m->addAction( "&Redo", this, SLOT(do_redo()), QKeySequence::Redo );
        m->addSeparator();

        a = m->addAction( "Edit palette...", this, SLOT(togglepaletteeditor()));
        m_ActionUseBrushPalette = a = m->addAction( "Use Brush Palette", this, SLOT(do_usebrushpalette()) );
        a = m->addAction( "&Load Palette...", this, SLOT( do_loadpalette()) );
        m->addSeparator();
        m->addAction( "X-Flip Brush", this, SLOT(do_xflipbrush()),QKeySequence("x") );
        m->addAction( "Y-Flip Brush", this, SLOT(do_yflipbrush()),QKeySequence("y") );
        m->addSeparator();


        m_ActionGridOnOff = a = m->addAction( "&Grid On?", this, SLOT( do_gridonoff(bool)), QKeySequence("g") );
        a->setCheckable(true);

        a = m->addAction( "Resize...", this, SLOT(do_resize()));
        connect(m, SIGNAL(aboutToShow()), this, SLOT( update_menu_states()));
    }

    // ANIM menu
    {
        QMenu* m = menubar->addMenu("&Anim");
        a = m->addAction( "&Add Frame", this, SLOT( do_addframe()) );
        m_ActionZapFrame = m->addAction( "&Delete Frame", this, SLOT( do_zapframe()) );
        m->addSeparator();
        m_ActionPrevFrame = m->addAction( "Previous Frame", this, SLOT( do_prevframe()),QKeySequence("1"));
        m_ActionNextFrame = m->addAction( "Next Frame", this, SLOT( do_nextframe()),QKeySequence("2"));
        m->addSeparator();
        m->addAction( m_ActionToSpritesheet);
        m->addAction( m_ActionFromSpritesheet);
    }

    // DrawMode menu
    {
        QMenu* m = menubar->addMenu("&DrawMode");
        m->addAction( m_ActionDrawmodeNormal);
        m->addAction( m_ActionDrawmodeColour);
        // TODO: REPLACE mode not yet working
        //m->addAction( m_ActionDrawmodeReplace);
        connect(m, SIGNAL(aboutToShow()), this, SLOT( update_menu_states()));
    }

    // Help menu
    {
        QMenu* m = menubar->addMenu("&Help");
        a = m->addAction( "Help...", this, SLOT(showHelp()));
        a = m->addAction( "About Evilpixie...", this, SLOT(showAbout()));
        connect(m, SIGNAL(aboutToShow()), this, SLOT( update_menu_states()));
    }

    return menubar;
}


// TODO: move most of the actions out of CreateMenuBar and put them here instead...
void EditorWindow::CreateActions()
{
    QAction* a;
  
    m_ActionToSpritesheet = new QAction("Anim to spritesheet...", this);
    m_ActionFromSpritesheet = new QAction("Spritesheet to Anim...", this);

    connect(m_ActionToSpritesheet, SIGNAL(triggered()), this, SLOT(do_tospritesheet()));
    connect(m_ActionFromSpritesheet, SIGNAL(triggered()), this, SLOT(do_fromspritesheet()));

    // draw modes 
    a = m_ActionDrawmodeNormal = new QAction("&Normal", this);
    a->setData(DrawMode::DM_NORMAL);
    a->setCheckable(true);
    //a->setShortcuts(QKeySequence::New);
    a->setStatusTip("Normal drawing");

    a = m_ActionDrawmodeColour = new QAction("&Colour", this);
    a->setData(DrawMode::DM_COLOUR);
    a->setCheckable(true);
    //a->setShortcuts(QKeySequence::New);
    a->setStatusTip("Draw with current colour");

    a = m_ActionDrawmodeReplace = new QAction("&Replace", this);
    a->setData(DrawMode::DM_REPLACE);
    a->setCheckable(true);
    //a->setShortcuts(QKeySequence::New);
    a->setStatusTip("Ignore transparency");

    QActionGroup* grp = new QActionGroup(this);
    grp->addAction(m_ActionDrawmodeNormal);
    grp->addAction(m_ActionDrawmodeColour);
    grp->addAction(m_ActionDrawmodeReplace);
    m_ActionDrawmodeNormal->setChecked(true);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(do_drawmodeChanged(QAction*)));

}


void EditorWindow::RethinkWindowTitle()
{
    std::string f = Proj().Filename();
    if( f.empty() )
        f = "Untitled";

    int w = Proj().ImgConst(0).W();
    int h = Proj().ImgConst(0).H();

    char dim[32];
    sprintf( dim, " (%dx%d) frame %d/%d", w,h,m_ViewWidget->Frame()+1,Proj().NumFrames() );


    std::string title = "[*]";
    title += f;
    title += dim;
    switch (Mode().mode) {
        case DrawMode::DM_NORMAL: title += " NORMAL"; break;
        case DrawMode::DM_COLOUR: title += " COLOUR"; break;
        case DrawMode::DM_REPLACE: title += " REPLACE"; break;
        default: break;
    }
    title += " - EvilPixie";

    setWindowModified( Proj().ModifiedFlag() );
    setWindowTitle( QString::fromStdString( title ) );
}


void EditorWindow::UpdateMouseInfo( Point const& mousepos )
{
    char buf[64];
    sprintf( buf, "%d,%d", mousepos.x, mousepos.y );
    m_StatusViewInfo->setText(buf);
}

// if outstanding changes, ask user if they want to discard them.
// return true if yes (or if project isn't modified)
bool EditorWindow::CheckZappingOK()
{
    if( !Proj().ModifiedFlag() )
        return true;

    int ret = QMessageBox::warning(this, tr("EvilPixie"),
               tr("The image has been modified.\n"
                  "Do you want to discard your changes?"),
                QMessageBox::Discard | QMessageBox::Cancel,
                QMessageBox::Cancel );

    if( ret == QMessageBox::Discard )
        return true;
    else
        return false;
}



void EditorWindow::closeEvent(QCloseEvent *event)
{
    if( CheckZappingOK() )
    {
        m_PaletteEditor->hide();
        event->accept();
    }
    else
        event->ignore();
}

//virtual
void EditorWindow::SetMouseStyle( MouseStyle s )
{
    m_ViewWidget->setCursor( *m_MouseCursors[s] );
}


