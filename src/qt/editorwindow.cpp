#include "../global.h"
#include "../project.h"
#include "../brush.h"
#include "../scale2x.h"
#include "../util.h"
#include "../exception.h"
#include "../file_save.h"
#include "../file_type.h"
#include "../cmd.h"
#include "../cmd_changefmt.h"
#include "../cmd_remap.h"
#include "../sheet.h"
#include "../img_convert.h"
#include "guistuff.h"
#include "editorwindow.h"
#include "editviewwidget.h"
#include "griddialog.h"
#include "palettewidget.h"
#include "rangeswidget.h"
#include "rgbpickerwidget.h"
#include "paletteeditor.h"
#include "changefmtdialog.h"
#include "newprojectdialog.h"
#include "resizeprojectdialog.h"
#include "spritesheetdialogs.h"
#include "miscwindows.h"
#include "layerswidget.h"

#include <algorithm>
#include <memory>
#include <cassert>
#ifdef WIN32
#include <unistd.h> // for getcwd()
#endif

#include <QtWidgets/QWidget>

#include <QPainter>
#include <QDir>

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
#include <QtWidgets/QTextEdit>

#include <QCloseEvent>
#include <QCursor>



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
    m_MagView(nullptr),
    m_PaletteWidget(0),
    m_RGBPicker(0),
    m_CurrentColourWidget(0),
    m_ColourTab(0),
    m_BrushButtons(0),
    m_ToolButtons(0),
    m_PaletteEditor(nullptr),
    m_AboutBox(0),
    m_HelpWindow(0),
    m_ActionUndo(0),
    m_ActionRedo(0),
    m_StatusViewInfo(0)
{
    // focus upon the first layer
    Layer *firstLayer = FindLayer(proj->mRoot);
    assert(firstLayer); // TODO: support null focus layer?
    assert(!firstLayer->mFrames.empty());
    m_Focus = CalcPath(firstLayer);
    m_Time = 0;
    m_Frame = 0;

    // set up mouse cursors
    {
        int i;
        for( i=0;i<MOUSESTYLE_NUM; ++i )
            m_MouseCursors[i]=0;
        m_MouseCursors[MOUSESTYLE_CROSSHAIR] = new QCursor( QPixmap( JoinPath(g_App->DataPath(), "icons/cursor_crosshair.png").c_str() ),15,15 );
        m_MouseCursors[MOUSESTYLE_EYEDROPPER] = new QCursor( QPixmap( JoinPath(g_App->DataPath(), "icons/cursor_eyedropper.png").c_str() ),9,22 );
        assert( MOUSESTYLE_NUM==2 );
    }

    m_PaletteEditor = new PaletteEditor(this, *this, m_Focus, m_Frame);
    m_PaletteEditor->hide();

    resize( 700,500 );

    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setMargin(0);

    CreateActions();
    QMenuBar* menubar = CreateMenuBar();
    layout->addWidget(menubar, 0, 0, 1, 2);

    m_ViewWidget = new EditViewWidget(*this, m_Focus, m_Frame);
    m_ViewWidget->setCursor(*m_MouseCursors[MOUSESTYLE_DEFAULT]);

    m_MagView = nullptr;

    m_ViewSplitter = new QSplitter(parent);
    layout->addWidget(m_ViewSplitter, 1,0,5,1 );

    m_ViewSplitter->addWidget(m_ViewWidget);

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

        {
            m_PaletteWidget = new PaletteWidget(Proj().PaletteConst(m_Focus, m_Frame));
            connect(m_PaletteWidget, SIGNAL(pickedLeftButton(int)), this, SLOT( fgColourPicked(int)));
            connect(m_PaletteWidget, SIGNAL(pickedRightButton(int)), this, SLOT( bgColourPicked(int)));
            m_ColourTab->addTab(m_PaletteWidget, "Palette");
        }

        {
            m_RangesWidget = new RangesWidget(this, *this, m_Focus, m_Frame);
            connect(m_RangesWidget, SIGNAL(pickedFGPen(PenColour)), this, SLOT( fgPenPicked(PenColour const&)));
            connect(m_RangesWidget, SIGNAL(pickedBGPen(PenColour)), this, SLOT( bgPenPicked(PenColour const&)));
            connect(m_RangesWidget, SIGNAL(pickedRange()), this, SLOT(rangePicked()));
            m_ColourTab->addTab(m_RangesWidget, "Range");
        }
#if 0
        {
            m_RGBPicker = new RGBPickerWidget();
            connect(m_RGBPicker, SIGNAL(pickedLeftButton(Colour)), this, SLOT( fgColourPickedRGB(Colour)));
            connect(m_RGBPicker, SIGNAL(pickedRightButton(Colour)), this, SLOT( bgColourPickedRGB(Colour)));
            m_ColourTab->addTab(m_RGBPicker, "RGB");
        }
#endif
    }

    //LayersWidget* layersWidget = new LayersWidget(&Proj());
    //layout->addWidget( layersWidget, 6,1 );


    /* status bar */
    {
        QStatusBar* statusbar = new QStatusBar();
        m_StatusViewInfo = new QLabel();
        m_StatusViewInfo->setFrameStyle(QFrame::Panel | QFrame::Sunken);

        statusbar->addWidget( m_StatusViewInfo );
//        statusbar->showMessage( "blah blah blah" );
        layout->addWidget( statusbar, 7,0,1,2 );
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


    setAcceptDrops(true);
    show();
}

