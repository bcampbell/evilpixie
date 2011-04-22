#include "miscwindows.h"
#include <QFile>
#include <QString>
#include <QTextEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>

HelpWindow::HelpWindow()
{
    resize( 400,400);
    QTextEdit* content = new QTextEdit();
    content->setReadOnly(true);
    {
        QFile file("data/help.html");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QString help_txt;
            help_txt = file.readAll();
            content->setHtml(help_txt);
        }
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(hide()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(content);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Help"));
}


AboutBox::AboutBox(QWidget* parent) :
    QDialog(parent)
{
    resize( 400,250);
    QTextEdit* content = new QTextEdit();
    content->setReadOnly(true);
    QString txt = "<h1>Evilpixie</h1>"
        "version 0.0000000000001  -  The TCE build (2011-04-22)<br/><br/>"
        "By Ben Campbell (ben@scumways.com)<br/><br/>"
        "Licensed under GPLv3<br/>"
        "Source at <a href=\"http://github.com/bcampbell/evilpixie\">http://github.com/bcampbell/evilpixie</a>";

    content->setHtml(txt);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(hide()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(content);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("About EvilPixie"));


}
