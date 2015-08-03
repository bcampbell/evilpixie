#include <QtWidgets/QtWidgets>
#include <QtWidgets/QWidget>

#include "newprojectdialog.h"



struct modepreset {
    const char* name;
    PixelFormat fmt;
    int palette_cnt;
};

static modepreset presets[] = {
    {"RGBA",FMT_RGBA8,0},
    {"RGB",FMT_RGBX8,0},
    {"256 colour palette",FMT_I8,256},
    {"128 colour palette",FMT_I8,128},
    {"64 colour palette",FMT_I8,64},
    {"32 colour palette",FMT_I8,32},
    {"16 colour palette",FMT_I8,16},
    {"8 colour palette",FMT_I8,8},
    {"4 colour palette",FMT_I8,4},
    {"2 colour palette",FMT_I8,2},
};

const int N_PRESETS = sizeof(presets)/sizeof(modepreset);

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    QFormLayout *l = new QFormLayout;

    QValidator *validator = new QIntValidator(1, 65536, this);
    m_WidthEdit = new QLineEdit(this);
    m_WidthEdit->setText( "256" );
    m_WidthEdit->setValidator(validator);
    l->addRow("Width", m_WidthEdit);

    m_HeightEdit = new QLineEdit(this);
    m_HeightEdit->setValidator(validator);
    m_HeightEdit->setText( "256" );
    l->addRow("Height", m_HeightEdit);

    {
        QSpinBox* w = new QSpinBox(this);
        num_frames=1;
        w->setValue(num_frames);
        w->setRange(1,65536);
        connect(w, SIGNAL(valueChanged(int)), this, SLOT(framesChanged(int)));
        l->addRow("Frames", w);
    }

    {
        QComboBox* w = new QComboBox(this);
        m_Format = w;
        int i;
        for(i=0;i<N_PRESETS;i++)
        {
            modepreset& pre = presets[i];
            w->addItem(pre.name,i);
        }
        num_colours = presets[0].palette_cnt;
        pixel_format = presets[0].fmt;
        w->setCurrentIndex(0);

        connect(w,SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)));
        l->addRow("Format", w);
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    l->addRow(buttonBox);
    setLayout(l);
    setWindowTitle(tr("New Project"));
}

QSize NewProjectDialog::GetSize()
{
    return QSize( m_WidthEdit->text().toInt(), m_HeightEdit->text().toInt() );
}

void NewProjectDialog::formatChanged( int idx )
{
    int n = m_Format->itemData(idx).toInt();
    if( n<0 || n>=N_PRESETS) {
        return;
    }

    modepreset const& pre = presets[n];
    pixel_format = pre.fmt;
    num_colours = pre.palette_cnt;
}