EditorWindow::~EditorWindow()
{
    delete m_PaletteEditor;
    delete m_AboutBox;
    delete m_HelpWindow;
    delete m_ViewWidget;
    delete m_MagView;
}


QLayout* EditorWindow::CreateToolButtons()
{
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(0);

    QButtonGroup* grp = new QButtonGroup(grid);
    m_ToolButtons = grp;

    grp->setExclusive(true);

    struct {
        int x,y;
        ToolType tool_id;
        const char* icon;
        QString tooltip;
        const char* shortcut;
     } inf[] = {
        {0, 0, TOOL_PENCIL, "penciltool.png", tr("Draw"), "d"},
        {1, 0, TOOL_LINE, "linetool.png", tr("Line"), "l"},
        {0, 1, TOOL_BRUSH_PICKUP, "brushpickuptool.png", tr("Pick up brush"), "b"},
        {1, 1, TOOL_FLOODFILL, "filltool.png", tr("Flood Fill"), "f"},
        {0, 2, TOOL_RECT, "recttool.png", tr("Rectangle"), "r"},
        {1, 2, TOOL_FILLEDRECT, "filledrecttool.png", tr("Filled Rectangle"), "shift+r"},
        {0, 3, TOOL_CIRCLE, "circletool.png", tr("Circle"), ("c")},
        {1, 3, TOOL_FILLEDCIRCLE, "filledcircletool.png", tr("Filled Circle"), "shift+c"},
        {1, 4, TOOL_EYEDROPPER, "eyedroppertool.png", tr("Pick up colour"), ","},
    };
    const int n = sizeof(inf) / sizeof(inf[0]);

    std::string iconDir(JoinPath(g_App->DataPath(), "icons"));

    int i;
    for( i=0; i<n; ++i )
    {
        QIcon icon;
        icon.addFile(JoinPath(iconDir, inf[i].icon).c_str(), QSize(), QIcon::Normal, QIcon::Off);

        QToolButton* b = new QToolButton();
        b->setIcon( icon );
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        b->setIconSize(QSize(64,64));
        b->setCheckable(true);
        if( i==0 )
            b->setChecked(true);
        b->setAutoRaise(true);
        b->setProperty("tool_id", QVariant((int)inf[i].tool_id));
        b->setToolTip(inf[i].tooltip);
        b->setShortcut(QKeySequence(inf[i].shortcut));
        grid->addWidget(b, inf[i].y, inf[i].x);
//        connect(b, SIGNAL(clicked()), this, SLOT(toolbuttonclicked()));
        grp->addButton(b);
    }
    connect(grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toolclicked( QAbstractButton* )));

    // special case for magnify button - it's a toggle button, not a tool.
    {
        QIcon icon;
        icon.addFile(JoinPath(iconDir, "magnify.png").c_str(), QSize(), QIcon::Normal, QIcon::Off);
        QToolButton* b = new QToolButton();
        b->setIcon( icon );
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //b->setIconSize(QSize(32,32));
        b->setCheckable(true);
        b->setAutoRaise(true);
        b->setToolTip(tr("Magnify"));
        b->setShortcut(QKeySequence("m"));
        grid->addWidget(b, 4, 0);
        connect(b, SIGNAL(toggled(bool)), this, SLOT(magnifyButtonToggled(bool)));
    }


    return grid;
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


