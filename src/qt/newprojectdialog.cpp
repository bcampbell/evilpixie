#include <QtGui>

#include "newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    QValidator *validator = new QIntValidator(1, 65536, this);

    m_WidthEdit = new QLineEdit(this);
    m_WidthEdit->setText( "256" );
    m_WidthEdit->setValidator(validator);
    QLabel* widthlabel = new QLabel(tr("Width:"));
    widthlabel->setBuddy(m_WidthEdit);


    m_HeightEdit = new QLineEdit(this);
    m_HeightEdit->setValidator(validator);
    m_HeightEdit->setText( "256" );
    QLabel* heightlabel = new QLabel(tr("Height:"));
    heightlabel->setBuddy(m_HeightEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widthlabel, 0, 0);
    l->addWidget(m_WidthEdit, 0, 1);
    l->addWidget(heightlabel, 1, 0);
    l->addWidget(m_HeightEdit, 1, 1);
    l->addWidget(buttonBox, 2, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("New Image"));
}

QSize NewProjectDialog::GetSize()
{
    return QSize( m_WidthEdit->text().toInt(), m_HeightEdit->text().toInt() );
}

