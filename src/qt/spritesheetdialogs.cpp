#include <QtWidgets/QtWidgets>

#include <vector>

#include "spritesheetdialogs.h"
#include "../project.h"
#include "../sheet.h"



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
    painter.setPen(QColor(255,0,0));
    painter.setBrush(QColor(0,0,0));

    const int F=8192;   // cheesy fixed-point math
    int rx = (F*r.width())/m_Contain.w;
    int ry = (F*r.height())/m_Contain.h;

    int s = rx>ry ? ry : rx;


//    printf("(%d %d) (%d %d) %d %d %d\n", r.width(), r.height(), m_Contain.w, m_Contain.h, rx,ry,s);
/*    if (s>F)
    {
        s=F;
    }
*/

    //TODO
//    int sy = r.height()/m_Contain.h;

    std::vector<Box>::iterator it;
    for (it=m_Frames.begin(); it!=m_Frames.end(); ++it)
    {
        Box const& b = *it;
        QRect frame((b.x*s)/F, (b.y*s)/F, (b.w*s)/F, (b.h*s)/F);
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
//    m_Width->setOrientation( Qt::Horizontal );
    m_Width->setRange( 1,m_Proj->NumFrames() );
//    m_Width->setTracking(true);
//    m_Width->setSingleStep(1);
//    m_Width->setPageStep(1);
    m_Width->setValue(initialWidth);

    QLabel* widthlabel = new QLabel(tr("Frames across:"));
    widthlabel->setBuddy(m_Width);

    connect( m_Width, SIGNAL( valueChanged(int) ), this, SLOT(widthChanged(int) ) );

    m_Preview = new SheetPreviewWidget(this);
    rethinkPreview();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widthlabel, 0, 0);
    l->addWidget(m_Width, 0, 1);

    l->addWidget(m_Preview, 1, 0,1,2);

    l->addWidget(buttonBox, 2, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("Convert animation to spritesheet"));

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
    Box extent = LayoutSpritesheet(m_Proj->GetAnimConst(), NumAcross(), frames);
    /*
    printf("extent: %d,%d %dx%d\n",
            extent.x,
            extent.y,
            extent.w,
            extent.h);
    */
    m_Preview->setFrames(frames,extent);


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

    int initialWidth = 4;
    m_NWide = new QSpinBox();
    m_NWide->setRange(1,proj->ImgConst(0).W());
    m_NWide->setValue(initialWidth);
    QLabel* widelabel = new QLabel(tr("Across:"));
    widelabel->setBuddy(m_NWide);

    m_NHigh = new QSpinBox();
    m_NHigh->setRange(1,proj->ImgConst(0).H());
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

    Box srcBox = m_Proj->ImgConst(0).Bounds();


    SplitSpritesheet(srcBox,getNWide(), getNHigh(), frames);

    m_Preview->setFrames(frames,srcBox);
}