void EditorWindow::SetFocus(NodePath const& focus)
{
    m_Focus = focus;
    Layer const& l = Proj().ResolveLayer(m_Focus);
    m_Frame = l.FrameIndexClipped(m_Time);
    // TODO: propagate!
    assert(false);
}

void EditorWindow::SetTime(uint64_t micros)
{
    m_Time = micros;
    Layer const& l = Proj().ResolveLayer(m_Focus);
    m_Frame = l.FrameIndexClipped(m_Time);
    // TODO: propagate!
    assert(false);
}


void EditorWindow::OnPenChanged()
{
    // new fg or bg pen
    {
        Colour fg = FGPen().rgb();
        Colour bg = BGPen().rgb();
        m_CurrentColourWidget->setFGColour(QColor(fg.r, fg.g, fg.b, fg.a));
        m_CurrentColourWidget->setBGColour(QColor(bg.r, bg.g, bg.b, bg.a));

        //assert(Proj().GetAnimConst().Fmt()==FMT_I8);
        if(FGPen().IdxValid()) {
            m_PaletteWidget->SetLeftSelected(FGPen().idx());
            if(m_PaletteEditor) {
                m_PaletteEditor->SetSelectedColour(PEN_FG, FGPen().idx());
            }
        }
        if(BGPen().IdxValid()) {
            m_PaletteWidget->SetRightSelected(BGPen().idx());
            if(m_PaletteEditor) {
                m_PaletteEditor->SetSelectedColour(PEN_BG, BGPen().idx());
            }
        }

        m_RangesWidget->SetFGPen(FGPen());
        m_RangesWidget->SetBGPen(BGPen());
    }
}

// Begin ProjectListener implementation


void EditorWindow::OnPaletteChanged(NodePath const& target, int frame, int index, Colour const& c)
{
    if (!Proj().SharesPalette(Focus(), m_Frame, target, frame)) {
        return;
    }
    // make sure the gui reflects any palette changes
    m_PaletteWidget->SetColour(index, c);

    // one of the active pens changed?
    if (FGPen().IdxValid() && FGPen().idx() == index) {
        SetFGPen(PenColour(c, index));
    }
    if (BGPen().IdxValid() && BGPen().idx() == index) {
        SetBGPen(PenColour(c, index));
    }
}

void EditorWindow::OnPaletteReplaced(NodePath const& target, int frame)
{
    if (!Proj().SharesPalette(Focus(), m_Frame, target, frame)) {
        return;
    }
    Palette const& pal = Proj().PaletteConst(target, frame);
    m_PaletteWidget->SetPalette(pal);

    // Ensure pen validity.
    int n = pal.NumColours();
    PenColour fg = FGPen();
    if (fg.IdxValid() && fg.idx() >= n) {
        fgColourPicked(n-1);
    }
    PenColour bg = BGPen();
    if (bg.IdxValid() && bg.idx() >= n) {
        bgColourPicked(n-1);
    }
    RethinkWindowTitle();
}

void EditorWindow::OnModifiedFlagChanged(bool)
{
    RethinkWindowTitle();
}

void EditorWindow::OnFramesAdded(NodePath const& /*target*/, int /*first*/, int /*count*/)
{
    RethinkWindowTitle();
}

void EditorWindow::OnFramesRemoved(NodePath const& target, int /*first*/, int /*count*/)
{
    Layer const& l = Proj().ResolveLayer(target);
    if (m_Frame >= (int)l.mFrames.size()) {
        // Make sure we're not left pointing at an invalid frame.
        setFrame((int)l.mFrames.size() - 1);
    } else {
        RethinkWindowTitle();
    }
}

void EditorWindow::OnFramesBlatted(NodePath const& target, int first, int count)
{
    // Don't worry about the image changes, but assume the palette
    // has been replaced.
    for (int i=first; i<count; ++i) {
        OnPaletteReplaced(target, i);
    }
}

// End of ProjectListener implementation

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

