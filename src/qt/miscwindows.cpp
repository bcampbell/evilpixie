#include "miscwindows.h"
#include "../app.h"
#include "../util.h"
#include "../version.h"
#include <QFile>
#include <QString>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

HelpWindow::HelpWindow()
{
    resize( 600,500);
    QTextEdit* content = new QTextEdit();
    content->setReadOnly(true);
    {
        QFile file( JoinPath(g_App->DataPath(), "help.html").c_str() );
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
    resize( 500,300);
    QTextEdit* content = new QTextEdit();
    content->setReadOnly(true);
    QString txt = "<h1>EvilPixie</h1>"
        "Version " VERSION_STRING " (" VERSION_NAME ")<br/>"
        "Built " __DATE__ "<br/><br/>"
        "By Ben Campbell (ben@scumways.com)<br/><br/>"
        "Licensed under GPLv3<br/>"
        "Homepage: <a href=\"http://evilpixie.scumways.com\">http://evilpixie.scumways.com</a><br/>"
        "Source: <a href=\"http://github.com/bcampbell/evilpixie\">http://github.com/bcampbell/evilpixie</a>";

    content->setHtml(txt);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(hide()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(content);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("About EvilPixie"));


}
