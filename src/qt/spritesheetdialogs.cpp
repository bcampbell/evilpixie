#include <QtWidgets/QtWidgets>

#include "spritesheetdialogs.h"
#include "../project.h"

ToSpritesheetDialog::ToSpritesheetDialog(QWidget *parent, Project* proj)
    : QDialog(parent),
    m_Proj(proj)
{
    m_Proj->AddListener(this);
    QValidator *validator = new QIntValidator(1, m_Proj->NumFrames(), this);

    m_WidthEdit = new QLineEdit(this);
    m_WidthEdit->setText(QString::number(1));
    m_WidthEdit->setValidator(validator);
    QLabel* widthlabel = new QLabel(tr("Frames across:"));
    widthlabel->setBuddy(m_WidthEdit);


    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *l = new QGridLayout;
//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(widthlabel, 0, 0);
    l->addWidget(m_WidthEdit, 0, 1);
    l->addWidget(buttonBox, 2, 0, 1, 2 );
    setLayout(l);
    setWindowTitle(tr("Convert Animation to Spritesheet"));

}

ToSpritesheetDialog::~ToSpritesheetDialog()
{
    m_Proj->RemoveListener(this);
}


int ToSpritesheetDialog::NumAcross()
{
    return m_WidthEdit->text().toInt();
}

// projectlistener implementation
void ToSpritesheetDialog::OnFramesAdded(int /*first*/, int /*last*/)
{
}

void ToSpritesheetDialog::OnFramesRemoved(int /*first*/, int /*last*/)
{
}