void EditorWindow::magnifyButtonToggled(bool checked)
{
    if (checked) {
        if (!m_MagView) {
            // If mouse is on the view (eg if magnify was turned on via
            // keyboard shortcut), then center both views upon it.
            // Otherwise, retain the current view center.
            QPoint focusPoint = m_ViewWidget->mapFromGlobal(QCursor::pos());
            if (!m_ViewWidget->rect().contains(focusPoint)) {
                focusPoint = m_ViewWidget->rect().center();
            }

            Point projFocus = m_ViewWidget->ViewToProj(Point(focusPoint.x(), focusPoint.y()));
            // Create the magnified view and figure out position/zoom.
            m_MagView = new EditViewWidget(*this, m_Focus, m_Frame);
            m_MagView->setCursor(*m_MouseCursors[MOUSESTYLE_DEFAULT]);
            m_MagView->SetZoom(m_ViewWidget->Zoom() * 4);
            m_ViewSplitter->addWidget(m_MagView);

            // adding the widget will have set up the view dimensions, so
            // now we can center both views around the focuspoint.
            m_ViewWidget->AlignView(
                Point(m_ViewWidget->Width() / 2, m_ViewWidget->Height() / 2),
                projFocus);
            m_MagView->AlignView(
                Point(m_MagView->Width() / 2, m_MagView->Height() / 2),
                projFocus);
        }
    } else {
        if (m_MagView) {
            // Destroy the magnified view (if it exists).
            delete m_MagView;
            m_MagView = nullptr;
        }
    }
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
    Colour c = Proj().PaletteConst(m_Focus, m_Frame).GetColour(idx);
    SetFGPen( PenColour(c,idx) );
}

void EditorWindow::bgColourPicked( int idx )
{
    Colour c = Proj().PaletteConst(m_Focus, m_Frame).GetColour(idx);
    SetBGPen( PenColour(c,idx) );
}


void EditorWindow::fgColourPickedRGB( Colour c )
{
    Palette const& pal = Proj().PaletteConst(m_Focus, m_Frame);
    int idx = pal.Closest(c);
    // snap to palette colour on indexed images
    Layer& l = Proj().ResolveLayer(Focus());
    if(l.Fmt()==FMT_I8 && idx >=0) {
        c = pal.Colours[idx];
    }
    SetFGPen(PenColour(c, idx));
}

void EditorWindow::bgColourPickedRGB( Colour c )
{
    Palette const& pal = Proj().PaletteConst(m_Focus, m_Frame);
    int idx = pal.Closest(c);
    // snap to palette colour on indexed images
    Layer& l = Proj().ResolveLayer(Focus());
    if(l.Fmt()==FMT_I8 && idx >=0) {
        c = pal.Colours[idx];
    }
    SetBGPen(PenColour(c, idx));
}

void EditorWindow::rangePicked()
{
    Box picked = m_RangesWidget->CurrentRange();
    SetCurrentRange(picked);
}

void EditorWindow::togglepaletteeditor()
{
    m_PaletteEditor->setVisible(!m_PaletteEditor->isVisible());
}

void EditorWindow::useeyedroppertool()
{
    UseTool( TOOL_EYEDROPPER );
}

