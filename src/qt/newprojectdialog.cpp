#include <QtGui>

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
        w->setValue(1);
        w->setRange(1,65536);
        connect(w, SIGNAL(valueChanged(int)), this, SLOT(framesChanged(int)));
        l->addRow("Frames", w);
    }

    {
        QComboBox* w = new QComboBox(this);
        w->addItem("2",2);
        w->addItem("4",4);
        w->addItem("8",8);
        w->addItem("16",16);
        w->addItem("32",32);
        w->addItem("64",64);
        w->addItem("128",128);
        w->addItem("256",256);
        num_colours = 32;
        w->setCurrentIndex(4);
        connect(w,SIGNAL(currentIndexChanged(QString)), this, SLOT(numcoloursChanged(QString)));
        l->addRow("Colours", w);
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

void NewProjectDialog::numcoloursChanged( QString const& txt )
{
    num_colours = txt.toInt();
    if( num_colours<2 )
        num_colours = 2;
    if( num_colours>255 )
        num_colours=255;
}

