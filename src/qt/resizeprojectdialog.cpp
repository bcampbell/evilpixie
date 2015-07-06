#include <QtWidgets/QtWidgets>

#include "resizeprojectdialog.h"

ResizeProjectDialog::ResizeProjectDialog(QWidget *parent, QRect const& initial)
    : QDialog(parent)
{
    QValidator *validator = new QIntValidator(1, 65536, this);

    m_WidthEdit = new QLineEdit(this);
    m_WidthEdit->setText(QString::number(initial.width()));
    m_WidthEdit->setValidator(validator);
    QLabel* widthlabel = new QLabel(tr("Width:"));
    widthlabel->setBuddy(m_WidthEdit);


    m_HeightEdit = new QLineEdit(this);
    m_HeightEdit->setValidator(validator);
    m_HeightEdit->setText(QString::number(initial.height()));
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
    setWindowTitle(tr("Resize Image"));
}

QRect ResizeProjectDialog::GetArea()
{
    return QRect( 0,0,m_WidthEdit->text().toInt(), m_HeightEdit->text().toInt() );
}

