#include <QtWidgets/QtWidgets>

#include "griddialog.h"

GridDialog::GridDialog(QWidget *parent, Box const& initial)
    : QDialog(parent)
{
    m_W = new QSpinBox(this);
    m_W->setRange(1,65535);
    m_W->setValue(initial.w);
    QLabel* widthlabel = new QLabel(tr("Width:"));
    widthlabel->setBuddy(m_W);


    m_H = new QSpinBox(this);
    m_H->setRange(1,65535);
    m_H->setValue(initial.h);
    QLabel* heightlabel = new QLabel(tr("Height:"));
    heightlabel->setBuddy(m_H);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widthlabel, 0, 0);
    l->addWidget(m_W, 0, 1);
    l->addWidget(heightlabel, 1, 0);
    l->addWidget(m_H, 1, 1);
    l->addWidget(buttonBox, 2, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("Resize Grid"));
}

Box GridDialog::Grid()
{
    return Box(0,0,m_W->value(), m_H->value());
}

