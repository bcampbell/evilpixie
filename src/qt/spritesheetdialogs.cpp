#include <QtWidgets/QtWidgets>

#include <QString>

#include <vector>

#include "spritesheetdialogs.h"
#include "../project.h"
#include "../sheet.h"



SheetPreviewWidget::SheetPreviewWidget(QWidget* parent) :
    QWidget(parent),
    m_Contain(0,0,0,0)
{
//    setFrameStyle( QFrame::Sunken );
}

void SheetPreviewWidget::setFrames(std::vector<Box> const& frames, Box const& contain)
{
    m_Frames.reserve(frames.size());
    m_Frames.assign(frames.begin(),frames.end());

    m_Contain = contain;
    update();
}

void SheetPreviewWidget::paintEvent(QPaintEvent *)
{
    QRect r = rect();
    QPainter painter(this);
    painter.eraseRect( r );

 

    const int F=8192;   // cheesy fixed-point math
    int rx = (F*r.width())/m_Contain.w;
    int ry = (F*r.height())/m_Contain.h;

    int s = rx>ry ? ry : rx;

    // draw extent
    {
        Box const& b = m_Contain;
        QRect extent((b.x*s)/F, (b.y*s)/F, (b.w*s)/F, (b.h*s)/F);
        painter.setBrush(QColor(0,0,0));
        painter.setPen(Qt::NoPen);
        painter.drawRect(extent);
    }

    // draw frames
    painter.setBrush(Qt::NoBrush);
    std::vector<Box>::iterator it;
    int n=0;
    for (it=m_Frames.begin(); it!=m_Frames.end(); ++it, ++n)
    {
        Box const& b = *it;
        QRect frame((b.x*s)/F, (b.y*s)/F, (b.w*s)/F, (b.h*s)/F);

        painter.setPen(QColor(255,0,0));
        painter.drawText( frame, Qt::AlignCenter, QString::number(n+1));

        painter.setPen(QColor(128,0,0));
        painter.drawRect( frame );
    }

}



QSize SheetPreviewWidget::sizeHint () const
    { return QSize( 128,128 ); }

QSize SheetPreviewWidget::minimumSizeHint () const
    { return QSize( 32,32 ); }


//---------------------------------------------------------

ToSpritesheetDialog::ToSpritesheetDialog(QWidget *parent, Project* proj)
    : QDialog(parent),
    m_Proj(proj)
{
    m_Proj->AddListener(this);

    int initialWidth = 4;
    m_Width = new QSpinBox();
    assert(false);  // TODO: sort it out
    //m_Width->setRange( 1,m_Proj->NumFrames() );
    m_Width->setValue(initialWidth);

    QLabel* widthlabel = new QLabel(tr("Frames across:"));
    widthlabel->setBuddy(m_Width);

    connect( m_Width, SIGNAL( valueChanged(int) ), this, SLOT(widthChanged(int) ) );

    m_Preview = new SheetPreviewWidget(this);


    m_Info = new QLabel(this);
    //m_Info->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_Info->setTextFormat(Qt::PlainText);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widthlabel, 0, 0);
    l->addWidget(m_Width, 0, 1);

    l->addWidget(m_Info, 1, 0, 1, 2 );

    l->addWidget(m_Preview, 2, 0,1,2);
    l->setRowStretch(2,1);   // preview widget gets any extra space

    l->addWidget(buttonBox, 3, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("Convert animation to spritesheet"));

    rethinkPreview();

}

ToSpritesheetDialog::~ToSpritesheetDialog()
{
    m_Proj->RemoveListener(this);
}


int ToSpritesheetDialog::NumAcross()
{
    return m_Width->value();
}


// projectlistener implementation
void ToSpritesheetDialog::OnFramesAdded(int /*first*/, int /*last*/)
{
    rethinkPreview();
}

void ToSpritesheetDialog::OnFramesRemoved(int /*first*/, int /*last*/)
{
    rethinkPreview();
}

void ToSpritesheetDialog::rethinkPreview()
{
    std::vector<Box> frames;
    //TODO: implement this!
    assert(false);
    NodePath fook;
    Layer const& layer0 = m_Proj->ResolveLayer(fook);

    Box extent = LayoutSpritesheet(layer0, NumAcross(), frames);
    /*
    printf("extent: %d,%d %dx%d\n",
            extent.x,
            extent.y,
            extent.w,
            extent.h);
    */
    m_Preview->setFrames(frames,extent);

    char buf[64];
    ::snprintf(buf, sizeof(buf), "resultant sheet: %dx%d:", extent.w, extent.h);
    m_Info->setText(QString::fromUtf8(buf));
}

void ToSpritesheetDialog::widthChanged(int)
{
    rethinkPreview();
}


//---------------------------------------------------------

FromSpritesheetDialog::FromSpritesheetDialog(QWidget *parent, Project* proj)
    : QDialog(parent),
    m_Proj(proj)
{
    m_Proj->AddListener(this);

    // TODO: multilayer support
//    assert(m_Proj->NumLayers()==1);
    assert(false);
    NodePath fook;

    int initialWidth = 4;
    m_NWide = new QSpinBox();
    m_NWide->setRange(1,proj->GetImgConst(fook,0).W());
    m_NWide->setValue(initialWidth);
    QLabel* widelabel = new QLabel(tr("Across:"));
    widelabel->setBuddy(m_NWide);

    m_NHigh = new QSpinBox();
    m_NHigh->setRange(1,proj->GetImgConst(fook,0).H());
    m_NHigh->setValue(initialWidth);
    QLabel* highlabel = new QLabel(tr("High:"));
    highlabel->setBuddy(m_NHigh);

    connect( m_NWide, SIGNAL( valueChanged(int) ), this, SLOT(rethinkPreview() ) );
    connect( m_NHigh, SIGNAL( valueChanged(int) ), this, SLOT(rethinkPreview() ) );

    m_Preview = new SheetPreviewWidget(this);
    rethinkPreview();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widelabel, 0, 0);
    l->addWidget(m_NWide, 0, 1);

    l->addWidget(highlabel, 1, 0);
    l->addWidget(m_NHigh, 1, 1);

    l->addWidget(m_Preview, 2, 0,1,2);

    l->addWidget(buttonBox, 3, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("Convert spritesheet to animation"));

}

FromSpritesheetDialog::~FromSpritesheetDialog()
{
    m_Proj->RemoveListener(this);
}


int FromSpritesheetDialog::getNWide()
{
    return m_NWide->value();
}

int FromSpritesheetDialog::getNHigh()
{
    return m_NHigh->value();
}


// projectlistener implementation
void FromSpritesheetDialog::OnFramesAdded(int /*first*/, int /*last*/)
{
    rethinkPreview();
}

void FromSpritesheetDialog::OnFramesRemoved(int /*first*/, int /*last*/)
{
    rethinkPreview();
}

void FromSpritesheetDialog::rethinkPreview()
{
    std::vector<Box> frames;
    
    // TODO: multilayer support
    assert(false);
//    assert(m_Proj->NumLayers()==1);
    NodePath fook;

    Box srcBox = m_Proj->GetImgConst(fook,0).Bounds();


    SplitSpritesheet(srcBox,getNWide(), getNHigh(), frames);

    m_Preview->setFrames(frames,srcBox);
}



