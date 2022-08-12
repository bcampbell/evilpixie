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

ToSpritesheetDialog::ToSpritesheetDialog(QWidget *parent, SpriteGrid const& initialGrid, Project& project, NodePath const& targ)
    : QDialog(parent),
    mProj(project),
    mTarg(targ),
    mGrid(initialGrid)
{
    //Layer const& layer = mProj.ResolveLayer(mTarg);
    mWidth = new QSpinBox();
    mWidth->setRange(1, mGrid.numFrames);
    mWidth->setValue(mGrid.numColumns);

    QLabel* widthlabel = new QLabel(tr("Columns:"));
    widthlabel->setBuddy(mWidth);

    connect(mWidth, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));

    mPreview = new SheetPreviewWidget(this);

    mInfo = new QLabel(this);
    //m_Info->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mInfo->setTextFormat(Qt::PlainText);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widthlabel, 0, 0);
    l->addWidget(mWidth, 0, 1);
    l->addWidget(mInfo, 1, 0, 1, 2 );
    l->addWidget(mPreview, 2, 0,1,2);
    l->setRowStretch(2,1);   // preview widget gets any extra space

    l->addWidget(buttonBox, 3, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("Convert animation to spritesheet"));

    rethinkPreview();
}

ToSpritesheetDialog::~ToSpritesheetDialog()
{
}


void ToSpritesheetDialog::rethinkPreview()
{
    std::vector<Box> cells;
    mGrid.Layout(cells);
    Box extent = mGrid.Extent();
    mPreview->setFrames(cells, extent);

    char buf[64];
    ::snprintf(buf, sizeof(buf), "Resultant sheet is %dx%d:", extent.w, extent.h);
    mInfo->setText(QString::fromUtf8(buf));
}

void ToSpritesheetDialog::widthChanged(int)
{
    mGrid.numColumns = mWidth->value();
    mGrid.numRows = 1 + ((mGrid.numFrames-1) / mGrid.numColumns);
    rethinkPreview();
}


//---------------------------------------------------------

FromSpritesheetDialog::FromSpritesheetDialog(QWidget *parent, Img const& srcImg, SpriteGrid const& initialGrid)
    : QDialog(parent),
    mSrcImg(srcImg),
    mGrid(initialGrid)
{
    mNWide = new QSpinBox();
    mNWide->setRange(1, mSrcImg.W());
    mNWide->setValue(mGrid.numColumns);
    QLabel* widelabel = new QLabel(tr("Across:"));
    widelabel->setBuddy(mNWide);

    mNHigh = new QSpinBox();
    mNHigh->setRange(1, mSrcImg.H());
    mNHigh->setValue(mGrid.numRows);
    QLabel* highlabel = new QLabel(tr("High:"));
    highlabel->setBuddy(mNHigh);

    connect(mNWide, SIGNAL(valueChanged(int)), this, SLOT(rethink()));
    connect(mNHigh, SIGNAL(valueChanged(int)), this, SLOT(rethink()));
    mPreview = new SheetPreviewWidget(this);
    rethink();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widelabel, 0, 0);
    l->addWidget(mNWide, 0, 1);

    l->addWidget(highlabel, 1, 0);
    l->addWidget(mNHigh, 1, 1);

    l->addWidget(mPreview, 2, 0,1,2);

    l->addWidget(buttonBox, 3, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("Convert spritesheet to animation"));
}

void FromSpritesheetDialog::rethink()
{
    // Update the grid data
    mGrid.SubdivideBox(mSrcImg.Bounds(), mNWide->value(), mNHigh->value());
    std::vector<Box> cells;
    mGrid.Layout(cells);
    Box extent = mGrid.Extent();
    mPreview->setFrames(cells, extent);
}


