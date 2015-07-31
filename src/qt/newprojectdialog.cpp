#include <QtWidgets/QtWidgets>
#include <QtWidgets/QWidget>

#include "newprojectdialog.h"

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
        w->addItem("RGB",-1);
        w->addItem("2 colour palette",2);
        w->addItem("4 colour palette",4);
        w->addItem("8 colour palette",8);
        w->addItem("16 colour palette",16);
        w->addItem("32 colour palette",32);
        w->addItem("64 colour palette",64);
        w->addItem("128 colour palette",128);
        w->addItem("256 colour palette",256);
        num_colours = 32;
        pixel_format = FMT_I8;
        w->setCurrentIndex(5);
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
    num_colours = m_Format->itemData(idx).toInt();
    if( num_colours == -1 )
    {
        pixel_format = FMT_RGBA8;
        num_colours = 32;   // kludge for now
    }
    else
    {
        pixel_format = FMT_I8;
        if( num_colours<2 )
            num_colours = 2;
        if( num_colours>255 )
            num_colours=255;
    }
}