void EditorWindow::update_menu_states()
{
    assert( m_ActionUndo && m_ActionRedo );
    m_ActionUndo->setEnabled( CanUndo() );
    m_ActionRedo->setEnabled( CanRedo() );
    m_ActionGridOnOff->setChecked( GridActive() );
    m_ActionUseBrushPalette->setEnabled( GetBrush() == -1 );

    // custom brush, and indexed?
    m_ActionScale2xBrush->setEnabled(
        GetBrush() == -1 && CurrentBrush().Fmt() == FMT_I8);

    {
        // custom brush, and focus has a palette?
        Palette const& pal = Proj().PaletteConst(m_Focus, m_Frame);
        m_ActionScale2xBrush->setEnabled(GetBrush() == -1 && pal.NColours >0);
    }

    // int nframes= Proj().GetLayer(ActiveLayer()).NumFrames();
    // TODO: IMPLEMENT!
    Layer const& l = Proj().ResolveLayer(m_Focus);
    int nframes = l.mFrames.size();
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

void EditorWindow::do_gridconfig()
{
    GridDialog dlg(this, Grid());
    if( dlg.exec() == QDialog::Accepted )
    {
        SetGrid(dlg.Grid());
    }
}

void EditorWindow::do_drawmodeChanged( QAction* act )
{
    DrawMode::Mode newMode = (DrawMode::Mode)act->data().toInt();
    SetMode(DrawMode(newMode)); 
    // TODO: should be some sort of Editor callback notification
    RethinkWindowTitle();
}

// resize the currently-focused layer
void EditorWindow::do_resize()
{
    // use the currently-focused frame to populate the resize dialog.
    Box b = m_ViewWidget->FocusedImgConst().Bounds();
    ResizeProjectDialog dlg(this,QRect(b.x,b.y,b.w,b.h));
    if (dlg.exec() == QDialog::Accepted)
    {
        QRect area = dlg.GetArea();
        Cmd* c = new Cmd_ResizeLayer(Proj(), Focus(),
            Box(0,0,area.width(),area.height()),
            BGPen() );
        AddCmd(c);
    }
}

void EditorWindow::do_changefmt()
{
    int currColours = Proj().PaletteConst(m_Focus, m_Frame).NumColours();
    Layer const& l = Proj().ResolveLayer(m_Focus);

    ChangeFmtDialog dlg(this, l.Fmt(), currColours);
    if( dlg.exec() == QDialog::Accepted ) {
        // ignore no-ops (eg rgba->rgba)
        if (dlg.pixel_format != l.Fmt() ||
            (l.Fmt() == FMT_I8 && dlg.num_colours != currColours)) {
            // TODO:
            // - don't remap if increasing palette size
            // - if decreasing palette size, give option to calculate new palette
            //   or to just remap using existing.
            // - give option of using brush palette or loading a palette?
            // TODO: use Cmd_Remap here?
            Cmd* c = new Cmd_ChangeFmt(Proj(), m_Focus, dlg.pixel_format, dlg.num_colours);
            AddCmd(c);
        }
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

    Palette const& brushPalette = CurrentBrush().GetPalette();

    // Ask the user if they want to remap to this new palette.
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Use brush palette");
    msgBox.setInformativeText("Do you want the image remapped?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
        case QMessageBox::Yes:
            {
                // Remap it.
                Layer& l = Proj().ResolveLayer(m_Focus);
                // keep the same pixelformat
                Cmd* cmd = new Cmd_Remap(Proj(), m_Focus, l.Fmt(), brushPalette);
                AddCmd(cmd);
            }
            break;
        case QMessageBox::No:
            {
                Cmd* cmd = new Cmd_PaletteReplace(Proj(), m_Focus, m_Frame, brushPalette);
                AddCmd(cmd);
            }
            break;
        case QMessageBox::Cancel:
        default:
            break;      // do nothing.
    }
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

void EditorWindow::do_scale2xbrush()
{
    if (GetBrush() != -1)
        return; // std brush - do nothing
    if (CurrentBrush().Fmt() != FMT_I8) {
        return;
    }
    HideToolCursor();

    Brush& oldBrush = CurrentBrush();
    Img* tmpImg = DoScale2x(oldBrush);

    Brush* newBrush = new Brush(oldBrush.Style(),
           *tmpImg,
           tmpImg->Bounds(),
           oldBrush.TransparentColour());
    newBrush->SetHandle(oldBrush.Handle() * 2.0f);
    newBrush->SetPalette(oldBrush.GetPalette());

    // UGH!
    g_App->SetCustomBrush( newBrush );
    SetBrush( -1 );
    delete tmpImg;
    ShowToolCursor();
}


void EditorWindow::do_remapbrush()
{
    // Convert the custom brush into the focus pixelformat and palette.
    if(GetBrush() != -1 )
        return; // std brush - do nothing

    // Create new image with the same format as the project target.
    Img const& focusImg = Proj().GetImgConst(m_Focus, m_Frame);
    // Get the palette we're remapping to.
    Palette const& destPalette = Proj().PaletteConst(m_Focus, m_Frame);
    Brush const& brush = CurrentBrush();

    if(destPalette.NColours == 0) {
        return; // no dest palette - do nothing.
    }


    HideToolCursor();
    Img* newImg = nullptr;
    switch(focusImg.Fmt()) {
        case FMT_I8:
            switch(brush.Fmt()) {
                case FMT_I8:    // I8 -> I8
                    newImg = new Img(brush);
                    RemapI8(*newImg, brush.GetPalette(), destPalette);  // Clone
                    break;
                case FMT_RGBX8:    // RGBX8 -> I8
                    newImg = ConvertRGBX8toI8(brush, destPalette);
                    break;
                case FMT_RGBA8:    // RGBA8 -> I8
                    newImg = ConvertRGBA8toI8(brush, destPalette);
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case FMT_RGBX8:
            switch(brush.Fmt()) {
                case FMT_I8:    // I8 -> RGBX8
                    newImg = ConvertI8toRGBX8(brush, brush.GetPalette());
                    RemapRGBX8(*newImg, destPalette);
                    break;
                case FMT_RGBX8:    // RGBX8 -> RGBX8
                    newImg = new Img(brush);    // Clone
                    RemapRGBX8(*newImg, destPalette);
                    break;
                case FMT_RGBA8:    // RGBA8 -> RGBX8
                    newImg = ConvertRGBA8toRGBX8(brush);
                    RemapRGBX8(*newImg, destPalette);
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case FMT_RGBA8:
            switch(brush.Fmt()) {
                case FMT_I8:    // I8 -> RGBA8
                    newImg = ConvertI8toRGBX8(brush, brush.GetPalette());
                    RemapRGBA8(*newImg, destPalette);
                    break;
                case FMT_RGBX8:    // RGBX8 -> RGBA8
                    newImg = ConvertRGBX8toRGBA8(brush);
                    RemapRGBA8(*newImg, destPalette);
                    break;
                case FMT_RGBA8:    // RGBA8 -> RGBA8
                    newImg = new Img(brush);    // Clone
                    RemapRGBA8(*newImg, destPalette);
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
    }

    // map across the transparent pen
    assert(destPalette.NColours > 0);
    int idx = destPalette.Closest(brush.TransparentColour().rgb());
    PenColour transparent(brush.TransparentColour().rgb(), idx);

    Brush* newBrush = new Brush(brush.Style(), *newImg, newImg->Bounds(), transparent);
    newBrush->SetPalette(destPalette);
    newBrush->SetHandle(brush.Handle());
    g_App->SetCustomBrush(newBrush);

    ShowToolCursor();
}


void EditorWindow::do_addlayer()
{
    assert(false);  //TODO: implement
#if 0
    Layer *l = new Layer();
    l->Append(new Img(FMT_RGBA8,128,128));  // TODO
    
    Cmd* c = new Cmd_NewLayer(Proj(), l, Proj().NumLayers());

    AddCmd(c);
#endif
}

void EditorWindow::do_addframe()
{
    assert(Focus().sel == NodePath::SEL_MAIN);
    Cmd* c = new Cmd_InsertFrames(Proj(), m_Focus, m_Frame+1, 1);
    AddCmd(c);
    // update frame in OnFramesAdded

    // go to newly-inserted frame
    setFrame(m_Frame + 1);
}

void EditorWindow::do_zapframe()
{
    assert(Focus().sel == NodePath::SEL_MAIN);
    Cmd* c= new Cmd_DeleteFrames(Proj(), m_Focus, m_Frame, 1);
    AddCmd(c);
    // update frame in OnFramesRemoved
}

void EditorWindow::do_prevframe()
{
    Layer const& l = Proj().ResolveLayer(m_Focus);
    assert(m_Focus.sel == NodePath::SEL_MAIN);
    if (m_Frame > 0) {
        setFrame(m_Frame - 1);
    } else {
        setFrame(l.mFrames.size() - 1); // Wrap.
    }
}

void EditorWindow::do_nextframe()
{
    Layer const& l = Proj().ResolveLayer(m_Focus);
    assert(m_Focus.sel == NodePath::SEL_MAIN);
    if (m_Frame < (int)l.mFrames.size() - 1) {
        setFrame(m_Frame + 1);
    } else {
        setFrame(0);    // Wrap.
    }
}

void EditorWindow::setFrame(int frame)
{
    //printf("EditorWindow::setFrame(%d->%d)\n", m_Frame, frame);
    Layer const& l = Proj().ResolveLayer(m_Focus);
    assert(frame >= 0 && frame < (int)l.mFrames.size());
    m_Frame = frame;
    m_Time = l.FrameTime(m_Frame);

    // TODO: if per-frame palette, need to update widgets

    m_ViewWidget->SetFrame(m_Frame);
    if (m_MagView) {
        m_MagView->SetFrame(m_Frame);
    }
    RethinkWindowTitle();
    //printf("end EditorWindow::setFrame()\n");
}

void EditorWindow::do_tospritesheet()
{
    assert(false);  //TODO: implement
#if 0
    ToSpritesheetDialog dlg(this, &Proj());
    if( dlg.exec() == QDialog::Accepted )
    {
        Cmd* c= new Cmd_ToSpriteSheet(Proj(), dlg.Columns());
        AddCmd(c);
    }
#endif
}

void EditorWindow::do_fromspritesheet()
{
    assert(false);  //TODO: implement
#if 0
    // TODO - multilayer!
    assert(Proj().NumLayers()==1);
    Layer& layer0 = Proj().GetLayer(0);

    FromSpritesheetDialog dlg(this, &Proj());
    if( dlg.exec() == QDialog::Accepted )
    {
        std::vector<Box> frames;
        SplitSpritesheet(layer0.GetImgConst(0).Bounds(), dlg.getNWide(), dlg.getNHigh(), frames);

        // TODO: pass in frames
        Cmd* c= new Cmd_FromSpriteSheet(Proj(), dlg.getNWide(), frames.size());
        AddCmd(c);
    }
#endif
}


void EditorWindow::do_loadpalette()
{

    QString filename = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    ProjDir(),
                    "GIMP Palette files (*.gpl)");
    if( filename.isNull() )
        return;

    try
    {
        std::unique_ptr<const Palette> newPalette(Palette::Load(filename.toStdString().c_str()));

        // Ask the user if they want to remap to this new palette.
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("Load new palette");
        msgBox.setInformativeText("Do you want the image remapped?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                {
                    // Remap it.
                    Layer& l = Proj().ResolveLayer(m_Focus);
                    // keep the same pixelformat
                    Cmd* cmd = new Cmd_Remap(Proj(), m_Focus, l.Fmt(), *newPalette);
                    AddCmd(cmd);
                }
                break;
            case QMessageBox::No:
                {
                    Cmd* cmd = new Cmd_PaletteReplace(Proj(), m_Focus, m_Frame, *newPalette);
                    AddCmd(cmd);
                }
                break;
            case QMessageBox::Cancel:
            default:
                break;      // do nothing.
        }
    }
    catch( Exception const& e )
    {
        GUIShowError( e.what() );
    }
//    RethinkWindowTitle();
}


QString EditorWindow::ProjDir()
{
    std::string d = Proj().Filename();
    return d.empty() ? QDir::homePath() : QString(DirName(d).c_str());
}


void EditorWindow::do_open()
{
//    if( !CheckZappingOK() )
//        return;
    QString loadfilters = "Image files (*.anim *.bmp *.gif *.iff *.ilbm *.lbm *.pbm *.pcx *.png *.jpg *.jpeg *.tga);;Any files (*)";

    QString filename = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    ProjDir(),  loadfilters );
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
    SaveProject(Proj().mFilename);
}

void EditorWindow::do_saveas()
{
    QString savefilters;
  
    Layer const& l = Proj().ResolveLayer(m_Focus);
    if (l.mFrames.size() > 1) {
        savefilters = "Animated GIF (*.gif);;Any files (*)";
    } else {
        savefilters = "Image files (*.bmp *.gif *.png);;Any files (*)";
    }

    QString filename = QFileDialog::getSaveFileName(
                    this,
                    "Save image as",
                    ProjDir(),
                    savefilters);

    if( filename.isNull() )
        return;

    SaveProject(filename.toStdString());
}

void EditorWindow::SaveProject(std::string const& filename)
{
    try
    {
        Filetype ft = FiletypeFromFilename(filename);
        if (ft == FILETYPE_UNKNOWN) {
            throw Exception("Unsupported file format.");
        }

        SaveRequirements reqs = CheckSave(*(Proj().mRoot), ft);
        if (reqs.cantSave) {
            throw Exception("Can't save in that format.");
        }
        if (reqs.flatten) {
            throw Exception("Can't save multiple layers in that format.");
        }
        if (reqs.quantise) {
            throw Exception("Format only supports paletted (indexed) images");
        }

        if (reqs.noAnim) {
            // TODO: Implement an Uber-savedialog to prompt user for assorted
            // save options...
            Layer const& l = Proj().ResolveLayer(m_Focus);
            // For now, just drop user into unexplained spritesheet dlg :-)
            ToSpritesheetDialog dlg(this, &l);
            if( dlg.exec() == QDialog::Accepted )
            {
                int cols = dlg.Columns();
                // convert to spritesheet
                Layer* tmp = LayerToSpriteSheet(l, cols);
                SaveLayer(*tmp, filename);
                // TODO: handle leak due to exceptions!!!!!!
                delete tmp;
            }
        } else {
            // Save directly - no processing required.
            Layer const& l = Proj().ResolveLayer(m_Focus);
            SaveLayer(l, filename);
        }
        Proj().mFilename = filename;
        Proj().SetModifiedFlag(false);
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
        m_ActionUseBrushPalette = a = m->addAction( "Use Brush Palette...", this, SLOT(do_usebrushpalette()) );
        a = m->addAction( "&Load Palette...", this, SLOT( do_loadpalette()) );
        m->addSeparator();
        m->addAction( "X-Flip Brush", this, SLOT(do_xflipbrush()),QKeySequence("x") );
        m->addAction( "Y-Flip Brush", this, SLOT(do_yflipbrush()),QKeySequence("y") );
        m_ActionScale2xBrush = m->addAction( "Scale2x Brush", this, SLOT(do_scale2xbrush()));
        m_ActionRemapBrush = m->addAction( "Remap Brush", this, SLOT(do_remapbrush()));
        m->addSeparator();


        m_ActionGridOnOff = a = m->addAction( "&Grid On?", this, SLOT( do_gridonoff(bool)), QKeySequence("g") );
        a->setCheckable(true);
        m_ActionGridConfig = m->addAction( "Grid Config...", this, SLOT( do_gridconfig()));

        a = m->addAction( "Resize...", this, SLOT(do_resize()));
        a = m->addAction( "Change format...", this, SLOT(do_changefmt()));
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

    // Layer menu
    {
        QMenu* m = menubar->addMenu("Layers");
        m->addAction( "&Add Layer", this, SLOT( do_addlayer()) );
    }


    // DrawMode menu
    {
        QMenu* m = menubar->addMenu("&DrawMode");
        m->addAction( m_ActionDrawmodeNormal);
        m->addAction( m_ActionDrawmodeColour);
        // TODO: REPLACE mode not yet working
        //m->addAction( m_ActionDrawmodeReplace);
        m->addAction( m_ActionDrawmodeRangeShift);
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
    a->setStatusTip("Normal drawing");

    a = m_ActionDrawmodeColour = new QAction("&Colour", this);
    a->setData(DrawMode::DM_COLOUR);
    a->setCheckable(true);
    a->setStatusTip("Draw with current colour");

    a = m_ActionDrawmodeReplace = new QAction("&Replace", this);
    a->setData(DrawMode::DM_REPLACE);
    a->setCheckable(true);
    a->setStatusTip("Ignore transparency");

    a = m_ActionDrawmodeRangeShift = new QAction("Range&Shift", this);
    a->setData(DrawMode::DM_RANGE);
    a->setCheckable(true);
    a->setStatusTip("Inc/dec according to range");

    QActionGroup* grp = new QActionGroup(this);
    grp->addAction(m_ActionDrawmodeNormal);
    grp->addAction(m_ActionDrawmodeColour);
    grp->addAction(m_ActionDrawmodeReplace);
    grp->addAction(m_ActionDrawmodeRangeShift);
    m_ActionDrawmodeNormal->setChecked(true);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(do_drawmodeChanged(QAction*)));

}


void EditorWindow::RethinkWindowTitle()
{
    std::string name = Proj().Filename();
    if( name.empty() )
        name = "Untitled";

    Layer const& l = Proj().ResolveLayer(m_Focus);
    Img const& img = *l.mFrames[m_Frame]->mImg;
    int w = img.W();
    int h = img.H();

    char dim[128];
    sprintf( dim, " (%dx%d) frame %d/%d", w, h, m_Frame+1, (int)l.mFrames.size());

    std::string title = "[*]";
    title += name;
    title += dim;
    switch (Mode().mode) {
        case DrawMode::DM_NORMAL: title += " NORMAL"; break;
        case DrawMode::DM_COLOUR: title += " COLOUR"; break;
        case DrawMode::DM_REPLACE: title += " REPLACE"; break;
        default: break;
    }
    title += " - EvilPixie";

    setWindowModified(Proj().ModifiedFlag());
    setWindowTitle(QString::fromStdString(title));
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




void EditorWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void EditorWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        try {
            QString fileName = url.toLocalFile();
            if (!url.isLocalFile()) {
                throw Exception("Not a local file");
            }


            Project* new_proj = new Project(fileName.toStdString());
            EditorWindow* fenster = new EditorWindow(new_proj);
            fenster->show();
            fenster->activateWindow();
            fenster->raise();

            if( Proj().Expendable() )
                this->close();

        } catch( Exception const& e ) {
            GUIShowError( e.what() );
        }
    }
}


